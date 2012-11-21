/*
	< DanaSah is a chess engine winboard. >
    Copyright (C) <2011>  <Pedro Castro Elgarresta>
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
Funciones que envían información al GUI
*/

/*Includes*/
#include <stdio.h>		/*prinft, scanf, sprinft, sscanf, fgets...*/
#include <string.h>
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

/*muestra información sobre el movimiento seleccionado, profundidad, puntuación, tiempo
empleado y nodos examinados, compatible con Arena y otros GUI*/
void print_pv(int depth, int score)
{
	movimiento	mi;
	int i, j, tiempousado;
	char c1, c2;

	i = pv_longuitud[ply];	                        /*calculamos la longuitud de la línea*/
	tiempousado =( get_ms()- tiempo_inicio)/10;	    /*tiempo usado en centisegundo*/

	if (i > 0) {
		/*enviamos profundidad, puntos, tiempo (centisegundos), nodos*/

		if (estado == PONDERANDO)
			printf("%d %d %d %d ", depth, -score, tiempousado, nodos);
		else
			printf("%d %d %d %d ", depth, score, tiempousado, nodos);

		/*imprimimos la mejor linea*/
		for (j = 0; j < i; ++j) {
			mi = pv[0][j];
			if (pv[0][j].a == 0 && pv[0][j].de == 0)
				break;

			if ((strcmp (variante,"fischerandom") == 0) && mi.tipo == ENROQUE) {
				if (mi.a == 62 || mi.a == 6) {
					printf("O-O ");
				}
				else if (mi.a == 58 || mi.a == 2) {
					printf("O-O-O ");
				}
			}
			else {
				c1 = ' ';	/*dejamos un espacio*/
				if (mi.tipo < PROMOCION_A_DAMA) {
					printf("%c%d%c%d%c",
						'a' + COLUMNA(mi.de),
						8 - FILA(mi.de),
						'a' + COLUMNA(mi.a),
						8 - FILA(mi.a),
						c1);
				}
				else  {
				    c2 = ' ';
					/*letra a imprimir si hay una promoción*/
					switch (mi.tipo) {
						case PROMOCION_A_DAMA:
							c2 = 'q';
							break;
						case PROMOCION_A_TORRE:
							c2 = 'r';
							break;
						case PROMOCION_A_ALFIL:
							c2 = 'b';
							break;
						case PROMOCION_A_CABALLO:
							c2 = 'n';
					}
					printf("%c%d%c%d=%c%c",
						'a' + COLUMNA(mi.de),
						8 - FILA(mi.de),
						'a' + COLUMNA(mi.a),
						8 - FILA(mi.a),
						c2,c1);
				}
			}
		}
		printf("\n");		/*cuando terminamos con la linea debemos mandar un retorno de carro*/
#if UNDER_CE
		fflush(stdout);		/*obligado por el protocolo xboard*/
#endif
	}
}

/*indica al GUI el movimiento que debe realizar*/
void Enviar_Movimiento(movimiento m)
{
	char c;
	/*ahora indicamos al motor que tipo de movimiento tiene que hacer*/

	/*Enviamos un enroque si estamos jugando a chess960*/
	if ((strcmp (variante,"fischerandom") == 0) && m.tipo == ENROQUE) {
		if (m.a == 62 || m.a == 6)
			printf("move O-O\n");
		if (m.a == 58 || m.a == 2)
			printf("move O-O-O\n");
	}
	/*si estamos en un juego normal*/
	else {
		/*según protocolo xboard*/
		switch (m.tipo) {
			case PROMOCION_A_DAMA:
				c = 'q';
				break;
			case PROMOCION_A_TORRE:
				c = 'r';
				break;
			case PROMOCION_A_ALFIL:
				c = 'b';
				break;
			case PROMOCION_A_CABALLO:
				c = 'n';
				break;
			default:
				c = ' ';
		}
		/*al enviar el movimiento al gui debemos anteponer la palabra move*/
#if UNDER_CE
		if (m.tipo < PROMOCION_A_DAMA)
			printf("move %c%d%c%d%c\n",
					'a' + COLUMNA(m.de),
					8 - FILA(m.de),
					'a' + COLUMNA(m.a),
					8 - FILA(m.a),
					c);
		else
			printf("move %c%d%c%d=%c\n",
					'a' + COLUMNA(m.de),
					8 - FILA(m.de),
					'a' + COLUMNA(m.a),
					8 - FILA(m.a),
					c);
#else
		printf("move %c%d%c%d%c\n",
				'a' + COLUMNA(m.de),
				8 - FILA(m.de),
				'a' + COLUMNA(m.a),
				8 - FILA(m.a),
				c);
#endif
	}
}

/*si es final de partida indica el resultado de la misma y para el motor*/
void imprime_resultado(int cuenta, movimiento *lista)
{
	int i;

	/*comprobamos si hay un movimiento legal*/
	for (i = 0; i < cuenta; ++i)
		if (Hacer_Movimiento(lista[i])) {
			Deshacer();
			break;
		}
	if (i == cuenta) {                  /*si hemos llegado al final de cuenta no hay movimientos entonces*/
		if (EstaEnJaque(turno)) {
			if (turno == BLANCO){
				/*si toca mover a blancas y están en jaque han perdido*/
				printf("0-1 {Black mates}\n");
			}
			else {
				/*si toca mover a negras y están en jaque han perdido*/
				printf("1-0 {White mates}\n");
			}
		}
		else {
			/*tablas porque quien mueve está ahogado*/
			printf("1/2-1/2 {Stalemate}\n");
		}
	}
	else if (repeticion() == 2) {
		/*tablas por repetición de 3 veces la misma posición*/
		printf("1/2-1/2 {Draw by repetition}\n");
	}
	else if (regla50 >= 100) {
		/*tablas por la regla de los 50 movimientos*/
		printf("1/2-1/2 {Draw by 50 move rule}\n");
	}
	else if (reclama_tablas_material() == VERDADERO) {
		/*tablas por que no hay suficiente material para hacer mate*/
		printf("1/2-1/2 {Draw by insufficient material}\n");
	}
}

