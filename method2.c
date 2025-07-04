#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 20

int gameOver;
int x, y, fruitX, fruitY, score;
int tailX[100], tailY[100];
int nTail;
enum eDirection { STOP = 0, LEFT, RIGHT, UP, DOWN };
enum eDirection dir;

HANDLE hConsole;
CHAR_INFO screenBuffer[(HEIGHT + 4) * (WIDTH + 2)];
COORD bufferSize = {WIDTH + 2, HEIGHT + 4};
COORD bufferCoord = {0, 0};
SMALL_RECT writeRegion = {0, 0, WIDTH + 1, HEIGHT + 3};

void Setup() {
    gameOver = 0;
    dir = STOP;
    x = WIDTH / 2;
    y = HEIGHT / 2;
    fruitX = rand() % WIDTH;
    fruitY = rand() % HEIGHT;
    score = 0;
    
    // Get console handle and hide cursor
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void Draw() {
    // Clear screen buffer
    for (int i = 0; i < (HEIGHT + 4) * (WIDTH + 2); i++) {
        screenBuffer[i].Char.AsciiChar = ' ';
        screenBuffer[i].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }
    
    // Top border
    for (int i = 0; i < WIDTH + 2; i++) {
        screenBuffer[i].Char.AsciiChar = '#';
    }
    
    // Game area
    for (int i = 0; i < HEIGHT; i++) {
        int rowStart = (i + 1) * (WIDTH + 2);
        
        // Left border
        screenBuffer[rowStart].Char.AsciiChar = '#';
        // Right border
        screenBuffer[rowStart + WIDTH + 1].Char.AsciiChar = '#';
        
        for (int j = 0; j < WIDTH; j++) {
            int pos = rowStart + j + 1;
            
            if (i == y && j == x) {
                screenBuffer[pos].Char.AsciiChar = 'O'; // Snake head
                screenBuffer[pos].Attributes = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            }
            else if (i == fruitY && j == fruitX) {
                screenBuffer[pos].Char.AsciiChar = 'F'; // Fruit
                screenBuffer[pos].Attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
            }
            else {
                int print = 0;
                for (int k = 0; k < nTail; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        screenBuffer[pos].Char.AsciiChar = 'o'; // Snake tail
                        screenBuffer[pos].Attributes = FOREGROUND_GREEN;
                        print = 1;
                        break;
                    }
                }
                if (!print) {
                    screenBuffer[pos].Char.AsciiChar = ' ';
                }
            }
        }
    }
    
    // Bottom border
    int bottomRow = (HEIGHT + 1) * (WIDTH + 2);
    for (int i = 0; i < WIDTH + 2; i++) {
        screenBuffer[bottomRow + i].Char.AsciiChar = '#';
    }
    
    // Score line
    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);
    int scoreRow = (HEIGHT + 2) * (WIDTH + 2);
    for (int i = 0; i < strlen(scoreText); i++) {
        screenBuffer[scoreRow + i].Char.AsciiChar = scoreText[i];
    }
    
    // Instructions line
    char instructions[] = "Use WASD to move, X to quit";
    int instrRow = (HEIGHT + 3) * (WIDTH + 2);
    for (int i = 0; i < strlen(instructions); i++) {
        screenBuffer[instrRow + i].Char.AsciiChar = instructions[i];
    }
    
    // Write buffer to console
    WriteConsoleOutput(hConsole, screenBuffer, bufferSize, bufferCoord, &writeRegion);
}

void Input() {
    if (_kbhit()) {
        switch (_getch()) {
            case 'a':
                dir = LEFT;
                break;
            case 'd':
                dir = RIGHT;
                break;
            case 'w':
                dir = UP;
                break;
            case 's':
                dir = DOWN;
                break;
            case 'x':
                gameOver = 1;
                break;
        }
    }
}

void Logic() {
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;
    tailX[0] = x;
    tailY[0] = y;
    
    for (int i = 1; i < nTail; i++) {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    
    switch (dir) {
        case LEFT:
            x--;
            break;
        case RIGHT:
            x++;
            break;
        case UP:
            y--;
            break;
        case DOWN:
            y++;
            break;
        default:
            break;
    }
    
    // Check wall collision
    if (x >= WIDTH) x = 0; else if (x < 0) x = WIDTH - 1;
    if (y >= HEIGHT) y = 0; else if (y < 0) y = HEIGHT - 1;
    
    // Check tail collision
    for (int i = 0; i < nTail; i++) {
        if (tailX[i] == x && tailY[i] == y)
            gameOver = 1;
    }
    
    // Check fruit collision
    if (x == fruitX && y == fruitY) {
        score += 10;
        fruitX = rand() % WIDTH;
        fruitY = rand() % HEIGHT;
        nTail++;
    }
}

int main() {
    srand(time(0));
    Setup();
    
    while (!gameOver) {
        Draw();
        Input();
        Logic();
        Sleep(100); // Control game speed
    }
    
    printf("\nGame Over! Final Score: %d\n", score);
    printf("Press any key to exit...\n");
    _getch();
    
    return 0;
}