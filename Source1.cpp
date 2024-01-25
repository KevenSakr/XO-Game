//Keven Sakr 201700823
//CSC271 GUI Project : XO game
//This game has 2 play mode: 1 player and 2 players
//1 player mode is a human vs computer mode where the computer can't be defeated with his smart algorithm
//2 player mode is a human vs human mode where the player enters the name before starting the game by cliclink on the name zone 
//  and entering the name by keyboard input
// !!! The round time for human is 15 second if the time is up the player lose the game
//This game has also a leaderboard to honor the first three player with most wins for there outstanding performance
//esc key can be used to return to the main menu or to exit the game (when the main menu is displayed)
//I hope that you like this game
//THANK YOU so much for your time

#include<fstream>
#include <windows.h>
#include <GL/GL.h>
#include <GL/GLU.h>
#include <C:\glut\glut.h>
#include <stdlib.h>
#include<iostream>
#include<string>
#include<ctime>
#include<mmsystem.h>
#define PI 3.14159265359
using namespace std;


//prototype and variables used in struct
int X = 400;//window size
bool menubool = true;//for the menu to diplay when true
void drawStringMiddle(void* font, string text, float y);

//struct for player name and nb of wins for the leaderboard
struct Player {
	string name;
	int score;
};

// struct for the button of the menu
struct Button
{
	bool pressed = false;
	bool effect = false;
	string text;
	double  y;
	/*Button(string txt, double yy) {
		text = txt;
		y = yy;
		drawBtn();
	}*/

	void drawBtn() {

		if (effect)
			glColor3f(0.843, 0.843, 0.843);//silver
		else
			glColor3f(0, 0, 0);
		glBegin(GL_QUADS);
		glVertex2f(0.5, y);
		glVertex2f(0.5, y - (X / 5) * 1.0 / X);
		glVertex2f(-0.5, y - (X / 5) * 1.0 / X);
		glVertex2f(-0.5, y);
		glEnd();
		//glColor3f(f2, 0, 0);
		glColor3f(0.83, 0.68, 0.21);
		drawStringMiddle(GLUT_BITMAP_TIMES_ROMAN_24, text, y - (X / 5) * 1.0 / X + 0.065);
	}

	void click(int button, int state, int x, int y) {
		if (x > 100 * X / 400 && x < 300 * X / 400 && y>(1 - Button::y) * X / 2 && y < (1.2 - Button::y) * X / 2)
		{
			if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
				effect = true;
			}
			else effect = false;
			if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
				pressed = true;
				menubool = false;
				//initgame();
			}
		}
		else effect = false;
	}
};

int table[3][3];//stores the x and o and blank ( 0 ->blank, 1 -> x ,2-> o)
int playerTurn;//playerturn ( 1 -> p1 , 2 ->p2 turn)
int win;//result of the game ( 0 : draw , 1 : p1 wins , 2 : p2 wins)
bool gameOver;//false not game over , true game over
string p1 = "";//hold p1 name
string p2 = "";//hold p2 name
int n1, n2;//hold the number of x and o not drawn (max 5)
bool startGame;//become true when first xo is drawn
bool btnR;//for button restart
bool btnG;//for button give-up
bool btnP1;//when pressing on p1 name
bool btnP2;//when pressing on p2 name
int q;//for sound on win and lose;
bool gamestarted;// become true so the player canot change name after start playing
int scorep1, scorep2;//display score when playing for each player
ofstream out;//output list of players in txt file
ifstream in;//read list of player of a txt file
string s1, s2, s3;//for leaderboard
int computerwin;
string tiime;//hold the time left for the round in string format
int totalTime = 15;//hold the time left for the round in int format
string linewin = "";//hold the a string that let the program know the line that resulted the win result to draw a line on it
//main menu variables used for animation
int anim = 0;
float animx = 0, animy = 0, animz = 0;
Button b[4];//main menu btns
Player players[100];//list of players data with max capacity of 100
int nbplayers = 0;//nb of active players profile

//function prototypes
void initgame();
void drawString(void* font, string text, float x, float y);
void drawGrid();
void click(int button, int state, int x, int y);
void cross(float x, float y, float raduis, int s = 0);
void circle(float x, float y, float raduis, int s = 0);
void drawXO();
void keyboard(bool& btnP, string& p, unsigned char key);
void KeyPress(unsigned char key, int x, int y);
bool winCheck();
bool drawCheck();
void buttonG();
void buttonR();
void p1name();
void p2name();
void animation();
int checkWinner1();
int minimax(int board[3][3], bool isMaximizing);
void bestMove();
void sound();
void Timer(int value);
void drawline();
void twoPlayers();
void MainMenu();
void leaderboard();
void computermove();
void oneplayer();
void display();
void resize(int width, int height);
void insertionSort(Player a[], int size);

//main function where the program start
int main(int argc, char** argv)
{
	//read players data from a txt file
	in.open("players.txt");
	in >> nbplayers;
	for (int i = 0; i < nbplayers; i++) {
		in >> players[i].score;
		in >> players[i].name;
	}
	in.close();
	//used with rand() fn
	srand(time(0));
	//for timer
	glutTimerFunc(1000, Timer, 0);
	//initialize the game and graphics
	initgame();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glOrtho(-1, 1, -1, 1, 0, 1);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(X, X);
	glutCreateWindow("XO GAME ");
	glutReshapeFunc(resize);
	glutDisplayFunc(display);
	glutKeyboardFunc(KeyPress);
	glutMouseFunc(click);
	glutIdleFunc(display);
	glutMainLoop();
}

//initialize the game
void initgame()
{
	playerTurn = rand() % 2 + 1; //random starts first
	startGame = false;
	btnR = false;
	btnG = false;
	btnP1 = false;
	btnP2 = false;
	gameOver = false;
	//number of xo in the container for each player
	if (!startGame) {
		if (playerTurn == 1)
			n1 = 5, n2 = 4;
		else n1 = 4, n2 = 5;
	}

	//clear the XO table
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			table[i][j] = 0;
		}
	}
}

//Used to write text in the window starting at (x,y)
void drawString(void* font, string text, float x, float y) {
	glRasterPos2f(x, y);
	for (int i = 0; i < text.length(); i++)
		glutBitmapCharacter(font, text[i]);
}

//Used to write text in the middle of window at y
void drawStringMiddle(void* font, string text, float y) {
	float mid = 0;
	for (int i = 0; i < text.length(); i++)
		mid += glutBitmapWidth(font, text[i]);
	mid = (-mid / X);
	glRasterPos2f(mid, y);
	for (int i = 0; i < text.length(); i++)
		glutBitmapCharacter(font, text[i]);
}

//This function is used to draw the 4 lines 2 vertical and 2 horizontal
//each line is formed of 3 lines
void drawGrid()
{
	glBegin(GL_LINES);
	//2 vertical lines
	glColor3f(1, 0, 0);
	glVertex2f(0.2, 0.4);
	glColor3f(0, 1, 0);
	glVertex2f(0.2, -0.8);

	glColor3f(1, 0, 0);
	glVertex2f(0.202, 0.4);
	glColor3f(0, 1, 0);
	glVertex2f(0.202, -0.8);

	glColor3f(1, 0, 0);
	glVertex2f(0.198, 0.4);
	glColor3f(0, 1, 0);
	glVertex2f(0.198, -0.8);


	glColor3f(0.5, 0.5, 1);
	glVertex2f(-0.2, 0.4);
	glColor3f(0, 0, 0);
	glVertex2f(-0.2, -0.8);

	glColor3f(0.5, 0.5, 1);
	glVertex2f(-0.202, 0.4);
	glColor3f(0, 0, 0);
	glVertex2f(-0.202, -0.8);

	glColor3f(0.5, 0.5, 1);
	glVertex2f(-0.198, 0.4);
	glColor3f(0, 0, 0);
	glVertex2f(-0.198, -0.8);
	//2 horizontal lines

	glColor3f(0, 1, 1);
	glVertex2f(0.6, 0);
	glColor3f(1, 0, 1);
	glVertex2f(-0.6, 0);

	glColor3f(0, 1, 1);
	glVertex2f(0.6, 0.002);
	glColor3f(1, 0, 1);
	glVertex2f(-0.6, 0.002);

	glColor3f(0, 1, 1);
	glVertex2f(0.6, -0.002);
	glColor3f(1, 0, 1);
	glVertex2f(-0.6, -0.002);

	glColor3f(1, 1, 0);
	glVertex2f(0.6, -0.4);
	glColor3f(0, 0, 1);
	glVertex2f(-0.6, -0.4);

	glColor3f(1, 1, 0);
	glVertex2f(0.6, -0.402);
	glColor3f(0, 0, 1);
	glVertex2f(-0.6, -0.402);

	glColor3f(1, 1, 0);
	glVertex2f(0.6, -0.398);
	glColor3f(0, 0, 1);
	glVertex2f(-0.6, -0.398);

	glEnd();
}


//This function is called when the mouse button is pressed 
void click(int button, int state, int x, int y)
{
	//if the main menu we have 4 buttons
	if (menubool) {
		b[0].click(button, state, x, y);
		b[1].click(button, state, x, y);
		b[2].click(button, state, x, y);
		b[3].click(button, state, x, y);
	}
	//if 2 players mode
	else if (b[1].pressed) {
		//puts the x or o on the blank box
		if (!gameOver && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			//check if cursor on the #
			if (x > X / 5 && x < 4 * X / 5 && y>3 * X / 10 && y < 9 * X / 10)
			{
				int j = (int)(((X - y) - X / 10) / (X / 5));
				int i = (((x)-2 * X / 10) / (X / 5));
				if (i >= 0 && i <= 2 && j >= 0 && j <= 2) {
					startGame = true;
					gamestarted = true;
					btnP1 = btnP2 = false;
					if (playerTurn == 1)
					{
						if (table[i][j] == 0)
						{
							table[i][j] = 1;
							n1--;
							playerTurn = 2;
							totalTime = 15;
						}
					}
					else if (table[i][j] == 0) {
						table[i][j] = 2;
						n2--;
						playerTurn = 1;
						totalTime = 15;
					}
				}
			}
		}
		//mouse on give-up button
		if (x > (12 * X / 400) && x <(140 * X / 400) && y>(360 * X / 400) && y < (396 * X / 400)
			&& button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			btnG = true;
		}
		else btnG = false;
		if (!gameOver && startGame && x > (12 * X / 400) && x <(140 * X / 400) && y>(360 * X / 400) && y < (396 * X / 400)
			&& button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
			if (playerTurn == 1) {
				win = 2;
			}
			if (playerTurn == 2) {
				win = 1;
			}
			totalTime = 15;
			gameOver = true;
			//initgame();
		}
		//mouse on restart button
		if (x > 260 * X / 400 && x < 388 * X / 400 && y>360 * X / 400 && y < 396 * X / 400)
		{
			if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
				btnR = true;
			}
			else btnR = false;
			if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
				gameOver = false;
				totalTime = 15;
				linewin = "";
				initgame();
			}
		}
		else btnR = false;
		//when entering p1 and p2 game not started
		if (!gamestarted) {
			scorep1 = 0;
			scorep2 = 0;
			//entering p1 name
			if (x > 0 && x < 4 * X / 10 && y>1 * X / 10 && y < 2 * X / 10)
			{
				//check if cursor on the p1
				if (!startGame && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
				{
					btnP1 = true;
					btnP2 = false;
				}
			}
			//entering p2 name
			if (x > 6 * X / 10 && x < X && y>1 * X / 10 && y < 2 * X / 10)
			{
				//check if cursor on the p2
				if (!startGame && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
				{
					btnP1 = false;
					btnP2 = true;
				}
			}
		}
	}
	//computer playing one player mode
	else if (b[0].pressed) {
		//puts the x on the blank box
		if (!gameOver && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			if (playerTurn == 1)
				//check if cursor on the #
				if (x > X / 5 && x < 4 * X / 5 && y>3 * X / 10 && y < 9 * X / 10)
				{
					int j = (int)(((X - y) - X / 10) / (X / 5));
					int i = (((x)-2 * X / 10) / (X / 5));
					if (i >= 0 && i <= 2 && j >= 0 && j <= 2) {
						startGame = true;
						gamestarted = true;
						totalTime = 15;
						btnP1 = btnP2 = false;
						if (table[i][j] == 0)
						{
							table[i][j] = 1;
							n1--;
							playerTurn = 2;
						}
					}
				}
		}
		//mouse on give-up button
		if (x > (12 * X / 400) && x <(140 * X / 400) && y>(360 * X / 400) && y < (396 * X / 400)
			&& button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			btnG = true;
		}
		else btnG = false;
		if (!gameOver && startGame && x > (12 * X / 400) && x <(140 * X / 400) && y>(360 * X / 400) && y < (396 * X / 400)
			&& button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
			if (playerTurn == 1) {
				win = 2;
			}
			if (playerTurn == 2) {
				win = 1;
			}
			totalTime = 15;
			linewin = "";
			gameOver = true;
			//initgame();
		}
		//mouse on restart button
		if (x > 260 * X / 400 && x < 388 * X / 400 && y>360 * X / 400 && y < 396 * X / 400)
		{
			if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
				btnR = true;
			}
			else btnR = false;
			if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
				gameOver = false;
				totalTime = 15;
				initgame();
			}
		}
		else btnR = false;
	}
}


//draw x 
void cross(float x, float y, float raduis, int s) {
	glBegin(GL_TRIANGLES);
	for (int r = 45; r < 360; r = r + 90) {
		float theta;
		glColor3f(0, 0, 0);
		glVertex2f(x, y);
		glColor3f(1, 0, 0);
		r += 5;
		theta = (s + r) * PI / 180;
		glVertex2f(x + raduis * cos(theta), y + raduis * sin(theta));
		r -= 10;
		theta = (s + r) * PI / 180;
		glVertex2f(x + raduis * cos(theta), y + raduis * sin(theta));
		r += 5;
	}
	glEnd();
}


//draw circle
void circle(float x, float y, float raduis, int s) {
	glBegin(GL_TRIANGLE_STRIP);
	float theta;
	for (int r = 0; r <= 360; r++) {
		theta = (r + s) * PI / 180;
		glColor3f(0.2, 0.2, 1 - r / 360.0);
		glVertex2f(x + (raduis)*cos(theta), y + (raduis)*sin(theta));
		glColor3f(0, 0, 1 - r / 360.0);
		glVertex2f(x + (raduis - 0.02) * cos(theta), y + (raduis - 0.02) * sin(theta));
	}
	glEnd();
}

//This Function Draw the x and o's on # dependant on table[][] data and in the cantainer of each player
void drawXO()
{
	float theta;
	for (int i = 0; i <= 2; i++)
	{
		for (int j = 0; j <= 2; j++)
		{
			if (table[i][j] == 1) //if it is 1 then draw x 
			{
				cross(-0.4 + 0.4 * i, -0.6 + 0.4 * j, 0.15);

				glEnd();
			}
			else if (table[i][j] == 2) //if it is 2 then draw o
			{
				circle(-0.4 + 0.4 * i, -0.6 + 0.4 * j, 0.15);
			}
		}
	}
	//2 player mode
	if (b[1].pressed) {
		if (playerTurn == 1) {
			//draw circle in p2 container 
			for (int i = 0; i < n2; i++) {
				circle(0.9 - 0.2 * i, 0.9, 0.08);
			}
			//draw x in p1 container 
			for (int i = 0; i < n1 - 1; i++) {
				cross(-0.9 + 0.2 * i, 0.9, 0.08);
			}
			cross(0, 0.9, 0.08);
		}
		if (playerTurn == 2) {
			//draw circle in p2 container 
			for (int i = 0; i < n2 - 1; i++) {
				circle(0.9 - 0.2 * i, 0.9, 0.08);
			}
			//draw x in p1 container 
			for (int i = 0; i < n1; i++) {
				cross(-0.9 + 0.2 * i, 0.9, 0.08);
			}
			circle(0, 0.9, 0.08);
		}
	}
	//1 player mode
	if (b[0].pressed) {
		for (int i = 0; i < n2; i++) {
			circle(0.9 - 0.2 * i, 0.9, 0.08);
		}
		//draw x in p1 container 
		for (int i = 0; i < n1; i++) {
			cross(-0.9 + 0.2 * i, 0.9, 0.08);
		}
	}
}

//input through keyboard
void keyboard(bool& btnP, string& p, unsigned char key) {
	if (key == 13 && !gamestarted && b[0].pressed) {
		gamestarted = true;
	}
	if (key == 13)
	{
		btnP = false;
	}
	else if (key == 32) {//space
		//ignore space in keyboard name input
	}
	else if ((key == 127 || key == 8) && p.length() > 0) {
		p = p.substr(0, p.length() - 1);
	}
	else if (p.length() <= 8) {
		p += key;
	}
}

//handeling keyboard input
void KeyPress(unsigned char key, int x, int y) {
	if (key == 27)//esc button exit from game or return to the main menu
		if (!menubool) {
			menubool = true;
			insertionSort(players, nbplayers);
			//auto back of the data of the players
			out.open("players.txt");
			out << nbplayers << " ";
			for (int i = 0; i < nbplayers; i++) {
				out << players[i].score << " " << players[i].name << " ";
			}
			out.close();
			initgame();
			for (int i = 0; i < 4; i++) {
				b[i].pressed = false;
			}
		}
		else
			exit(0);
	//entering p1 name
	if (btnP1)
	{
		keyboard(btnP1, p1, key);
	}
	//entering p2 name
	if (btnP2)
	{
		keyboard(btnP2, p2, key);
	}
}

//This function checks if there is any winner
bool winCheck()
{
	//check if there are horizontal row that has same value !=0
	for (int i = 0; i < 3; i++)
	{
		if (table[i][0] != 0 && table[i][0] == table[i][1] && table[i][0] == table[i][2]) {
			linewin = "c" + to_string(i);
			return true;
		}
	}
	//check if there are vertical row that has same value !=0
	for (int j = 0; j < 3; j++)
	{
		if (table[0][j] != 0 && table[0][j] == table[1][j] && table[0][j] == table[2][j]) {
			linewin = "r" + to_string(j);
			return true;
		}
	}
	//check if there is any diagonal row that has same value !=0
	if (table[1][1] != 0) {
		if (table[1][1] == table[0][0] && table[1][1] == table[2][2]) {
			linewin = "d1";
			return true;
		}
		if (table[1][1] == table[0][2] && table[1][1] == table[2][0]) {
			linewin = "d2";
			return true;
		}
	}
	return false;//no win
}

//return true if all table is filled  
bool drawCheck()
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (table[i][j] == 0)
				return false;
		}
	}
	return true;
}

//give-up button graphics
void buttonG() {
	float f1 = 0, f2 = 0.8;
	if (btnG)  swap(f1, f2);
	glColor3f(0, f1, 0);
	glBegin(GL_QUADS);
	glVertex2f(-.94, -.8);
	glVertex2f(-.94, -.98);
	glVertex2f(-.3, -.98);
	glVertex2f(-.3, -.8);
	glEnd();
	glColor3f(0, f2, 0);
	drawString(GLUT_BITMAP_TIMES_ROMAN_24, "GIVE-UP", -.88, -0.925);
}

//restart button graphics
void buttonR() {
	float f1 = 0, f2 = 0.8;
	if (btnR)  swap(f1, f2);
	glColor3f(0, f1, 0);
	glBegin(GL_QUADS);
	glVertex2f(.94, -.8);
	glVertex2f(.94, -.98);
	glVertex2f(.3, -.98);
	glVertex2f(.3, -.8);
	glEnd();
	glColor3f(0, f2, 0);
	drawString(GLUT_BITMAP_TIMES_ROMAN_24, "RESTART", .345, -0.925);
}

//displaying p1 name
void p1name() {
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-.99, 0.8);
	glColor3f(1, 0, 0);
	glVertex2f(-.99, 0.6);
	glColor3f(0, 0, 0);
	glVertex2f(-.2, 0.6);
	glColor3f(1, 0, 0);
	glVertex2f(-.2, 0.8);
	glColor3f(0.65, 0, 0);
	glEnd();
	if (!gamestarted)
		drawString(GLUT_BITMAP_HELVETICA_18, (p1 != "" ? p1 : "Enter P1 Name"), -.95, 0.668);
	else
		drawString(GLUT_BITMAP_HELVETICA_18, (p1 != "" ? p1 : "Player 1"), -.95, 0.668);
	glFlush();
}

//displaying p2 name
void p2name() {
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(.99, 0.8);
	glColor3f(0, 0, 1);
	glVertex2f(.99, 0.6);
	glColor3f(0, 0, 0);
	glVertex2f(.2, 0.6);
	glColor3f(0, 0, 1);
	glVertex2f(.2, 0.8);
	glEnd();
	glColor3f(0, 0, 0.65);
	if (!gamestarted)
		drawString(GLUT_BITMAP_HELVETICA_18, (p2 != "" ? p2 : "Enter P2 Name"), 0.25, 0.668);
	else
		drawString(GLUT_BITMAP_HELVETICA_18, (p2 != "" ? p2 : "Player 2"), 0.25, 0.668);
	glFlush();
}

//animation for the game lunch
void animation() {
	int slp = 10;

	if (anim < 45) {

		Sleep(slp);
		cross(-0.4, 0.75, 0.15, anim);
		circle(0.4, 0.75, 0.15, anim);
		anim += 5;
	}
	else {
		if (animx < 0.25) {
			Sleep(slp);
			cross(-0.4 + animx, 0.75, 0.15, anim);
			circle(0.4 - animx, 0.75, 0.15, anim);
			animx += 0.01;
		}
		else {
			if (animx < 0.55) {
				Sleep(slp);
				cross(-0.15 * cos(anim * PI / 360), 0.75 - 0.15 * sin(anim * PI / 360), 0.15, 1 * anim);
				circle(0.15 * cos(anim * PI / 360), 0.75 + 0.15 * sin(anim * PI / 360), 0.15, 1 * anim);
				animx += 0.01;
				anim += 6;
			}
			else {
				if (animx < 0.8) {
					Sleep(slp);
					cross(-0.4 + animx, 0.75, 0.15, anim - 6);
					circle(0.4 - animx, 0.75, 0.15, anim - 6);
					animx += 0.01;
				}
				else {
					if (anim < 270) {
						Sleep(slp);
						cross(-0.4 + animx, 0.75, 0.15, anim);
						circle(0.4 - animx, 0.75, 0.15, anim);
						anim += 5;
					}
					else {
						if (anim < 270 + 45) {
							Sleep(slp);
							circle(-0.4, 0.75, 0.15, anim);
							cross(0.4, 0.75, 0.15, anim);
							anim += 5;
						}
						else {
							if (animy < 0.25) {
								Sleep(slp);
								circle(-0.4 + animy, 0.75, 0.15, anim - 1);
								cross(0.4 - animy, 0.75, 0.15, anim - 1);
								animy += 0.01;
							}
							else {
								if (animy < 0.55) {
									Sleep(slp);
									cross(-0.15 * cos((anim + 90) * PI / 360), 0.75 - 0.15 * sin((anim + 90) * PI / 360), 0.15, 1 * anim - 90);
									circle(0.15 * cos((anim + 90) * PI / 360), 0.75 + 0.15 * sin((anim + 90) * PI / 360), 0.15, 1 * anim - 90);
									animy += 0.01;
									anim += 6;
								}
								else {
									if (animy < 0.8) {
										Sleep(slp);
										circle(-0.4 + animy, 0.75, 0.15, anim - 7);
										cross(0.4 - animy, 0.75, 0.15, anim - 7);
										animy += 0.01;
									}
									else {
										if (anim < 270 + 270) {
											Sleep(slp);
											circle(-0.4 + animy, 0.75, 0.15, anim);
											cross(0.4 - animy, 0.75, 0.15, anim);
											anim += 5;
										}
										else {
											if (animz < 0.26) {
												Sleep(slp);
												cross(-0.4 + animz, 0.75, 0.18, anim);
												circle(0.4 - animz, 0.75, 0.15, anim);
												if (animz < 0.23)
													animz += 0.01;
												else
													if (menubool)
														drawStringMiddle(GLUT_BITMAP_TIMES_ROMAN_24, "G  A  M  E", 0.48);
													else if (b[2].pressed) {
														glColor3f(1, 1, 1);
														glBegin(GL_QUADS);
														glVertex2f(-0.7, 0.5);
														glVertex2f(-0.7, 0.3);
														glVertex2f(0.7, 0.3);
														glVertex2f(0.7, 0.5);
														glEnd();
														glColor3f(0, 0, 0);
														glBegin(GL_LINE_LOOP);
														glVertex2f(-0.7, 0.5);
														glVertex2f(-0.7, 0.3);
														glVertex2f(0.7, 0.3);
														glVertex2f(0.7, 0.5);
														glEnd();
														drawStringMiddle(GLUT_BITMAP_TIMES_ROMAN_24, "L E A D E R B O A R D", 0.36);
													}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	glFlush();
	//else anim = 0;
}


//AI minimax for best possible move for the computer
//winner check for the ai minimax
int checkWinner1()
{
	//check if there are horizontal row that has same value !=0
	for (int i = 0; i < 3; i++)
	{
		if (table[i][0] == 1 && table[i][0] == table[i][1] && table[i][0] == table[i][2])
			return -10;
	}
	for (int i = 0; i < 3; i++)
	{
		if (table[i][0] == 2 && table[i][0] == table[i][1] && table[i][0] == table[i][2])
			return 10;
	}
	//check if there are vertical row that has same value !=0
	for (int j = 0; j < 3; j++)
	{
		if (table[0][j] == 1 && table[0][j] == table[1][j] && table[0][j] == table[2][j])
			return -10;
	}
	for (int j = 0; j < 3; j++)
	{
		if (table[0][j] == 2 && table[0][j] == table[1][j] && table[0][j] == table[2][j])
			return 10;
	}
	//check if there is any diagonal row that has same value !=0
	if (table[1][1] == table[0][0] && table[1][1] == table[2][2] && table[1][1] == 1)
		return -10;
	if (table[1][1] == table[0][2] && table[1][1] == table[2][0] && table[1][1] == 1)
		return -10;
	if (table[1][1] == table[0][0] && table[1][1] == table[2][2] && table[1][1] == 2)
		return 10;
	if (table[1][1] == table[0][2] && table[1][1] == table[2][0] && table[1][1] == 2)
		return 10;
	//no winner
	return 0;
}

//minimax algorithim to determin the best possible move
int minimax(int board[][3], bool isMaximizing) {
	int result = checkWinner1();
	if (result != 0) {
		return result;
	}
	else if (drawCheck())
		return 0;
	if (isMaximizing) {
		int bestScore = INT_MIN;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				// Is the spot available?
				if (board[i][j] == 0) {
					board[i][j] = 2;
					int score = minimax(board, false);
					board[i][j] = 0;
					bestScore = (score > bestScore) ? score : bestScore;
				}
			}
		}
		return bestScore;
	}
	else {
		int bestScore = INT_MAX;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				// Is the spot available?
				if (board[i][j] == 0) {
					board[i][j] = 1;
					int score = minimax(board, true);
					board[i][j] = 0;
					bestScore = (score < bestScore) ? score : bestScore;
				}
			}
		}
		return bestScore;
	}
}

//set x y for the first empty spot not used in the program after update
void setxy(int& x, int& y) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (table[i][j] == 0) {
				x = i;
				y = j;
				return;
			}
		}
	}
}

//function that call the algorithm minimax and set the move for the computer
void bestMove() {
	// AI to make its turn
	/*if (table[0][0] == 1 && table[1][1] == 0) {
		table[1][1] = 2;
		n2--;
		playerTurn = 1;
		return;
	}*/
	int bestScore = INT_MIN;
	int x = 0, y = 0;
	//setxy(x, y);
	//startGame = true;
	//gamestarted = true;
	//btnP1 = btnP2 = false;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			// Is the spot available?
			if (table[i][j] == 0) {
				table[i][j] = 2;
				int score = minimax(table, false);
				table[i][j] = 0;
				if (score > bestScore) {
					bestScore = score;
					x = i;
					y = j;
				}
			}
		}
	}
	table[x][y] = 2;
	n2--;
	totalTime = 15;
}

//play sound on lose draw and win and modify scores check the player list and update it
void sound() {
	q++;
	if (win > 0) {
		if (b[1].pressed)
			PlaySound(TEXT("win.wav"), NULL, SND_ASYNC);
		if (b[0].pressed)
			PlaySound(TEXT("lose.wav"), NULL, SND_ASYNC);
		if (win == 1) {
			scorep1++;
			if (!b[0].pressed) {
				if (p1 != "") {
					int i;
					for (i = 0; i < nbplayers; i++) {
						if (players[i].name == p1) {
							players[i].score++;
							break;
						}
					}
					if (i == nbplayers) {
						players[i].name = p1;
						players[i].score++;
						nbplayers++;

					}
				}
			}
		}
		else {
			scorep2++;
			if (!b[0].pressed) {
				if (p2 != "") {
					int i;
					for (i = 0; i < nbplayers; i++) {
						if (players[i].name == p2) {
							players[i].score++;
							break;
						}
					}
					if (i == nbplayers) {
						players[i].name = p2;
						players[i].score++;
						nbplayers++;
					}
				}
			}
		}
	}
	else
		PlaySound(TEXT("draw.wav"), NULL, SND_ASYNC);
}

//used for the timer function
void Timer(int value)
{
	if (totalTime > 0)
	{
		totalTime--;
		tiime = to_string(totalTime);
	}
	glutPostRedisplay();
	glutTimerFunc(1000, Timer, 0);
}

//draw line for the winning triplet
void drawline() {
	if (win == 1)
		glColor3f(1, 0.1, 0.1);
	else
		glColor3f(0.1, 0.1, 1);
	if (linewin == "r2") {
		glBegin(GL_POLYGON);
		glVertex2f(-0.6, 0.2);
		glVertex2f(-0.45, 0.215);
		glVertex2f(0.45, 0.215);
		glVertex2f(0.6, 0.2);
		glVertex2f(0.45, 0.185);
		glVertex2f(-0.45, 0.185);
		glEnd();
		glFlush();
	}
	if (linewin == "r1") {
		glBegin(GL_POLYGON);
		glVertex2f(-0.6, 0.2 - 0.4);
		glVertex2f(-0.45, 0.215 - 0.4);
		glVertex2f(0.45, 0.215 - 0.4);
		glVertex2f(0.6, 0.2 - 0.4);
		glVertex2f(0.45, 0.185 - 0.4);
		glVertex2f(-0.45, 0.185 - 0.4);
		glEnd();
		glFlush();
	}
	if (linewin == "r0") {
		glBegin(GL_POLYGON);
		glVertex2f(-0.6, 0.2 - 0.8);
		glVertex2f(-0.45, 0.215 - 0.8);
		glVertex2f(0.45, 0.215 - 0.8);
		glVertex2f(0.6, 0.2 - 0.8);
		glVertex2f(0.45, 0.185 - 0.8);
		glVertex2f(-0.45, 0.185 - 0.8);
		glEnd();
		glFlush();
	}
	if (linewin == "c2") {
		glBegin(GL_POLYGON);
		glVertex2f(0.2 + 0.2, -0.6 - 0.2);
		glVertex2f(0.215 + 0.2, -0.45 - 0.2);
		glVertex2f(0.215 + 0.2, 0.45 - 0.2);
		glVertex2f(0.2 + 0.2, 0.6 - 0.2);
		glVertex2f(0.185 + 0.2, 0.45 - 0.2);
		glVertex2f(0.185 + 0.2, -0.45 - 0.2);
		glEnd();
		glFlush();
	}
	if (linewin == "c1") {
		glBegin(GL_POLYGON);
		glVertex2f(0.2 - 0.2, -0.6 - 0.2);
		glVertex2f(0.215 - 0.2, -0.45 - 0.2);
		glVertex2f(0.215 - 0.2, 0.45 - 0.2);
		glVertex2f(0.2 - 0.2, 0.6 - 0.2);
		glVertex2f(0.185 - 0.2, 0.45 - 0.2);
		glVertex2f(0.185 - 0.2, -0.45 - 0.2);
		glEnd();
		glFlush();
	}
	if (linewin == "c0") {
		glBegin(GL_POLYGON);
		glVertex2f(0.2 - 0.6, -0.6 - 0.2);
		glVertex2f(0.215 - 0.6, -0.45 - 0.2);
		glVertex2f(0.215 - 0.6, 0.45 - 0.2);
		glVertex2f(0.2 - 0.6, 0.6 - 0.2);
		glVertex2f(0.185 - 0.6, 0.45 - 0.2);
		glVertex2f(0.185 - 0.6, -0.45 - 0.2);
		glEnd();
		glFlush();
	}
	if (linewin == "d1") {
		glBegin(GL_POLYGON);
		glVertex2f(0.58, 0.38);
		glVertex2f(0.2 + 0.104, 0 + 0.084);
		glVertex2f(-0.2 - 0.084, -0.4 - 0.104);
		glVertex2f(-0.58, -0.78);
		glVertex2f(-0.2 - 0.104, -0.4 - 0.084);
		glVertex2f(0.2 + 0.084, 0 + 0.104);

		glEnd();
		glFlush();
	}
	if (linewin == "d2") {
		glBegin(GL_POLYGON);
		glVertex2f(-0.58, 0.38);
		glVertex2f(-0.2 - 0.104, 0 + 0.084);
		glVertex2f(0.2 + 0.084, -0.4 - 0.104);
		glVertex2f(+0.58, -0.78);
		glVertex2f(+0.2 + 0.104, -0.4 - 0.084);
		glVertex2f(-0.2 - 0.084, 0 + 0.104);

		glEnd();
		glFlush();
	}
}

//play sound when time is running out 
void timesound() {
	if (totalTime <= 5 && gamestarted) {
		PlaySound(TEXT("clock.wav"), NULL, SND_ASYNC);
	}
}

//the main function for the 2 player mode
void twoPlayers() {
	//setting the graphics and data
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.015, 0.949, 0.78, 1);
	p1name();
	p2name();
	buttonG();
	buttonR();
	drawGrid();
	drawXO();
	//play sound on when game is over
	if (q == 0 && gameOver) {
		sound();
	}
	//display time left per round at the bottom of the window
	glColor3f(0, 0, 0);
	if (totalTime <= 5 && gamestarted) {
		glColor3f(1, 0, 0);
	}
	if (!gameOver && gamestarted) {
		drawStringMiddle(GLUT_BITMAP_HELVETICA_18, "      " + tiime + " s", -0.93);
		glBegin(GL_LINE_LOOP);
		for (int r = 0; r <= 360; r += 1) {
			glVertex2f(-0.1 + (0.06) * cos(r * PI / 180), -0.9 + (0.06) * sin(r * PI / 180));
		}
		glEnd();
		glBegin(GL_LINES);
		glVertex2f(-0.1, -0.9);
		glVertex2f(-0.1 + (0.04) * cos(totalTime * 24 * PI / 180 + PI / 2), -0.9 + (0.04) * sin(totalTime * 24 * PI / 180 + PI / 2));
		glEnd();
		//glFlush();
	}
	//if times is up player lose
	if (totalTime == 0 && gamestarted) {
		if (playerTurn == 1)
		{
			gameOver = true;
			win = 2; //player2 wins
		}
		else
			//playerTurn == 2
		{
			gameOver = true;
			win = 1; //player1 wins
		}
	}
	if (winCheck())//if true
	{
		//the player who made the previous move is the winner
		if (playerTurn == 1)
		{
			gameOver = true;
			win = 2; //player2 wins
		}
		else
			//playerTurn == 2
		{
			gameOver = true;
			win = 1; //player1 wins
		}
	}
	else if (drawCheck())//if true
	{
		linewin = "";
		gameOver = true;
		win = 0;
	}
	//display score
	if (scorep1 == scorep2)
		glColor3f(0, 0, 0);
	else if (scorep1 > scorep2)
		glColor3f(1, 0, 0);
	else
		glColor3f(0, 0, 1);
	drawStringMiddle(GLUT_BITMAP_TIMES_ROMAN_24, to_string(scorep1) + "-" + to_string(scorep2), 0.668);
	//when game over display message
	if (gameOver == true)
	{
		glColor3f(0, 0, 0);
		//Sleep(1000);
		drawStringMiddle(GLUT_BITMAP_HELVETICA_18, "Game Over", 0.52);
		if (win == 0)
			drawStringMiddle(GLUT_BITMAP_HELVETICA_18, "Its a draw", 0.42);

		if (win == 1) {
			glColor3f(1, 0, 0);
			drawStringMiddle(GLUT_BITMAP_HELVETICA_18, (p1 != "" ? p1 : "Player 1") + " wins", 0.42);
			drawline();
			glFlush();
		}
		if (win == 2) {
			glColor3f(0, 0, 1);
			drawStringMiddle(GLUT_BITMAP_HELVETICA_18, (p2 != "" ? p2 : "Player 2") + " wins", 0.42);
			drawline();
			glFlush();

		}
		glColor3f(0, 0, 0);
	}
	//display message for player turn
	if (gameOver == false)
		if (playerTurn == 1) {
			glColor3f(1, 0, 0);
			drawStringMiddle(GLUT_BITMAP_HELVETICA_18, ((p1 != "" ? p1 : "Player 1") + "'s turn"), 0.465);
		}
		else {
			glColor3f(0, 0, 1);
			drawStringMiddle(GLUT_BITMAP_HELVETICA_18, ((p2 != "" ? p2 : "Player 2") + "'s turn"), 0.465);
		}
	if (!gameOver)
		q = 0;
	glutSwapBuffers();
}

//main menu main function and graphics
void MainMenu() {
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.83, 0.68, 0.21, 1);
	glColor3f(0, 0, 0);
	glBegin(GL_TRIANGLES);
	glVertex2f(-1, 1);
	glVertex2f(-1, 0.8);
	glVertex2f(-0.8, 1);
	glVertex2f(1, 1);
	glVertex2f(1, 0.8);
	glVertex2f(0.8, 1);
	glVertex2f(-1, -1);
	glVertex2f(-1, -0.8);
	glVertex2f(-0.8, -1);
	glVertex2f(1, -1);
	glVertex2f(1, -0.8);
	glVertex2f(0.8, -1);
	glEnd();
	animation();
	b[0].text = "ONE PLAYER";
	b[0].y = 0.4;
	b[0].drawBtn();
	b[1].text = "TWO PLAYERS";
	b[1].y = 0.1;
	b[1].drawBtn();
	b[2].text = "LEADERBOARD";
	b[2].y = -0.2;
	b[2].drawBtn();
	b[3].text = "EXIT";
	b[3].y = -0.5;
	b[3].drawBtn();
	glutPostRedisplay();
	glutSwapBuffers();
}

//leader board main function and graphics
void leaderboard() {
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.031, 0.266, 0.066, 1);//dark green
	glColor3f(0, 0, 0);
	glBegin(GL_TRIANGLES);
	glVertex2f(-1, 1);
	glVertex2f(-1, 0.8);
	glVertex2f(-0.8, 1);
	glVertex2f(1, 1);
	glVertex2f(1, 0.8);
	glVertex2f(0.8, 1);
	glVertex2f(-1, -1);
	glVertex2f(-1, -0.8);
	glVertex2f(-0.8, -1);
	glVertex2f(1, -1);
	glVertex2f(1, -0.8);
	glVertex2f(0.8, -1);
	glEnd();
	animation();
	glColor3f(0.83, 0.68, 0.21);//gold
	glBegin(GL_QUADS);
	glVertex2f(-0.5, 0.1);
	glVertex2f(-0.5, -0.05);
	glVertex2f(0.5, -0.05);
	glVertex2f(0.5, 0.1);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-0.5, 0.1);
	glVertex2f(-0.5, -0.05);
	glVertex2f(0.5, -0.05);
	glVertex2f(0.5, 0.1);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex2f(-0.5 + 0.02, 0.1 - 0.02);
	glVertex2f(-0.5 + 0.02, -0.05 + 0.02);
	glVertex2f(0.5 - 0.02, -0.05 + 0.02);
	glVertex2f(0.5 - 0.02, 0.1 - 0.02);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex2f(-0.5 + 0.01, 0.1 - 0.01);
	glVertex2f(-0.5 + 0.01, -0.05 + 0.01);
	glVertex2f(0.5 - 0.01, -0.05 + 0.01);
	glVertex2f(0.5 - 0.01, 0.1 - 0.01);
	glEnd();
	glColor3f(0.843, 0.843, 0.843);//silver
	glBegin(GL_QUADS);
	glVertex2f(-0.5, 0.1 - 0.25);
	glVertex2f(-0.5, -0.05 - 0.25);
	glVertex2f(0.5, -0.05 - 0.25);
	glVertex2f(0.5, 0.1 - 0.25);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-0.5, 0.1 - 0.25);
	glVertex2f(-0.5, -0.05 - 0.25);
	glVertex2f(0.5, -0.05 - 0.25);
	glVertex2f(0.5, 0.1 - 0.25);
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex2f(-0.5 + 0.02, 0.1 - 0.25 - 0.02);
	glVertex2f(-0.5 + 0.02, -0.05 - 0.25 + 0.02);
	glVertex2f(0.5 - 0.02, -0.05 - 0.25 + 0.02);
	glVertex2f(0.5 - 0.02, 0.1 - 0.25 - 0.02);
	glEnd();
	//80.4, 49.8, 19.6
	glColor3f(0.804, 0.498, 0.196);//bronze
	glBegin(GL_QUADS);
	glVertex2f(-0.5, 0.1 - 0.5);
	glVertex2f(-0.5, -0.05 - 0.5);
	glVertex2f(0.5, -0.05 - 0.5);
	glVertex2f(0.5, 0.1 - 0.5);
	glEnd();
	glColor3f(0, 0, 0);
	glBegin(GL_LINE_LOOP);
	glVertex2f(-0.5, 0.1 - 0.5);
	glVertex2f(-0.5, -0.05 - 0.5);
	glVertex2f(0.5, -0.05 - 0.5);
	glVertex2f(0.5, 0.1 - 0.5);
	glEnd();
	if (nbplayers >= 3) {
		drawStringMiddle(GLUT_BITMAP_HELVETICA_18, "I- " + players[nbplayers - 1].name + " " + to_string(players[nbplayers - 1].score) + " wins!", 0.0);
		drawStringMiddle(GLUT_BITMAP_HELVETICA_18, "II- " + players[nbplayers - 2].name + " " + to_string(players[nbplayers - 2].score) + " wins!", -0.25);
		drawStringMiddle(GLUT_BITMAP_HELVETICA_18, "III- " + players[nbplayers - 3].name + " " + to_string(players[nbplayers - 3].score) + " wins!", -0.5);
	}
	else if (nbplayers == 2) {
		drawStringMiddle(GLUT_BITMAP_HELVETICA_18, "I- " + players[nbplayers - 1].name + " " + to_string(players[nbplayers - 1].score) + " wins!", 0.0);
		drawStringMiddle(GLUT_BITMAP_HELVETICA_18, "II- " + players[nbplayers - 2].name + " " + to_string(players[nbplayers - 2].score) + " wins!", -0.25);
		drawStringMiddle(GLUT_BITMAP_HELVETICA_18, "NON", -0.5);
	}
	else if (nbplayers == 1) {
		drawStringMiddle(GLUT_BITMAP_HELVETICA_18, "I- " + players[nbplayers - 1].name + " " + to_string(players[nbplayers - 1].score) + " wins!", 0.0);
		drawStringMiddle(GLUT_BITMAP_HELVETICA_18, "NON", -0.25);
		drawStringMiddle(GLUT_BITMAP_HELVETICA_18, "NON", -0.5);
	}
	else if (nbplayers == 0) {
		drawStringMiddle(GLUT_BITMAP_HELVETICA_18, "NON", 0.0);
		drawStringMiddle(GLUT_BITMAP_HELVETICA_18, "NON", -0.25);
		drawStringMiddle(GLUT_BITMAP_HELVETICA_18, "NON", -0.5);
	}
	glutPostRedisplay();
	glutSwapBuffers();
}

//randomize the computer move not used anymore after implemention of the ai algorithm
void computermove() {
	if (!drawCheck()) {
		startGame = true;
		gamestarted = true;
		int x;
		int y;
		do {
			x = rand() % 3;
			y = rand() % 3;
		} while (table[x][y] != 0);
		table[x][y] = 2;
		n2--;
	}
}

//main function for the 1 player mode
void oneplayer() {
	//setting graphics and data
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(0.015, 0.949, 0.78, 1);
	p1 = "Player";
	p1name();
	p2 = "Computer";
	p2name();
	buttonG();
	buttonR();
	drawGrid();
	drawXO();
	//play sound on draw and lose
	if (q == 0 && gameOver) {
		sound();
	}
	//display time left at the bottom of the window
	glColor3f(0, 0, 0);
	if (totalTime <= 5 && gamestarted) {
		glColor3f(1, 0, 0);
	}
	if (!gameOver && gamestarted) {
		drawStringMiddle(GLUT_BITMAP_HELVETICA_18, "      " + tiime + " s", -0.93);
		glBegin(GL_LINE_LOOP);
		for (int r = 0; r <= 360; r += 1) {
			glVertex2f(-0.1 + (0.06) * cos(r * PI / 180), -0.9 + (0.06) * sin(r * PI / 180));
		}
		glEnd();
		glBegin(GL_LINES);
		glVertex2f(-0.1, -0.9);
		glVertex2f(-0.1 + (0.04) * cos(totalTime * 24 * PI / 180 + PI / 2), -0.9 + (0.04) * sin(totalTime * 24 * PI / 180 + PI / 2));
		glEnd();
	}
	//if the time is up player lose
	if (totalTime == 0 && gamestarted) {
		linewin = "";
		if (playerTurn == 1)
		{
			gameOver = true;
			win = 2; //player2 wins
		}
		else
			//playerTurn == 2
		{
			gameOver = true;
			win = 1; //player1 wins
		}
	}
	//check for the winner
	if (winCheck() && !gameOver)//if true
	{
		//the player who made the previous move is the winner
		if (playerTurn == 1)
		{
			gameOver = true;
			win = 2; //player2 wins
		}
		else
			//playerTurn == 2
		{
			gameOver = true;
			win = 1; //player1 wins
		}
	}
	else if (drawCheck())//if true
	{
		linewin = "";
		gameOver = true;
		win = 0;
	}
	//display score
	if (scorep1 == scorep2)
		glColor3f(0, 0, 0);
	else if (scorep1 > scorep2)
		glColor3f(1, 0, 0);
	else
		glColor3f(0, 0, 1);
	//drawStringMiddle(GLUT_BITMAP_TIMES_ROMAN_24, to_string(scorep1) + "-" + to_string(scorep2), 0.668);
	//display message on game end
	if (gameOver == true)
	{
		glColor3f(0, 0, 0);
		//Sleep(1000);
		drawStringMiddle(GLUT_BITMAP_HELVETICA_18, "Game Over", 0.52);
		drawline();
		if (win == 0)
			drawStringMiddle(GLUT_BITMAP_HELVETICA_18, "Its a draw", 0.42);
		else {
			drawStringMiddle(GLUT_BITMAP_HELVETICA_18, (p2 != "" ? p2 : "Player 2") + " wins", 0.42);
			glFlush();
		}
	}
	glColor3f(0, 0, 0);
	//diplay message for player turn
	if (gameOver == false)
		if (playerTurn == 1) {
			glColor3f(1, 0, 0);
			drawStringMiddle(GLUT_BITMAP_HELVETICA_18, ((p1 != "" ? p1 : "Player 1") + "'s turn"), 0.465);
		}
		else {
			glColor3f(0, 0, 1);
			drawStringMiddle(GLUT_BITMAP_HELVETICA_18, ((p2 != "" ? p2 : "Player 2") + "'s turn"), 0.465);
		}
	if (!gameOver)
		q = 0;
	//if(playerTurn==2)
	if (playerTurn == 2 && !gameOver) {
		//computermove();
		bestMove();
		playerTurn = 1;
	}
	glutSwapBuffers();
}

//Display function that navigates through available option for the window main function
void display()
{
	if (menubool) {
		gamestarted = false;
		scorep1 = scorep2 = 0;
		p2 = "";
		p1 = "";
		MainMenu();
	}
	if (b[0].pressed) {
		oneplayer();
	}
	if (b[1].pressed) {

		twoPlayers();
	}
	if (b[2].pressed) {
		leaderboard();
	}

	if (b[3].pressed)
		exit(0);
}

//This function is called on windows resize
void resize(int width, int height) {
	// we ignore the params and do:
	glutReshapeWindow(X, X);
}

//keep the list of player sorted by score ascending
void insertionSort(Player a[], int size) {
	Player temp;
	int j;
	for (int i = 1; i < size; i++) {
		temp = a[i];
		for (j = i; i > 0 && a[j - 1].score > temp.score; j--) {
			a[j] = a[j - 1];
		}
		a[j] = temp;
	}
}
