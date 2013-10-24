#include "../include/muzdb.hpp"

#include <boost/typeof/typeof.hpp>
#include <boost/foreach.hpp>

int main(int argc, char **argv)
{
	BOOST_AUTO(mdb, muzdb::muzdb_init());

	try {
		mdb->load(argv[1]);

		BOOST_AUTO(tracks, mdb->tracks());

		std::cout << tracks.size() << " Tracks Loaded" << std::endl;

		BOOST_FOREACH(BOOST_TYPEOF(*tracks.begin()) t, tracks) {
			BOOST_AUTO(track, t.second);

			std::cout << "filename\t:\t" << track->filename() << std::endl;
			std::cout << "ref_filename\t:\t" << track->ref_filename() << std::endl;

			BOOST_AUTO(tuid, track->uid());

			std::cout << "\t\tuid 	: " << tuid.u1 << " " << tuid.u2 << std::endl;

			BOOST_AUTO(time, track->time());

			std::cout << "\t\tstart	:" << time.start	<< std::endl;
			std::cout << "\t\tend		:" << time.end		<< std::endl;
			std::cout << "\t\tduration	:" << time.duration	<< std::endl;

			BOOST_AUTO(fis, track->file_info());

			BOOST_FOREACH(BOOST_TYPEOF(*fis.begin()) d, fis) {
				std::cout << "\t" << d.first << "\t:\t" << d.second << std::endl;
			}

			BOOST_AUTO(fs, track->fields());

			BOOST_FOREACH(BOOST_TYPEOF(*fs.begin()) d, fs) {
				std::cout << "\t" << d.first << "\t:\t" << d.second << std::endl;
			}

			std::cout << std::endl;
		}
	} catch (...) {
	}

	return 0;
}

