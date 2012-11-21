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

/* Definiciones y tipos que vamos a utilizar a lo largo de todo el programa */

/*definimos las 6 piezas, quizás hubiera sido mejor definir piezas blancas y negras por
separado ya que luego sería menos costoso hacer comparaciones, una en lugar de 2*/
#define	PEON    0
#define	CABALLO 1
#define	ALFIL	2
#define	TORRE	3
#define	DAMA	4
#define	REY		5
#define	VACIO   6	/*VACIO es color de pieza y tipo de pieza*/

/*colores de las piezas y turnos*/
#define	BLANCO  1
#define	NEGRO   0

#define VERDADERO	1
#define FALSO		0

/*valores de las piezas en centipeones*/
#define	VALOR_PEON		100
#define	VALOR_CABALLO   315
#define	VALOR_ALFIL		320
#define	VALOR_TORRE     500
#define	VALOR_DAMA		950
#define	VALOR_REY		32767	/*este valor no llego a utilizarlo*/
#define	VALOR_VACIO		0

#define	MATE            32767
#define VAL_BITBASE		0

#define MAX_PROFUNDIDAD	100		/*máxima ply con la que pensar*/
#define STACK			4096	/*máximo número de movimientos que se generan en el arbol y que guardamos*/
#define PLY				4		/*utilizado para extensiones fraccionales*/

#define MARGEN			20

#define COLUMNA(x)	(x & 7)		/*columna*/
#define FILA(y)		(y >> 3)	/*fila*/

/*tipos de movimientos*/
#define NINGUNO					0
#define NORMAL					1
#define ENROQUE					2
#define AL_PASO					3
#define PEON_AVANZA_2			4
#define PROMOCION_A_DAMA		5
#define PROMOCION_A_TORRE		6
#define PROMOCION_A_ALFIL		7
#define PROMOCION_A_CABALLO		8

/*TTables*/
#define hasfNULL	0
#define hasfEXACT	1
#define hasfALPHA	2
#define hasfBETA	3
#define valUNKNOWN	32767

/*diferentes formas de cargar las bitbases en memoria*/
#define LOAD_NONE	0
#define LOAD_4MEN	1
#define SMART_LOAD	2
#define LOAD_5MEN	3

/*Búsqueda*/
#define BUSCANDO		1
#define PONDERANDO		2
#define ANALIZANDO		3

/*fases del juego según la cantidad de material*/
/*#define APERTURA	((2*VALOR_DAMA)+(4*VALOR_TORRE)+(4*VALOR_ALFIL)+(4*VALOR_CABALLO)+(16*VALOR_PEON))*/
#define MEDIOJUEGO	((VALOR_DAMA)+(2*VALOR_TORRE)+(2*VALOR_ALFIL)+(2*VALOR_CABALLO)+(8*VALOR_PEON))
#define FINAL		((4*VALOR_TORRE)+(VALOR_ALFIL)+(VALOR_CABALLO)+(4*VALOR_PEON))

#define STARTPOS		"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNRwKQkq-"

#define MAX(x,y) ((x) > (y) ? (x) : (y))
#define MIN(x,y) ((x) < (y) ? (x) : (y))

#if defined (_MSC_VER)
    typedef unsigned __int64 U64;
    typedef unsigned __int32 U32;
#elif defined (__GNUC__) || (__MINGW32__)
    #include <stdint.h>
    typedef uint64_t U64;
    typedef uint32_t U32;
#else
    typedef unsigned long long U64;
    typedef unsigned long U32;
#endif

/*un movimiento tiene 4 características, casilla inicial de, final a, tipo de movimiento que
se trata y un valor dado durante la generación para luego poder ordenarlo, en la busqueda es
importante analizar primero los movimientos más fuertes*/
typedef struct estructura_movimiento {
    int  de,
		 a,
         tipo;
	int valor;
}	movimiento;

/*una jugada tiene las siguientes características, el movimiento realizado, pieza capturada
por si tenemos que deshacer el movimiento, si tiene posibilidad de enroque, al paso, lleva
cuenta de la regla de los 50 movimientos y asociamos un número único a la posición*/
typedef struct estrutura_jugada {
    movimiento	m;
    int	captura,
		enroque,
		alpaso,
		regla50;
	U64 hash;
}	jugada;

/*Elemento de la tabla hash, en la tabla hash guardamos el número único asociado con la
posición, la puntuación para la posición, tipo de valor guardado (si es más alto, bajo o exacto),
la profundidad en la que nos encontramos y el mejor movimiento a jugar*/
typedef struct {
	U64 Hash;
	int Type;
	int Depth;
	int Value;
	movimiento Mov;
} TType;

/*Elemento de la evaluación de cache, aquí guardamos la llave de la posición y la puntuación, en
este caso la puntuación es la verdadera obtenida en la evaluación*/
typedef struct {
	U64 Hash;
	int Bound;
} EType;

/*see*/
typedef struct {
	int deslizante;
	int direccion;
	int casilla;	
} tipo_see;

/*utilizado para la versión pocketpc*/
#if _UNICODE
    #define STRING(s_) L##s_
#else
    #define STRING(s_) s_
#endif
