#include "../include/muzdb.hpp"

#include <boost/typeof/typeof.hpp>
#include <boost/foreach.hpp>

int main(int argc, char **argv)
{
	BOOST_AUTO(mdb, muzdb::muzdb_init());

	try {
		mdb->load(argv[1]);

		BOOST_AUTO(meta, mdb->get());

		BOOST_FOREACH(BOOST_TYPEOF(*meta.begin()) m, meta) {
			std::cout << m.first << std::endl;

			BOOST_FOREACH(BOOST_TYPEOF(*m.second.begin()) f, m.second) {
				std::cout << "\t" << f->filename() << std::endl;
				std::cout << "\t" << f->ref_filename() << std::endl;

				BOOST_AUTO(time, f->time());

				std::cout << "\t\t\tstart	:" << time.start	<< std::endl;
				std::cout << "\t\t\tend		:" << time.end		<< std::endl;
				std::cout << "\t\t\tduration	:" << time.duration	<< std::endl;

				BOOST_AUTO(fs, f->fields());

				BOOST_FOREACH(BOOST_TYPEOF(*fs.begin()) d, fs) {
					std::cout << "\t\t" << d.first << "\t:\t" << d.second << std::endl;
				}
			}

			std::cout << std::endl;
		}
	} catch (...) {
	}

	return 0;
}

