#include <cstddef>
#include <unordered_map>

#include "datapoint.h"
#include "table.h"


class DataBase
{
  public:
	void create_table(const std::string& name, const Table::Schema);
	void insert(const std::string& table_name, const DataPoint& data);
	std::vector<DataPoint> query(); // when to return vector by reference?
  private:
	std::unordered_map<std::string, std::unique_ptr<Table>> tables;
};

int main() { return 0; }
