#include <stdlib.h>
#include <curses.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>

#define DELAY 55000
#define PADDLE_HEIGHT 3
#define WIN_WIDTH 40
#define WIN_HEIGHT 20

// Represents a game object with x and y coordinates (ball and padles)
typedef struct {
    int x, y;
} Object;

// Stores the scores of both players (left and right)
typedef struct {
    int left, right;
} Scores;

//Function declarations
void init_game();
void draw_paddle(int x, int y);
void draw_ball(int x, int y);
void move_paddle(int *y, int direction);
int random_coordinate();
void draw_border();
void print_scores(int left_score, int left_high_score, int right_score, int right_high_score);
void display_menu();
void display_high_score();
void reset_positions(Object *ball, Object *left_paddle, Object *right_paddle);
void load_high_scores(Scores *high);
void save_high_scores(Scores high);
void update_high_scores(Scores *high, Scores current);
void game_loop(Scores *high);

//Main function
int main() {
    display_menu();
    char choice;

    //Game loop
    do {
        Scores high;
        load_high_scores(&high);
        game_loop(&high);

        mvprintw(WIN_HEIGHT / 2 - 4, WIN_WIDTH / 2 - 10, "Wanna play again? (y/n): ");
        refresh();
       
        //Choice if the players want to continue
        do {
            choice = getch();
        } while (choice != 'y' && choice != 'Y' && choice != 'n' && choice != 'N');

    } while (choice == 'y' || choice == 'Y');

    endwin();
    return 0;
}

//Game initialization
void init_game() {
    initscr();
    noecho();
    cbreak();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    start_color();
}

//Loads the highscore from file score.txt
void load_high_scores(Scores *high) {
    FILE *fw = fopen("score.txt", "r+");
    if (fw == NULL) {
        fw = fopen("score.txt", "w+");
        fprintf(fw, "0 0");
        fflush(fw);
        fseek(fw, 0, SEEK_SET);
    }
    fscanf(fw, "%d %d", &high->left, &high->right);
    fclose(fw);
}

//Saves a highscore
void save_high_scores(Scores high) {
    FILE *fw = fopen("score.txt", "w+");
    fprintf(fw, "%d %d", high.left, high.right);
    fclose(fw);
}

//Resets position of ball and players
void reset_positions(Object *ball, Object *left_paddle, Object *right_paddle) {
    ball->x = WIN_WIDTH / 2;
    ball->y = WIN_HEIGHT / 2;

    left_paddle->x = 1;
    left_paddle->y = WIN_HEIGHT / 2 - PADDLE_HEIGHT / 2;

    right_paddle->x = WIN_WIDTH - 2;
    right_paddle->y = WIN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
}

//Function for the game loop
void game_loop(Scores *high) {
    init_game();

    Object ball;
    Object left_paddle;
    Object right_paddle;

    reset_positions(&ball, &left_paddle, &right_paddle);

    //Setting the coordinate to ball for which direction it goes at the start of the game
    int ball_dx = random_coordinate();
    int ball_dy = random_coordinate();

    Scores current;
    current.left = 0;
    current.right = 0;

    int speed = DELAY;

    while (true) {
        clear();
        draw_border();
        draw_paddle(left_paddle.x, left_paddle.y);
        draw_paddle(right_paddle.x, right_paddle.y);
        draw_ball(ball.x, ball.y);
        print_scores(current.left, high->left, current.right, high->right);
        refresh();

        //Update the ball position
        ball.x = ball.x + ball_dx;
        ball.y = ball.y + ball_dy;

        if (ball.y <= 0 || ball.y >= WIN_HEIGHT - 1) {
            ball_dy = ball_dy * -1;
        }

        // Check collision with the right paddle
        if (ball.x == right_paddle.x - 1 && ball.y >= right_paddle.y && ball.y < right_paddle.y + PADDLE_HEIGHT) {
            ball_dx = -1;
            current.right = current.right + 1;
            speed = speed - 5000;
            if (speed < 10000) {
                speed = 10000;
            }
        }

        // Check collision with the left paddle
        if (ball.x == left_paddle.x + 1 && ball.y >= left_paddle.y && ball.y < left_paddle.y + PADDLE_HEIGHT) {
            ball_dx = 1;
            current.left = current.left + 1;
            speed = speed - 5000;
            if (speed < 10000) {
                speed = 10000;
            }
        }

        if (ball.x <= 0) {
            break;
        }

        if (ball.x >= WIN_WIDTH - 1) {
            break;
        }

        int key = getch();

        //Movement for both players
        switch (key) {
            case 'w': {
                move_paddle(&left_paddle.y, -1);
                break;
            }
            case 's': {
                move_paddle(&left_paddle.y, 1);
                break;
            }
            case KEY_UP: {
                move_paddle(&right_paddle.y, -1);
                break;
            }
            case KEY_DOWN: {
                move_paddle(&right_paddle.y, 1);
                break;
            }
            case 'p': {
                nodelay(stdscr, FALSE);
                mvprintw(WIN_HEIGHT / 2, WIN_WIDTH / 2 - 5, "GAME PAUSED");
                refresh();
                getch();
                nodelay(stdscr, TRUE);
                break;
            }
            default: {
                break;
            }
        }

        if (left_paddle.y < 0) {
            left_paddle.y = 0;
        }

        if (left_paddle.y > WIN_HEIGHT - PADDLE_HEIGHT) {
            left_paddle.y = WIN_HEIGHT - PADDLE_HEIGHT;
        }

        if (right_paddle.y < 0) {
            right_paddle.y = 0;
        }

        if (right_paddle.y > WIN_HEIGHT - PADDLE_HEIGHT) {
            right_paddle.y = WIN_HEIGHT - PADDLE_HEIGHT;
        }

        usleep(speed);
    }

    clear();
    update_high_scores(high, current);
    save_high_scores(*high);

    mvprintw(WIN_HEIGHT / 2 - 2, WIN_WIDTH / 2 - 10, "PLAYER 1 score: %d", current.left);
    mvprintw(WIN_HEIGHT / 2 - 3, WIN_WIDTH / 2 - 10, "PLAYER 2 score: %d", current.right);
    refresh();
}

//Updates highscore on screen if either player has achieved one
void update_high_scores(Scores *high, Scores current) {
    if (current.left > high->left) {
        high->left = current.left;
        mvprintw(WIN_HEIGHT / 2, WIN_WIDTH / 2 - 10, "PLAYER 1 achieved a new high score: %d", high->left);
    }

    if (current.right > high->right) {
        high->right = current.right;
        mvprintw(WIN_HEIGHT / 2 + 1, WIN_WIDTH / 2 - 10, "PLAYER 2 achieved a new high score: %d", high->right);
    }

    refresh();
    sleep(2);
}

//Random coordintate for the ball for which direction to go
int random_coordinate() {
    int value;
    if (rand() % 2 == 0) {
        value = 1;
    } else {
        value = -1;
    }
    return value;
}

//Drawing functions for the paddles and game border
void draw_paddle(int x, int y) {
    for (int i = 0; i < PADDLE_HEIGHT; i++) {
        mvprintw(y + i, x, "|");
    }
}

void draw_ball(int x, int y) {
    mvprintw(y, x, "O");
}

void move_paddle(int *y, int direction) {
    *y = *y + direction;
}

void draw_border() {
    for (int i = 0; i < WIN_WIDTH; i++) {
        mvprintw(0, i, "-");
        mvprintw(WIN_HEIGHT - 1, i, "|");
    }

    for (int i = 0; i < WIN_HEIGHT; i++) {
        mvprintw(i, 0, "|");
        mvprintw(i, WIN_WIDTH - 1, "|");
    }
}

void print_scores(int left_score, int left_high_score, int right_score, int right_high_score) {
    mvprintw(1, WIN_WIDTH / 4 - 5, "PLAYER 1: %d", left_score);
    mvprintw(2, WIN_WIDTH / 4 - 5, "HIGHSCORE: %d", left_high_score);

    mvprintw(1, 3 * WIN_WIDTH / 4 - 6, "PLAYER 2: %d", right_score);
    mvprintw(2, 3 * WIN_WIDTH / 4 - 6, "HIGHSCORE: %d", right_high_score);
}

//Game menu
void display_menu() {
    initscr();
    clear();
    start_color();
    init_pair(5, COLOR_RED, COLOR_BLACK);
    attron(COLOR_PAIR(5));

    mvprintw(WIN_HEIGHT / 2 - 3, WIN_WIDTH / 2 - 10, "Pong Menu");
    mvprintw(WIN_HEIGHT / 2 - 1, WIN_WIDTH / 2 - 10, "1. Play Game");
    mvprintw(WIN_HEIGHT / 2, WIN_WIDTH / 2 - 10, "2. High Scores");
    mvprintw(WIN_HEIGHT / 2 + 1, WIN_WIDTH / 2 - 10, "3. Exit");

    int choice = getch();

    while (choice != '1' && choice != '2' && choice != '3') {
        clear();
        mvprintw(WIN_HEIGHT / 2 - 3, WIN_WIDTH / 2 - 10, "Pong Menu");
        mvprintw(WIN_HEIGHT / 2 - 1, WIN_WIDTH / 2 - 10, "1. Play Game");
        mvprintw(WIN_HEIGHT / 2, WIN_WIDTH / 2 - 10, "2. High Scores");
        mvprintw(WIN_HEIGHT / 2 + 1, WIN_WIDTH / 2 - 10, "3. Exit");
        mvprintw(WIN_HEIGHT / 2 + 2, WIN_WIDTH / 2 - 10, "Invalid choice, please select again.");
        choice = getch();
    }

    attroff(COLOR_PAIR(5));

    if (choice == '2') {
        display_high_score();
    } else {
        if (choice == '3') {
            endwin();
            exit(0);
        }
    }
}

//Displaying highscore in the highscore menu
void display_high_score() {
    clear();
    Scores high;
    load_high_scores(&high);

    mvprintw(WIN_HEIGHT / 2 - 1, WIN_WIDTH / 2 - 10, "PLAYER 1 Highscore: %d", high.left);
    mvprintw(WIN_HEIGHT / 2, WIN_WIDTH / 2 - 10, "PLAYER 2 Highscore: %d", high.right);
    mvprintw(WIN_HEIGHT / 2 + 1, WIN_WIDTH / 2 - 10, "Press 1 to go back.");

    int back = getch();
    while (back != '1') {
        back = getch();
    }

    clear();
    display_menu();
}
