#pragma once
#include <filesystem>
#include <memory>
#include <unordered_map>

#include "datapoint.h"
#include "table.h"

class DataBase
{
  public:
	DataBase(const std::string& db_name, const std::string& filepath)
		: m_name(db_name)
		, m_dbpath(filepath)
	{
		std::filesystem::create_directories(filepath);
	}

	void create_table(const std::string& name);
	void insert(const std::string& table_name, const DataPoint& point);
	std::vector<DataPoint> const query(
		const std::string& table_name,
		int64_t start_ts,
		int64_t end_ts
	);
	
	Table* get_table(const std::string& table_name) { return m_tables[table_name].get(); }
	
	private:
	std::string m_name;
	std::string m_dbpath;
	std::unordered_map<std::string, std::unique_ptr<Table>> m_tables;
	
	std::string create_table_path(const std::string& table_name)
	{
		return m_dbpath + '/' + table_name;
	}
};