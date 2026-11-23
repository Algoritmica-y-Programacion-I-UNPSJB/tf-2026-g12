# Trabajo Integrador Final 2026 — Grupo 12

**Juego aprobado:** Buscaminas  
**Interfaz / Modalidad:** Consola — 1 jugador  

## Integrantes y aportes
- Mateo Carlos Cuellos Barrera (GitHub no informado)
- Mateo Miguel Mendez (GitHub no informado)
- Mariano Marcelo Jara (GitHub no informado)
- Amadeo Federico Fernández Jara (@AmaSB9)

## Reglas y alcance
La propuesta aprobada consiste en desarrollar Buscaminas por consola.

El juego es el Buscaminas clásico en un tablero de 8x8, para un solo jugador y con interfaz de consola.

- **Objetivo:** revelar todas las casillas seguras sin pisar una mina. No hace falta marcar las minas con banderas para ganar.
- Las minas se colocan al azar al empezar la partida. Cada número indica cuántas minas hay en las 8 casillas vecinas (lados y diagonales).
- Al revelar una casilla con 0 minas alrededor, se revelan solas sus vecinas en cascada (función recursiva).
- Dificultades: Fácil (6 minas), Normal (10 minas) y Difícil (15 minas).
- Las banderas son una ayuda: no se puede revelar una casilla con bandera, la cascada no pasa por ellas y no influyen en el puntaje.
- Antes de jugar, el programa pide el nombre, saluda y presenta las reglas.
- **Alcance:** una partida por ejecución. Al terminar (victoria, derrota o abandono) se muestra un resumen de resultados con el puntaje y el programa finaliza al presionar ENTER.

  ### Criterio de puntaje

| Concepto | Puntos |
|---|---|
| Casilla segura revelada | Fácil +10, Normal +15, Difícil +20 por casilla |
| Ganar la partida | +500 |
| Rapidez (solo si se gana) | +1 por cada segundo por debajo de 300 (300 menos los segundos que duró la partida) |
| Derrota o abandono | Solo suman las casillas seguras reveladas |

`puntaje = casillas reveladas * puntos por casilla`, y si se gana se suma además el bonus de victoria y el de rapidez.

## Compilación y ejecución

Desde la carpeta donde está `buscaminas.c`:

```bash
gcc  buscaminas.c -o buscaminas
./buscaminas
```

- usando el Cygwin64 

### Controles

| Entrada | Qué hace |
|---|---|
| `1`, `2`, `3` al elegir la dificultad | Fácil, Normal o Difícil |
| `R fila columna` (ej. `R 3 5`) | Revela una casilla |
| `B fila columna` (ej. `B 2 7`) | Pone o quita una bandera |
| `X` | Abandona la partida (se muestra el resumen y el programa termina) |
| `ENTER` | Continúa en las pantallas de reglas y de avisos, y cierra el programa en la pantalla final |

Las filas y las columnas van del 1 al 8.


## Organización de datos y funciones
### Datos (estado de la partida)

| Dato | Qué guarda |
|---|---|
| `mina[FILAS][COLS]` | `1` si la casilla tiene mina, `0` si no |
| `vecinas[FILAS][COLS]` | Cantidad de minas vecinas (0 a 8) |
| `estado[FILAS][COLS]` | `0` oculta, `1` revelada, `2` bandera |
| `minas` | Cantidad de minas de la partida (6, 10 o 15). Define cuántas se colocan, cuántas casillas seguras hay y cuántos puntos vale cada casilla. |
| `nombreDificultad` | Texto de la dificultad ("Facil", "Normal" o "Dificil"), solo para mostrarlo en pantalla. |
| `Resultado` | Estructura con los datos de la partida terminada para el resumen: resultado (abandono, victoria o derrota), casillas reveladas, segundos y puntaje |

### Funciones principales

| Responsabilidad | Funciones |
|---|---|
| Preparar la partida | `inicializar`, `colocarMinas`, `calcularVecinas`, `dentro` |
| Reglas y resultado | `revelar` (recursiva), `hayVictoria`, `contarReveladas`, `calcularPuntaje` |
| Entrada y presentación | `pedirNombre`, `mostrarReglas`, `elegirDificultad`, `mostrar`, `mostrarNumero`, `contarBanderas` (contador del tablero), `mostrarResumen`, `mostrarTitulo`, `limpiarPantalla`, `limpiarBuffer`, `esperarEnter` |
| Control de la partida | `jugar` (bucle de una partida), `main` |

El programa sigue este orden: `main` pide el nombre, muestra el saludo y las reglas, llama a `jugar` (que pide la dificultad, prepara el tablero y repite jugadas hasta ganar, perder o abandonar) y termina mostrando el resumen.

### Casos límite que se validan

- Dificultad fuera de 1-3 o texto no numérico: se vuelve a pedir.
- Fila o columna fuera de 1-8, coordenadas no numéricas o comando inválido: mensaje de error y se vuelve a pedir la jugada.
- Revelar una casilla con bandera o ya revelada: aviso, sin cambios.
- Nombre vacío o solo espacios: se vuelve a pedir.

## Interfaz gráfica
No corresponde al alcance aprobado actualmente.
