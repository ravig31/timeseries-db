# TimeSeries-DB

A high-performance time series database optimised for fast insertion and querying of time-ordered data. Built following the TimescaleDB architecture

## Features

- **Fast Performance**: Optimised for rapid time series data operations
- **Time-Based Chunking**: Efficient storage organisation for time-ordered data access patterns
- **B+Tree Indexing**: Balanced tree structure for optimised query performance
- **Multi-threaded**: Asynchronus I/O operations for reduced query latency
- **CLI**: Command line interface for interacting with database

### Building from Source

```bash
# Clone the repository
git clone https://github.com/yourusername/timeseries-db.git
cd timeseries-db

# Build & compile
cmake -S . -B build/
cmake --build build 
```

### Running

Run command line interface.

```bash
# Execute the benchmark
./build/src/tsdb
```

## Load Performance

Ran on M1 Pro 16GB, troughput of around 1.5million rows/s with deliberate worst case cache conditions. Benchmark was to mainly profile Btree query performance and concurrent I/O of persisted database files.

```bash
-> % /usr/bin/time -l ./build/assets/benchmarking/benchmark
Inserting data...
Data loaded from assets/sample.csv
Insert Time: 5713.95 ms

Running query benchmark...
Ran query: 1/25, 618526 rows in: 1151.96 ms
Ran query: 2/25, 9320 rows in: 10.798 ms
Ran query: 3/25, 1318469 rows in: 1534.41 ms
Ran query: 4/25, 2025861 rows in: 1902.07 ms
Ran query: 5/25, 692608 rows in: 486.44 ms
Ran query: 6/25, 2697923 rows in: 2096.62 ms
Ran query: 7/25, 252733 rows in: 191.202 ms
Ran query: 8/25, 733233 rows in: 381.829 ms
Ran query: 9/25, 2057489 rows in: 1324.26 ms
Ran query: 10/25, 3323535 rows in: 2253.88 ms
Ran query: 11/25, 488776 rows in: 308.156 ms
Ran query: 12/25, 4097314 rows in: 2580.91 ms
Ran query: 13/25, 2512840 rows in: 1450.5 ms
Ran query: 14/25, 3560087 rows in: 2048.75 ms
Ran query: 15/25, 3102197 rows in: 1726.86 ms
Ran query: 16/25, 3379060 rows in: 2098.58 ms
Ran query: 17/25, 4827345 rows in: 3230.12 ms
Ran query: 18/25, 4463012 rows in: 2584.81 ms
Ran query: 19/25, 2457577 rows in: 1353.14 ms
Ran query: 20/25, 4763561 rows in: 2635.36 ms
Ran query: 21/25, 2326919 rows in: 1285.3 ms
Ran query: 22/25, 4469671 rows in: 2418.01 ms
Ran query: 23/25, 590779 rows in: 323.285 ms
Ran query: 24/25, 911409 rows in: 503.148 ms
Ran query: 25/25, 187554 rows in: 100.642 ms
Queries Ran: 25
Average Query Time: 1439.24 ms
Rows Queried: 55867798
Average Rows/s: 1.5527e+06
Cache Miss %: 99.2507
```

Under these worst conditions peak memory usage was around 1.29GB for this particular dataset, tested using `/usr/bin/time -l` on Mac.

## Roadmap

- [ ] Comprehensive test suite
- [ ] Command-line interface
- [ ] Backgrounf thread to flush asynchronously
- [ ] Multi-column/multi-sensor data point support
- [ ] Data compression
