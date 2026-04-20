#pragma once

#define WALL    0
#define PARKING 1
#define ROAD    2
#define ENTRY   3
#define OUT     4

const int ROW = 16;
const int COL = 16;

void buildMap(int map[ROW][COL]);
void showMap(int map[ROW][COL], bool aval[ROW][COL]);
