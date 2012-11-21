/*
	< DanaSah is a chess engine winboard. >
    Copyright (C) <2012>  <Pedro Castro Elgarresta>
	E-mail <pecastro@msn.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
Variables globales del programa
*/

#include "definiciones.h"

/*nombre y fecha del programa*/
char nombre[]="DanaSah 5.00";
char fecha[]="09/10/2012";

char ownbook[256] = "on";	/*con ownbook activado utilizamos el libro propio del programa en las aperturas*/
char libroprincipal[256] = "mainbook.bin";
char librotorneo[256] = "tourbook.bin";
int men = 4;				/*número de piezas de las bitbases*/
int egbb_load_type = 1;
char castling[256] = "OO";		/*predeterminamos el gui para FRC como Arena o ChessGUI, lo podemos cambiar en el archivo ini*/

#if UNDER_CE
char egbb_path[256] = "\\Storage Card\\egbb\\";	/*ruta donde están situadas las bitbases por defecto*/
int egbb_cache_size = 1;					/*tamaño por defecto para el caché de las bitbases*/
#else
char egbb_path[256] = "./egbb/";	        /*ruta donde están situadas las bitbases :  por defecto*/
int egbb_cache_size = 4;					/*tamaño por defecto para el caché de las bitbases*/
#endif

int turno;			        			/*quien mueve? blanco o negro*/
int motor;			        			/*color con el que juega el programa*/
int ponder;			        			/*piensa en el tiempo del contrario*/
int hacer_ponder;						/*inicialmente no hacemos ponder a no ser que recibamos confirmación*/
int estado;			        			/*controla si estamos buscando, ponderando o analizando*/

int post;                   			/*utilizada para imprimir la linea principal o no (no utilizada de momento)*/

char variante[256] = "normal";			/*danasah juega 2 variantes; normal y fischerandom (FRC)*/
char servidor[256];						/*indica el servidor donde está jugando el motor*/

int max_profundidad = MAX_PROFUNDIDAD;	/*nivel de busqueda en el árbol*/
int depthnull;		        /*profundidad para el movimiento nulo*/
int sigue_Pv;		        /*nos permite saber si estamos siguiendo la variante principal*/

int max_tiempo;		        /*máximo de tiempo disponible para un movimiento en milisegundos*/
int mps;			        /*número de jugadas a realizar para cumplir el control de tiempo*/
int base;			        /*tiempo base en minutos para cumplir el control de tiempo*/
int inc;			        /*tiempo de incremento para el reloj en segundos despues de jugar*/
int quedan;			        /*número de movimientos que quedan para el control de tiempo*/
int tiempo_inicio;	        /*momento en el que empezamos un movimiento*/
int limite_tiempo;			/*no podremos exceder este límite de tiempo nunca durante una búsqueda*/
int no_nuevo_movimiento;	/*no empezar a buscar un nuevo movimiento si ya hemos sobrepasado un tiempo*/
int no_nueva_iteracion;		/*no empezar un nuevo nivel de profundidad si no disponemos de suficiente tiempo*/
int tiempo_total = 0;

int aborta;
int para_busqueda;	        /*para poder salir de la búsqueda si termina el tiempo*/
int para_ponder;	        /*para poder salir del ponder*/
int para_analisis;	        /*para poder salir del análisis*/

int nodos;			        /*número de nodos buscados*/
int cknodos;
int ply;                    /*nivel de profundidad que estamos en el alphabeta*/
int njugadas;		        /*número de movimientos en la partida (blancos+negros)*/

/*int puntos;			        //evaluación de una posición en centipeones */
int puntosblanco;	        /*turno blanco, puntos = puntosblanco-puntosnegro*/
int puntosnegro;	        /*turno negro, puntos = puntosnegro-puntosblanco*/
int puntosblanco_m, puntosblanco_f, puntosnegro_m, puntosnegro_f;	/*fase intermedia*/
int material;		        /*la suma de los puntos del blanco y del negro*/
int hacer_lazy_eval;

int	piezasblanco;	        /*puntos que suman las piezas blancas del tablero (no peones)*/
int	piezasnegro;	        /*puntos que suman las piezas negras del tablero (no peones)*/
int	peonesblanco;	        /*puntos que suman los peones blancos del tablero*/
int	peonesnegro;	        /*puntos que suman los peones negros del tablero*/

int old_piezasblanco;		/*antes de iniciar la búsqueda guardamos información del material*/
int old_piezasnegro;		/*luego lo podremos utilizar en la evaluación*/
int old_peonesblanco;
int old_peonesnegro;

int peonesblancos, peonesnegros;		/*número de peones blancos y negros*/
int caballosblancos, caballosnegros;	/*número de caballos blancos y negros*/
int alfilesblancos, alfilesnegros;		/*número de alfiles blancos y negros*/
int torresblancas, torresnegras;		/*número de torres blancas y negras*/
int damasblancas, damasnegras;			/*número de damas blancas y negras*/

int wbcolor, bbcolor, wpcolor, bpcolor, npcolor;
int alfilblanco, alfilnegro;	/*para saber si los dos alfiles están en la misma diagonal*/

int rb;				        /*posición del rey blanco*/
int rn;				        /*posición del rey negro*/

int tb1, tb2, tn1, tn2;	    /*posiciones de las torres (para chess960)*/

int distancia[64][64];	    /*distancia entre 2 casillas (filas y columnas)*/

/*guardamos la fila del peon para cada uno de los bandos, suponemos 10 columnas en lugar de
ocho para luego hacer más fácil la comprobación de peones en columnas adyacentes*/
int fila_peon[2][10];
int aislado;			    /*guarda si el peón está aislado*/
int retrasado;			    /*guarda si el peón está retrasado*/
int abierto;			    /*guarda si el peón está abierto*/
int tmp;				    /*casilla donde vamos a coronar*/

/*variable valor_pieza utilizada en el cálculo de material*/
int	valor_pieza[7] = {VALOR_PEON, VALOR_CABALLO, VALOR_ALFIL, VALOR_TORRE, VALOR_DAMA, VALOR_REY, VALOR_VACIO};
int valor_pieza2[7] = { 100, 300, 300, 500, 900, 10000, 0};

int pieza[64], color[64];   /*donde guardarmos la posición del tablero*/

int enroque;	            /*posibilidad para el enroque*/

int alpaso;                 /*posibilidad para comer al paso*/
int regla50;                /*contador para la regla de los 50 movimientos*/

/*Variables para almacenar una posición, utilizadas en las tablas hash utlizando llaves de Zobrist
Para que no haya colisiones con las posiciones necesitamos tener bien definido en el tablero el
color de la pieza, que tipo de pieza es, la casilla donde está, no es lo mismo que sea el turno de
las blancas o de las negras y si en el momento de mover tenemos la posibilidad de comer al paso.
Deberíamos quizás también tener en cuenta los enroques aunque yo no lo hago, parece funcionar bien.
*/
U64 hash_piezas[2][6][64];	/*color, tipo de pieza, casilla*/
U64 hash_turno[2];			/*turno*/
U64 hash_alpaso[64];		/*si hay posibilad de comer al paso*/
U64 hash_enroque[16];		/*si podemos o no enrocar*/

U64 hash;	                /*llave hash*/

/*TTables*/
int TTSize;		            /*tamaño para las tablas hash*/

/*ETables*/
int ETSize;		            /*tamaño para la evaluation cache*/

/*libro*/
int usa_libro;				                /*determina si usuamos el libro de aperturas o no*/
int libro_cargado = FALSO;
int fuera_del_libro	= 0;	                /*para no comprobar el libro de aperturas si estamos fuera de éste*/

/*bitbases*/
int egbb_is_loaded = FALSO;

char fenstring[256];		/*aquí mantenemos una posición en formato fen*/

int lista_piezas[33];		/*mantenemos una semilista de piezas para hacer los bucles más rápidos*/
int cuenta_piezas;			/*llevamos una cuenta con las piezas que hay en el tablero*/

/*vector utilizado para actualizar los enroques después de mover una pieza (como TSCP)
Si movemos cualquier pieza que no sea el rey o torre seguiremos teniendo intactas las posi-
bilidades de enroque (15), si movemos una torre o el rey perderemos alguno de los enroques*/
int enroque_mascara[64] = {
	 7, 15, 15, 15,  3, 15, 15, 11,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	13, 15, 15, 15, 12, 15, 15, 14
};

/*la variante principal tiene una forma trianqular, una explicaci?e porqu頥sto es necesitado se
puede encontrar en el libro "How Computers Play Chess" by Levy and Newborn*/
movimiento pv[MAX_PROFUNDIDAD+1][MAX_PROFUNDIDAD+1];
int pv_longuitud[MAX_PROFUNDIDAD+1];	        /*cuentos movimientos tiene la variante principal*/

jugada jugadas[STACK];		                /*almacenamos las jugadas que se van produciendo*/

movimiento killer1[MAX_PROFUNDIDAD+1];		/*guarda los movimiento#include "definiciones.h"s que producen un cuttoff*/
movimiento killer2[MAX_PROFUNDIDAD+1];
movimiento matekiller[MAX_PROFUNDIDAD+1];	    /*movimiento que amenaza mate*/

movimiento no_move = {0,0,0,0};			    /*cuando no tenemos un movimiento, por ejemplo en las tablas hash*/

movimiento mp;				                /*movimiento que creemos que harᠥl contrario*/
movimiento mc;				                /*movimiento que realiza el contrario*/
int li = 1;					                /*profundidad desde la cual iniciamos*/

int Historia[64][64];		                /*utilizado para la ordenaci?e movimientos*/
int maxhistoria;			                /*limitamos la historia para que no sobrepase el valor de otros movimientos*/

int valor1[MAX_PROFUNDIDAD+1];
int valor2[MAX_PROFUNDIDAD+1];


/*see*/
tipo_see atacantes[2][16];
int num_atacantes[2];

/* mailbox */
int mailbox[120] = {
	 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	 -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
	 -1,  8,  9, 10, 11, 12, 13, 14, 15, -1,
	 -1, 16, 17, 18, 19, 20, 21, 22, 23, -1,
	 -1, 24, 25, 26, 27, 28, 29, 30, 31, -1,
	 -1, 32, 33, 34, 35, 36, 37, 38, 39, -1,
	 -1, 40, 41, 42, 43, 44, 45, 46, 47, -1,
	 -1, 48, 49, 50, 51, 52, 53, 54, 55, -1,
	 -1, 56, 57, 58, 59, 60, 61, 62, 63, -1,
	 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	 -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

int mailbox64[64] = {
	21, 22, 23, 24, 25, 26, 27, 28,
	31, 32, 33, 34, 35, 36, 37, 38,
	41, 42, 43, 44, 45, 46, 47, 48,
	51, 52, 53, 54, 55, 56, 57, 58,
	61, 62, 63, 64, 65, 66, 67, 68,
	71, 72, 73, 74, 75, 76, 77, 78,
	81, 82, 83, 84, 85, 86, 87, 88,
	91, 92, 93, 94, 95, 96, 97, 98
};

int offset[6][8] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0 },
	{ -21, -19, -12, -8, 8, 12, 19, 21 },
	{ -11, -9, 9, 11, 0, 0, 0, 0 },
	{ -10, -1, 1, 10, 0, 0, 0, 0 },
	{ -11, -10, -9, -1, 1, 9, 10, 11 },
	{ -11, -10, -9, -1, 1, 9, 10, 11 }
};
