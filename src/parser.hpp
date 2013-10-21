#ifndef __MUZDB_PARSER_HPP__
#define __MUZDB_PARSER_HPP__

namespace muzdb {

class Parser {
public:
	virtual void parse() = 0;
	virtual const Metadata &metadata() const = 0;
};

class MTrack : public Track {
private:
	Path path;
	Path ref_path;

	std::map<std::string, std::string> flds;

	TimeInfo time_info;

public:
	MTrack(const Path &path, const Path &ref_path);

	void set_time(int start, int duration, int end);
	void set_time(const TimeInfo &ti);

	void push(const std::pair<std::string, std::string> &field);
	void push(const std::string &key, const std::string &value);

	void set(const std::map<std::string, std::string> &fields);

	const Path &filename() const;
	const Path &ref_filename() const;
	const std::map<std::string, std::string> &fields() const;
	const TimeInfo &time() const;
};

class ParserGen {
private:
	const Path path;

public:
	ParserGen(const Path &path) : path(path) {}

	std::vector<boost::shared_ptr<Parser> > operator()() const;
};

class TrackGen {
private:
	const MTrack trk;

public:
	TrackGen(const MTrack &trk) : trk(trk) {}
	TrackGen(const Path &filename, const Path &ref_filename)
		: trk(MTrack(filename, ref_filename)) {}

	boost::shared_ptr<MTrack> operator()() const;
};

} // namespace muzdb

#endif /* __MUZDB_PARSER_HPP__ */

