#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <stack>
#include <queue>
#include <map>
#include <fstream>
#include <windows.h>
#include <cmath>


using namespace std;


void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

struct Pengeluaran {
    string tanggal;
    string keterangan;
    string kategori;
    double jumlah;
};

bool bandingTanggal(const string& t1, const string& t2) {
    int d1 = stoi(t1.substr(0, 2));
    int m1 = stoi(t1.substr(3, 2));
    int y1 = stoi(t1.substr(6, 4));


    int d2 = stoi(t2.substr(0, 2));
    int m2 = stoi(t2.substr(3, 2));
    int y2 = stoi(t2.substr(6, 4));


    if (y1 != y2) return y1 < y2;
    if (m1 != m2) return m1 < m2;
    return d1 < d2;
}
