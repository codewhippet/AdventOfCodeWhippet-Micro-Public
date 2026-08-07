import serial
import io
import time
from inputbuilder import *

puzzleInput = InputBuilder(".\\")
puzzleInput.parse_args()
puzzleInput.build_input()

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
        #best_time = min([ExecutePuzzle(puzzle, input_contents)[1] for _ in range(5)])
        #best_time = min([ExecutePuzzle(puzzle, input_contents)[1] for _ in range(1)])
        #best_time = 0.0
        print("[" + f"{best_time:0.3f}".rjust(10) + "]"+ line.decode("ascii"), end="")
        #print("[" + f"{best_time:0.3f}".rjust(10) + "] " + puzzle.filename)

connection.close()
print("Done")
