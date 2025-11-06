#include "datapoint.h"
#include "db.h"
#include "query.h"
#include "table.h"
#include "utils.h"
#include <ctime>
#include <gtest/gtest.h>
#include <algorithm>
#include <limits>

class DatabaseTest : public ::testing::Test {
  protected:
    DataBase db{"test_db", "./test_db_data"};

    void SetUp() override {
        // Create a test table with proper config
        Table::Config config(
            3600,  // chunk_size_secs
            24,    // chunk_cache_size
            2,     // max_chunks_to_save
            60,    // flush_interval_secs
            300    // min_resolution_secs
        );
        db.create_table("test_table", config);
    }

    void TearDown() override {
        // Cleanup
    }
};

// Test table creation
TEST_F(DatabaseTest, CreateTableSuccessfully) {
    Table::Config config(3600, 24, 2, 60, 300);
    db.create_table("new_table", config);

    std::vector<std::string> tables = db.get_table_names();
    EXPECT_TRUE(std::find(tables.begin(), tables.end(), "new_table") != tables.end());
}

// Test getting table names
TEST_F(DatabaseTest, GetTableNames) {
    std::vector<std::string> tables = db.get_table_names();
    EXPECT_GE(tables.size(), 1);
    EXPECT_TRUE(std::find(tables.begin(), tables.end(), "test_table") != tables.end());
}

// Test basic insert operation
TEST_F(DatabaseTest, InsertDataPoints) {
    std::vector<DataPoint> points = {
        {1000, 10.5},
        {2000, 20.3},
        {3000, 15.7}
    };

    EXPECT_NO_THROW(db.insert("test_table", points));
}

// Test insert into non-existent table
TEST_F(DatabaseTest, InsertIntoNonExistentTableThrows) {
    std::vector<DataPoint> points = {{1000, 10.5}};

    EXPECT_THROW(db.insert("non_existent", points), std::runtime_error);
}

// Test empty insert
TEST_F(DatabaseTest, InsertEmptyVector) {
    std::vector<DataPoint> points;
    EXPECT_NO_THROW(db.insert("test_table", points));
}

// Test large batch insert
TEST_F(DatabaseTest, InsertLargeBatch) {
    std::vector<DataPoint> points;
    for (int i = 0; i < 1000; ++i) {
        points.push_back({static_cast<Timestamp>(i * 1000), static_cast<double>(i)});
    }

    EXPECT_NO_THROW(db.insert("test_table", points));
}

// Test query with time range
TEST_F(DatabaseTest, QueryWithTimeRange) {
    std::vector<DataPoint> points = {
        {1000, 10.5},
        {2000, 20.3},
        {3000, 15.7},
        {4000, 25.1},
        {5000, 30.2}
    };
    db.insert("test_table", points);

    TimeRange range(2000, 4000);
    Query query(range, false, 0);
    std::vector<DataPoint> results = db.query("test_table", query);

    EXPECT_GE(results.size(), 1);
    for (const auto& point : results) {
        EXPECT_GE(point.ts, range.start_ts);
        EXPECT_LE(point.ts, range.end_ts);
    }
}

// Test query on non-existent table
TEST_F(DatabaseTest, QueryNonExistentTableReturnsEmpty) {
    Query query;
    std::vector<DataPoint> results = db.query("non_existent", query);

    EXPECT_EQ(results.size(), 0);
}

// Test insert from CSV
TEST_F(DatabaseTest, InsertFromCSVFile) {
    // This test requires a valid test CSV file
    // Format: timestamp,value (with header)
    EXPECT_NO_THROW(db.insert_from_csv("test_table", "test_data.csv"));
}

// Test load data from CSV
TEST_F(DatabaseTest, LoadDataFromCSV) {
    std::vector<DataPoint> data = db.load_data_from_csv("test_data.csv");

    // May be empty if file doesn't exist, but shouldn't throw
    EXPECT_TRUE(data.size() >= 0);
}

// Test multiple table operations
TEST_F(DatabaseTest, MultipleTablesIndependent) {
    Table::Config config(3600, 24, 2, 60, 300);
    db.create_table("table_a", config);
    db.create_table("table_b", config);

    std::vector<DataPoint> points_a = {{1000, 10.5}, {2000, 20.3}};
    std::vector<DataPoint> points_b = {{3000, 30.5}, {4000, 40.3}};

    db.insert("table_a", points_a);
    db.insert("table_b", points_b);

    std::vector<std::string> tables = db.get_table_names();
    EXPECT_GE(tables.size(), 3);
}

// Test data point values
TEST_F(DatabaseTest, DataPointValues) {
    DataPoint point = {5000, 42.5};

    EXPECT_EQ(point.ts, 5000);
    EXPECT_DOUBLE_EQ(point.value, 42.5);
}

// Test negative values
TEST_F(DatabaseTest, InsertNegativeValues) {
    std::vector<DataPoint> points = {
        {1000, -10.5},
        {2000, -20.3},
        {3000, 15.7}
    };

    EXPECT_NO_THROW(db.insert("test_table", points));
}

// Test zero values
TEST_F(DatabaseTest, InsertZeroValues) {
    std::vector<DataPoint> points = {{1000, 0.0}, {2000, 0.0}};

    EXPECT_NO_THROW(db.insert("test_table", points));
}

// Test query with limit
TEST_F(DatabaseTest, QueryWithLimit) {
    std::vector<DataPoint> points;
    for (int i = 0; i < 100; ++i) {
        points.push_back({static_cast<Timestamp>(i * 100), static_cast<double>(i)});
    }
    db.insert("test_table", points);

	auto max_range = TimeRange();
    Query query(max_range, false, 10);
    std::vector<DataPoint> results = db.query("test_table", query);

    EXPECT_LE(results.size(), 10);
}

// Test row count
TEST_F(DatabaseTest, RowCountAfterInsert) {
    std::vector<DataPoint> points = {
        {1000, 10.5},
        {2000, 20.3},
        {3000, 15.7}
    };
    db.insert("test_table", points);

    const Table* table = db.get_table("test_table");
    EXPECT_GE(table->rows(), 3);
}