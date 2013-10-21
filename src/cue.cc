#include "common.hpp"

#include "parser.hpp"
#include "cue.hpp"

#include "cdetect.hpp"

#include <sstream>

#include <boost/filesystem/fstream.hpp>

namespace muzdb {

CueParser::CueParser(const Path &filename)
{
	std::string tmp;
	boost::filesystem::ifstream ifs(filename);

	tmp.assign(
		std::istreambuf_iterator<char>(ifs),
		std::istreambuf_iterator<char>()
	);

	data = CDetect(tmp).convert(tmp);
}

CueParser::CueParser(const std::string &data)
{
	this->data = CDetect(data).convert(data);
}

void CueParser::parse()
{
}

const Metadata &CueParser::metadata() const
{
	return meta;
}

} // namespace muzdb

