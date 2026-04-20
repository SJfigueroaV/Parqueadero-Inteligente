# Sistema de Gestión de Parqueadero

## Funcionalidades innovadoras implementadas

### 1. Validación de formato de placa (`validPlate`)

**¿Para qué se implementó?**
Para evitar que el usuario ingrese placas con formatos inválidos que rompan la consistencia del registro. Sin esta validación cualquier texto podría registrarse como placa, lo que haría imposible identificar vehículos de forma confiable.

**¿Por qué se consideró necesario?**
En un parqueadero real las placas siguen un formato estándar colombiano: tres letras seguidas de tres números (ej: `ABC123`). Aceptar entradas arbitrarias como `123`, `hola` o cadenas vacías generaría datos inútiles en el sistema y dificultaría la búsqueda al momento de la salida.

**¿Cómo se llevó a cabo?**
Se implementó la función `validPlate` que recibe la placa por referencia y realiza tres verificaciones: que tenga exactamente 6 caracteres, que los primeros 3 sean letras y que los últimos 3 sean dígitos. Adicionalmente convierte las letras a mayúsculas antes de registrar, de forma que `abc123` y `ABC123` se traten como la misma placa. Se usaron las funciones `std::isalpha`, `std::isdigit` y `std::toupper` del header `<cctype>`.

---

### 2. Búsqueda de ruta con BFS (`buildRoute`)

**¿Para qué se implementó?**
Para calcular el camino real que recorre un vehículo desde la entrada hasta su espacio asignado, y desde su espacio hasta la salida, siguiendo únicamente las vías definidas en el mapa.

**¿Por qué se consideró necesario?**
Sin un algoritmo de búsqueda de rutas, la animación del vehículo en el HTML no tendría ningún sentido lógico — el carro se movería en línea recta atravesando paredes y espacios de parqueo, lo que no representa el comportamiento real de un parqueadero. Implementar BFS garantiza que la ruta calculada sea válida y además sea la más corta posible.

**¿Cómo se llevó a cabo?**
Se implementó el algoritmo BFS (Breadth-First Search / búsqueda en anchura) usando una `std::queue` de pares de coordenadas. El algoritmo parte desde la celda origen, explora las 4 celdas adyacentes (arriba, abajo, izquierda, derecha) y solo avanza por celdas de tipo `ROAD`, `ENTRY` u `OUT`. Para reconstruir el camino al final, se mantiene una matriz `parent` que registra desde qué celda se llegó a cada una. Una vez encontrado el destino, se recorre `parent` hacia atrás y se invierte el vector resultante con `std::reverse`. Se usó `std::function<bool(int,int)>` del header `<functional>` para definir la función auxiliar `passable` como lambda con captura por referencia.

---

### 3. Visualización web en tiempo real (`exportHTML`)

**¿Para qué se implementó?**
Para ofrecer una vista visual del estado del parqueadero en el navegador, complementando la vista de consola. El archivo `parking.html` se regenera automáticamente cada vez que un vehículo entra o sale, mostrando el mapa actualizado y animando el recorrido del vehículo.

**¿Por qué se consideró necesario?**
La vista de consola con colores ANSI cumple los requisitos mínimos, pero tiene limitaciones visuales evidentes: el tamaño de las celdas es fijo, no hay animaciones fluidas y la experiencia de usuario es básica. Una vista en el navegador permite mostrar el mapa con colores más ricos, estadísticas de disponibilidad y la animación del carro moviéndose celda por celda en tiempo real, lo que representa una mejora significativa en la experiencia de usuario.

**¿Cómo se llevó a cabo?**
La función `exportHTML` serializa el estado completo del parqueadero (mapa, disponibilidad y placas) a tres arrays JavaScript incrustados directamente en el HTML generado. La cuadrícula se construye con CSS Grid, ajustándose dinámicamente al tamaño `ROW x COL` definido en `mapa.h`. La animación del vehículo usa un `<div>` posicionado de forma absoluta sobre el grid que se desplaza celda por celda mediante transiciones CSS (`transition: top .25s ease, left .25s ease`), avanzando un paso cada 240ms con una función JS recursiva con `setTimeout`. La ruta que sigue el `<div>` es exactamente la calculada por `buildRoute` en C++, convertida a un array JS con `std::ostringstream`. Al terminar la animación la página se recarga automáticamente para reflejar el estado final.

> **Nota sobre el uso de IA en esta funcionalidad:** La lógica de C++ de `buildRoute` y la serialización del estado del parqueadero a arrays fue desarrollada por los integrantes del grupo. Sin embargo, el diseño visual del HTML (estilos CSS, colores, layout del grid, la animación del `<div>` del carro y la integración con el array de ruta) fue generado con asistencia de inteligencia artificial. La razón es que adaptar manualmente el código de C++ a JavaScript, definir coordenadas de píxeles para cada celda y diseñar una interfaz visualmente coherente resultaba excesivamente tedioso y alejado de los objetivos del curso, que se centran en lógica de programación en C++. La IA tomó las funciones ya implementadas (`buildRoute`, la serialización del mapa) como base y generó el código JS/CSS que las consume para producir la visualización.

---

## Compilación y uso

```bash
g++ main.cpp mapa.cpp parqueadero.cpp -o parqueadero
./parqueadero
```

Al registrar el ingreso o salida de un vehículo, el programa genera o actualiza el archivo `parking.html` en la misma carpeta. Abrirlo en el navegador muestra el mapa en tiempo real con la animación del recorrido.
