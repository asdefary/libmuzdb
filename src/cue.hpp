#ifndef __MUZDB_CUE_HPP__
#define __MUZDB_CUE_HPP__

namespace muzdb {

class CueParser : public Parser {
private:
	std::string data;
	Metadata meta;

public:
	CueParser(const Path &filename);
	CueParser(const std::string &data);

	void parse();
	const Metadata &metadata() const;
};

} // anmespace muzdb

#endif /* __MUZDB_CUE_HPP__ */

