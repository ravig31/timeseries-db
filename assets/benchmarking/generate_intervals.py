import random
import csv

def generate_intervals(num_points, min_ts, max_ts):
    """Generates random timestamp intervals."""
    if num_points == 0 or min_ts > max_ts:
        return []

    intervals = []
    for _ in range(num_points):
        start_ts = random.randint(min_ts, max_ts)
        end_ts = random.randint(min_ts, max_ts)
        if start_ts > end_ts:
            start_ts, end_ts = end_ts, start_ts  # Swap to ensure start <= end
        intervals.append((start_ts, end_ts))

    intervals.sort()  
    return intervals

def save_intervals_to_csv(intervals, filename):
    """Saves intervals to a CSV file."""
    with open(filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(['start_ts', 'end_ts'])  
        writer.writerows(intervals)

if __name__ == "__main__":
    num_intervals = 25
    min_timestamp = 1740618000
    max_timestamp = 1740618000 + (300*1000000)
    output_filename = 'assets/intervals.csv'

    intervals = generate_intervals(num_intervals, min_timestamp, max_timestamp)
    save_intervals_to_csv(intervals, output_filename)
    print(f"Intervals saved to {output_filename}")