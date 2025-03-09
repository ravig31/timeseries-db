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

## Performance

Running on my local machine throughput is around 1.5million rows/s. These are numbers are very preliminary, feel free to benchmark yourself.

```bash
-> % ./build/assets/benchmarking/benchmark
Inserting data...
Data loaded from assets/sample.csv
Insert Time: 5621.38 ms

Running query benchmark...
Ran query: 1/25, 618526 rows in: 1149.42 ms
Ran query: 2/25, 9320 rows in: 9.664 ms
Ran query: 3/25, 1331045 rows in: 1675.9 ms
Ran query: 4/25, 2038197 rows in: 1944.76 ms
Ran query: 5/25, 692608 rows in: 486.71 ms
Ran query: 6/25, 2710259 rows in: 2227.42 ms
Ran query: 7/25, 252805 rows in: 161.642 ms
Ran query: 8/25, 753333 rows in: 456.928 ms
Ran query: 9/25, 2091473 rows in: 1290.37 ms
Ran query: 10/25, 3348699 rows in: 2233.84 ms
Ran query: 11/25, 496168 rows in: 312.916 ms
Ran query: 12/25, 4067524 rows in: 2630.94 ms
Ran query: 13/25, 2532220 rows in: 1437.99 ms
Ran query: 14/25, 3591635 rows in: 2387.62 ms
Ran query: 15/25, 3118493 rows in: 1739.26 ms
Ran query: 16/25, 3396448 rows in: 1844.86 ms
Ran query: 17/25, 4789857 rows in: 3289.41 ms
Ran query: 18/25, 4429220 rows in: 2503.99 ms
Ran query: 19/25, 2457577 rows in: 1340.39 ms
Ran query: 20/25, 4730993 rows in: 2678.91 ms
Ran query: 21/25, 2329307 rows in: 1248.86 ms
Ran query: 22/25, 4440796 rows in: 2421.33 ms
Ran query: 23/25, 590779 rows in: 321.057 ms
Ran query: 24/25, 911409 rows in: 505.074 ms
Ran query: 25/25, 173297 rows in: 92.89 ms
Queries Ran: 25
Average Query Time: 1455.69 ms
Rows Queried: 55901988
Average Rows/s: 1.5361e+06
```

## Roadmap

- [ ] Comprehensive test suite
- [ ] Command-line interface
- [ ] Multi-column/multi-sensor data point support
- [ ] Data compression
