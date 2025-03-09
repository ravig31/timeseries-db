import csv
import random
from typing import List, NamedTuple
import sys

class DataPoint(NamedTuple):
    ts: int  
    value: float

def save_data_to_csv(data: List[DataPoint], filename: str) -> None:
    try:
        with open(filename, 'w', newline='') as outfile:
            writer = csv.writer(outfile)
            writer.writerow(["Timestamp", "Value"])  # Header

            for point in data:
                writer.writerow([point.ts, point.value])

        print(f"Data saved to {filename}")
    except OSError as e:
        print(f"Error: Could not open file {filename} for writing: {e}", file=sys.stderr)

def generate_test_data(num_points: int, start_time: int, interval: int) -> List[DataPoint]:
    data: List[DataPoint] = []

    for i in range(num_points):
        ts = start_time + i * interval
        value = random.uniform(-1.0, 1.0)
        data.append(DataPoint(ts, value))
    return data

if __name__ == '__main__':
    num_points = 100
    start_time = 1678886400  
    interval = 60  

    test_data = generate_test_data(num_points, start_time, interval)
    save_data_to_csv(test_data, "assets/test_data.csv")

    # print a few points as a sanity check
    print("Sample of Generated Data:")
    for i in range(min(5,len(test_data))):
        print(test_data[i])