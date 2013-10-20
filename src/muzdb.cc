#include "common.hpp"

#include "muzdb.hpp"
#include "parser.hpp"

using namespace boost::filesystem;

namespace muzdb {

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
}

void MDB::del_file(const Path &p)
{
	std::cout << "DEL_FILE: " << p << std::endl;
}

void MDB::mod_file(const Path &p)
{
	std::cout << "MOD_FILE: " << p << std::endl;
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
	this->cb = cb;
}

const std::map<Path, Metadata> &MDB::get() const
{
	return meta;
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

