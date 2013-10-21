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

static struct MuzdbInitializer {
	MuzdbInitializer()
	{
		av_register_all();
		std::locale::global(boost::locale::generator()(""));
	}

	~MuzdbInitializer()
	{
	}
} initalizer;

template<class Archive>
inline void serialize(Archive &ar, muzdb::TimeInfo &t, const unsigned int)
{
	ar & t.start;
	ar & t.duration;
	ar & t.end;
}

struct track {
	path filename;
	path ref_filename;
	std::map<std::string, std::string> fields;
	muzdb::TimeInfo time;

	template<class Archive>
	inline void serialize(Archive &ar, const unsigned int)
	{
		ar & filename;
		ar & ref_filename;
		ar & fields;
		ar & time;
	}
};

template<class Archive>
inline void save(Archive &ar, const path &t, const unsigned int)
{
	ar << t.wstring();
}

template<class Archive>
inline void load(Archive &ar, path &t, const unsigned int)
{
	std::wstring str;

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

MDB::MDB(const Path &root)
	: root_path(root)
{
}

const Path &MDB::root() const
{
	return root_path;
}

void MDB::new_file(const Path &p)
{
	BOOST_AUTO(meta, parse(ParserGen(p)));
	BOOST_AUTO(file, std::make_pair(p, meta));

	if (mcallback) {
		mcallback->new_file(file);
	}

	metadata.insert(file);
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

	metadata.erase(file);
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

		metadata.insert(file);
	} else {
		if (mcallback) {
			mcallback->mod_file(*prev, file);
		}

		metadata.erase(prev);
		metadata.insert(file);
	}
}

void MDB::update()
{
	BOOST_TYPEOF(mtimes) ntimes;

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
}

void MDB::callback(boost::shared_ptr<MuzdbCallback> cb)
{
	this->mcallback = cb;
}

const std::map<Path, Metadata> &MDB::get() const
{
	return metadata;
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

