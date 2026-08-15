import json
import os

TIMING_DEVICES = ("PC", "RP2040", "RP2350", "RP2350_300MHz", "IO")

class Timings:
    def __init__(self):
        self.path = "timings.json"
        self.data = {}
        if os.path.exists(self.path):
            with open(self.path, "r") as file:
                try:
                    self.data = json.load(file)
                except json.JSONDecodeError:
                    self.data = {}

    def Update(self, puzzle, device, time_ms):
        if device not in TIMING_DEVICES:
            raise ValueError(f"Unknown timing device '{device}'")
        year = self.data.setdefault(puzzle.year, {})
        day = year.setdefault(puzzle.puzzle, {})
        target_parts = ["1", "2"] if puzzle.part == "3" else [puzzle.part]
        for target in target_parts:
            part = day.setdefault(target, {})
            for known in TIMING_DEVICES:
                part.setdefault(known, None)
            part[device] = round(time_ms, 3)

    def Save(self):
        with open(self.path, "w") as file:
            json.dump(self.data, file, indent=2)
            file.write("\n")
