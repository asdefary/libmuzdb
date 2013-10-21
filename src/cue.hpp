#ifndef __MUZDB_CUE_HPP__
#define __MUZDB_CUE_HPP__

namespace muzdb {

class CueParser : public Parser {
private:
	std::string data;
	Metadata meta;

	const Path filename;

	boost::shared_ptr<TrackGen> track_gen;

public:
	CueParser(const Path &filename);
	CueParser(const Path &filename, const std::string &data);

	void parse_lines(const std::vector<std::vector<std::string> > &lines);

	void parse();
	const Metadata &metadata() const;
};

} // anmespace muzdb

#endif /* __MUZDB_CUE_HPP__ */

