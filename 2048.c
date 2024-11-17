#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>

#define SIZE 4 // Size of the board
#define GEN4PROB 25 // Probability(%) of generating a 4.
#define EXIT_KEY 27 // The key to exit the game. The default key is Esc.

int score = 0;
int board[SIZE][SIZE];
struct Cell{int val, x, y;};
bool colorized;

void InitEnv(){
    // Check whether the OS is Windows10 or higher by Registry, and decide whether to apply colorized output.
    // Reference: https://www.oryoy.com/news/ru-he-zai-c-yu-yan-zhong-jian-ce-he-shi-bie-windows-cao-zuo-xi-tong-de-ban-ben-xin-xi.html
    HKEY hKey;
    char version[64] = {0};
    DWORD versionSize = sizeof(version);
    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS){
        if(RegQueryValueEx(hKey, "ProductName", NULL, NULL, (LPBYTE)version, &versionSize) == ERROR_SUCCESS){
            if(strstr(version, "Windows 10") != NULL || strstr(version, "Windows 11") != NULL){
                colorized = true;
            }
        }
        RegCloseKey(hKey);
    }
    system("title 2048!");
}

bool genRandomCell(){
    // Get all empty cells
    struct Cell emptyCells[SIZE * SIZE];
    int tot = 0;
    for(int i = 0; i < SIZE; ++i){
        for(int j = 0; j < SIZE; ++j){
            if(!board[i][j]){
                emptyCells[tot].val = 0;
                emptyCells[tot].x = i;
                emptyCells[tot++].y = j;
            }
        }
    }
    // Pick a random empty cell
    if(!tot) return false;
    int idx = rand() % tot;
    int value = (rand() % 100 < GEN4PROB) ? 4 : 2;
    board[emptyCells[idx].x][emptyCells[idx].y] = value;
    return true;
}

void printBoard(){
    system("cls");
    puts("2048 Game. By Coast23.\n");
    for(int i = 0; i < SIZE; ++i) printf("+-----"); puts("+");
    for(int i = 0; i < SIZE; ++i){
        for(int j = 0; j < SIZE; ++j)
            if(board[i][j]){
                if(!colorized) printf("|%5d", board[i][j]);
                else{
                    switch(board[i][j]){
                        case 2: printf("|%s%5d\033[0m", "\033[48;2;238;228;218m\033[1;38;2;119;110;101m", board[i][j]); break;
                        case 4: printf("|%s%5d\033[0m", "\033[48;2;237;224;200m\033[1;38;2;119;110;101m", board[i][j]); break;
                        case 8: printf("|%s%5d\033[0m", "\033[48;2;242;177;121m\033[1;38;2;249;246;242m", board[i][j]); break;
                        case 16: printf("|%s%5d\033[0m", "\033[48;2;245;149;99m\033[1;38;2;249;246;242m", board[i][j]); break;
                        case 32: printf("|%s%5d\033[0m", "\033[48;2;246;124;95m\033[1;38;2;249;246;242m", board[i][j]); break;
                        case 64: printf("|%s%5d\033[0m", "\033[48;2;246;94;59m\033[1;38;2;249;246;242m", board[i][j]); break;
                        case 128: printf("|%s%5d\033[0m", "\033[48;2;237;207;114m\033[1;38;2;249;246;242m", board[i][j]); break;
                        case 256: printf("|%s%5d\033[0m", "\033[48;2;237;204;97m\033[1;38;2;249;246;242m", board[i][j]); break;
                        case 512: printf("|%s%5d\033[0m", "\033[48;2;237;200;80m\033[1;38;2;249;246;242m", board[i][j]); break;
                        case 1024: printf("|%s%5d\033[0m", "\033[48;2;237;197;63m\033[1;38;2;249;246;242m", board[i][j]); break;
                        case 2048: printf("|%s%5d\033[0m", "\033[48;2;237;194;46m\033[1;38;2;249;246;242m", board[i][j]); break;
                        default: printf("|%s%5d\033[0m", "\033[48;2;60;58;50m\033[1;38;2;249;246;242m", board[i][j]); break;
                    }
                }
            }
            else printf("|     ");
        puts("|");
        for(int j = 0; j < SIZE; ++j) printf("+-----"); puts("+");
        fflush(stdout);
    }
    printf("\nScore: %d\n", score);
}

// The most difficult part of the code.
// I decide to deal all directions separately.
// Yeah, this sounds very foolish, but it can reduce bugs and make the code more readable.
bool moveAndMerge(unsigned char direction){
    /*
    Return values:
        true if the board is changed.
        false if the board is unchanged.
    */
    char tmp[SIZE][SIZE];
    for(int i = 0; i < SIZE; ++i) for(int j = 0; j < SIZE; ++j) tmp[i][j] = board[i][j];

    switch(direction){
        case 'W':
            for(int col = 0; col < SIZE; ++col){
                // Merge
                for(int row = 1; row < SIZE; ++row){
                    if(!board[row][col]) continue; // Skip empty cells.
                    int target = row;
                    while(target && !board[target-1][col]) --target; --target;// Find the first non-empty cell.
                    if(target < 0 || board[target][col] != board[row][col]) continue;
                    board[target][col] <<= 1;
                    score += board[target][col];
                    board[row][col] = -1; // Mark as merged.
                }
                // Move
                for(int row = 0; row < SIZE; ++row){
                    if(board[row][col] == -1) board[row][col] = 0;
                    if(!board[row][col]) continue;
                    int destination = row;
                    while(destination && !board[destination-1][col]) --destination;
                    if(destination == row) continue; // No need to move.
                    board[destination][col] = board[row][col];
                    board[row][col] = 0;
                }
            }
            break;
        case 'S':
            for(int col = 0; col < SIZE; ++col){
                // Merge
                for(int row = SIZE - 2; ~row; --row){
                    if(!board[row][col]) continue; // Skip empty cells.
                    int target = row;
                    while(target < SIZE-1 && !board[target+1][col]) ++target; ++target;
                    if(target >= SIZE || board[target][col] != board[row][col]) continue;
                    board[target][col] <<= 1;
                    score += board[target][col];
                    board[row][col] = -1;
                }
                // Move
                for(int row = SIZE - 1; ~row; --row){
                    if(board[row][col] == -1) board[row][col] = 0;
                    if(!board[row][col]) continue;
                    int destination = row;
                    while(destination < SIZE-1 && !board[destination+1][col]) ++destination;
                    if(destination == row) continue; // No need to move.
                    board[destination][col] = board[row][col];
                    board[row][col] = 0;
                }
            }
            break;
        case 'A':
            for(int row = 0; row < SIZE; ++row){
                // Merge
                for(int col = 1; col < SIZE; ++col){
                    int target = col;
                    while(target && !board[row][target-1]) --target; --target;
                    if(!board[row][col] || board[row][col] != board[row][target]) continue;
                    board[row][target] <<= 1;
                    score += board[row][target];
                    board[row][col] = -1;
                }
                // Move
                for(int col = 0; col < SIZE; ++col){
                    if(board[row][col] == -1) board[row][col] = 0;
                    if(!board[row][col]) continue;
                    int destination = col;
                    while(destination && !board[row][destination-1]) --destination;
                    if(destination == col) continue; // No need to move.
                    board[row][destination] = board[row][col];
                    board[row][col] = 0;
                }
            }
            break;
        case 'D':
            for(int row = 0; row < SIZE; ++row){
                // Merge
                for(int col = SIZE - 2; ~col; --col){
                    int target = col;
                    while(target < SIZE-1 && !board[row][target+1]) ++target; ++target;
                    if(!board[row][col] || board[row][col] != board[row][target]) continue;
                    board[row][target] <<= 1;
                    score += board[row][target];
                    board[row][col] = -1;
                }
                // Move
                for(int col = SIZE - 1; ~col; --col){
                    if(board[row][col] == -1) board[row][col] = 0;
                    if(!board[row][col]) continue;
                    int destination = col;
                    while(destination < SIZE-1 && !board[row][destination+1]) ++destination;
                    if(destination == col) continue; // No need to move.
                    board[row][destination] = board[row][col];
                    board[row][col] = 0;
                }
            }
            break;
    }
    bool flag = false;
    for(int i = 0; i < SIZE; ++i) for(int j = 0; j < SIZE; ++j) if(tmp[i][j] != board[i][j]) flag = true;
    return flag;
}

int onUserInput(){
    /*
    Return values:
        -1: Exit Game
        0: Invalid Input
        1: move and merge successfully
    */
    unsigned char cmd = getch();
    int ret = 0;
    if(cmd == EXIT_KEY) return -1;
    if(islower(cmd)) cmd &= 95; // Convert to uppercases.
    if(cmd == 224){
        unsigned char arrow = getch();
        /*
            Arrow keys:
            72: Up
            80: Down
            77: Right
            75: Left
        */
        switch(arrow){
            case 72: ret = moveAndMerge('W'); break;
            case 75: ret = moveAndMerge('A'); break;
            case 80: ret = moveAndMerge('S'); break;
            case 77: ret = moveAndMerge('D'); break;
        }
    }
    else if(cmd == 'W' || cmd == 'A' || cmd == 'S' || cmd == 'D')
        ret = moveAndMerge(cmd);
    return ret;
}

bool isGameOver(){
    for(int i = 0; i < SIZE; ++i) for(int j = 0; j < SIZE; ++j)
        if(!board[i][j]) return false;

    for(int i = 0; i < SIZE; ++i) for(int j = 0; j < SIZE; ++j)
        if(i > 0 && board[i][j] == board[i-1][j] || j > 0 && board[i][j] == board[i][j-1]) return false;
    return true;
}

int main(){
    srand(time(NULL)); InitEnv();
    genRandomCell(); genRandomCell(); printBoard();
    int opCode = 0;
    while(true){
        opCode = onUserInput();
        if(opCode == -1) break; // Or do something, like saving the game. Marked as TODO.
        if(opCode == 0) continue; // Do nothing
        opCode = genRandomCell();
        printBoard();
        if(isGameOver()){
            printf("Game Over! Final Score: %d\n", score);
            system("pause");
            break;
        }
    }
    return 0^(0-0)^0;
}