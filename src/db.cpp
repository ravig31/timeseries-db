#include "db.h"
#include "datapoint.h"
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

void DataBase::create_table(const std::string& name)
{
	std::string table_path = create_table_path(name);
	std::filesystem::create_directories(table_path);
	m_tables[name] = std::make_unique<Table>(name, table_path);
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

std::vector<DataPoint> const DataBase::query(
	const std::string& table_name,
	int64_t start_ts,
	int64_t end_ts
)
{
	auto& table = m_tables[table_name];
	return table->query(start_ts, end_ts);
}