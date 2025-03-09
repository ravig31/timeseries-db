#include "datapoint.h"
#include "db.h"
#include "query.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

void DataBase::create_table(const std::string& name, Table::Config& options)
{
	std::string table_path = create_table_path(name);
	std::filesystem::create_directories(table_path);
	m_tables[name] = std::make_unique<Table>(name, table_path, options);
}

const std::vector<std::string> DataBase::get_table_names() const {
    std::vector<std::string> names;
    for (const auto& [name, table] : m_tables) {
        names.push_back(name);
    }
    return names;
}

void DataBase::insert(const std::string& table_name, const std::vector<DataPoint>& points)
{
	if (auto table = m_tables.find(table_name); table != m_tables.end())
	{
		table->second->insert(points);
	}
	else
	{
		throw std::runtime_error( "Table not found");
	}
}

std::vector<DataPoint> DataBase::query(const std::string& table_name, const Query& query)
{	
	std::vector<DataPoint> results {};
	if (auto table = m_tables.find(table_name); table != m_tables.end())
	{
		results = table->second->query(query);
	}
	else
	{
		std::cerr << "Table not found: " << table_name << '\n';
	}
	return results;
}


void DataBase::insert_from_csv(const std::string& table_name, const std::string& file_path)
{
	std::vector<DataPoint> points = load_data_from_csv(file_path);
	insert(table_name, points);
}


std::vector<DataPoint>  DataBase::load_data_from_csv(const std::string& file_path) {
	std::vector<DataPoint> data;
	std::ifstream infile(file_path);
	if (!infile.is_open()) {
		std::cout << "Error: Could not open file " << file_path << " for reading." << std::endl;
		return data;
	}

	std::string line;
	std::getline(infile, line); // Skip header

	while (std::getline(infile, line)) {
		std::stringstream ss(line);
		std::string timestamp_str, value_str, extra;

		if (std::getline(ss, timestamp_str, ',') && std::getline(ss, value_str, ',')) {

			if (std::getline(ss, extra, ',')) {
				std::cerr << "Warning: Extra columns found in line: " << line << std::endl;
				continue; // Skip the line
			}

			try {
				std::time_t timestamp = std::stoll(timestamp_str);
				double value = std::stod(value_str);
				data.push_back({ timestamp, value });
			} catch (const std::invalid_argument& e) {
				std::cerr << "Warning: Invalid data format in line: " << line << std::endl;
			} catch (const std::out_of_range& e) {
				std::cerr << "Warning: Data out of range in line: " << line << std::endl;
			}

		} else {
			std::cerr << "Warning: Incorrect line format: " << line << std::endl;
		}
	}

	infile.close();
	std::cout << "Data loaded from " << file_path << std::endl;
	return data;
}