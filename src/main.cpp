#include <cassert>
#include <chrono>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <random>

#include "Stopwatch.hpp"
#include "cli.h"
#include "config.h"
#include "db.h"

std::vector<DataPoint> generate_test_data(
	size_t num_points,
	std::chrono::time_point<std::chrono::system_clock> start_time,
	std::chrono::seconds interval
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

long get_peak_rss() {
    std::ifstream status_file("/proc/self/status");
    std::string line;
    long peak_rss = 0;

	std::string ignore;
	std::ifstream ifs("/proc/self/stat", std::ios_base::in);
	ifs >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
			>> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
			>> ignore >> ignore >> ignore >> peak_rss;
			
    return peak_rss;
}

void save_data_to_csv(const std::vector<DataPoint>& data, const std::string& filename)
{
	std::ofstream outfile(filename);
	if (!outfile.is_open())
	{
		std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
		return;
	}

	outfile << "Timestamp,Value\n"; // Header

	for (const auto& point : data)
	{
		outfile << point.ts << "," << point.value << "\n";
	}

	outfile.close();
	std::cout << "Data saved to " << filename << std::endl;
}

int main()
{
	stopwatch::Stopwatch watch;
	int data_res_secs = 300;

	std::chrono::system_clock::time_point anchor =
		std::chrono::system_clock::from_time_t(1740618000);
	// Create DB & Table
	DataBase db{ "db1", "tmp/tsdb" };
	Table::Config config{ Config::CHUNK_INTERVAL_SECS,
						  Config::CHUNK_CACHE_SIZE,
						  Config::MAX_CHUNKS_TO_SAVE,
						  Config::FLUSH_INTERVAL_SECS,
						  data_res_secs};

	CLI cli(db);

	cli.run();

		


	// // Create test data points
	// size_t num_test_points = 10000; // Number of data points
	// auto test_data =
	// 	generate_test_data(num_test_points, anchor, std::chrono::seconds(data_res_secs));

	// save_data_to_csv(test_data, "assets/sample.csv");

	
	// // Insert
	// watch.start();
	// db.insert_from_csv("test_table", "assets/sample.csv");
	// auto insert_time = watch.elapsed<stopwatch::mus>();

	// // Query
	// auto anchor_int = std::chrono::system_clock::to_time_t(anchor);
	// int points_to_query = (10000);
	// int q_duration = (points_to_query * (data_res_secs));
	// Query q = { TimeRange{ anchor_int, anchor_int + q_duration } };

	// watch.start();
	// auto res = db.query("test_table", q);
	// auto query_time = watch.elapsed<stopwatch::mus>();

	// // Output operation times
	// std::cout << "Insert Time: " << static_cast<double>(insert_time) / 1000 << " ms" << "\n";
	// std::cout << "Query Time: " << static_cast<double>(query_time) / 1000 << " ms" << "\n";

	// // Validate that all data points are within q_range
	// for (const auto& p : res)
	// {
	// 	if (!(p.ts >= q.m_time_range.start_ts && p.ts <= q.m_time_range.end_ts))
	// 	{
	// 		std::cerr << "Error: Data point " << p.ts << " is outside the query range."
	// 				  << std::endl;
	// 	}
	// }
	// std::cout << "Rows: " << res.size() << "\n";
	// std::cout << "Rows expected: " << points_to_query << "\n";
	// std::cout << "Peak memory: " << get_peak_rss() << " KB" << std::endl;

	return 0;
}
