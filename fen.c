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
Funciones para manejar archivos del tipo fen
*/

/*Includes*/
#include <stdio.h>		/*prinft, scanf, sprinft, sscanf, fgets...*/
#include <stdlib.h>
#include <string.h>		/*strncat ...*/
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

/*recibimos una posición en formato FEN del GUI cuando editamos y la configuramos para que la entienda el motor*/
void SetBoard(char *string)
{
	char c, r;
	int sq=0, i=0, j=0;
	/*tipo de pieza según la letra que recibimos, ejemplo B es bishop (nosotros la tenemos definida con el número 2*/
	int	tpieza[26] = {6,2,6,6,6,6,6,6,6,6,5,6,6,1,6,0,4,3,6,6,6,6,6,6,6,6};
	int ntb = 0, ntn = 0;

	/*número de piezas iniciales*/
	peonesblancos = peonesnegros = 0;
	caballosblancos = caballosnegros = 0;
	alfilesblancos = alfilesnegros = 0;
	torresblancas = torresnegras = 0;
	damasblancas = damasnegras = 0;

	/*Rellenamos la tabla de posiciones y colores*/
	while (sq < 64) {
		enroque_mascara[sq] = 15;
		c = string[i++];
		if ('a'<c && c<'z') {
			color[sq] = NEGRO;
			pieza[sq] = tpieza[c - 'a'];
			switch (pieza[sq]) {
				case PEON:
					peonesnegros++;
					break;
				case CABALLO:
					caballosnegros++;
					break;
				case ALFIL:
					alfilesnegros++;
					break;
				case TORRE:
					ntn++;
					torresnegras++;
					if (ntn == 1) tn2 = sq;
					else if (ntn == 2) tn1 = sq;
					break;
				case DAMA:
					damasnegras++;
					break;
				case REY:
					rn = sq;
					break;
			}
			sq++;
		}
		else if ('A'<c && c<'Z') {
			color[sq] = BLANCO;
			pieza[sq] = tpieza[c - 'A'];
			switch (pieza[sq]) {
				case PEON:
					peonesblancos++;
					break;
				case CABALLO:
					caballosblancos++;
					break;

				case ALFIL:
					alfilesblancos++;
					break;
				case TORRE:
					ntb++;
					torresblancas++;
					if (ntb == 1) tb2 = sq;
					else if (ntb == 2) tb1 = sq;
					break;
				case DAMA:
					damasblancas++;
					break;
				case REY:
					rb = sq;
					break;
			}
			sq++;
		}
		else if ('1'<=c && c<='8') {
			for (j=0; j<(c - '0'); j++) {
				color[sq+j] = VACIO;
				pieza[sq+j] = VACIO;
			}
			sq += j;
		}
	}

	/*Color para el turno*/
	c = string[i++];
	if (c == 'w')		{ turno = BLANCO; }
	else if (c == 'b')	{ turno = NEGRO; }

	motor = VACIO;

    /*Flag del enroque*/
	if (strcmp (variante,"fischerandom") == 0) {
		if (strcmp (castling,"HAha") == 0) {
			enroque = 0;
			c = string[i++];
			while ((c >= 'A' && c <= 'H') || (c >= 'a' && c <= 'h')) {
				if (c >= 'A' && c <= 'H') {
					r = 65 + COLUMNA(rb);
					if (c > r) {
						enroque = enroque + 1;
						tb1 = c - 'A' + 56;
					}
					if (c < r) {
						enroque = enroque + 2;
						tb2 = c - 'A' + 56;
					}
				}
				if (c >= 'a' && c <= 'h') {
					r = 97 + COLUMNA(rn);
					if (c > r) {
						enroque = enroque + 4;
						tn1 = c - 'a';
					}
					if (c < r) {
						enroque = enroque + 8;
						tn2 = c - 'a';
					}
				}
				c = string[i++];
			}
		}
		if (strcmp (castling,"OO") == 0) {
			enroque = 0;
			c = string[i++];
			while (c == 'K' || c == 'Q' || c == 'k' || c == 'q') {
				if (c == 'K') {
					enroque = enroque + 1;
				}
				if (c == 'Q') {
					enroque = enroque + 2;
				}
				if (c == 'k') {
					enroque = enroque + 4;
				}
				if (c == 'q') {
					enroque = enroque + 8;
				}
				c = string[i++];
			}
		}
	}
	else {
		enroque = 0;
		c = string[i++];
		while (c == 'K' || c == 'Q' || c == 'k' || c == 'q') {
			if (c == 'K') {
				enroque = enroque + 1;
				tb1 = 63;
			}
			if (c == 'Q') {
				enroque = enroque + 2;
				tb2 = 56;
			}
			if (c == 'k') {
				enroque = enroque + 4;
				tn1 = 7;
			}
			if (c == 'q') {
				enroque = enroque + 8;
				tn2 = 0;
			}
			c = string[i++];
		}
	}

	/*máscaras para los enroques*/
	enroque_mascara[rb] = 12;
	enroque_mascara[rn] = 3;
	enroque_mascara[tb2] = 13;
	enroque_mascara[tb1] = 14;
	enroque_mascara[tn2] = 7;
	enroque_mascara[tn1] = 11;

	/*Flag de al paso*/
	c = string[i++];
	if (c>='a' && c<='h') {
		alpaso = (c - 'a');
		c = string[i++];
		if (c>='1' && c<='8') alpaso += 8*(7-(c - '1'));
		else alpaso = -1;
	}
	else alpaso = -1;


	/*no comprobamos en el fen la regla de 50 movimientos o el número de jugadas, la ponemos directamente a 0*/
	regla50 = 0;	        /*inicamos la regla de los 50 movimientos*/
	njugadas = 0;	        /*ponemos a 0 el contador de jugadas*/

	fuera_del_libro = 0;
	hacer_ponder = FALSO;	/*inicialmente tenemos que buscar más que ponderar*/

	Calcula_material();		/*calculamos el material en centipeones*/

	/*calculamos las distancias entre casillas*/
	for (i=0;i<64;i++) {
		for (j=0;j<64;j++) {
			distancia[i][j] = MAX(abs(FILA(j) - FILA(i)), abs(COLUMNA(j) - COLUMNA(i)));
		}
	}

	clear_hash();			/*borramos las tablas hash*/
	poner_hash();			/*calculamos la llave hash para la posición*/
}

/*convertimos una posición de tablero a formato FEN (epd) para poder consultar el libro de aperturas*/
void Tablero_FEN(char *string)
{
	int x, y, l=0, i=0, sq;
	char row[8];

	strcpy(string,"");

	for (y=0;y<8;y++) {
		i=l=0;
		strcpy(row,"");
		for (x=0;x<8;x++) {
			sq = (y*8)+x;
			if (color[sq] == VACIO) l++;
			else {
				if (l>0) {
					row[i] = (char) (l+48);
					i++;
				}
				l=0;
				if (color[sq] == BLANCO) {
					switch (pieza[sq]) {
						case PEON	 : row[i]='P';break;
						case CABALLO : row[i]='N';break;
						case ALFIL	 : row[i]='B';break;
						case TORRE	 : row[i]='R';break;
						case DAMA	 : row[i]='Q';break;
						case REY	 : row[i]='K';break;
					}
				}
				else {
					switch (pieza[sq]) {
						case PEON	 : row[i]='p';break;
						case CABALLO : row[i]='n';break;
						case ALFIL	 : row[i]='b';break;
						case TORRE	 : row[i]='r';break;
						case DAMA	 : row[i]='q';break;
						case REY	 : row[i]='k';break;
					}
				}
				i++;
			}
		}
		if (l>0) {
			row[i] = (char) (l+48);
			i++;
		}
		strncat(string,row,i);
		if (y<7) strcat(string,"/");
	}

	if (turno == BLANCO) strcat(string," w ");
	else strcat(string," b ");
}

