#ifndef __MUZDB_PARSER_HPP__
#define __MUZDB_PARSER_HPP__

#include "track.hpp"

namespace muzdb {

class Parser {
public:
	virtual void parse() = 0;
	virtual const Metadata &metadata() const = 0;
};

class ParserGen {
private:
	const Path path;

public:
	ParserGen(const Path &path) : path(path) {}

	std::vector<boost::shared_ptr<Parser> > operator()() const;
};

} // namespace muzdb

#endif /* __MUZDB_PARSER_HPP__ */

