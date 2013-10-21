#include "common.hpp"

#include "parser.hpp"

namespace muzdb {

std::vector<boost::shared_ptr<Parser> > ParserGen::operator()() const
{
	std::vector<boost::shared_ptr<Parser> > ret;

	ret.push_back(boost::make_shared<AVParser>(path));

	return ret;
}

} // namespace muzdb

