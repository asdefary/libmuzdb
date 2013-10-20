#ifndef __MUZDB__METADATA_HPP__
#define __MUZDB__METADATA_HPP__

#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>

#include <map>
#include <vector>
#include <string>

namespace muzdb {

struct TimeInfo {
	int start;
	int duration;
	int end;
};

class Track {
public:
	virtual const boost::filesystem::path &filename() const = 0;
	virtual const boost::filesystem::path &ref_filename() const = 0;
	virtual const std::map<std::string, std::string> &fields() const = 0;
	virtual const TimeInfo &time() const = 0;
};

typedef std::vector<boost::shared_ptr<const Track> > Metadata;

} // namespace muzdb

#endif /* __MUZDB__METADATA_HPP__ */

