#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <ncurses.h>

class GAME{
private:
    // define the static variables
    static const int RUNNING = 1;
    static const int STOPPED = 0;
    static const int SIZE = 40;     // set the size of the gamefield
    static const int FPS = 60;      // set the Frames drawed per Second
    static const int PPS = 120;     // set the Physics calculation per calculation

    // define the front colors
    static const int BLACK = 30;
    static const int RED = 31;
    static const int GREEN = 32;
    static const int YELLOW = 33;
    static const int BLUE = 34;
    static const int MAGENTA = 35;
    static const int CYAN = 36;
    static const int WHITE = 37;

    // define the game variables
    int status = RUNNING;
    int test = 10;  // only for testing stuff
    std::mutex mtx;

    // define the common functions
    void clearScreen() {
        std::cout << "\033[2J\033[H"; // Clear Screen and position the Cursor
        std::cout << "\033[?25l"; // Hide the Cursor
    }

    void setCursor(int row, int col) {
        /* set cursor to a specific position */
        std::cout << "\033[" << row << ";" << col << "H";
    }

    void setFrontColor(int color) {
        /* set the front color to a specific value */
        std::cout << "\x1b[" << color << "m";
    }

    void setDefaultFrontColor() {
        /* set the front color to the default value */
        std::cout << "\x1b[0m";
    }

    void askReplay() {
        /* ask for replay */
        initscr();              // start ncurses-Mode
        cbreak();               // activate raw-mode for ncurses 

        char user_in;
        std::cout << "\033[" << SIZE+4<< ";" << 0 << "H";
        std::cout << "Neues Spiel? (j): ";
        std::cin >> user_in;
        replay = (user_in == 'j' || user_in == 'J');

        endwin();
    }

    void drawGameEnd() {
        /* draw the endscreen of the game */

    }

    // threaded functions in the game
    void playerCtrl() {
        /* get the inputs from the Players */ 
        while (status == RUNNING) {
            int input = getch(); // non-blocking variant
 
            // Use a mutex to ensure thread-safe access to direction
            std::lock_guard<std::mutex> lock(mtx);
            switch (input) {
                case 'w':
                    test += 1;
                    break;
                case 's':
                    test -= 1;
                    break;
                case KEY_UP:
                    test += 1;
                    break;
                case KEY_DOWN:
                    test -= 1;
                    break;
            }
        }
    }

    void physics() {
        /* do the calculations */
        auto p_last_calc = std::chrono::high_resolution_clock::now();
        while (status == RUNNING) {
            // set PPS
            auto p_calc_duration = std::chrono::milliseconds(1000 / PPS);
            auto p_now = std::chrono::high_resolution_clock::now();
            auto p_elapsed = p_now - p_last_calc;
            if(p_elapsed < p_calc_duration) {
                std::this_thread::sleep_for(p_calc_duration - p_elapsed);
            }
            p_last_calc = std::chrono::high_resolution_clock::now();

            // do the calculation stuff
            test += 1;
            if(test >= 300) {
                status = STOPPED;
            }
        }
    }

    void drawings() {
        /* draw the outputs */
        auto d_last_calc = std::chrono::high_resolution_clock::now();
        while (status == RUNNING) {
            auto d_calc_duration = std::chrono::milliseconds(1000 / FPS);
            auto d_now = std::chrono::high_resolution_clock::now();
            auto d_elapsed = d_now - d_last_calc;
            if(d_elapsed < d_calc_duration) {
                std::this_thread::sleep_for(d_calc_duration - d_elapsed);
            }
            d_last_calc = std::chrono::high_resolution_clock::now();

            //do the drawing stuff
            clearScreen();
            setCursor(5, 5);
            setFrontColor(CYAN);
            std::cout << "Current test: " << test << std::endl;
            setDefaultFrontColor();
            std::cout << std::flush; // Needed for a stable drawing
        }
    }

    // the 3 Game-Status
    void init() {
        /* Do the stuff to init the game, then call Play() */
        // init ncurses for player input
        initscr();              // start ncurses-Mode
        timeout(0);             // no Timeout for non-blocking
        keypad(stdscr, TRUE);   // activate arrow-keys
        cbreak();               // activate raw-mode for ncurses
        noecho();               // disable output from keyboard entry
        curs_set(0);            // disable Cursor

        //start the game loop
        play();
    }

    void play() {
        /* loop the game */
        // first: create the needed threads
        std::thread player(&GAME::playerCtrl, this);
        std::thread physics(&GAME::physics, this);
        std::thread draw(&GAME::drawings, this);

        // check if termination condition is reached
        while(status == RUNNING) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // last, let join the threads again
        player.join();
        physics.join();
        draw.join();
    }

    void endGame(GAME &game) {
        endwin(); // end ncurses
        drawGameEnd();
        askReplay();
    }

public:
    bool replay = false; 

    // Konstruktor
    GAME() {
        init();
    }
};

// TESTAREA
int main() {
    bool replay = false;

    do {
        GAME game;
        replay = game.replay;
    } while (replay);
    return 0;
}