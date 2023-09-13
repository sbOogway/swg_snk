#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>

#define HEIGHT 40
#define WIDTH  80

volatile sig_atomic_t stop;

void inthand(int signum) {
	stop = 1;
}

int main(int argc, char * argv []) {
	signal(SIGINT, inthand);
	srand(time(NULL));

	typedef struct pos {
		int x;
		int y;
	} pos;

	// init objs
	// creating the map 	
	int _width = WIDTH + 2; int _height = HEIGHT + 2; int max_snake_length = _width * _height;
	char * map [_width][_height];
	for (int i = 0; i < _height; i++) {
		for (int j = 0; j < _width; j++) {
			if (j == 0 || j == _width -1) {
				if (i == 0 || i == _height-1 ) {
					map[j][i] = "#";
					continue;
				}
				map[j][i] = "|";
				continue;
					}
			if (i == 0 || i == _height-1) {
				if (j == 0 || j == _width-1) {
					map[j][i] = "#";
					continue;
				}
				map[j][i] = "=";
				continue;
				}
			map[j][i] = " ";
		}
	}
	// creating the snake
	typedef struct snake {
			int length;
			char * body;
			int direction;
			pos body_loc[3200];//max_snake_length];
	} snake;
	snake _snake;
	_snake.body_loc[0].x = _width  / 2;
	_snake.body_loc[0].y = _height / 2;
	_snake.length = 1;
	_snake.direction = 0; // 8 up, 2 down, 6 right, 4 left, 0 stop
	_snake.body = "🀫";
	// _snake.body_loc = pos[max_snake_length];
	
	// adding snake to map
	map[_snake.body_loc[0].x][_snake.body_loc[0].y] = _snake.body;

	// creating the food
	typedef struct food {
			pos loc;
			char * body;
	} food;
	food _food;
	_food.loc.x = 1 + rand() % _width;
	_food.loc.y = 1 + rand() % _height;
	_food.body = "₿";

	map[_food.loc.x][_food.loc.y] = _food.body;

	// chatgpt shit
	struct termios old_term, new_term;
    char key;

    // Save the current terminal settings
    tcgetattr(STDIN_FILENO, &old_term);
    new_term = old_term;

    // Set terminal to non-canonical mode and disable echo
    new_term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);

    // Set file descriptor for standard input to non-blocking
    int flags = fcntl(STDIN_FILENO, F_GETFL);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
	// end chatgpt shit

	// main loop
	while (!stop) {	
		system("clear");
		map[0][0] = "#";
		// render the full scene
		for (int i = 0; i < _height; i++) {
			for (int j = 0; j < _width; j++) {
				printf("%s", map[j][i]);
			}
			printf("%s", "\n");
		}

		// handles key events to move the snake
		int bytes_read = read(STDIN_FILENO, &key, 1);
        if (bytes_read == 1) {
            printf("Key pressed: %c\n", key);
			if (key == 'w') {_snake.direction = 8;}
			if (key == 'a') {_snake.direction = 4;}
			if (key == 's') {_snake.direction = 2;}
			if (key == 'd') {_snake.direction = 6;}
		}	

		// update snake pieces: get the coordinates of the piece before (head is already updated)
		for (int i = _snake.length - 1; i >= 1; i--) {
			_snake.body_loc[i].x = _snake.body_loc[i-1].x;
			_snake.body_loc[i].y = _snake.body_loc[i-1].y;
		} 	

		/*
		  n 
		o + e
		  s  
		*/

		// the snakes changes direction to nord
		if (_snake.direction == 8) {
			_snake.body_loc[0].y -= 1;
		}

		// the snakes changes direction to sud
		if (_snake.direction == 2) {
			_snake.body_loc[0].y += 1;
		}

		// the snakes changes direction to ovest
		if (_snake.direction == 4) {
			_snake.body_loc[0].x -= 1;
		}

		// the snakes changes direction to est
		if (_snake.direction == 6) {
			_snake.body_loc[0].x += 1;
		}

		// checks if snake has hit wall
		if (_snake.body_loc[0].x > _width -1 || _snake.body_loc[0].x < 0 || _snake.body_loc[0].y > _height -1 || _snake.body_loc[0].y < 0) {
			printf("game over\n");
			stop = 1;
		}

		// snake eats food
		if (_snake.body_loc[0].x == _food.loc.x && _snake.body_loc[0].y == _food.loc.y) {
			// map[_food.loc.x][_food.loc.y] = " ";
			_food.loc.x = 1 + rand() % (_width -2);
			_food.loc.y = 1 + rand() % (_height-2);
			map[_food.loc.x][_food.loc.y] = _food.body;
			_snake.length++;
			// _snake.body_loc[_snake.length-1].x = _snake.body_loc[_snake.length-2].x;
			// _snake.body_loc[_snake.length-1].y = _snake.body_loc[_snake.length-2].y;
		}

		printf("length -> %d\n", _snake.length);
		printf("snake direction -> %d\n", _snake.direction);
		
		// wipe map from snake pieces
		for (int i = 0; i < _height; i++) {
			for (int j = 0; j < _width; j++) {
				if (map[j][i] == _snake.body) {
					map[j][i] = " ";
				}
			}
		}
		


		// update map with snake body
		for (int i = 0; i < _snake.length; i++) {
			// map[_snake.body_loc[i].x][_snake.body_loc[i].y] = _snake.body;
			// printf("%d pezzo. x -> %d, y -> %d\n", i, _snake.body_loc[i].x, _snake.body_loc[i].y);
			map[_snake.body_loc[i].x][_snake.body_loc[i].y] = _snake.body;
		}	


				
		// map[_snake.body_loc[0].x][_snake.body_loc[0].y] = _snake.body;
		// map[_snake.body_loc[_snake.length-1].x][_snake.body_loc[_snake.length-1].y] = " ";		
		// _snake.length += 1;

		usleep(125000); // 500000 half a sec
	}
	tcsetattr(STDIN_FILENO, TCSANOW, &old_term);
	return 0;
}
