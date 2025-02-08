#include <chrono>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class DataPoint
{
  public:
	using Timestamp = std::chrono::system_clock::time_point;

	Timestamp ts;
	std::unordered_map<std::string, double> fields;
};

class Chunk
{
};

class Table
{
  public:
	struct Schema
	{
		std::vector<std::string> fields;
	};

	void insert(const DataPoint& data);
	std::vector<DataPoint> const query();

  private:
	std::string name;
	std::vector<std::unique_ptr<Chunk>> chunks;
};

class DataBase
{
  public:
    void create_table(const std::string& name, const Table::Schema);
    void insert(const std::string& table_name, const DataPoint& data);
    std::vector<DataPoint> query(); // when to return vector by reference?
  private:
	std::unordered_map<std::string, std::unique_ptr<Table>> tables;
};

int main()
{
    return 0;
}
