#include <cassert>
#include <cstddef>
#include <chrono>
#include <iostream>
#include <random>

#include "config.h"
#include "db.h"
#include "Stopwatch.hpp"


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
	// Create DB & Table
	DataBase db{ "db1", "tmp/tsdb" };
	Table::Config config{ Config::CHUNK_INTERVAL_SECS,
							Config::MAX_CHUNK_SIZE,
						  Config::CHUNK_CACHE_SIZE,
						  Config::MAX_CHUNKS_TO_SAVE,
						  Config::FLUSH_INTERVAL_SECS,
						  Config::MIN_DATA_RESOLUTION_SECS };
	db.create_table("test_table", config);

	stopwatch::Stopwatch watch;

	// Create test data points
	std::chrono::system_clock::time_point anchor =
		std::chrono::system_clock::from_time_t(1740618000);
	size_t num_test_points = 1000000; // Number of data points
	int resolution_mins = 5;
	auto test_data =
		generate_test_data_fixed_intervals(
			num_test_points, 
			anchor, 
			std::chrono::minutes(resolution_mins));
	

	// Insert 		
	watch.start();
	db.insert("test_table", test_data);
	auto insert_time = watch.elapsed<stopwatch::mus>(); 

	// Query
	auto anchor_int = std::chrono::system_clock::to_time_t(anchor);
	int q_duration = (3600*12);
	Query q = { TimeRange{ anchor_int, anchor_int + q_duration } };

	watch.start();
	auto res = db.query("test_table", q);
	auto query_time = watch.elapsed<stopwatch::mus>();
	
	std::cout << "Insert Time: " << static_cast<double>(insert_time) / 1000 << " ms" << "\n";
	std::cout << "Query Time: " << static_cast<double>(query_time) / 1000 << " ms" << "\n";

	// Validate that all data points are within q_range
	for (const auto& p : res)
	{
		if (!(p.ts >= q.m_time_range.start_ts && p.ts <= q.m_time_range.end_ts))
		{
			std::cerr << "Error: Data point " << p.ts << " is outside the query range."
					  << std::endl;
		}
	}


	std::cout << "data_within_range: " << "true" << "\n";
	std::cout << "rows: " << res.size() << "\n";
	std::cout << "rows expected: " << q_duration / (resolution_mins * 60) << "\n";
	return 0;
}
