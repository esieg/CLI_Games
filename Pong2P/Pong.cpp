#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <ncurses.h>
#include <vector>
#include <random>
#include <cmath>
#include <string>

class PONG{
private:
    // define the static variables
    static const int RUNNING = 1;
    static const int STOPPED = 0;
    static const int SIZE = 40;    // set the size of the gamefield
    static const int FPS = 60;     // set the Frames drawed per Second
    int PPS = 4;                  // set the Physics calculation per calculation

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
    static const int PADDLESIZE = 5;
    static const char BALL = 'O';
    static const char PADDLE = '#';
    static const int ENDSCORE = 5;
    std::mutex mtx;

    int paddle1 = 10; // Uppermost "Pixel" of the Paddle
    int paddle2 = 10; 
    std::vector<int> ball = {20, 20};
    std::vector<double> speed = {0, 1}; //Initial Speed 
    std::vector<int> score = {0, 0};
    std::vector<double> bounce = {-1, -0.5, 0, 0.5, 1};
    double vspeed_leftover = 0;
    int winner = 0; // winner variable 

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

    void setStartDirection() {
        /* set start direction of the ball */
        std::mt19937 generator(static_cast<unsigned int>(std::time(0)));
        std::uniform_int_distribution<int> distribution(0, 1);

        double direction = (distribution(generator)) ? -1 : 1; 
        speed = {0 , direction};
    }

    void startNewRound() {
        ball = {20, 20};
        setStartDirection();
        PPS = 4;
        vspeed_leftover = 0;
    }

   void checkBallCollision() {
        /* check if the Ball hits something */
        // First: handle Cornercases
        if(ball[0] <= 1 && ball[1] <= 1 && paddle1 == 2) {
            ball[1] = 2;
            PPS++;
            speed[0] = bounce[0];
            speed[1] *= -1;
        } else if ((ball[0] <= 1 && ball[1] <= SIZE && paddle1 == SIZE - PADDLESIZE)) {
            ball[1] = 2;
            PPS++;
            speed[0] = bounce[4];
            speed[1] *= -1;
        } else if ((ball[0] <= SIZE && ball[1] <= 1 && paddle2 == 2)) {
            ball[1] = SIZE - 1;
            PPS++;
            speed[0] = bounce[0];
            speed[1] *= -1;
        } else if ((ball[0] <= SIZE && ball[1] <= SIZE && paddle2 == SIZE - PADDLESIZE)) {
            ball[1] = SIZE - 1;
            PPS++;
            speed[0] = bounce[4];
            speed[1] *= -1;
        }
        // Second: Check if a Wall is hit
        else if(ball[0] <= 1 || ball[0] >= SIZE) {
            speed[0] *= -1;
            PPS++;
            vspeed_leftover = 0; //to avoid always 0 when -0.5 + 0.5
            ball[0] = (ball[0] < 2) ? 2 : SIZE-1;
        } else if(ball[1] <= 1) { // 3rd: Check if left Paddle is hit
            for (int i = 0; i < PADDLESIZE; i++) {
                if(ball[0] == paddle1 + i) {
                    ball[1] = 2;
                    speed[0] = bounce[i]; // bounce, depending where the ball hit the paddle
                    speed[1] *= -1;
                    PPS++;
                    break;
                } else if(i == PADDLESIZE - 1) {
                    score[1] += 1;
                    if(score[1] == ENDSCORE) {
                        winner = 2;
                        status = STOPPED;
                    } else {
                        startNewRound();
                    }
                }
            }
        } else if (ball[1] >= SIZE) { // 4th: Check if right Paddle is hit
            for (int i = 0; i < PADDLESIZE; i++) {
                if(ball[0] == paddle2 + i) {
                    ball[1] = SIZE-2;
                    speed[0] = bounce[i]; // bounce, depending where the ball hit the paddle
                    speed[1] *= -1;
                    PPS++;
                    break;
                } else if(i == PADDLESIZE - 1) {
                    score[0] += 1;
                    if(score[0] == ENDSCORE) {
                        winner = 1;
                        status = STOPPED;
                    } else {
                        startNewRound();
                    }
                }
            }
        }
    }

    void drawWalls() {
        setDefaultFrontColor();
        for (int i = 1; i <= SIZE; ++i) {
            std::cout << "\033[1;" << i << "H─"; // Wall
            std::cout << "\033[" << SIZE << ";" << i << "H─"; // Wall
        }
    }

    void drawBall() {
        setFrontColor(RED);
        setCursor(ball[0], ball[1]);
        std::cout << "\033[" << ball[0] << ";" << ball[1] << "H"; // set cursor to ball position
        std::cout << BALL;
        setDefaultFrontColor();
    }

    void drawPaddles() {
        setFrontColor(RED);
        for (int i = 0; i < PADDLESIZE; i++) {
            setCursor(paddle1 + i, 1);// Paddle1 is always in COL 1
            std::cout << PADDLE;
            setCursor(paddle2 + i, SIZE);// Paddle1 is always in COL 1
            std::cout << PADDLE;
        }

        setDefaultFrontColor();
    }

    void drawScore() {
        /* draw the Score under the Playfield */
        setFrontColor(RED);
        setCursor(SIZE + 2, 15);
        std::cout << score[0]; // Score Player 1
        setCursor(SIZE + 2, 20);
        std::cout << ":"; // ":"
        setCursor(SIZE + 2, 25);
        std::cout << score[1]; // Score Player 2
        setDefaultFrontColor();
    }

    void drawWinner() {
        /* draw the Winner under the Score */
        setFrontColor(RED);
        setCursor(SIZE + 3, 13);
        std::cout << "WINNER PLAYER: " << winner << std::endl;
        setDefaultFrontColor();
    }

    void drawGameEnd() {
        /* draw the endscreen of the game */
        clearScreen();
        drawPaddles();
        drawWalls();
        drawScore();
        drawWinner();
    }

    void moveBall() {
        /* move the Ball to his new Position */
        // use this to handle 0.5-Values
        double vspeed = speed[0];
        if(speed[0] == 0.5) {
            vspeed = std::floor(speed[0] + vspeed_leftover);
            vspeed_leftover = speed[0] + vspeed_leftover - vspeed;
        }
        else if (speed[0] == -0.5)  {
            vspeed = std::ceil(speed[0] + vspeed_leftover);
            vspeed_leftover = speed[0] + vspeed_leftover - vspeed;
        }

        // actually move the ball
        ball[0] += vspeed;
        ball[1] += speed[1];
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
                    paddle1 -= 1;
                    paddle1 = (paddle1 < 2) ? 2 : paddle1;  
                    break;
                case 's':
                    paddle1 += 1;
                    paddle1 = (paddle1 >35) ? 35 : paddle1;
                    break;
                case KEY_UP:
                    paddle2 -= 1;
                    paddle2 = (paddle2 < 2) ? 2 : paddle2;  
                    break;
                case KEY_DOWN:
                    paddle2 += 1;
                    paddle2 = (paddle2 >35) ? 35 : paddle2;
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
            moveBall();
            checkBallCollision();
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
            drawPaddles();
            drawBall();
            drawWalls();
            drawScore();
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
        std::thread player(&PONG::playerCtrl, this);
        std::thread physics(&PONG::physics, this);
        std::thread draw(&PONG::drawings, this);

        // check if termination condition is reached
        while(status == RUNNING) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // last, let join the threads again
        player.join();
        physics.join();
        draw.join();
    }

    void endGame(PONG &game) {
        endwin(); // end ncurses
        drawGameEnd();
        askReplay();
    }

public:
    bool replay = false; 

    // Konstruktor
    PONG() {
        init();
    }
};

int main() {
    bool replay = false;

    do {
        PONG pong;
        replay = pong.replay;
    } while (replay);
    return 0;
}