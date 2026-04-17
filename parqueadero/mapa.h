#pragma once

#define PARQUEADERO 1
#define PARED 0
#define VIA 2
#define ENTRADA 3
#define SALIDA 4

const int ROWS = 16;
const int COLS = 16;

void buildMap(int mapa[ROWS][COLS]);
void showMap(int mapa[FILAS][COLS], bool aval[][])
