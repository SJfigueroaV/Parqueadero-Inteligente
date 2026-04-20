#include "parqueadero.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cctype>
#include <functional>

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

// Usa BFS (busqueda en anchura) para encontrar el camino mas corto entre dos celdas.
// Solo avanza por celdas transitables: ROAD, ENTRY y OUT.
// El origen y destino si pueden ser PARKING porque ahi empieza o termina el recorrido.
static std::vector<std::pair<int,int>> buildRoute(int fromRow, int fromCol,
                                                   int toRow,   int toCol,
                                                   int map[ROW][COL]) {
  bool visited[ROW][COL] = {};

  // En parent guardamos desde que celda llegamos a cada una,
  // asi al final podemos reconstruir el camino hacia atras
  std::pair<int,int> parent[ROW][COL];
  for (int i = 0; i < ROW; i++)
    for (int j = 0; j < COL; j++)
      parent[i][j] = {-1, -1};

  // Retorna true si la celda (r,c) se puede visitar
  std::function<bool(int, int)> passable = [&](int r, int c) {
    if (r < 0 || r >= ROW || c < 0 || c >= COL) return false;
    if ((r == fromRow && c == fromCol) || (r == toRow && c == toCol)) return true;
    int v = map[r][c];
    return v == ROAD || v == ENTRY || v == OUT;
  };

  std::queue<std::pair<int,int>> q;
  q.push({fromRow, fromCol});
  visited[fromRow][fromCol] = true;

  // Los 4 movimientos posibles: arriba, abajo, izquierda, derecha
  const int dr[] = {-1, 1,  0, 0};
  const int dc[] = { 0, 0, -1, 1};
  bool found = false;

  while (!q.empty() && !found) {
    int r = q.front().first;
    int c = q.front().second;
    q.pop();
    for (int d = 0; d < 4; d++) {
      int nr = r + dr[d];
      int nc = c + dc[d];
      if (!passable(nr, nc) || visited[nr][nc]) continue;
      visited[nr][nc]  = true;
      parent[nr][nc]   = {r, c};
      if (nr == toRow && nc == toCol) { found = true; break; }
      q.push({nr, nc});
    }
  }

  std::vector<std::pair<int,int>> route;
  if (!found) return route; // si no encontro camino el carro simplemente no se mueve

  // Reconstruimos el camino yendo hacia atras desde el destino hasta el origen
  for (std::pair<int,int> cur = {toRow, toCol}; cur.first != -1; cur = parent[cur.first][cur.second])
    route.push_back(cur);

  std::reverse(route.begin(), route.end());
  return route;
}

// Genera parking.html con el estado actual del mapa.
// Si animRow != -1 tambien incluye la animacion del carro entrando o saliendo.
void exportHTML(Parking* p, int animRow, int animCol, bool leaving) {

  // Entrada: el carro va de [1][1] (ENTRY) hasta el espacio asignado
  // Salida:  el carro va del espacio hasta [1][14] (OUT)
  std::vector<std::pair<int,int>> route;
  if (animRow != -1) {
    if (!leaving)
      route = buildRoute(1, 1, animRow, animCol, p->map);
    else
      route = buildRoute(animRow, animCol, 1, 14, p->map);
  }

  // Convertir la ruta a un array JS: [[r,c],[r,c],...]
  std::ostringstream rJS;
  rJS << "[";
  for (int i = 0; i < (int)route.size(); i++)
    rJS << (i ? "," : "") << "[" << route[i].first << "," << route[i].second << "]";
  rJS << "]";

  // Convertir el mapa, disponibilidad y placas a arrays JS para que el navegador los dibuje
  std::ostringstream mJS, aJS, pJS;
  mJS << "["; aJS << "["; pJS << "[";
  int total = 0, free = 0;

  for (int i = 0; i < ROW; i++) {
    mJS << "["; aJS << "["; pJS << "[";
    for (int j = 0; j < COL; j++) {
      if (j) { mJS << ","; aJS << ","; pJS << ","; }

      mJS << p->map[i][j];
      aJS << (p->aval[i][j] ? "true" : "false");

      // Buscar si hay un vehiculo estacionado en esta celda para mostrar su placa
      std::string pl = "";
      if (p->aval[i][j]) {
        for (int k = 0; k < p->tVehicles; k++) {
          if (p->vehicles[k].row == i && p->vehicles[k].col == j) {
            pl = p->vehicles[k].plate;
            break;
          }
        }
      }
      pJS << "\"" << pl << "\"";

      if (p->map[i][j] == PARKING) { total++; if (!p->aval[i][j]) free++; }
    }
    mJS << "]"; aJS << "]"; pJS << "]";
    if (i < ROW - 1) { mJS << ","; aJS << ","; pJS << ","; }
  }
  mJS << "]"; aJS << "]"; pJS << "]";

  std::ofstream f("parking.html");
  f << "<!DOCTYPE html>\n"
    << "<html lang=\"es\"><head><meta charset=\"UTF-8\"><title>Parqueadero</title>\n"
    << "<style>\n"
    << "*{box-sizing:border-box;margin:0;padding:0}\n"
    << "body{background:#0f0f1a;color:#e0e0e0;font-family:'Courier New',monospace;display:flex;flex-direction:column;align-items:center;padding:20px;min-height:100vh}\n"
    << "h1{color:#7eb8f7;margin-bottom:6px;font-size:22px;letter-spacing:2px}\n"
    << ".stats{display:flex;gap:24px;margin-bottom:16px;font-size:13px;color:#aaa}\n"
    // El grid se ajusta automaticamente al tamaño del mapa (ROW x COL)
    << "#grid{display:grid;grid-template-columns:repeat(" << COL << ",44px);grid-template-rows:repeat(" << ROW << ",30px);gap:2px;position:relative}\n"
    << ".cell{display:flex;align-items:center;justify-content:center;font-size:9px;font-weight:bold;border-radius:3px;position:relative}\n"
    << ".WALL{background:#2a2a2a;color:#555}.ROAD{background:#1a3d1a;color:#4caf50}\n"
    << ".ENTRY{background:#5a4000;color:#ffc107}.OUT{background:#4a0030;color:#e91e8c}\n"
    << ".FREE{background:#0d2a4a;color:#42a5f5}.BUSY{background:#4a0d0d;color:#ef5350}\n"
    // El carro es un div con posicion absoluta que se mueve con transiciones CSS
    << "#car{position:absolute;width:36px;height:24px;background:#f9a825;border-radius:4px;display:flex;align-items:center;justify-content:center;font-size:8px;color:#000;font-weight:bold;box-shadow:0 0 8px #f9a825aa;transition:top .25s ease,left .25s ease;z-index:10;pointer-events:none}\n"
    << "#car.hidden{display:none}\n"
    << ".legend{display:flex;gap:16px;margin-top:14px;flex-wrap:wrap;justify-content:center;font-size:11px}\n"
    << ".leg{display:flex;align-items:center;gap:5px}.leg-box{width:14px;height:14px;border-radius:2px}\n"
    << "</style></head><body>\n"
    << "<h1>PARQUEADERO</h1>\n"
    << "<div class=\"stats\">"
    << "<div>Total:<span>"    << total        << "</span></div>"
    << "<div>Libres:<span>"   << free         << "</span></div>"
    << "<div>Ocupados:<span>" << (total-free) << "</span></div>"
    << "</div>\n"
    << "<div id=\"grid\"><div id=\"car\" class=\"hidden\"></div></div>\n"
    << "<div class=\"legend\">\n"
    << "  <div class=\"leg\"><div class=\"leg-box\" style=\"background:#2a2a2a\"></div>Pared</div>\n"
    << "  <div class=\"leg\"><div class=\"leg-box\" style=\"background:#1a3d1a\"></div>Via</div>\n"
    << "  <div class=\"leg\"><div class=\"leg-box\" style=\"background:#0d2a4a\"></div>Libre</div>\n"
    << "  <div class=\"leg\"><div class=\"leg-box\" style=\"background:#4a0d0d\"></div>Ocupado</div>\n"
    << "  <div class=\"leg\"><div class=\"leg-box\" style=\"background:#5a4000\"></div>Entrada</div>\n"
    << "  <div class=\"leg\"><div class=\"leg-box\" style=\"background:#4a0030\"></div>Salida</div>\n"
    << "</div>\n"
    << "<script>\n"
    // CW y CH = ancho y alto de cada celda en px (tamaño + gap del grid)
    // Se usan para calcular exactamente donde posicionar el div del carro
    << "const CW = 46, CH = 32;\n"
    << "const mapData   = " << mJS.str() << ";\n"
    << "const avalData  = " << aJS.str() << ";\n"
    << "const plateData = " << pJS.str() << ";\n"
    << "const route     = " << rJS.str() << ";\n"
    << "const leaving   = " << (leaving ? "true" : "false") << ";\n"
    << "const WALL=0, PARKING=1, ROAD=2, ENTRY=3, OUT=4;\n"
    << "const grid = document.getElementById('grid');\n"
    << "const car  = document.getElementById('car');\n"
    // Crear un div por cada celda y pintarlo segun su tipo
    << "for (let i = 0; i < " << ROW << "; i++) {\n"
    << "  for (let j = 0; j < " << COL << "; j++) {\n"
    << "    const cell = document.createElement('div');\n"
    << "    cell.className = 'cell';\n"
    << "    cell.id = `c${i}_${j}`;\n"
    << "    const v = mapData[i][j];\n"
    << "    if      (v === WALL)    { cell.classList.add('WALL');  cell.textContent = '###'; }\n"
    << "    else if (v === ROAD)    { cell.classList.add('ROAD');  cell.textContent = 'via'; }\n"
    << "    else if (v === ENTRY)   { cell.classList.add('ENTRY'); cell.textContent = 'ENT'; }\n"
    << "    else if (v === OUT)     { cell.classList.add('OUT');   cell.textContent = 'OUT'; }\n"
    << "    else if (v === PARKING) {\n"
    << "      cell.classList.add(avalData[i][j] ? 'BUSY' : 'FREE');\n"
    << "      cell.textContent = avalData[i][j] ? (plateData[i][j] || 'BSY') : 'FRE';\n"
    << "    }\n"
    << "    grid.appendChild(cell);\n"
    << "  }\n"
    << "}\n"
    // Posiciona el div del carro sobre la celda [r,c] usando coordenadas absolutas
    << "function moveCar(r, c) {\n"
    << "  car.style.top  = (r * CH + 3) + 'px';\n"
    << "  car.style.left = (c * CW + 4) + 'px';\n"
    << "}\n"
    << "function animateRoute(route, plate, onDone) {\n"
    << "  if (!route.length) { if (onDone) onDone(); return; }\n"
    << "  car.textContent = plate;\n"
    << "  car.classList.remove('hidden');\n"
    // Sin transicion en el primer paso para que el carro aparezca en el origen de golpe
    << "  car.style.transition = 'none';\n"
    << "  moveCar(route[0][0], route[0][1]);\n"
    << "  let step = 1;\n"
    // Avanzamos una celda cada 240ms usando una funcion que se reprograma a si misma
    << "  (function next() {\n"
    << "    if (step >= route.length) {\n"
    << "      setTimeout(() => { car.classList.add('hidden'); if (onDone) onDone(); }, 400);\n"
    << "      return;\n"
    << "    }\n"
    << "    car.style.transition = 'top .22s ease, left .22s ease';\n"
    << "    moveCar(route[step][0], route[step][1]);\n"
    << "    step++;\n"
    << "    setTimeout(next, 240);\n"
    << "  })();\n"
    << "}\n"
    << "if (route.length) {\n"
    // Si sale, la placa esta en la primera celda de la ruta (donde estaba estacionado)
    // Si entra, esta en la ultima celda (el espacio que se le acaba de asignar)
    << "  const plate = leaving\n"
    << "    ? (plateData[route[0][0]]?.[route[0][1]] || 'CAR')\n"
    << "    : (plateData[route[route.length-1][0]]?.[route[route.length-1][1]] || 'CAR');\n"
    << "  animateRoute(route, plate, () => setTimeout(() => location.reload(), 1000));\n"
    << "} else {\n"
    // Si no hay animacion igual recargamos para reflejar el nuevo estado del parqueadero
    << "  setTimeout(() => location.reload(), 2000);\n"
    << "}\n"
    << "</script></body></html>\n";
  f.close();
}

void buildParking(Parking* p) {
  buildMap(p->map);
  p->tVehicles = 0;
  for (int i = 0; i < ROW; i++)
    for (int j = 0; j < COL; j++)
      p->aval[i][j] = false;
  exportHTML(p, -1, -1, false); // genera el HTML inicial sin animacion
}

// Cobra minimo 1 hora aunque el vehiculo haya estado menos tiempo
double calcPay(std::time_t checkIn, std::time_t checkOut) {
  double hours = std::difftime(checkOut, checkIn) / 3600.0;
  if (hours < 1.0) hours = 1.0;
  return hours * 5500.0;
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

  if (!validPlate(plate))           { std::cout << "  Placa invalida. Ej: ABC123\n"; return; }
  if (findByPlate(p, plate) != -1)  { std::cout << "  Placa ya registrada.\n";       return; }

  // idx es el indice donde guardamos el vehiculo nuevo; tVehicles se incrementa en la misma linea
  int idx = p->tVehicles++;
  p->vehicles[idx] = {spotRow, spotCol, std::time(nullptr), plate};
  p->aval[spotRow][spotCol] = true;

  char buf[10];
  std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&p->vehicles[idx].checkIn));
  std::cout << "  OK - " << plate << " en [" << spotRow << "][" << spotCol << "]"
            << "  entrada: " << buf << "\n"
            << "  Abre parking.html para ver la animacion.\n";

  exportHTML(p, spotRow, spotCol, false);
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

  // Exportamos ANTES de liberar el espacio para que la animacion muestre el carro aun ahi
  exportHTML(p, vRow, vCol, true);
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
    if (!p->tVehicles) { std::cout << "No hay vehiculos estacionados.\n"; return; }

    std::cout << "\nPlaca      Espacio  Entrada\n";

    for (int i = 0; i < p->tVehicles; i++) {
        char buf[9];
        std::strftime(buf, sizeof(buf), "%H:%M:%S", std::localtime(&p->vehicles[i].checkIn));
        Vehicle& v = p->vehicles[i];
        std::cout << std::left << std::setw(11) << v.plate
                  << "[" << v.row << "][" << v.col << "]  "
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
    
    std::cin >> op;


    switch (op) {
      case 1: checkInVeh(p);   break;
      case 2: checkOutVeh(p);  break;
      case 3: showVehicles(p); break;
      case 4: showAval(p);     break;
      case 5: std::cout << "\n  Hasta luego.\n\n"; break;
      default: std::cout << "\n  Opcion invalida.\n";
    }

    if (op != 5) {
      std::cout << "\ncontinuar...";
      std::cin.ignore();
      std::cin.get();
    }
  }
}
