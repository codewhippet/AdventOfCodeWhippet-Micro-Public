import csv
import json
import sys

HEADER = [
    "Year",
    "Day",
    "Part",
    "PC (ms)",
    "RP2040 (ms)",
    "RP2350 (ms)",
    "RP2350 @ 300MHz (ms)",
]

def fmt_time(value):
    if value is None:
        return ""
    return f"{value:.3f}".rstrip("0").rstrip(".")

def rows_for_year(year, days):
    for day in sorted(days, key=int):
        parts = days[day]
        for part in sorted(parts, key=int):
            t = parts[part]
            yield [
                year,
                day,
                part,
                fmt_time(t.get("PC")),
                fmt_time(t.get("RP2040")),
                fmt_time(t.get("RP2350")),
                fmt_time(t.get("RP2350_300MHz")),
            ]

with open("timings.json", encoding="utf-8") as f:
    data = json.load(f)

with open("timings.csv", "w", newline="", encoding="utf-8") as f:
    writer = csv.writer(f)
    for i, year in enumerate(sorted(data, key=int)):
        if i > 0:
            writer.writerow([])  # blank line separating each year's table
        writer.writerow(HEADER)
        for row in rows_for_year(year, data[year]):
            writer.writerow(row)
