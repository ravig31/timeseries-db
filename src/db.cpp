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

void DataBase::insert(const std::string& table_name, const DataPoint& point)
{
	if (auto table = m_tables.find(table_name); table != m_tables.end())
	{
		table->second->insert(point);
	}
	else
	{
		std::cerr << "Table not found: " << table_name << '\n';
	}
}

void DataBase::insert(const std::string& table_name, const std::vector<DataPoint>& points)
{
	if (auto table = m_tables.find(table_name); table != m_tables.end())
	{
		for (const auto& point : points)
		{
			table->second->insert(point);
		}
		table->second->finalise_all();
		table->second->flush_chunks();
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