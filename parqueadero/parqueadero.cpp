#include "parqueadero.h"
#include <iostream>
#include <iomanip>
#include <cctype>

// Recorre el mapa buscando el primer espacio libre y guarda su posicion en r y c
static bool firstFreeSpot(Parking* p, int* r, int* c) {
  for (int i = 0; i < ROW; i++) {
    for (int j = 0; j < COL; j++) {
      if (p->map[i][j] == PARKING && !p->aval[i][j]) {
        *r = i;
        *c = j;
        return true;
      }
    }
  }
  return false;
}

// Busca una placa en el arreglo de vehiculos y retorna su indice, o -1 si no existe
static int findByPlate(Parking* p, const std::string& plate) {
  for (int i = 0; i < p->tVehicles; i++) {
    if (p->vehicles[i].plate == plate)
      return i;
  }
  return -1;
}

// Valida que la placa tenga exactamente 3 letras seguidas de 3 numeros (ej: ABC123)
// Si es valida, convierte las letras a mayusculas para que "abc123" y "ABC123" sean lo mismo
static bool validPlate(std::string& plate) {
  if (plate.size() != 6) return false;

  for (int i = 0; i < 3; i++)
    if (!std::isalpha((unsigned char)plate[i])) return false;

  for (int i = 3; i < 6; i++)
    if (!std::isdigit((unsigned char)plate[i])) return false;

  for (int i = 0; i < 3; i++)
    plate[i] = std::toupper((unsigned char)plate[i]);

  return true;
}

void buildParking(Parking* p) {
  buildMap(p->map);
  p->tVehicles = 0;
  for (int i = 0; i < ROW; i++)
    for (int j = 0; j < COL; j++)
      p->aval[i][j] = false;
}

// Cobra minimo 1 hora aunque el vehiculo haya estado menos tiempo
double calcPay(std::time_t checkIn, std::time_t checkOut) {
  double hours = std::difftime(checkOut, checkIn) / 3600.0;
  if (hours < 1.0) hours = 1.0;
  return hours * 3500.0;
}
