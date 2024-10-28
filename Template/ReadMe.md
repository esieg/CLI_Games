# Game Template
After all that I have learned, I would like to bundle the collected knowledge into a template for classes.
This way, my previous games can be standardized and improved in a remade and new games can be created faster and more consistently. 

## Compile
I use simple g++ for compiling and add the ncurses-Header
> g++ -std=c++17 -o Game Game.cpp -lncurses

## Steps
(/) Init-Function called automatically
(/) Multithread in Play()
  (/) Physics
  (/) Drawings
  (/) Controll_Player_1
  (/) Controll_Player_2
(/) "solve" common cases
  (/) clear screen
  (/) set Cursorposition
  (/) set Frontcolor
  (/) replay-Question