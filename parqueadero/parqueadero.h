
#pragma once
#include <ctime>
#include <string>
#include "mapa.h"

const int M_VEHICLES = 50;

struct Vehicle {
  int         row;      // minuscula para no chocar con la constante ROW de mapa.h
  int         col;      // minuscula para no chocar con la constante COL de mapa.h
  std::time_t checkIn;
  std::string plate;
};

struct Parking {
  Vehicle vehicles[M_VEHICLES];
  int     tVehicles;
  int     map[ROW][COL];
  bool    aval[ROW][COL];
};

void   buildParking (Parking* p);
void   menu         (Parking* p);
void   checkInVeh   (Parking* p);
void   checkOutVeh  (Parking* p);
void   showAval     (Parking* p);
void   showVehicles (Parking* p);
double calcPay      (std::time_t checkIn, std::time_t checkOut);
void   exportHTML   (Parking* p, int animRow, int animCol, bool leaving);
