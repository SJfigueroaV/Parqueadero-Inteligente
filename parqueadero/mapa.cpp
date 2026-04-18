#include "mapa.h"
#include <iostream>

void buildMap( int map[ROW][COL]) {

  int initialDesign[ROW][COL] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 4, 0},
    {0, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 0},
    {0, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 0},
    {0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0},
    {0, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 0},
    {0, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 0},
    {0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0},
    {0, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 0},
    {0, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 0},
    {0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0},
    {0, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 0},
    {0, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 0},
    {0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0},
    {0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
  };
  //llenamos el mapa que vamos a utilizar con el diseño inicial que creamos
  for (int i = 0; i < ROW; i++){
    for (int j = 0; j < COL; j++){
      map[i][j] = initialDesign[i][j];   
    }
  }
}

void showMap(int mapa[ROW][COL], bool aval[ROW][COL]){
  //IA
  const std::string R = "\033[0m";
  const std::string W = "\033[100;37m";
  const std::string RO = "\033[42;30m";
  const std::string F = "\033[44;37m";
  const std::string B = "\033[41;37m";
  const std::string EN = "\033[43;30m";
  const std::string EX = "\033[45;37m";

  for (int i = 0; i < ROW; i++) {
    for (int j = 0; j < COL; j++) {
      switch (mapa[i][j]) {
        case WALL: std::cout << W << " ### " << R;
                   break;
        case ROAD: std::cout << RO << " via " << R;
                   break;
        case ENTRY: std::cout << EN << " EN " << R;
                    break;
        case OUT: std::cout << EX << " EX " << R;
                  break;
        case PARKING:
                  if(aval[i][j]){
                    std::cout << B << " BSY " << R;
                  } else {
                    std::cout << F << " FRE " << R;
                  }
            break;

      }
    }
    std::cout << "\n";
  }
}
