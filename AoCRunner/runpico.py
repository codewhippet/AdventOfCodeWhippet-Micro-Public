import serial
import io
import time
from inputbuilder import *
from timings import *

puzzleInput = InputBuilder(".\\")
puzzleInput.parse_args()
puzzleInput.build_input()

timings = None
if puzzleInput.args.timing:
    timings = Timings()

connection = serial.Serial(port=puzzleInput.args.com, baudrate=115200, timeout=10)

def ExecutePuzzle(puzzle, input_contents):
    connection.write(puzzle.header.encode("ascii"))
    connection.write(f"{len(input_contents)}\n".encode("ascii"))
    connection.flush()

    start = time.time();
    connection.write(input_contents.encode("ascii"))
    connection.write(b"\x17")
    connection.flush()

    line = bytearray()
    while True:
        b = connection.read(1)
        if b == b"\x17":
            break
        line += b
    stop = time.time();

    return (line, (stop - start) * 1000)

for puzzle in puzzleInput.puzzles:
    with open(puzzle.filename, "r") as file:
        input_contents = file.read()
        line, best_time = ExecutePuzzle(puzzle, input_contents)
        decoded_line = line.decode("ascii")
        print("[" + f"{best_time:0.3f}".rjust(10) + "]" + decoded_line, end="")
        if timings is not None and "[UNSUPPORTED]" not in decoded_line:
            timings.Update(puzzle, puzzleInput.args.timing, best_time)

if timings is not None:
    timings.Save()

connection.close()
print("Done")
