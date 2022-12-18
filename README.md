# Waffle Game Solver

[Waffle Game](https://wafflegame.net) is a variation of Wordle where there is a waffle-shaped grid of words. The squares are colored green, yellow, or gray following similar rules to the original Wordle game. You are allowed to make operations where you swap two squares, and your goal is to arrange all the squares in the correct way in the minimum number of swaps.

# Usage

To compile, run `make`, which creates an executable `waffle`

To use this program, create a text file with a similar format to `res/day1.txt` and run the program with the puzzle as standard input. For example, `./waffle < ./res/day1.txt` will solve the example puzzle.
