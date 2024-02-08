#include <iostream>
#include <random>
#include <fstream>
#include <map>

using namespace std;

#define MAX 100
#define DEBUG 0

void drawBoard(char matrix[][MAX], int n, int m);
void drawPath(char matrix[][MAX], const int bestpath[MAX], int n, int m, int startx);
int makeRandomBoard(char matrix[][MAX], int n, int m);
void findWays(char map[][MAX], int firstpath[MAX], int bestpath[MAX], int mapdata[][MAX][4], int n, int m, int startx);
void loadMapData(int mapdata[][MAX][4], int n, int m);
void saveMapData(int bestpath[MAX], int mapdata[][MAX][4], int n, int m, int startx);
void copyArray(const int a1[MAX], int a2[MAX]);
void sortdir(int mapdata[][MAX][4], int  dir[4][3], const int point[2]);
void printStatisticsOfThePoint(int mapdata[][MAX][4], int point[2]);

int main() {
    char map[MAX][MAX] = {{0}};
    int bestpath[MAX]; // First element is the length of the path, the rest is the directions
    int firstpath[MAX];
    int mapdata[MAX][MAX][4] = {{0}};

    int n = 10, m = 16;  // n is the number of rows, m is the number of columns
    int startx;

    startx = makeRandomBoard(map, n, m);
    drawBoard(map, n, m);

    // Loads mapdata from txt file
    loadMapData(mapdata, n, m);

    // find the shortest path
    map[0][startx] = 1;
    bestpath[0] = n*m;
    findWays(map, firstpath, bestpath, mapdata, n, m, startx);

    // Save map data
    saveMapData(bestpath, mapdata, n, m, startx);

    // Print first found path
    cout << endl << endl << "First found path: \n";
    drawPath(map, firstpath, n, m, startx);
    // Print shortest path
    cout << endl << endl << "Shortest path: \n";
    drawPath(map, bestpath, n, m, startx);

    cout << endl << endl << "Length of First Path\t\tLength of Shortest Path\t\tDifference" << endl;
    cout << "\t\t" << firstpath[0] << "\t\t\t\t\t\t\t" << bestpath[0] << "\t\t\t\t\t\t" <<  firstpath[0] - bestpath[0];
    return 0;
}


int makeRandomBoard(char matrix[][MAX], int n, int m) {
    int startx, finishx;
    int point[2];

    random_device rd;
    uniform_int_distribution dist(0,5000);

    // Put Start and Finish points
    startx = rd() % m;
    finishx = rd() % m;

    point[0] = startx;
    point[1] = 0;

    // Guarantee that there will be at least one path from start to finish
    while(point[0] != finishx or point[1] < n-1){

        if(point[1] < n-1 and rd()%2){
            point[1]++;
        }
        else if((point[0] - finishx) < 0) {
            point[0]++;
        }
        else if((point[0] - finishx) > 0){
            point[0]--;
        }

        if ((point[0] != finishx or (point[1] != n-1)) and (point[0] != startx or point[1] != 0))
            matrix[point[1]][point[0]] = 1;
    }

    // Remove some walls
    for (int i = 0; i < rd()%(n*m-100)+110; i++) {
        matrix[rd()%n][rd()%m] = 1;
    }

    matrix[0][startx] = 'S';
    matrix[n-1][finishx] = 'F';

    return startx;
}

void drawBoard(char matrix[][MAX], int n, int m) {
    int i, j, k;
    cout << "\n ";
    for (k = 0; k < m; k++) {
        if(k == 10)
            cout << " ";
        printf(" %3d", k);
    }
    for (i = 0; i < n; i++) {
        cout << "\n  -";
        for (k = 0; k < m; k++)
            cout << "----";
        cout << "\n" << i << " |";

        for (j = 0; j < m; j++) {
            if (matrix[i][j] == 0) {
                cout << "##" << "#|";
            }
            else if (matrix[i][j] == 1) {
                cout << "   |";
            }
            else{
                if (matrix[i][j] == 'S' || matrix[i][j] == 'F' || matrix[i][j] == '+') {
                    cout << "\033[1;32m"; // Set the text color to green
                }
                cout << " " << matrix[i][j];
                cout << "\033[0m"; // Reset the text color
                cout << " |";
            }
        }
    }
    cout << "\n  -";
    for (k = 0; k < m; k++)
        cout << "----";
}

void drawPath(char matrix[][MAX], const int bestpath[MAX], int n, int m, int startx) {
    int i,j;
    int point[2] = {0, startx};

    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            if (matrix[i][j] == '+')
                matrix[i][j] = '-';
        }
    }

    matrix[point[0]][point[1]] = 'S'; // Start point
    for (i = 1; i < bestpath[0]+1; i++) {
        switch (bestpath[i]) {
            case 'v':
                point[0]++;
                break;
            case '>':
                point[1]++;
                break;
            case '<':
                point[1]--;
                break;
            case '^':
                point[0]--;
                break;
        }
        matrix[point[0]][point[1]] = '+';
    }

    drawBoard(matrix, n, m);
}

void findWays(char map[][MAX], int firstpath[MAX], int bestpath[MAX], int mapdata[][MAX][4], int n, int m, int startx){
    static bool isfirstrun = true;
    static int path[MAX] = {0}; // First element is the length of the path, the rest is the directions
    static int point[2] = {0, startx};
    int dir[4][3] = {{1,0,'v'},{0,1,'>'},{0,-1,'<'},{-1,0,'^'}};

    // Debug
    if (DEBUG)
        printStatisticsOfThePoint(mapdata, point);

     // Sort directions according to their possibility of reaching the finish point
    sortdir(mapdata, dir, point);

    for (int* d : dir) {
        // Move the robot according to the direction
        map[point[0]][point[1]] = '+';
        point[0] += d[0];
        point[1] += d[1];

        // Find the shortest path
        if (map[point[0]][point[1]] == 'F'){
            if(isfirstrun){
                copyArray(path, firstpath);
                isfirstrun = false;
            }

            if (path[0] < bestpath[0]){
                copyArray(path, bestpath);
                // print shorter path
                if (DEBUG){
                    cout << endl << endl << "Final Point was Found! length: "  << path[0] << endl;
                    drawBoard(map, n, m);
                }
            }


        }

        // if robot tackles borders or encounters a wall or already visited point
        if ((point[0] >= 0 and point[0] < n and point[1] >= 0 and point[1] < m) and (map[point[0]][point[1]] == 1 or map[point[0]][point[1]] == '-')){
            map[point[0]][point[1]] = d[2];
            path[++path[0]] = d[2]; // length++

            // Debug
            if (DEBUG){
                cout << endl << "point: " << point[1] << " " << point[0] << endl;
                drawBoard(map, n, m);
            }

            findWays(map, firstpath, bestpath, mapdata, n, m, startx);
            path[0]--; // length--
            // Move the robot back
            map[point[0]][point[1]] = '-';
        }

        point[0] -= d[0];
        point[1] -= d[1];
    }
}

void loadMapData(int mapdata[][MAX][4], int n, int m){
    ifstream file("data.txt");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            for (int k = 0; k < m; k++) {
                file >> mapdata[i][j][k];
            }
        }
    }
    file.close();
}

void saveMapData(int bestpath[MAX], int mapdata[][MAX][4], int n, int m, int startx){
    map<char, short> dirmap{{'v', 0}, {'>', 1}, {'<', 2}, {'^', 3}};
    int point[2] = {0, startx};

    for (int i = 1; i < bestpath[0]+1; i++) {
        mapdata[point[0]][point[1]][dirmap[bestpath[i]]]++;
        switch (bestpath[i]) {
            case 'v':
                point[0]++;
                break;
            case '>':
                point[1]++;
                break;
            case '<':
                point[1]--;
                break;
            case '^':
                point[0]--;
                break;
        }
    }

    ofstream file("data.txt");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            for (int k = 0; k < m; k++) {
                file << mapdata[i][j][k];
                file << '\n';
            }
        }
    }
    file.close();
}

void printStatisticsOfThePoint(int mapdata[][MAX][4], int point[2]){
    char dir[4] = {'v', '>', '<', '^'};
    cout << endl << "Statistics of the point: " << point[1] << " " << point[0] << endl;
    for (int i = 0; i < 4; i++) {
        cout << dir[i] << ": " << mapdata[point[0]][point[1]][i] << endl;
    }
}

void sortdir(int mapdata[][MAX][4], int  dir[4][3], const int point[2]){
    int tmp;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3-i; j++) {
            if(mapdata[point[0]][point[1]][j]<mapdata[point[0]][point[1]][j+1]){
                tmp = mapdata[point[0]][point[1]][j];
                mapdata[point[0]][point[1]][j] = mapdata[point[0]][point[1]][j+1];
                mapdata[point[0]][point[1]][j+1] = tmp;

                for (int k = 0; k < 3; k++) {
                    tmp = dir[j][k];
                    dir[j][k] = dir[j+1][k];
                    dir[j+1][k] = tmp;
                }
            }
        }
    }
}

void copyArray(const int a1[MAX], int a2[MAX]){
    int i;
    for ( i = 0; i < a1[0]+1; i++)
        a2[i] = a1[i];
}