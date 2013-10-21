#include "common.hpp"

#include "parser.hpp"
#include "cue.hpp"

#include "cdetect.hpp"

#include <sstream>

#include <boost/tokenizer.hpp>

#include <boost/assign/list_of.hpp>

#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string.hpp>

namespace muzdb {

class CueTokenizerFunction {
public:
	template <typename InputIterator, typename Token>
	bool operator()(InputIterator &next, InputIterator &end, Token &tok);

	void reset();
};

template <typename InputIterator, typename Token>
bool CueTokenizerFunction::operator()(InputIterator &next, InputIterator &end, Token &tok)
{
	bool is_text(false);

	while (next != end && isspace(*next) && *next != '"') {
		++next;
	}

	if (next == end)
		return false;

	if (*next == '"') {
		is_text = true;
		++next;
	}

	BOOST_AUTO(start, next);

	if (is_text) {
		while (next != end && *next != '"') {
			++next;
		}
	} else {
		while (next != end && !isspace(*next)) {
			++next;
		}
	}

	tok.assign(start, next);

	if (is_text && next != end) {
		++next;
	}

	return true;
}

void CueTokenizerFunction::reset()
{
}

static std::vector<std::string> tokenize(const std::string &str)
{
	std::vector<std::string> ret;

	boost::tokenizer<CueTokenizerFunction> tok(str);

	for (BOOST_AUTO(it, tok.begin()); it != tok.end(); ++it) {
		ret.push_back(*it);
	}

	return ret;
}

CueParser::CueParser(const Path &filename)
	: filename(filename)
{
	std::string tmp;
	boost::filesystem::ifstream ifs(filename);

	tmp.assign(
		std::istreambuf_iterator<char>(ifs),
		std::istreambuf_iterator<char>()
	);

	data = CDetect(tmp).convert(tmp);
}

CueParser::CueParser(const Path &filename, const std::string &data)
	: filename(filename)
	, track_gen(boost::make_shared<TrackGen>(filename, filename))
{
	this->data = CDetect(data).convert(data);
}

void CueParser::parse_lines(const std::vector<std::vector<std::string> > &lines)
{
	int track = 0;
	std::vector<boost::shared_ptr<MTrack> > tracks;

	static std::map<std::string, std::string> disc_field_alias
		= boost::assign::map_list_of
			("PERFORMER", "performer")
			("SONGWRITER", "composer")
			("TITLE", "album");

	if (!track_gen) {
		BOOST_FOREACH(std::vector<std::string> line, lines) {
			if (line[0] == "TRACK") {
				break;
			}

			if (line[0] == "FILE") {
				BOOST_AUTO(parent, filename.parent_path());

				track_gen = boost::make_shared<TrackGen>(filename, parent /= line[1]);
				break;
			}
		}

		if (!track_gen) {
			track_gen = boost::make_shared<TrackGen>(filename, filename);
		}
	}

	BOOST_AUTO(disc, (*track_gen)());
	BOOST_AUTO(line, lines.begin());

	while (line != lines.end() && track == 0) {
		if (line->at(0) == "REM") {
			disc->push(boost::locale::to_lower(line->at(1)), line->at(2));
		} else if (line->at(0) == "TRACK") {
			++track;
		} else if (line->at(0) == "FILE") {
		} else if (disc_field_alias.find(line->at(0)) != disc_field_alias.end()) {
			disc->push(disc_field_alias[line->at(0)], line->at(1));
		}  else {
			disc->push(boost::locale::to_lower(line->at(0)), line->at(1));
		}

		++line;
	}

	if (line == lines.end()) {
		return;
	}

	track_gen = boost::make_shared<TrackGen>(*disc);

	BOOST_AUTO(trk, (*track_gen)());
	trk->push("track", boost::lexical_cast<std::string>(track));

	for (; line != lines.end(); ++line) {
		if (line->at(0) == "TRACK") {
			tracks.push_back(trk);
			
			trk = (*track_gen)();

			++track;
			trk->push("track", boost::lexical_cast<std::string>(track));
		} else if (line->at(0) == "INDEX") {
			if (atoi(line->at(1).c_str()) != 1)
				continue;

			int min, sec, frame;

			sscanf(line->at(2).c_str(), "%d:%d:%d", &min, &sec, &frame);

			trk->set_time(min * 60000 + sec * 1000 + frame * 1000 / 75, 0, 0);
		} else {
			trk->push(boost::locale::to_lower(line->at(0)), line->at(1));
		}
	}

	tracks.push_back(trk);

	for (BOOST_AUTO(it, tracks.begin()); it != tracks.end(); ++it) {
		int start, end, duration;

		BOOST_AUTO(time, (*it)->time());
		BOOST_AUTO(next, it + 1);

		start = time.start;

		if (next == tracks.end()) {
			(*it)->set_time(start, -1, 0);
		} else {
			BOOST_AUTO(ntime, (*next)->time());

			end = ntime.start;
			duration = end - start;

			(*it)->set_time(start, end, duration);
		}
	}

	meta.insert(meta.end(), tracks.begin(), tracks.end());
}

void CueParser::parse()
{
	meta.clear();

	std::stringstream ss(data);
	std::string line;

	std::vector<std::vector<std::string> > lines;

	while (getline(ss, line)) {
		boost::algorithm::trim(line);

		BOOST_AUTO(toks, tokenize(line));

		if (toks.size() > 0) {
			lines.push_back(toks);
		}
	}

	parse_lines(lines);
}

const Metadata &CueParser::metadata() const
{
	return meta;
}

} // namespace muzdb

