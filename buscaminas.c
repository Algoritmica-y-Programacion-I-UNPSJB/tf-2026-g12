#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FILAS 8
#define COLS 8

#define TAM_NOMBRE 32

#define MINAS_FACIL 6
#define MINAS_NORMAL 10
#define MINAS_DIFICIL 15

#define PUNTOS_FACIL 10
#define PUNTOS_NORMAL 15
#define PUNTOS_DIFICIL 20

#define BONUS_VICTORIA 500
#define BONUS_TIEMPO_MAX 300

#define RES_ABANDONO 0
#define RES_VICTORIA 1
#define RES_DERROTA 2

#define RESET "\033[0m"
#define ROJO "\033[31m"
#define VERDE "\033[32m"
#define AMARILLO "\033[33m"
#define AZUL "\033[34m"
#define CELESTE "\033[36m"
#define GRIS "\033[90m"
#define NEGRITA "\033[1m"
#define BLANCO "\033[97m"
#define FONDO "\033[44m"

typedef struct {
    int resultado;
    int reveladas;
    int segundos;
    int puntaje;
} Resultado;

int mina[FILAS][COLS];
int vecinas[FILAS][COLS];
int estado[FILAS][COLS];

int minas;
const char *nombreDificultad;

void limpiarPantalla(void);
void limpiarBuffer(void);
void esperarEnter(void);
void mostrarTitulo(void);
void pedirNombre(char *nombre, int tam);
void mostrarReglas(const char *nombre);
void elegirDificultad(void);
void inicializar(void);
void colocarMinas(void);
int dentro(int f, int c);
void calcularVecinas(void);
int contarBanderas(void);
int contarReveladas(void);
void mostrarNumero(int numero);
void mostrar(int verTodo);
void revelar(int f, int c);
int hayVictoria(void);
int calcularPuntaje(int resultado, int reveladas, int segundos);
Resultado jugar(void);
void mostrarResumen(const char *nombre, Resultado r);

void limpiarPantalla(void) {
    printf("\033[2J\033[3J\033[H");
}

void limpiarBuffer(void) {
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

void esperarEnter(void) {
    printf("\nPresione ENTER para continuar...");
    limpiarBuffer();
}

void mostrarTitulo(void) {
    printf(CELESTE NEGRITA);
    printf("========================================\n");
    printf("             BUSCAMINAS %dx%d\n", FILAS, COLS);
    printf("========================================\n");
    printf(RESET);
}

void pedirNombre(char *nombre, int tam) {
    int valido = 0;

    while (!valido) {
        limpiarPantalla();
        mostrarTitulo();

        printf("\nBienvenido al Buscaminas!\n\n");
        printf("Como te llamas? ");

        if (fgets(nombre, tam, stdin) == NULL) {
            exit(0);
        }

        size_t largo = strlen(nombre);

        if (largo > 0 && nombre[largo - 1] == '\n') {
            nombre[largo - 1] = '\0';
        } else {
            limpiarBuffer();
        }

        for (int i = 0; nombre[i] != '\0'; i++) {
            if (nombre[i] != ' ') {
                valido = 1;
            }
        }
    }
}

void mostrarReglas(const char *nombre) {
    limpiarPantalla();
    mostrarTitulo();

    printf("\nHola, " AMARILLO NEGRITA "%s" RESET "!\n", nombre);
    printf(AMARILLO NEGRITA "\nREGLAS\n\n" RESET);

    printf(NEGRITA "Objetivo\n" RESET);
    printf("  Revelar todas las casillas seguras sin pisar una mina.\n");
    printf("  No hace falta marcar las minas para ganar.\n\n");

    printf(NEGRITA "Dificultad\n" RESET);
    printf("  Facil: %d minas | Normal: %d minas | Dificil: %d minas\n\n",
           MINAS_FACIL, MINAS_NORMAL, MINAS_DIFICIL);

    printf(NEGRITA "El tablero\n" RESET);
    printf("  [ ] casilla oculta       F bandera       * mina\n");
    printf("  Un numero indica cuantas minas hay en las 8 casillas\n");
    printf("  vecinas (lados y diagonales). Una casilla vacia tiene 0.\n\n");

    printf(NEGRITA "Comandos" RESET " (fila y columna del 1 al %d)\n", FILAS);
    printf("  " VERDE "R fila columna" RESET "  Revela una casilla.   Ej: R 3 5\n");
    printf("  " AMARILLO "B fila columna" RESET "  Pone o quita bandera. Ej: B 2 7\n");
    printf("  " GRIS "X" RESET "                Abandona la partida.\n\n");

    printf(NEGRITA "Al revelar\n" RESET);
    printf("  - Mina: perdes y se muestran todas las minas.\n");
    printf("  - Numero: queda visible.\n");
    printf("  - Casilla con 0: se abren sola sus vecinas en cascada.\n\n");

    printf(NEGRITA "Banderas\n" RESET);
    printf("  Son solo una ayuda: no se puede revelar una casilla con\n");
    printf("  bandera y la cascada no pasa por ellas.\n\n");

    printf(NEGRITA "Puntaje\n" RESET);
    printf("  Cada casilla segura revelada suma: Facil %d | Normal %d | Dificil %d.\n",
           PUNTOS_FACIL, PUNTOS_NORMAL, PUNTOS_DIFICIL);
    printf("  Si ganas: +%d de bonus y hasta +%d por rapidez\n",
           BONUS_VICTORIA, BONUS_TIEMPO_MAX);
    printf("  (%d menos los segundos que tardaste).\n", BONUS_TIEMPO_MAX);
    printf("  Si perdes o abandonas solo suman las casillas reveladas.\n");

    esperarEnter();
}

void elegirDificultad(void) {
    int opcion = 0;

    do {
        limpiarPantalla();
        mostrarTitulo();

        printf("\nElija la dificultad:\n\n");
        printf(VERDE "1" RESET " = Facil    (%d minas)\n", MINAS_FACIL);
        printf(AMARILLO "2" RESET " = Normal   (%d minas)\n", MINAS_NORMAL);
        printf(ROJO "3" RESET " = Dificil  (%d minas)\n\n", MINAS_DIFICIL);
        printf("Opcion: ");

        int leidos = scanf("%d", &opcion);

        if (leidos == EOF) {
            exit(0);
        }

        limpiarBuffer();

        if (leidos == 0) {
            opcion = 0;
        }
    } while (opcion < 1 || opcion > 3);

    switch (opcion) {
        case 1:
            minas = MINAS_FACIL;
            nombreDificultad = "Facil";
            break;
        case 2:
            minas = MINAS_NORMAL;
            nombreDificultad = "Normal";
            break;
        default:
            minas = MINAS_DIFICIL;
            nombreDificultad = "Dificil";
    }
}

void inicializar(void) {
    for (int f = 0; f < FILAS; f++) {
        for (int c = 0; c < COLS; c++) {
            mina[f][c] = 0;
            vecinas[f][c] = 0;
            estado[f][c] = 0;
        }
    }
}

void colocarMinas(void) {
    int puestas = 0;

    while (puestas < minas) {
        int f = rand() % FILAS;
        int c = rand() % COLS;

        if (mina[f][c] == 0) {
            mina[f][c] = 1;
            puestas++;
        }
    }
}

int dentro(int f, int c) {
    return f >= 0 && f < FILAS &&
           c >= 0 && c < COLS;
}

void calcularVecinas(void) {
    for (int f = 0; f < FILAS; f++) {
        for (int c = 0; c < COLS; c++) {
            int cuenta = 0;

            for (int df = -1; df <= 1; df++) {
                for (int dc = -1; dc <= 1; dc++) {
                    int nf = f + df;
                    int nc = c + dc;

                    if ((df != 0 || dc != 0) &&
                        dentro(nf, nc) &&
                        mina[nf][nc] == 1) {
                        cuenta++;
                    }
                }
            }

            vecinas[f][c] = cuenta;
        }
    }
}

int contarBanderas(void) {
    int total = 0;

    for (int f = 0; f < FILAS; f++) {
        for (int c = 0; c < COLS; c++) {
            if (estado[f][c] == 2) {
                total++;
            }
        }
    }

    return total;
}

int contarReveladas(void) {
    int total = 0;

    for (int f = 0; f < FILAS; f++) {
        for (int c = 0; c < COLS; c++) {
            if (estado[f][c] == 1) {
                total++;
            }
        }
    }

    return total;
}

void mostrarNumero(int numero) {
    switch (numero) {
        case 1:
            printf(AZUL NEGRITA " 1 " RESET);
            break;
        case 2:
            printf(VERDE NEGRITA " 2 " RESET);
            break;
        case 3:
            printf(ROJO NEGRITA " 3 " RESET);
            break;
        case 4:
            printf(AZUL NEGRITA " 4 " RESET);
            break;
        case 5:
            printf(ROJO NEGRITA " 5 " RESET);
            break;
        case 6:
            printf(CELESTE NEGRITA " 6 " RESET);
            break;
        case 7:
            printf(BLANCO NEGRITA " 7 " RESET);
            break;
        case 8:
            printf(GRIS NEGRITA " 8 " RESET);
            break;
        default:
            printf("   ");
    }
}

void mostrar(int verTodo) {
    mostrarTitulo();

    printf(" Dificultad: %s%s%s\n", AMARILLO, nombreDificultad, RESET);

    printf(" Minas: %s%d%s/%d",
           ROJO, contarBanderas(), RESET, minas);

    printf("   Casillas seguras: %s%d%s/%d\n\n",
           VERDE, contarReveladas(), RESET,
           FILAS * COLS - minas);

    printf("      ");
    for (int c = 0; c < COLS; c++) {
        printf(NEGRITA " %d " RESET, c + 1);
    }
    printf("\n");

    printf("     +");
    for (int c = 0; c < COLS; c++) {
        printf("---");
    }
    printf("+\n");

    for (int f = 0; f < FILAS; f++) {
        printf("  %s%2d%s |", AMARILLO, f + 1, RESET);

        for (int c = 0; c < COLS; c++) {
            if (verTodo && mina[f][c] == 1) {
                printf(ROJO NEGRITA " * " RESET);
            } else if (estado[f][c] == 2) {
                printf(ROJO NEGRITA " F " RESET);
            } else if (estado[f][c] == 0) {
                printf(GRIS "[ ]" RESET);
            } else if (vecinas[f][c] == 0) {
                printf(FONDO "   " RESET);
            } else {
                mostrarNumero(vecinas[f][c]);
            }
        }

        printf("|\n");
    }

    printf("     +");
    for (int c = 0; c < COLS; c++) {
        printf("---");
    }
    printf("+\n");

    printf("\n");
    printf(GRIS "[ ] Oculta  " RESET);
    printf(ROJO NEGRITA "F Bandera  " RESET);
    printf(ROJO NEGRITA "* Mina  " RESET);
    printf(VERDE "Numeros: minas vecinas\n" RESET);
}

void revelar(int f, int c) {
    if (!dentro(f, c)) {
        return;
    }

    if (estado[f][c] != 0) {
        return;
    }

    estado[f][c] = 1;

    if (vecinas[f][c] == 0) {
        for (int df = -1; df <= 1; df++) {
            for (int dc = -1; dc <= 1; dc++) {
                revelar(f + df, c + dc);
            }
        }
    }
}

int hayVictoria(void) {
    return contarReveladas() == FILAS * COLS - minas;
}

int calcularPuntaje(int resultado, int reveladas, int segundos) {
    int porCasilla;
    int puntos;

    if (minas == MINAS_FACIL) {
        porCasilla = PUNTOS_FACIL;
    } else if (minas == MINAS_NORMAL) {
        porCasilla = PUNTOS_NORMAL;
    } else {
        porCasilla = PUNTOS_DIFICIL;
    }

    puntos = reveladas * porCasilla;

    if (resultado == RES_VICTORIA) {
        puntos += BONUS_VICTORIA;

        if (segundos < BONUS_TIEMPO_MAX) {
            puntos += BONUS_TIEMPO_MAX - segundos;
        }
    }

    return puntos;
}

Resultado jugar(void) {
    char accion;
    int f, c;
    int jugando = 1;
    Resultado res = {RES_ABANDONO, 0, 0, 0};
    time_t inicio;

    elegirDificultad();

    inicializar();
    colocarMinas();
    calcularVecinas();

    inicio = time(NULL);

    while (jugando) {
        limpiarPantalla();
        mostrar(0);

        printf(CELESTE NEGRITA "\nCOMANDOS\n" RESET);
        printf(VERDE "R" RESET " = Revelar casilla\n");
        printf(AMARILLO "B" RESET " = Colocar o quitar bandera\n");
        printf(GRIS "X" RESET " = Abandonar la partida\n\n");

        printf("Ingrese jugada (accion fila columna): ");

        if (scanf(" %c", &accion) != 1) {
            break;
        }

        if (accion == 'X' || accion == 'x') {
            limpiarBuffer();
            break;
        }

        if (scanf("%d %d", &f, &c) != 2) {
            printf(ROJO "\nEntrada invalida.\n" RESET);
            limpiarBuffer();
            esperarEnter();
            continue;
        }

        limpiarBuffer();

        f--;
        c--;

        if (!dentro(f, c)) {
            printf(ROJO "\nPosicion invalida. Use valores del 1 al %d.\n" RESET, FILAS);
            esperarEnter();
            continue;
        }

        if (accion == 'B' || accion == 'b') {
            if (estado[f][c] == 0) {
                estado[f][c] = 2;
            } else if (estado[f][c] == 2) {
                estado[f][c] = 0;
            } else {
                printf(AMARILLO "\nLa casilla ya esta revelada.\n" RESET);
                esperarEnter();
            }
        } else if (accion == 'R' || accion == 'r') {
            if (estado[f][c] == 2) {
                printf(AMARILLO "\nQuite la bandera antes de revelar.\n" RESET);
                esperarEnter();
                continue;
            }

            if (estado[f][c] == 1) {
                printf(AMARILLO "\nLa casilla ya esta revelada.\n" RESET);
                esperarEnter();
                continue;
            }

            if (mina[f][c] == 1) {
                res.resultado = RES_DERROTA;
                res.segundos = (int)difftime(time(NULL), inicio);

                limpiarPantalla();
                mostrar(1);

                printf(ROJO NEGRITA "\n   BOOM! PERDISTE.\n" RESET);
                esperarEnter();
                jugando = 0;
            } else {
                revelar(f, c);

                if (hayVictoria()) {
                    res.resultado = RES_VICTORIA;
                    res.segundos = (int)difftime(time(NULL), inicio);

                    limpiarPantalla();
                    mostrar(1);

                    printf(VERDE NEGRITA "\n   FELICIDADES! GANASTE!\n" RESET);
                    esperarEnter();
                    jugando = 0;
                }
            }
        } else {
            printf(ROJO "\nComando invalido. Use R, B o X.\n" RESET);
            esperarEnter();
        }
    }

    if (res.resultado == RES_ABANDONO) {
        res.segundos = (int)difftime(time(NULL), inicio);
    }

    res.reveladas = contarReveladas();
    res.puntaje = calcularPuntaje(res.resultado, res.reveladas, res.segundos);

    return res;
}

void mostrarResumen(const char *nombre, Resultado r) {
    const char *textos[] = {"Abandono", "Victoria", "Derrota"};

    limpiarPantalla();
    mostrarTitulo();

    printf(AMARILLO NEGRITA "\nRESUMEN DE LA PARTIDA\n\n" RESET);
    printf("Jugador:           %s\n", nombre);
    printf("Dificultad:        %s\n", nombreDificultad);
    printf("Resultado:         %s\n", textos[r.resultado]);
    printf("Casillas seguras:  %d de %d\n", r.reveladas, FILAS * COLS - minas);
    printf("Tiempo:            %d segundos\n", r.segundos);
    printf(VERDE NEGRITA "Puntaje:           %d\n" RESET, r.puntaje);

    esperarEnter();
}

int main(void) {
    char nombre[TAM_NOMBRE];
    Resultado res;

    srand((unsigned int)time(NULL));

    pedirNombre(nombre, TAM_NOMBRE);
    mostrarReglas(nombre);

    res = jugar();
    mostrarResumen(nombre, res);

    printf("\n" CELESTE "Gracias por jugar, %s!\n" RESET, nombre);

    return 0;
}