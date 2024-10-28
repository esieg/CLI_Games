# CLI_Games
This is my place to bundle my various games for the CLI. In the respective folders there is another ReadMe for the individual games. The template folder contains a template for new games, which contains the recurring functions as well as the timeschedule and the basic functions Init, Play and endGame. 

## Compile
To compile a game, we use g++ and link the ncurses-Header
> g++ -std=c++17 -o Game Game.cpp -lncurses

## Styleguide
a little Styleguide to get a constant naming
* **CLASS**: full capitalized
* **functionsAndMethods**: camelCase
* **variables_and_properties**: snake_case
* **CONSTANTS**: full capitalized