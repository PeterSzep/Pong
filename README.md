This project was created as part of a school assignment.
🏓 Pong Game in C (ncurses)

A simple terminal-based Pong game written in C using the ncurses library.
The game runs directly in the terminal, featuring two-player controls, dynamic ball movement, and score tracking.

🎮 Features

Two-player paddle control (left and right)

Ball movement with realistic bouncing

Speed increase after each paddle hit

Score tracking for both players

Simple and smooth ncurses-based rendering

Persistent high scores stored in a text file

🧠 Controls
Player	Move Up	Move Down
Left Paddle	W	S
Right Paddle	↑	↓

Press Q to quit the game.

⚙️ Requirements

Before compiling, make sure you have the ncurses library installed.

On Linux (Ubuntu/Debian)

sudo apt update
sudo apt install libncurses5-dev libncursesw5-dev

On macOS (using Homebrew)

brew install ncurses

🛠️ Compilation

To compile the game, run:

gcc -std=c11 -Wall -Werror pong.c -lncurses -o pong

Then start the game with:

./pong

👨‍💻 Author

Created by Peter Szepesi
📅 Year: 2024
🧱 Language: C 
