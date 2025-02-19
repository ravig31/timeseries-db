#include <chrono>
#include <cstddef>
#include <iostream>

#include "datapoint.h"
#include "db.h"

int main()
{
	DataBase db{ "db1", "../tmp/tsdb" };
	db.create_table("test_table");

	// Generate sample data
	auto now = std::chrono::system_clock::now();

	for (int i = 0; i < 1000000; ++i)
	{
		auto ts = std::chrono::system_clock::to_time_t(now + std::chrono::seconds(i));
		db.insert("test_table", DataPoint{ ts, sin(i * 0.01) });
	}

	std::cout << db.get_table("test_table")->size() << '\n';

	// std::cout << db.query(
	// 	"test_table",
	// 	std::chrono::system_clock::to_time_t(now),
	// 	std::chrono::system_clock::to_time_t(now + std::chrono::seconds(60))
	// ).size() << '\n';
	return 0;
}
