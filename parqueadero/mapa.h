#pragma once

#define PARKING 1
#define WALL 0
#define ROAD 2
#define ENTRY 3
#define OUT 4

const int ROW = 16;
const int COL = 16;

void buildMap(int mapa[ROW][COL]);
void showMap(int mapa[ROW][COL], bool aval[ROW][COL]);
