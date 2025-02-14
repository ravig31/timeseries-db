#include "db.h"
#include "datapoint.h"
#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include <string>

void DataBase::create_table(const std::string& name)
{
	tables[name] = std::make_unique<Table>(name);
}

void DataBase::insert(const std::string& table_name, const DataPoint& point)
{
	if (auto table = tables.find(table_name); table != tables.end())
	{
		table->second->insert(point);
	}
	else
	{
		std::cerr << "Table not found: " << table_name << '\n';
	}
}

std::vector<DataPoint> const DataBase::query(
	const std::string& table_name,
	DataPoint::Timestamp start_ts,
	DataPoint::Timestamp end_ts
)
{
    if (auto table = tables.find(table_name); table != tables.end())
        return table->second->query(table_name, start_ts, end_ts);

    std::cerr << "No results found." << '\n';
    return {}; 
}

std::string format_timestamp(const DataPoint::Timestamp& ts) {
    std::time_t t = std::chrono::system_clock::to_time_t(ts);
    char mbstr[100]; // Increased buffer size just in case
    std::strftime(mbstr, sizeof(mbstr), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return std::string(mbstr);
}

std::ostream& operator<<(std::ostream& out, const DataBase& db) {
    out << "\n--- Database Visualization ---\n";
    if (db.tables.empty()) {
        out << "No tables in the database.\n";
        return out;
    }

    for (const auto& [table_name, table]: db.tables) {
        out << "Table: " << table_name << "\n";

        if (table->get_chunks().empty()) {
            out << "  No chunks in this table.\n";
            continue;
        }

        for (const auto& chunk : table->get_chunks()) {
            std::cout << "  Chunk : "
                      << format_timestamp(chunk->get_start_ts()) << " - "
                      << format_timestamp(chunk->get_end_ts()) << "\n";

            if (chunk->get_data().empty()) {
                std::cout << "    No data points in this chunk.\n";
                continue;
            }

            // Print header
            std::cout << "    " << std::setw(20) << std::left << "Timestamp"
                      << std::setw(10) << "Value" << "\n";
            std::cout << "    " << std::setfill('-') << std::setw(30) << "-" << std::setfill(' ') << "\n";

            for (const auto& point: chunk->get_data()) {
                std::cout << "    " << std::setw(20) << std::left << format_timestamp(point.ts)
                          << std::setw(10) << point.value << "\n";
            }
        }
    }
}