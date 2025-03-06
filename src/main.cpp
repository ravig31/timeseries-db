#include <chrono>
#include <cstddef>
#include <iostream>
#include <random>

#include "config.h"
#include "db.h"

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
							Config::MAX_CHUNK_SIZE,
						  Config::CHUNK_CACHE_SIZE,
						  Config::MAX_CHUNKS_TO_SAVE,
						  Config::FLUSH_INTERVAL_SECS,
						  Config::MIN_DATA_RESOLUTION_SECS };
	db.create_table("test_table", config);

	std::chrono::system_clock::time_point anchor =
		std::chrono::system_clock::from_time_t(1740618000);
	size_t num_test_points = 1000; // Number of data points
	auto test_data =
		generate_test_data_fixed_intervals(num_test_points, anchor, std::chrono::minutes(15));

	db.insert("test_table", test_data);

	// Add functionality to db later




	// Query
	auto now_int = std::chrono::system_clock::to_time_t(anchor);
	TimeRange q_range = TimeRange{ now_int, now_int + (7200*4) };
	Query q = { q_range };
	auto start = std::chrono::high_resolution_clock::now(); // Start timer

	auto res = db.query("test_table", q);

	auto end = std::chrono::high_resolution_clock::now(); // End timer
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

	std::cout << "Query Time: " << duration.count() << " us" << "\n";

	// Validate that all data points are within q_range
	for (const auto& p : res)
	{
		if (!(p.ts >= q.m_time_range.start_ts && p.ts <= q.m_time_range.end_ts))
		{
			std::cerr << "Error: Data point " << p.ts << " is outside the query range."
					  << std::endl;
		}
	}

	std::cout << "q_range: " << "(" << q.m_time_range.start_ts << "," << q.m_time_range.end_ts
			  << ")" << "\n";
	std::cout << "rows: " << res.size() << "\n";
	// for (const auto& p : res)
	// {
	// 	std::cout << p.ts << ": " << p.value << "\n";
	// }

	// table->flush_chunks();
	// ... (Your query logic)
	return 0;
}
