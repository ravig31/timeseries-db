#pragma once
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "datapoint.h"
#include "query.h"
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
	void create_table(const std::string& name, Table::Config&);
	const std::vector<std::string> get_table_names() const;
	const Table* get_table(const std::string& table_name) const { return m_tables.at(table_name).get(); }

	std::vector<DataPoint> query(const std::string& table_name, const Query& query);
	void insert(const std::string& table_name, const std::vector<DataPoint>& points);
	void insert_from_csv(const std::string &table_name, const std::string& file_path);
	std::vector<DataPoint> load_data_from_csv(const std::string& filename);
	
	private:
	std::string m_name;
	std::string m_dbpath;
	std::unordered_map<std::string, std::unique_ptr<Table>> m_tables;
	
	std::string create_table_path(const std::string& table_name)
	{
		return m_dbpath + '/' + table_name;
	}
};