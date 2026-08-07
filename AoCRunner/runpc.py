import subprocess
import io
import time
from inputbuilder import *

puzzleInput = InputBuilder(".\\")
puzzleInput.parse_args()
puzzleInput.build_input()

proc = subprocess.Popen(
    [".\\x64\\Release\\AdventOfCodeWhippet.exe", "--client"],
    stdin=subprocess.PIPE,
    stdout=subprocess.PIPE,
    bufsize=0
)

def ExecutePuzzle(puzzle, input_contents):
    proc.stdin.write(puzzle.header.encode("ascii"))
    proc.stdin.write(f"{len(input_contents)}\n".encode("ascii"))
    proc.stdin.flush()

    start = time.time();
    proc.stdin.write(input_contents.encode("ascii"))
    proc.stdin.write(b"\x17")
    proc.stdin.flush()

    line = bytearray()
    while True:
        b = proc.stdout.read(1)
        if b == b"\x17":
            break
        line += b
    stop = time.time();

    return (line, (stop - start) * 1000)

for puzzle in puzzleInput.puzzles:
    with open(puzzle.filename, "r") as file:
        input_contents = file.read()
        line = ExecutePuzzle(puzzle, input_contents)[0]
        best_time = min([ExecutePuzzle(puzzle, input_contents)[1] for _ in range(5)])
        print("[" + f"{best_time:0.3f}".rjust(7) + "]"+ line.decode("ascii"), end="")

proc.kill()
print("Done")
