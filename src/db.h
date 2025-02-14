#pragma once
#include <unordered_map>

#include "datapoint.h"
#include "table.h"

class DataBase
{
  public:
	void create_table(const std::string& name);
	void insert(const std::string& table_name, const DataPoint& point);
	std::vector<DataPoint> const query(
		const std::string& table_name,
		DataPoint::Timestamp start_ts,
		DataPoint::Timestamp end_ts
	); // when to return vector by reference?
    friend std::ostream& operator<<(std::ostream& out, const DataBase& db);
  private:
	std::unordered_map<std::string, std::unique_ptr<Table>> tables;
};