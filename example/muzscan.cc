#include "../include/muzdb.hpp"

#include <boost/assign/list_of.hpp>
#include <boost/typeof/typeof.hpp>

static std::vector<std::string> scan_ext =
	boost::assign::list_of(".cue")(".flac")(".tak")(".tta")(".ape")(".aac")(".m4a")(".wav")(".mp3")(".ogg");

int main(int argc, char **argv)
{
	BOOST_AUTO(mdb, muzdb::muzdb_init(argv[1]));

	BOOST_AUTO(&config, mdb->get_config());

	config.extensions.insert(scan_ext.begin(), scan_ext.end());

	mdb->update();

	return 0;
}

