#include "../include/muzdb.hpp"

#include <boost/scoped_ptr.hpp>
#include <boost/typeof/typeof.hpp>

int main(int argc, char **argv)
{
	BOOST_AUTO(mdb, muzdb::muzdb_init(argv[1]));

	mdb->update();

	return 0;
}

