#include "Stopwatch.hpp"
#include "datapoint.h"
#include "db.h"
#include "query.h"
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

void DataBase::create_table(const std::string& name, Table::Config& options)
{
	std::string table_path = create_table_path(name);
	std::filesystem::create_directories(table_path);
	m_tables[name] = std::make_unique<Table>(name, table_path, options);
}


void DataBase::insert(const std::string& table_name, const std::vector<DataPoint>& points)
{
	if (auto table = m_tables.find(table_name); table != m_tables.end())
	{
		stopwatch::Stopwatch watch;
		watch.start();
		table->second->insert(points);
		// table->second->finalise_all();
		std::cout << "Insert (MEM) Time: "<< watch.elapsed() << " ms" << "\n";
		// watch.start();
		// table->second->flush_chunks();
		// std::cout << "Flush Time: "<< watch.elapsed() << " ms" << "\n";
	}
	else
	{
		std::cerr << "Table not found: " << table_name << '\n';
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