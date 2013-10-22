#ifndef __MUZDB_HPP__
#define __MUZDB_HPP__

#include "muzdb/exception.hpp"
#include "muzdb/metadata.hpp"

#include <boost/shared_ptr.hpp>
#include <boost/filesystem.hpp>

#include <map>
#include <set>
#include <vector>

namespace muzdb {

typedef boost::filesystem::path Path;
typedef std::pair<Path, Metadata> FileRef;

class MuzdbCallback {
public:
	virtual void new_file(const FileRef &file) = 0;
	virtual void del_file(const FileRef &file) = 0;
	virtual void mod_file(const FileRef &prevf, const FileRef &newf) = 0;
};

struct MuzdbConfig {
	std::set<std::string> extensions;
};

class Muzdb {
public:
	virtual const Path &root() const = 0;

	virtual void update() = 0;
	virtual void save(const Path &path) = 0;
	virtual void load(const Path &path) = 0;

	virtual void callback(boost::shared_ptr<MuzdbCallback> cb) = 0;

	virtual const std::map<Path, Metadata> &get() const = 0;
	virtual const std::map<TrackUID, boost::shared_ptr<const Track> > &tracks() const = 0;

	virtual MuzdbConfig &get_config() = 0;
};

boost::shared_ptr<Muzdb> muzdb_init(const boost::filesystem::path &root = Path("."));

} // namespace muzdb

#endif /* __MUZDB_HPP__ */

