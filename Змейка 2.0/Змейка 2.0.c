#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h> 
#include <conio.h>
#include <dos.h>

#define UP 72
#define DOWN 80
#define RIGHT 77
#define LEFT 75
#define WIDTH 150
#define HEIGHT 35
#define TIME 150 
#define PERSENTAGE_OF_WALLS 3
#define PERSENTAGE_OF_APPLES 0.5

int game_over;
char global_symb = '0';
//int count;

//��������� � ������������ ����� ���� ������(row - ����� ������, col - ����� �������)
typedef struct place {
	int row, col;
} place;
//��������� � ����������� � ������
typedef struct Snake {
	place head;	//���������� ������
	place* body;	//��������� �� ������ � ������������ ���� ������ ���� ����(0-� ������� � ������� - ������)
	int length;	//����� ����
	int score;	//���-�� ����� � ���� (����� ��������� �����)
	char direct;
	int move_x;
	int move_y;
	int life;
	char up;
	char down;
	char left;
	char right;
} Snake;

typedef struct Field {
	int width;	//������ ����
	int height;	//������ ����
	int time;	//����� ���������� ������
	float persentage_of_walls;	//������� ���������� ���� �������
	float persentage_of_apples;		//������� ���������� ���������� ���� ��������
	int maximum;	//���������� ����� �� ����
	char** arr;	//��������� �� ���� (��������� ������ �� ��������)
	int apples;
} Field;
//������� ��� ��������� ������� � ����� (x, y) (���� � ������)
void setcur(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
};
//������� ������� ������ � n-� ������� ��� ����� (��� ����� ������� �����)
void clean_string(Field* field, int n) {
	setcur(0, field->height + 2 + n);
	printf("                                                                                                        ");
	setcur(0, field->height + 2 + n);
}
//������ ������� � �������������� ����������
void start() {
	game_over = 0;
	//	count = 0;
}
//������� ������������� ����
void init_snake(Snake* snake, Field* field, int x, int y, int num) {
	//���������� ������ ���������� - (x, y)
	snake->body = (place*)malloc(sizeof(place) * field->height * field->width);
	snake->head.col = x;
	snake->head.row = y;
	snake->body[0].col = snake->head.col;
	snake->body[0].row = snake->head.row;
	snake->length = 1;
	snake->score = 0;
	snake->direct = 'v';
	snake->life = num;
	field->arr[y][x] = 'v';
	if (num == 2) {
		snake->up = 72;
		snake->down = 80;
		snake->left = 75;
		snake->right = 77;
	}
	else {
		snake->up = 'w';
		snake->down = 's';
		snake->left = 'a';
		snake->right = 'd';
	}
}
//������� ���������� ���������� �������
void put_wall(Field* field) {
	int wall_x, wall_y;
	for (int i = 0; i < (float)field->persentage_of_walls * field->width * field->height / 100; i++) {
		do {
			wall_x = rand() % field->width + 1;
			wall_y = rand() % field->height + 1;
		} while (field->arr[wall_y][wall_x] != ' ');
		field->arr[wall_y][wall_x] = '#';
	}
}
//������� ���������� ���������� ��������
void put_apple(Field* field) {
	int apple_x, apple_y;
	int maximum = 0;
	for (int i = 0; i < (float)field->persentage_of_apples * field->width * field->height * (100 - field->persentage_of_walls) / 100 / 100; i++) {
		do {
			apple_x = rand() % field->width + 1;
			apple_y = rand() % field->height + 1;
		} while (field->arr[apple_y][apple_x] != ' ');
		field->arr[apple_y][apple_x] = '@';
		maximum++;
	}
	field->maximum = maximum;
	field->apples = maximum;
}
//������� ������������� ���� (���������� �������� � �������������, ��������� ��������� ����, ��������� ���� ������, ���������� ��������)
void init_field(Field* field) {
	int speed;
	int flag = 0;
	printf("Press 'D' to use default settings, 'S' to set personal arguments.\n");
	while (!flag) {
		switch (_getch()) {
		case 's':
			printf("Enter width of the field: ");
			scanf("%d", &field->width);
			printf("Enter height of the field: ");
			scanf("%d", &field->height);
			printf("Enter speed of the snake(1-10): ");
			scanf("%d", &speed);
			field->time = 750 / speed;
			printf("Enter percentage of walls: ");
			scanf("%f", &field->persentage_of_walls);
			printf("Enter percentage of apples: ");
			scanf("%f", &field->persentage_of_apples);
			flag = 1;
			break;
		case 'd':
			field->width = WIDTH;
			field->height = HEIGHT;
			field->time = TIME;
			field->persentage_of_walls = PERSENTAGE_OF_WALLS;
			field->persentage_of_apples = PERSENTAGE_OF_APPLES;
			flag = 1;
			break;
		}
	}
	system("cls");
	//��������� ������ ��� ������ � �������� ����
	field->arr = (char**)malloc(sizeof(char*) * (field->height + 2));
	for (int i = 0; i < field->height + 2; i++) {
		field->arr[i] = (char*)malloc(sizeof(char) * (field->width + 2));
	}
	//��������� ������
	for (int i = 0; i < field->width + 2; i++) {
		field->arr[0][i] = '#';
		field->arr[field->height + 1][i] = '#';
	}
	for (int i = 1; i < field->height + 1; i++) {
		field->arr[i][0] = '#';
		field->arr[i][field->width + 1] = '#';
	}
	for (int i = 1; i < field->height + 1; i++) {
		for (int j = 1; j < field->width + 1; j++) {
			field->arr[i][j] = ' ';
		}
	}
	//��������� ���� ������, ���������� ��������
	put_wall(field);
	put_apple(field);
}
//������� �������� � ����� (1, 1) �, �������� � ������� "�����, ������, ����, �����", ��������� ������ ������ �������� "*" (������ ����� �������� ������ "%" ������������� ��� ��������)
void check_cell(int y, int x, Field* field, int recurs) {
	if (field->arr[y][x] == ' ' && recurs < 4500) {
		recurs++;	//������� ��� �������� ������������ �����
		field->arr[y][x] = '*';
		check_cell(y - 1, x, field, recurs);
		check_cell(y, x + 1, field, recurs);
		check_cell(y + 1, x, field, recurs);
		check_cell(y, x - 1, field, recurs);
	}
	else if (field->arr[y][x] == '@' && recurs < 4500) {
		recurs++;	//������� ��� �������� ������������ �����
	//	count++;	//������� ��������� ����� (�.�. ���, �� ������� ������ ����� ���������)
		field->arr[y][x] = '%';
		check_cell(y - 1, x, field, recurs);
		check_cell(y, x + 1, field, recurs);
		check_cell(y + 1, x, field, recurs);
		check_cell(y, x - 1, field, recurs);
	}
}
//������� �������� ���� �� ��, ��� �� ���� ����� ������ ��������� � ��� ������ ����� �� ����� ����� � �������� ������ � ����� ������
void check_access(Field* field, int recurs) {
	check_cell(1, 1, field, recurs);
	if (/*(count == field->maximum) &&*/ ((field->arr[1][1] == '*') || (field->arr[1][1] == '%')) && ((field->arr[field->height][field->width] == '*') || (field->arr[field->height][field->width] == '%'))) {
		game_over = 0;
	}
	else {
		game_over = 3;
	}

}
//������� ������� ������� �� �������� "*" � "%", ���������� ����� ������� check_cell � ������ �� ����������� ������� " " � "@"
void clean(Field* field) {
	setcur(0, 0);
	for (int i = 0; i < field->height + 2; i++) {
		for (int j = 0; j < field->width + 2; j++) {
			if (field->arr[i][j] == '*') {
				field->arr[i][j] = ' ';
			}
			else if (field->arr[i][j] == '%') {
				field->arr[i][j] = '@';
			}
		}
	}
}
//������� ���������� ������ � ���� � ���� "Progress.txt"
void save(Snake* snake1, Snake* snake2, Field* field) {
	FILE* fp = fopen("Progress.txt", "w");
	fprintf(fp, "%d %d\n", field->height, field->width);
	fprintf(fp, "%d\n", field->time);
	fprintf(fp, "%f %f\n", field->persentage_of_walls, field->persentage_of_apples);
	fprintf(fp, "%d\n", field->maximum);
	for (int i = 0; i < field->height + 2; i++) {
		for (int j = 0; j < field->width + 2; j++) {
			fprintf(fp, "%c", field->arr[i][j]);
		}
		fprintf(fp, "%c", '\n');
	}
	fprintf(fp, "%d\n", snake1->length);
	for (int i = 0; i < snake1->length; i++) {
		fprintf(fp, "%d %d\n", snake1->body[i].row, snake1->body[i].col);
	}
	fprintf(fp, "%d\n", snake1->life);
	fprintf(fp, "%c\n", snake1->direct);

	fprintf(fp, "%d\n", snake2->length);
	for (int i = 0; i < snake2->length; i++) {
		fprintf(fp, "%d %d\n", snake2->body[i].row, snake2->body[i].col);
	}
	fprintf(fp, "%d\n", snake2->life);
	fprintf(fp, "%c", snake2->direct);

	fclose(fp);
}
void move_snake(Snake* snake, char symb) {
	if (snake->life == 1 || snake->life == 2) {
		if (symb == snake->up) {
			if (snake->direct != 'v') {	//��������, ��� ������ �� ����� ����� (�.�. ���� � ����)
				snake->move_x = 0;
				snake->move_y = -1;
				snake->direct = '^';	//��������� ������� ������
			}
		}
		else if (symb == snake->down) {
			if (snake->direct != '^') {	//��������, ��� ������ �� ����� ����� (�.�. ���� � ����)
				snake->move_x = 0;
				snake->move_y = 1;
				snake->direct = 'v';	//��������� ������� ������
			}
		}
		else if (symb == snake->left) {
			if (snake->direct != '>') {	//��������, ��� ������ �� ����� ����� (�.�. ���� � ����)
				snake->move_x = -1;
				snake->move_y = 0;
				snake->direct = '<';	//��������� ������� ������
			}
		}
		else if (symb == snake->right) {
			if (snake->direct != '<') {	//��������, ��� ������ �� ����� ����� (�.�. ���� � ����)
				snake->move_x = 1;
				snake->move_y = 0;
				snake->direct = '>';	//��������� ������� ������
			}
		}
	}
}
//������� ������ ����������� �������� � ����������
void move(Snake* snake1, Snake* snake2, Field* field) {
	//������� �� ������ �������� ������ ����
	int flag = 0;
	char temp;
	if (snake1->head.col == 1 && snake1->head.row == 0) {
		while (!flag) {
			if (_kbhit()) {
				temp = _getch();
				if (temp == snake1->down || temp == snake2->down) {
					snake1->move_x = 0;
					snake1->move_y = 1;
					snake2->move_x = 0;
					snake2->move_y = 1;
					snake1->direct = 'v';
					snake2->direct = 'v';
					flag = 1;
					clean_string(field, 1);
					for (int i = 3; i > 0; i--) {
						printf("Before start: %d...", i);
						Sleep(1000);
						clean_string(field, 1);
					}
					//���� ����� ���������� ���, ������������ ������ �������
					if (field->maximum == snake1->score) {
						clean_string(field, 2);
						printf("There are no apples here. Find a way out!");
					}
					else {
						clean_string(field, 1);
						printf("Up, down, left, right - snake control, 'C' - pause.\nApples number - %d", field->maximum);
					}
				}
			}
		}
	}
	else if (_kbhit()) {	//������� �������� ������ ��� ����� �������
		temp = _getch();
		if (temp == 'c') {
			clean_string(field, 1);
			printf("Press 'C' to continue, 'F' to save your progress, 'E' finish the game.");
			while (!flag) {
				if (_kbhit()) {
					switch (_getch()) {
					case 'c':	//������� �� ����������� ����
						flag = 1;
						clean_string(field, 1);
						printf("Up, Down, Left, Right - right snake control. 'W', 'S', 'A', 'D' - left snake control, 'C' - pause.");
						break;
					case 'f':	//������� �� ������ � ����
						save(snake1, snake2, field);
						clean_string(field, 1);
						printf("Your progress is saved!");
						Sleep(2000);
						clean_string(field, 1);
						printf("Press 'C' to continue, 'F' to save your progress, 'E' to finish the game.");
						break;
					case 'e':	//������� �� ����� �� ����
						clean_string(field, 1);
						clean_string(field, 2);
						printf("See you again! :)\n");
						Sleep(1000);
						exit(1);
					}
				}
			}
		}
		else if (temp == -32) {
			move_snake(snake2, _getch());
			while (_kbhit()) {
				temp = _getch();
				if (temp == 'w' || temp == 's' || temp == 'a' || temp == 'd') {
					move_snake(snake1, temp);
					break;
				}
				else if (temp == -32) {
					_getch();
				}
			}
		}
		else if (temp == 'w' || temp == 's' || temp == 'a' || temp == 'd') {
			move_snake(snake1, temp);
			while (_kbhit()) {
				temp = _getch();
				if (temp == -32) {
					move_snake(snake2, _getch());
					break;
				}
			}
		}
	}
}
//������� ��������� ������� symb � ����� (x, y)
void draw_symbol(int y, int x, char symb, int color) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	setcur(x, y);

	if (symb == '#') {
		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 11));
	}
	else if (symb == '@') {
		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 12));
	}
	else if (symb == 'o') {
		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 8 + color));
	}
	else if (symb == '^' || symb == '>' || symb == 'v' || symb == '<') {
		SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 8 + color));
	}

	putchar(symb);

	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));
}
//������� ������� �������� �� ������������ ��� �������� ������
void check(Snake* snake, Field* field) {
	if (snake->head.col == field->width / 2 + 1 && snake->head.row == field->height + 1) {

		snake->life += 4;
		for (int i = 0; i < snake->length; i++) {
			field->arr[snake->body[i].row][snake->body[i].col] = ' ';
			draw_symbol(snake->body[i].row, snake->body[i].col, ' ', 0);
		}
		snake->move_x = 0;
		snake->move_y = 0;
	}	//������������
	else if (field->arr[snake->head.row + snake->move_y][snake->head.col + snake->move_x] == '#' || field->arr[snake->head.row + snake->move_y][snake->head.col + snake->move_x] == 'o' || field->arr[snake->head.row + snake->move_y][snake->head.col + snake->move_x] == '^' || field->arr[snake->head.row + snake->move_y][snake->head.col + snake->move_x] == 'v' || field->arr[snake->head.row + snake->move_y][snake->head.col + snake->move_x] == '<' || field->arr[snake->head.row + snake->move_y][snake->head.col + snake->move_x] == '>') {
		snake->life += 2;
		snake->move_x = 0;
		snake->move_y = 0;
	}
	else if (field->arr[snake->head.row + snake->move_y][snake->head.col + snake->move_x] == '@') {	//�������� ������
		field->arr[snake->head.row][snake->head.col] = 'o';
		draw_symbol(snake->head.row, snake->head.col, 'o', snake->life);

		snake->head.col += snake->move_x;
		snake->head.row += snake->move_y;

		for (int i = snake->length; i > 0; i--) {
			snake->body[i].col = snake->body[i - 1].col;
			snake->body[i].row = snake->body[i - 1].row;
		}
		snake->body[0].col = snake->head.col;
		snake->body[0].row = snake->head.row;

		field->arr[snake->head.row][snake->head.col] = snake->direct;
		draw_symbol(snake->head.row, snake->head.col, snake->direct, snake->life);

		snake->length++;
		snake->score++;
		field->apples--;
		
		//����� ���������� �� �����
		if (snake->life == 1) {
			clean_string(field, 2);
			printf("Apples number - %d.", field->apples);
			clean_string(field, 3);
			printf("Left player's score - %d.", snake->score);
		}
		else {
			clean_string(field, 4);
			printf("Left player's score - %d.", snake->score);
			clean_string(field, 2);
			printf("Apples number - %d.", field->apples);
		}
	}
	else {	//�������� �� ��������� ������ ������ ��� �����
		field->arr[snake->head.row][snake->head.col] = 'o';
		draw_symbol(snake->head.row, snake->head.col, 'o', snake->life);

		field->arr[snake->body[snake->length - 1].row][snake->body[snake->length - 1].col] = ' ';
		draw_symbol(snake->body[snake->length - 1].row, snake->body[snake->length - 1].col, ' ', 0);

		snake->head.col += snake->move_x;
		snake->head.row += snake->move_y;
		for (int i = snake->length; i > 0; i--) {
			snake->body[i].col = snake->body[i - 1].col;
			snake->body[i].row = snake->body[i - 1].row;
		}

		snake->body[0].col = snake->head.col;
		snake->body[0].row = snake->head.row;

		field->arr[snake->head.row][snake->head.col] = snake->direct;
		draw_symbol(snake->head.row, snake->head.col, snake->direct, snake->life);
	}
	//������� �� �������� ������� " " � ����� ������ ����� ������ ����
	if (field->arr[0][1] == ' ') {
		field->arr[0][1] = '#';
		draw_symbol(0, 1, '#', 0);
	}
	if (field->arr[0][field->width] == ' ') {
		field->arr[0][field->width] = '#';
		draw_symbol(0, field->width, '#', 0);
	}
	//������� �� �������� ������� ��� ����� � "��������� ����� �� ���������", �.�. �������� � ����� ������ ������ " "
	if (snake->head.col == 1 && snake->head.row == 1) {
		field->arr[field->height + 1][field->width/2 + 1] = ' ';
		draw_symbol(field->height + 1, field->width/2 + 1, ' ', 0);
	}
}
//������� �������� �� ��������� � ����� �� ���������
void check_victory(Snake* snake1, Snake* snake2, Field* field) {
	if ((snake1->life == 5) && (snake2->life == 6)) {
		game_over = 2;
	}
	else if ((snake1->life == 3) && (snake2->life == 4)) {
		game_over = -1;
	} 
	else if (((snake1->life == 3) && (snake2->life == 6)) || ((snake2->life == 5) && (snake2->life == 4))) {
		game_over = 1;
	}
}
//������� ��������� ����� ����
void draw(Field* field) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	setcur(0, 0);
	for (int i = 0; i < field->height + 2; i++) {
		for (int j = 0; j < field->width + 2; j++) {
			draw_symbol(i, j, field->arr[i][j], 1);
		}
		putchar('\n');
	}
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 7));
}
//������� ���������� ���� � ������ �� ����� "Progress.txt"
void read(FILE* fp, Snake* snake1, Snake* snake2, Field* field) {
	//������ ���������� � ����
	fscanf(fp, "%d", &field->height);
	fgetc(fp);
	fscanf(fp, "%d", &field->width);
	fgetc(fp);
	fscanf(fp, "%d", &field->time);
	fgetc(fp);
	fscanf(fp, "%f", &field->persentage_of_walls);
	fgetc(fp);
	fscanf(fp, "%f", &field->persentage_of_apples);
	fgetc(fp);
	fscanf(fp, "%d", &field->maximum);
	fgetc(fp);
	//��������� ������ ��� ���� � ������ ���� � ������
	field->arr = (char**)malloc(sizeof(char*) * (field->height + 2));
	for (int i = 0; i < field->height + 2; i++) {
		field->arr[i] = (char*)malloc(sizeof(char) * (field->width + 2));
	}
	for (int i = 0; i < field->height + 2; i++) {
		for (int j = 0; j < field->width + 2; j++) {
			fscanf(fp, "%c", &field->arr[i][j]);
		}
		fgetc(fp);
	}
	//��������� ������ ��� ���� ������ � ������ ���������� � ������
	snake1->body = (place*)malloc(sizeof(place) * field->height * field->width);
	//������ ����� ������
	fscanf(fp, "%d", &snake1->length);
	fgetc(fp);
	//������ ��������� ��������� ���� ������ � ���������
	for (int i = 0; i < snake1->length; i++) {
		fscanf(fp, "%d", &snake1->body[i].row);
		fgetc(fp);
		fscanf(fp, "%d", &snake1->body[i].col);
		fgetc(fp);
	}
	fscanf(fp, "%d", &snake1->life);
	fgetc(fp);
	//������ ��������� ������
	snake1->head.col = snake1->body[0].col;
	snake1->head.row = snake1->body[0].row;
	fscanf(fp, "%c", &snake1->direct);	//������ ������ ������(^,>,v,<)
	fgetc(fp);
	snake1->score = snake1->length - 1;

	//��������� ������ ��� ���� ������ � ������ ���������� � ������
	snake2->body = (place*)malloc(sizeof(place) * field->height * field->width);
	//������ ����� ������
	fscanf(fp, "%d", &snake2->length);
	fgetc(fp);
	//������ ��������� ��������� ���� ������ � ���������
	for (int i = 0; i < snake2->length; i++) {
		fscanf(fp, "%d", &snake2->body[i].row);
		fgetc(fp);
		fscanf(fp, "%d", &snake2->body[i].col);
		fgetc(fp);
	}
	fscanf(fp, "%d", &snake2->life);
	fgetc(fp);
	//������ ��������� ������
	snake2->head.col = snake2->body[0].col;
	snake2->head.row = snake2->body[0].row;
	fscanf(fp, "%c", &snake2->direct);	//������ ������ ������(^,>,v,<)
	fgetc(fp);
	snake2->score = snake2->length - 1;
	field->apples = field->maximum - snake1->score - snake2->score;
		snake2->up = 72;
		snake2->down = 80;
		snake2->left = 75;
		snake2->right = 77;

		snake1->up = 'w';
		snake1->down = 's';
		snake1->left = 'a';
		snake1->right = 'd';
}
//������� �������� ������� ��� ����, ������� ��� ���� ������, ��������� ����, ��������� ������ 
void free_game(Snake* snake1, Snake* snake2, Field* field) {
	free(snake1->body);
	free(snake1);

	free(snake2->body);
	free(snake2);
	for (int i = 0; i < field->height + 2; i++) {
		free(field->arr[i]);
	}
	free(field->arr);
	free(field);
}
void get_direction(Snake* snake) {
	switch (snake->direct) {
	case '^':
		snake->move_x = 0;
		snake->move_y = -1;
		break;
	case 'v':
		snake->move_x = 0;
		snake->move_y = 1;
		break;
	case '<':
		snake->move_x = -1;
		snake->move_y = 0;
		break;
	case '>':
		snake->move_x = 1;
		snake->move_y = 0;
		break;
	}
}
int main() {
	
	while (1) {
		Field* field = (Field*)malloc(sizeof(Field));	//��������� ������ ��� ����
		Snake* snake1 = (Snake*)malloc(sizeof(Snake));  //��������� ������ ��� ������
		Snake* snake2 = (Snake*)malloc(sizeof(Snake));
		start();
		FILE* fp;
		int flag = 0;	//������ ��� ����� ������� ����
		int extraflag = 0;
		int recurs = 0;
		printf("Press 'C' to continue your last game, 'N' to start a new game.");
		while (!extraflag) {
			switch (_getch()) {
			case 'c':
				//������� �� ����� �� �����
				system("cls");
				fp = fopen("Progress.txt", "r");
				read(fp, snake1, snake2, field);
				get_direction(snake1);
				get_direction(snake2);
				draw(field);
				clean_string(field, 1);
				printf("Press any key to continue.");
				
				//������� �� ������ ��������
				while (!flag) {
					if (_kbhit()) {
						flag = 1;
						clean_string(field, 1);
						for (int i = 3; i > 0; i--) {
							printf("Before start: %d...", i);
							Sleep(1000);
							clean_string(field, 1);
						}
						printf("Up, Down, Left, Right - right snake control. 'W', 'S', 'A', 'D' - left snake control, 'C' - pause.\nApples number - %d.\nLeft player's score - %d.\nRight player's score - %d.", field->apples, snake1->score, snake2->score);
					}
				}
				extraflag = 1;
				break;
			case 'n':
				//������� �� ����� ����
				system("cls");
				//���������
				srand(time(NULL));
				init_field(field);
				init_snake(snake1, field, 1, 0, 1);
				init_snake(snake2, field, field->width, 0, 2);
				check_access(field, recurs);
				clean(field);
				draw(field);
				clean_string(field, 1);
				printf("Press 'Down' or 'S' to start the game:	");
				extraflag = 1;
				break;
			default:
				break;
			}
		}
		//��������������� ������� ����
		while (!game_over) {
			move(snake1, snake2, field);
			if (snake1->move_x != 0 || snake1->move_y != 0) {
				check(snake1, field);
			}
			if (snake2->move_x != 0 || snake2->move_y != 0) {
				check(snake2, field);
			}
			check_victory(snake1, snake2, field);
			setcur(0, field->height + 7);
			Sleep(field->time);
		}
		//����� �����������
		if (game_over == -1) {
			clean_string(field, 2);
			clean_string(field, 3);
			clean_string(field, 4);
			clean_string(field, 5);
			clean_string(field, 1);
			printf("Left and right players both lost :(");
			free_game(snake1, snake2, field);
		}
		else if (game_over == 1) {
			clean_string(field, 2);
			clean_string(field, 3);
			clean_string(field, 4);
			clean_string(field, 1);
			if (snake1->life == 3) {
				printf("Right player escaped! :)\n Total score - %d", snake2->score);
			}
			else {
				printf("Left player escaped! :)\n Total score - %d", snake1->score);
			}
			clean_string(field, 2);
			free_game(snake1, snake2, field);
		}
		else if (game_over == 2) {
			clean_string(field, 2);
			clean_string(field, 3);
			clean_string(field, 4);
			clean_string(field, 1);
			printf("Both players successfully escaped!\n");
			printf("Left player's score - %d, right player's score - %d\n", snake1->score, snake2->score);
			if (snake1->score > snake2->score) {
				printf("Left player won! :))");
			}
			else if (snake1->score < snake2->score) {
				printf("Right player won! :))");
			}
			else {
				printf("WOW, dead heat!! :))");
			}
			free_game(snake1, snake2, field);
		}
		else if (game_over == 3) {
			clean_string(field, 2);
			clean_string(field, 3);
			clean_string(field, 4);
			clean_string(field, 1);
			printf("This map is incorrect. Try again.");
			free_game(snake1, snake2, field);
		}
		Sleep(3000);
		//������� �� ������ ����� ���� ��� �����
		system("cls");
		flag = 0;
		printf("Press 'R' to restart the game,'E' to finish the game: ");
		while (!flag) {
			if (_kbhit()) {
				switch (_getch()) {
				case 'r':	//������� �� ����������� ����
					flag = 1;
					system("cls");
					break;
				case 'e':	//������� �� ����� �� ����
					system("cls");
					printf("See you again! :)\n");
					Sleep(1000);
					exit(1);
				}
			}
		}
	}
	return 0;
}