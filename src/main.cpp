#include <chrono>
#include <cstddef>
#include <iostream>

#include "datapoint.h"
#include "db.h"

int main()
{
	DataBase db{};
	db.create_table("stock-prices");

	auto now = std::chrono::system_clock::now();
	db.insert("stock-prices", DataPoint{ now, 10.2 });

	db.create_table("sensor_readings");
	auto later = now + std::chrono::seconds(60);
	db.insert("sensor_readings", DataPoint{ later, 24.0 });

	std::cout << db << '\n';

	return 0;
}
