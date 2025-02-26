#include <chrono>
#include <cstddef>
#include <iostream>
#include <random>

#include "config.h"
#include "datapoint.h"
#include "db.h"
#include "utils.h"

std::vector<DataPoint> generate_test_data_fixed_intervals(
	size_t num_points,
	std::chrono::time_point<std::chrono::system_clock> start_time,
	std::chrono::minutes interval
)
{
	std::vector<DataPoint> data;
	data.reserve(num_points);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> distrib(-1.0, 1.0); // Adjust range as needed

	for (size_t i = 0; i < num_points; ++i)
	{
		auto ts = std::chrono::system_clock::to_time_t(start_time + i * interval);
		double value = distrib(gen);
		data.push_back({ ts, value });
	}
	return data;
}

int main()
{
	DataBase db{ "db1", "../tmp/tsdb" };
	Table::Config config{ Config::CHUNK_INTERVAL_SECS,
						  Config::CHUNK_CACHE_SIZE,
						  Config::MAX_CHUNKS_TO_SAVE,
						  Config::FLUSH_INTERVAL_SECS,
						  Config::MIN_DATA_RESOLUTION_SECS };
	db.create_table("test_table", config);

	auto now = std::chrono::system_clock::now();
	// Generate test data with 15-minute intervals:
	size_t num_test_points = 30; // Number of data points
	auto test_data =
	generate_test_data_fixed_intervals(num_test_points, now, std::chrono::minutes(15));
	
	for (const auto& point : test_data)
	{
		db.insert("test_table", point);
	}
	
	auto now_int = std::chrono::system_clock::to_time_t(now);
	TimeRange q_range = TimeRange{now_int, now_int + 3600}; 
	Query q = {q_range};
	auto res = db.query("test_table", q);
	for (const auto& p : res)
	{
		std::cout << p.ts << ": " << p.value << "\n";
	}

	// table->flush_chunks();
	// ... (Your query logic)
	return 0;
}
