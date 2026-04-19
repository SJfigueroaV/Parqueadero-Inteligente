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

void checkInVeh(Parking* p) {
  if (p->tVehicles >= M_VEHICLES) { std::cout << "\n  Parqueadero lleno.\n"; return; }

  int spotRow = -1, spotCol = -1;
  if (!firstFreeSpot(p, &spotRow, &spotCol)) {
    std::cout << "\n  No hay espacios disponibles.\n";
    return;
  }

  std::string plate;
  std::cout << "\n  Placa (formato AAA000): ";
  std::cin >> plate;

  if (!validPlate(plate))          { std::cout << "  Placa invalida. Ej: ABC123\n"; return; }
  if (findByPlate(p, plate) != -1) { std::cout << "  Placa ya registrada.\n";       return; }

  // idx es el indice donde guardamos el vehiculo nuevo; tVehicles se incrementa en la misma linea
  int idx = p->tVehicles++;
  p->vehicles[idx] = {spotRow, spotCol, std::time(nullptr), plate};
  p->aval[spotRow][spotCol] = true;

  char buf[10];
  std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&p->vehicles[idx].checkIn));
  std::cout << "  OK - " << plate << " en [" << spotRow << "][" << spotCol << "]"
            << "  entrada: " << buf << "\n";
}

void checkOutVeh(Parking* p) {
  if (!p->tVehicles) { std::cout << "\n  No hay vehiculos en el parqueadero.\n"; return; }

  std::string plate;
  std::cout << "\n  Placa a retirar: ";
  std::cin >> plate;

  int idx = findByPlate(p, plate);
  if (idx == -1) { std::cout << "  Placa no encontrada.\n"; return; }

  std::time_t now = std::time(nullptr);
  int vRow = p->vehicles[idx].row;
  int vCol = p->vehicles[idx].col;
  int mins = (int)(std::difftime(now, p->vehicles[idx].checkIn) / 60);

  p->aval[vRow][vCol] = false;

  char bufIn[10], bufOut[10];
  std::strftime(bufIn,  sizeof(bufIn),  "%H:%M:%S", std::localtime(&p->vehicles[idx].checkIn));
  std::strftime(bufOut, sizeof(bufOut), "%H:%M:%S", std::localtime(&now));

  std::cout << "\n  Placa   : " << plate
            << "\n  Entrada : " << bufIn
            << "\n  Salida  : " << bufOut
            << "\n  Tiempo  : " << mins << " min"
            << "\n  Total   : $" << (int)calcPay(p->vehicles[idx].checkIn, now) << "\n\n";

  // Eliminamos el vehiculo reemplazandolo con el ultimo del arreglo y reduciendo el contador
  p->vehicles[idx] = p->vehicles[--p->tVehicles];
}

void showAval(Parking* p) {
  int total = 0, free = 0;
  for (int i = 0; i < ROW; i++) {
    for (int j = 0; j < COL; j++) {
      if (p->map[i][j] == PARKING) {
        total++;
        if (!p->aval[i][j]) free++;
      }
    }
  }
  std::cout << "\n  Total    : " << total
            << "\n  Libres   : " << free
            << "\n  Ocupados : " << total - free << "\n\n";
}

void showVehicles(Parking* p) {
  if (!p->tVehicles) { std::cout << "\n  No hay vehiculos estacionados.\n\n"; return; }

  std::cout << "\n  " << std::left << std::setw(10) << "Placa"
            << std::setw(10) << "Espacio" << "Entrada\n  "
            << std::string(30, '-') << "\n";

  for (int i = 0; i < p->tVehicles; i++) {
    char buf[10];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&p->vehicles[i].checkIn));
    std::cout << "  " << std::left << std::setw(10) << p->vehicles[i].plate
              << "[" << p->vehicles[i].row << "][" << p->vehicles[i].col << "]    "
              << buf << "\n";
  }
  std::cout << "\n";
}

void menu(Parking* p) {
  int op = 0;
  while (op != 5) {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif

    showMap(p->map, p->aval);
    showAval(p);

    std::cout << "  1. Registrar ingreso\n"
              << "  2. Registrar salida\n"
              << "  3. Listar vehiculos\n"
              << "  4. Ver disponibilidad\n"
              << "  5. Salir\n"
              << "\n  Opcion: ";

    // Leemos como string para que si el usuario escribe letras el programa no explote
    std::string inp;
    std::cin >> inp;
    try { op = std::stoi(inp); } catch (...) { op = -1; }

    switch (op) {
      case 1: checkInVeh(p);   break;
      case 2: checkOutVeh(p);  break;
      case 3: showVehicles(p); break;
      case 4: showAval(p);     break;
      case 5: std::cout << "\n  Hasta luego.\n\n"; break;
      default: std::cout << "\n  Opcion invalida.\n";
    }

    if (op != 5) {
      std::cout << "\n  Enter para continuar...";
      std::cin.ignore();
      std::cin.get();
    }
  }
}
