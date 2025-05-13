import board
from digitalio import DigitalInOut, Direction
from analogio import AnalogIn
from time import sleep

# setup pins
in1 = AnalogIn(board.IO36)
in2 = AnalogIn(board.IO48)
EnA = AnalogIn(board.IO35)

in3 = AnalogIn(board.IO34)
in4 = AnalogIn(board.IO47)
EnB = AnalogIn(board.IO33)
