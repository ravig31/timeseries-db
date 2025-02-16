#include "db.h"
#include "datapoint.h"
#include <iostream>
#include <memory>
#include <string>

void DataBase::create_table(const std::string& name)
{
	m_tables[name] = std::make_unique<Table>(name);
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

void DataBase::flush_table(const std::string& table_name) 
{
    auto& table = m_tables[table_name];
    std::string table_path =  m_dbpath + '/' + table_name;
    std::filesystem::create_directories(table_path);
    
    // Write each block to disk
    for (size_t i = 0; i < table->m_chunks.size(); ++i) {
        table->m_chunks[i]->persist(
            table_path + "/block_" + std::to_string(i)
        );
    }
}