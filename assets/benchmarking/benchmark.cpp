#include <cassert>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <vector>

#include "Stopwatch.hpp"
#include "cli.h"
#include "db.h"
#include "query.h"
#include "utils.h"

std::vector<std::pair<Timestamp, Timestamp>> load_intervals_from_csv(const std::string& filename)
{
	std::vector<std::pair<Timestamp, Timestamp>> intervals;
	std::ifstream file(filename);
	if (!file.is_open())
	{
		std::cerr << "Error: Could not open file " << filename << std::endl;
		return intervals; // Return empty vector on error
	}

	std::string line;
	std::getline(file, line); // Skip header row

	while (std::getline(file, line))
	{
		std::stringstream ss(line);
		std::string start_str, end_str;
		if (std::getline(ss, start_str, ',') && std::getline(ss, end_str))
		{
			try
			{
				Timestamp start_ts = std::stoll(start_str);
				Timestamp end_ts = std::stoll(end_str);
				intervals.emplace_back(start_ts, end_ts);
			}
			catch (const std::invalid_argument& e)
			{
				std::cerr << "Error: Invalid timestamp in CSV: " << line << std::endl;
			}
			catch (const std::out_of_range& e)
			{
				std::cerr << "Error: Timestamp out of range in CSV: " << line << std::endl;
			}
		}
		else
		{
			std::cerr << "Error: Invalid line format in CSV: " << line << std::endl;
		}
	}

	return intervals;
}

int main()
{
	stopwatch::Stopwatch watch;
	TimeDelta data_res_secs = 300;
	Timestamp anchor = 1740618000;
	size_t num_test_points = 10000; // Number of data points
	// Create DB & Table
	DataBase db{ "db1", "tmp/tsdb" };
	Table::Config config{ 3600,
						  1000,
						  12,
						  30, //
						  data_res_secs };

	db.create_table("benchmark", config);

	// // Create sample data points
	// auto test_data =
	// 	generate_test_data(num_test_points, anchor, std::chrono::seconds(data_res_secs));
	// save_data_to_csv(test_data, "assets/sample.csv");

	// Insert
	std::cout << "Inserting data..." << "\n";
	watch.start();
	db.insert_from_csv("benchmark", "assets/sample.csv");
	auto insert_time = watch.elapsed<stopwatch::mus>();
	std::cout << "Insert Time: " << static_cast<double>(insert_time) / 1000 << " ms" << "\n\n";

	// Query
	long long total_rows_queried = 0;
	long long total_time_us = 0;
	std::vector<std::pair<Timestamp, Timestamp>> intervals =
		load_intervals_from_csv("assets/intervals.csv");
	std::cout << "Running query benchmark..." << "\n";
	int i = 1;
	for (const auto& [start, end] : intervals)
	{	
		Query q = { TimeRange{ start, end } };
		watch.start();
		auto res = db.query("benchmark", q);
		auto query_time_us = watch.elapsed<stopwatch::mus>();
		
		// Validate that all data points are within q_range
		for (const auto& p : res)
		{
			if (!(p.ts >= q.m_time_range.start_ts && p.ts <= q.m_time_range.end_ts))
			{
				std::cerr << "Error: Data point " << p.ts << " is outside the query range."
				<< std::endl;
			}
		}
		total_rows_queried += res.size();
		total_time_us += query_time_us;
		std::cout << "Ran query: " << i << "/" << intervals.size() << ", " << res.size() << " rows in: " << static_cast<double>(query_time_us) / 1000 << " ms\n";
		i++;
	}

	// Convert to ms
	double avg_query_time_ms = static_cast<double>(total_time_us) / intervals.size() / 1000;
	double query_us_per_row =
		(total_time_us / (static_cast<double>(total_rows_queried)));

	double rows_per_us = 1 / query_us_per_row;
	double rows_per_s = rows_per_us * 1000 * 1000; 

	// Output operation times
	std::cout << "Queries Ran: " << intervals.size() << "\n";
	std::cout << "Average Query Time: " << avg_query_time_ms << " ms" << "\n";
	std::cout << "Rows Queried: " << total_rows_queried << "\n";
	std::cout << "Average Rows/s: " << rows_per_s << "\n";
	return 0;
}
