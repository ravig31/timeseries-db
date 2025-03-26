#include <cassert>
#include <cstddef>

#include "cli.h"
#include "db.h"

int main()
{
	// Create DB
	DataBase db{ "db1", "tmp/tsdb" };

	CLI cli(db);

	cli.run();

	return 0;
}
