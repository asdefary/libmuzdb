#include "common.hpp"

#include "muzdb.hpp"
#include "parser.hpp"

using namespace boost::filesystem;

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
	av_register_all();
}

const Path &MDB::root() const
{
	return root_path;
}

void MDB::new_file(const Path &p)
{
	std::cout << "NEW_FILE: " << p << std::endl;

	BOOST_AUTO(meta, parse(ParserGen(p)));
	BOOST_AUTO(file, std::make_pair(p, meta));

	if (mcallback) {
		mcallback->new_file(file);
	}

	metadata.insert(file);
}

void MDB::del_file(const Path &p)
{
	std::cout << "DEL_FILE: " << p << std::endl;

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
	std::cout << "MOD_FILE: " << p << std::endl;

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
		
		if (config.extensions.find(p.extension().string()) == config.extensions.end())
			continue;

		ntimes.insert(std::make_pair(p.wstring(), last_write_time(p)));
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
}

void MDB::save(const Path &p)
{
}

void MDB::load(const Path &p)
{
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

