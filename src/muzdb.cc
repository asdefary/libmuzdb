#include "common.hpp"

#include <boost/filesystem/fstream.hpp>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/serialization/set.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>

#include "muzdb.hpp"
#include "parser.hpp"
#include "cdetect.hpp"

using namespace boost::filesystem;

namespace boost {
namespace serialization {

template<class Archive>
inline void serialize(Archive &ar, muzdb::TimeInfo &t, const unsigned int)
{
	ar & t.start;
	ar & t.duration;
	ar & t.end;
}

template<class Archive>
inline void serialize(Archive &ar, muzdb::TrackUID &t, const unsigned int)
{
	ar & t.u1;
	ar & t.u2;
}

struct track {
	path filename;
	path ref_filename;

	std::map<std::string, std::string> fields;

	muzdb::TrackUID tuid;
	muzdb::TimeInfo time;

	template<class Archive>
	inline void serialize(Archive &ar, const unsigned int)
	{
		ar & filename;
		ar & ref_filename;
		ar & fields;
		ar & tuid;
		ar & time;
	}
};

template<class Archive>
inline void save(Archive &ar, const path &t, const unsigned int)
{
	ar << t.string();
}

template<class Archive>
inline void load(Archive &ar, path &t, const unsigned int)
{
	std::string str;

	ar >> str;

	t = str;
}

template<class Archive>
inline void serialize(Archive &ar, path &t, const unsigned int file_version)
{
	boost::serialization::split_free(ar, t, file_version);
}

template<class Archive>
inline void save(Archive &ar, const muzdb::Metadata &t, const unsigned int)
{
	std::vector<track> tmp;

	BOOST_FOREACH(BOOST_TYPEOF(*t.begin()) it, t) {
		tmp.push_back((track) {
			it->filename(),
			it->ref_filename(),
			it->fields(),
			it->uid(),
			it->time()
		});
	}

	ar << tmp;
}

template<class Archive>
inline void load(Archive &ar, muzdb::Metadata &t, const unsigned int)
{
	std::vector<track> tmp;

	ar >> tmp;
	
	BOOST_FOREACH(track &m, tmp) {
		BOOST_AUTO(trk, boost::make_shared<muzdb::MTrack>(m.filename, m.ref_filename));

		trk->set(m.fields);
		trk->set_time(m.time);

		t.push_back(trk);
	}
}

template<class Archive>
inline void serialize(Archive &ar, muzdb::Metadata &t, const unsigned int file_version)
{
	boost::serialization::split_free(ar, t, file_version);
}

} // namespace serialization
} // namespace boost

namespace muzdb {

static struct MuzdbInitializer {
	MuzdbInitializer()
	{
		av_register_all();
		std::locale::global(boost::locale::generator()(""));

		init_uid();
	}

	~MuzdbInitializer()
	{
	}
} initalizer;

static Metadata parse(const ParserGen &pg)
{
	Metadata meta;

	BOOST_AUTO(ps, pg());

	BOOST_FOREACH(boost::shared_ptr<Parser> par, ps) {
		try {
			par->parse();
			BOOST_AUTO(res, par->metadata());

			meta.insert(meta.end(), res.begin(), res.end());			
		} catch (...) {
		}
	}

	return meta;
}

static bool operator<(const muzdb::TrackUID &t1, const muzdb::TrackUID &t2)
{
	if (t1.u1 != t2.u1) {
		return (t1.u1 < t2.u1);
	}

	return (t1.u2 < t2.u2);
}

MDB::MDB(const Path &root)
	: root_path(root)
{
}

const Path &MDB::root() const
{
	return root_path;
}

void MDB::insert_file(const FileRef &file)
{
	metadata.insert(file);

	BOOST_FOREACH(BOOST_TYPEOF(*file.second.begin()) f, file.second) {
		mtracks.insert(std::make_pair(f->uid(), f));
	}
}

void MDB::erase_file(const FileRef &file)
{
	metadata.erase(file.first);

	BOOST_FOREACH(BOOST_TYPEOF(*file.second.begin()) f, file.second) {
		mtracks.erase(f->uid());
	}
}

void MDB::new_file(const Path &p)
{
	BOOST_AUTO(meta, parse(ParserGen(p)));
	BOOST_AUTO(file, std::make_pair(p, meta));

	if (mcallback) {
		mcallback->new_file(file);
	}

	insert_file(file);
}

void MDB::del_file(const Path &p)
{
	BOOST_AUTO(file, metadata.find(p));

	if (file == metadata.end()) {
		return;
	}

	if (mcallback) {
		mcallback->del_file(*file);
	}

	erase_file(*file);
}

void MDB::mod_file(const Path &p)
{
	BOOST_AUTO(meta, parse(ParserGen(p)));

	BOOST_AUTO(file, std::make_pair(p, meta));
	BOOST_AUTO(prev, metadata.find(p));

	if (prev == metadata.end()) {
		if (mcallback) {
			mcallback->new_file(file);
		}

		insert_file(file);
	} else {
		if (mcallback) {
			mcallback->mod_file(*prev, file);
		}

		erase_file(*prev);
		insert_file(file);
	}
}

void MDB::insert_directory(const Path &path, BOOST_TYPEOF(mtimes) &ntimes)
{
	for (BOOST_AUTO(it, recursive_directory_iterator(root_path, symlink_option::recurse));
		it != recursive_directory_iterator(); ++it) {

		BOOST_AUTO(p, it->path());

		if (!exists(p)) {
			continue;
		}

		BOOST_AUTO(ext, boost::locale::to_lower(p.extension().string()));
		
		if (config.extensions.find(ext) == config.extensions.end())
			continue;

		ntimes.insert(std::make_pair(p, last_write_time(p)));
	}

}

void MDB::insert_file(const Path &path, BOOST_TYPEOF(mtimes) &ntimes)
{
	if (exists(path)) {
		ntimes.insert(std::make_pair(path, last_write_time(path)));
	}
}

void MDB::update()
{
	BOOST_TYPEOF(mtimes) ntimes;

	if (is_directory(root_path)) {
		insert_directory(root_path, ntimes);
	} else {
		insert_file(root_path, ntimes);
	}

	BOOST_AUTO(mit, mtimes.begin());
	BOOST_AUTO(nit, ntimes.begin());

	while (!(mit == mtimes.end() && nit == ntimes.end())) {
		if (mit == mtimes.end()) {
			new_file(nit->first);
			++nit;
		} else if (nit == ntimes.end()) {
			del_file(mit->first);
			++mit;
		} else {
			int cmp = nit->first.compare(mit->first);

			if (cmp > 0) {
				del_file(mit->first);
				++mit;
			} else if (cmp < 0) {
				new_file(nit->first);
				++nit;
			} else {
				if (nit->second != mit->second) {
					mod_file(nit->first);
				}

				++mit;
				++nit;
			}
		}
	}

	mtimes.swap(ntimes);
}

void MDB::save(const Path &p)
{
	ofstream ofs(p, std::ios_base::out | std::ios_base::binary);
	boost::archive::binary_oarchive oa(ofs);

	oa << mtimes;
	oa << metadata;
}

void MDB::load(const Path &p)
{
	ifstream ifs(p, std::ios_base::in | std::ios_base::binary);
	boost::archive::binary_iarchive ia(ifs);

	ia >> mtimes;
	ia >> metadata;

	BOOST_FOREACH(BOOST_TYPEOF(*metadata.begin()) m, metadata) {
		BOOST_AUTO(tracks, m.second);

		BOOST_FOREACH(BOOST_TYPEOF(*tracks.begin()) t, tracks) {
			mtracks.insert(std::make_pair(t->uid(), t));
		}
	}
}

void MDB::callback(boost::shared_ptr<MuzdbCallback> cb)
{
	this->mcallback = cb;
}

const std::map<Path, Metadata> &MDB::get() const
{
	return metadata;
}

const std::map<TrackUID, boost::shared_ptr<const Track> > &MDB::tracks() const
{
	return mtracks;
}

MuzdbConfig &MDB::get_config()
{
	return config;
}

boost::shared_ptr<Muzdb> muzdb_init(const Path &p)
{
	return boost::make_shared<MDB>(p);
}

} // namespace muzdb

