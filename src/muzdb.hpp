#ifndef __MUZDB_MUZDB_HPP__
#define __MUZDB_MUZDB_HPP__

#include "common.hpp"

extern "C" {
#include <libavformat/avformat.h>
}

namespace muzdb {

class MDB : public Muzdb {
private:
	const Path root_path;

	std::set<std::pair<Path, time_t> > mtimes;

	std::map<Path, Metadata> metadata;
	std::map<TrackUID, boost::shared_ptr<const Track> > mtracks;
	
	boost::shared_ptr<MuzdbCallback> mcallback;

	MuzdbConfig config;

	void insert_file(const FileRef &file);
	void erase_file(const FileRef &file);

	void new_file(const Path &path);
	void del_file(const Path &path);
	void mod_file(const Path &path);
	
public:
	MDB(const Path &root);

	const Path &root() const;

	void update();
	void save(const Path &path);
	void load(const Path &path);

	void callback(boost::shared_ptr<MuzdbCallback> cb);

	const std::map<Path, Metadata> &get() const;
	const std::map<TrackUID, boost::shared_ptr<const Track> > &tracks() const;

	MuzdbConfig &get_config();
};
	
} // namespace muzdb

#endif /* __MUZDB_MUZDB_HPP__ */

