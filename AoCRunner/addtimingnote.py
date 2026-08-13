import argparse
import json
import os
import sys

path = "timings.json"

parser = argparse.ArgumentParser()
parser.add_argument("year")
parser.add_argument("day")
parser.add_argument("part")
parser.add_argument("note")
args = parser.parse_args()


year, day, part = str(args.year), str(args.day), str(args.part)


data = None
with open(path, "r") as file:
    data = json.load(file)

if year not in data or day not in data[year] or part not in data[year][day]:
    sys.exit(f"No timing entry for {year} day {day} part {part}")


target = data[year][day][part]

if args.note == "":
    removed = target.pop("note", None)
    print(f"[{year}][{day}][{part}] Note removed")
else:
    target["note"] = args.note
    print(f"[{year}][{day}][{part}]: Set note '{args.note}'")


with open(path, "w") as file:
    json.dump(data, file, indent=2)
    file.write("\n")
