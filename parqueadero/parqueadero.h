#pragma once
#include <ctime>
#include <string>
#include "mapa.h"

#define M_VEHICLES 50

struct Vehicle {
  int ROW;
  int COL;
  std::time_t checkIn;
  std::string plate;
};

struct Parking {
  Vehicle vehicles[M_VEHICLES];
  int tVehicles;
  int map[ROW][COL];
  bool aval[ROW][COL];
};

void menu(Parking* p);
void buildParking(Parking* p);
void chekIn(Parking* p);
void showAval(Parking* p);
void showVehicles(Parking* p);
void calcPay(std::time_t checkIn, std::time_t checkOut);
