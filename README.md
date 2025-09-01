
A time series database optimised for fast insertion and querying of time-ordered data. Built following the TimescaleDB architecture as a little hobby project.

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

```bash
# Run command line interface.
./build/src/tsdb
```

