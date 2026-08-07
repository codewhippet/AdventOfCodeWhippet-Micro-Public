import argparse
from puzzles import puzzle_table

class PuzzleInput:
    def __init__(self, year, puzzle, part, filename):
        self.year = year
        self.puzzle = puzzle
        self.part = part
        self.header = f"PUZZLE {year} {puzzle} {part}\n"
        self.filename = filename

class InputBuilder:
    def __init__(self, directory):
        self.directory = directory
        pass

    def parse_args(self):
        parser = argparse.ArgumentParser(prog="runpc", description="Run AoC solutions")
        parser.add_argument("-y", "--years", type=str, default="")
        parser.add_argument("-p", "--puzzles", type=str, default="")
        parser.add_argument("-c", "--com", type=str, default="")
        self.args = parser.parse_args()
        
    def build_input(self):
        self.puzzles = []
        selected_years = [y for y in self.args.years.split(";") if len(y) > 0]
        selected_puzzles = [p for p in self.args.puzzles.split(";") if len(p) > 0]
        for year, puzzles in puzzle_table.items():
            if len(selected_years) == 0 or year in selected_years:
                for puzzle in puzzles["puzzles"].split(";"):
                    if len(selected_puzzles) == 0 or puzzle in selected_puzzles:
                        self.puzzles.append(PuzzleInput(year, puzzle, "1", self.directory + f"AoCInput\\{year}\\Puzzle{puzzle.rjust(2,'0')}.txt"))
                        if puzzle != puzzles["last_day"]:
                            self.puzzles.append(PuzzleInput(year, puzzle, "2", self.directory + f"AoCInput\\{year}\\Puzzle{puzzle.rjust(2,'0')}.txt"))
