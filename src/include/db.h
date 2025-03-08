#pragma once
#include <filesystem>
#include <memory>
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
	// Need a way to batch insert points and after batch insertion make sure all  
	std::vector<DataPoint> query(const std::string& table_name, const Query& query);
	void create_table(const std::string& name, Table::Config&);
	void insert(const std::string& table_name, const std::vector<DataPoint>& points);
	void insert_from_csv(const std::string &table_name, const std::string& file_path);
	
	private:
	std::string m_name;
	std::string m_dbpath;
	std::unordered_map<std::string, std::unique_ptr<Table>> m_tables;
	
	std::string create_table_path(const std::string& table_name)
	{
		return m_dbpath + '/' + table_name;
	}
	std::vector<DataPoint> load_data_from_csv(const std::string& filename);
};