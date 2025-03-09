# TimeSeries-DB

A high-performance time series database optimised for fast insertion and querying of time-ordered data. Built following the TimescaleDB architecture

## Features

- **Fast Performance**: Optimised for rapid time series data operations
- **Time-Based Chunking**: Efficient storage organisation for time-ordered data access patterns
- **B+Tree Indexing**: Balanced tree structure for optimised query performance
- **Multi-threaded**: Asynchronus I/O operations for reduced query latency
- **C++20 Implementation**: Modern C++ capabilities for maximum efficiency

### Building from Source

```bash
# Clone the repository
git clone https://github.com/yourusername/timeseries-db.git
cd timeseries-db

# Configure the build
cmake -S . -B build/

# Build the project using multiple threads for faster compilation
cmake --build build -j$(nproc)
```

### Running

Currently, the project provides benchmarking functionality in `main.cpp` to evaluate insertion and query performance.

```bash
# Execute the benchmark
./build/timeseries_db
```

## Performance

Running on my local machine throughput is around 1.5million rows/s. These are numbers are very preliminary, feel free to benchmark yourself.

![alt text](image.png)

## Roadmap

- [ ] Historical data insertion API
- [ ] Comprehensive test suite
- [ ] Command-line interface
- [ ] Multi-column/multi-sensor data point support
- [ ] Data compression
