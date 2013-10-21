#include "common.hpp"

#include "parser.hpp"

#include "avformat.hpp"
#include "cue.hpp"

namespace muzdb {

std::vector<boost::shared_ptr<Parser> > ParserGen::operator()() const
{
	std::vector<boost::shared_ptr<Parser> > ret;

	if (boost::locale::to_lower(path.extension().string()) == ".cue") {
		ret.push_back(boost::make_shared<CueParser>(path));
	} else {
		ret.push_back(boost::make_shared<AVParser>(path));
	}

	return ret;
}

} // namespace muzdb

