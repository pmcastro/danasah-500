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
Funciones que reciben información del GUI
*/

/*Includes*/
#include <stdio.h>		/*prinft, scanf, sprinft, sscanf, fgets...*/
#if defined (_WIN32) || defined(_WIN64)
    #ifndef WINDOWS
        #define WINDOWS
    #endif
    #include <windows.h>	/*bioskey()*/
#else
    #include <sys/time.h>
    #include <string.h>
    struct timeval tv;
    struct timezone tz;
#endif
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

/*la siguiente función nos va a permitir comprobar que tipo de movimiento nos llega,
es utilizada para la siguiente función "Comprobar_Movimiento()"*/
int Comprobar_Tipo_Movimiento(int de, int a, char *s)
{
	int tipo = NINGUNO;

	/*comprueba si nos llega un enroque si el tipo es variante normal*/
	if (strcmp (variante,"normal") == 0 && pieza[de] == REY) {
		if ((s[0] == 'e' && s[1] == '1') || (s[0] == 'e' && s[1] == '8')) {
			if ((s[2] == 'g' && s[3] == '1') || (s[2] == 'c' && s[3] == '1') || (s[2] == 'g' && s[3] == '8') || (s[2] == 'c' && s[3] == '8'))
				tipo = ENROQUE;
		}
	}
	if (pieza[de] == PEON) {
		/*comprueba si es un movimiento al paso*/
		if ((alpaso != -1) && (a == alpaso))
			tipo = AL_PASO;
		/*comprueba si es un movimiento de avance del peon 2 casillas*/
		else if ((s[1] == '2' && s[3] == '4') || (s[1] == '7' && s[3] == '5'))
			tipo = PEON_AVANZA_2;
		/*comprueba si el movimiento es una promoción*/
		else if (a < 8 || a > 55) {
#if UNDER_CE
			if (s[4] == '=') {
				if (s[5] == 'q' || s[5] =='Q')
					tipo = PROMOCION_A_DAMA;
				else if (s[5] == 'r' || s[5] == 'R')
					tipo = PROMOCION_A_TORRE;
				else if (s[5] == 'b' || s[5] == 'B')
					tipo = PROMOCION_A_ALFIL;
				else if (s[5] == 'n' || s[5] == 'N')
					tipo = PROMOCION_A_CABALLO;
			}
			else {
				if (s[4] == 'q' || s[4] =='Q')
					tipo = PROMOCION_A_DAMA;
				else if (s[4] == 'r' || s[4] == 'R')
					tipo = PROMOCION_A_TORRE;
				else if (s[4] == 'b' || s[4] == 'B')
					tipo = PROMOCION_A_ALFIL;
				else if (s[4] == 'n' || s[4] == 'N')
					tipo = PROMOCION_A_CABALLO;
			}
#else
			if (s[4] == 'q' || s[4] =='Q')
				tipo = PROMOCION_A_DAMA;
			else if (s[4] == 'r' || s[4] == 'R')
				tipo = PROMOCION_A_TORRE;
			else if (s[4] == 'b' || s[4] == 'B')
				tipo = PROMOCION_A_ALFIL;
			else if (s[4] == 'n' || s[4] == 'N')
				tipo = PROMOCION_A_CABALLO;
#endif
		}
	}
	return tipo;
}

/*la siguiente función va a comprobar si llega un movimiento del usuario y si este movimiento es legal o no*/
void Comprobar_Movimiento(char *s)
{
	int de, a, tipo, i;
	int contar;
	movimiento lista[256];

	/*comprobamos si recibimos un enroque en chess960*/
	if (strcmp (s,"O-O") == 0) {
		if (turno == BLANCO) {
			de = rb;
			a = 62;
		}
		else {
			de = rn;
			a = 6;
		}
		tipo = ENROQUE;
	}
	else if (strcmp (s,"O-O-O") == 0) {
		if (turno == BLANCO) {
			de = rb;
			a = 58;
		}
		else {
			de = rn;
			a = 2;
		}
		tipo = ENROQUE;
	}
	else {
	/*comprueba si estamos recibiendo un movimiento*/
		if (s[0] < 'a' || s[0] > 'h' ||
			s[1] < '0' || s[1] > '9' ||
			s[2] < 'a' || s[2] > 'h' ||
			s[3] < '0' || s[3] > '9') {
			printf("Error (unknown command): %s\n", s);     /*si no es un movimiento debe ser un comando desconocido*/
			return;
			}

		de = s[0] - 'a';
		de += 8 * (8 - (s[1] - '0'));
		a = s[2] - 'a';
		a += 8 * (8 - (s[3] - '0'));
		tipo = NORMAL;

		/*comprueba tipo de movimientos*/
		if (Comprobar_Tipo_Movimiento(de, a, s) != NINGUNO)
			tipo = Comprobar_Tipo_Movimiento(de, a, s);
	}

	ply = 0;

    /*nosotros generamos una lista con todos los movimientos posibles*/
	contar = Generar(lista);

    /*recorre todos los movimientos para ver si es legal*/
	for (i = 0; i < contar; i++) {
		if (lista[i].de == de && lista[i].a == a && lista[i].tipo == tipo) {
			if (Hacer_Movimiento(lista[i]))		/*hacemos el movimiento si está en nuestra lista*/
				return;
			else {
				printf("Illegal move: %s\n", s);
				return;
			}
		}
	}
	printf("Illegal move: %s\n", s);	            /*si el movimiento no es legal se lo dice al GUI*/
	return;
}

/*la siguiente función permite salir de una búsqueda si llega una interrupción por parte del
GUI o permite saber cuando tenemos que dejar de ponderar y empezar a pensar, esta función está
basada en el programa Olithink y hay cantidad de programas que la utilizan*/
int bioskey(void)
{
#ifndef WINDOWS
	fd_set readfds;
	FD_ZERO (&readfds);
	FD_SET (fileno(stdin), &readfds);
	tv.tv_sec=0; tv.tv_usec=0;
	select(16, &readfds, 0, 0, &tv);

	return (FD_ISSET(fileno(stdin), &readfds));
#else
    #if UNDER_CE
        return InputAvailable();
    #else
	    static int init = 0, pipe;
	    static HANDLE inh;
	    DWORD dw;

	    if (!init) {
		    init = 1;
		    inh = GetStdHandle(STD_INPUT_HANDLE);
		    pipe = !GetConsoleMode(inh, &dw);
		    if (!pipe) {
			    SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
			    FlushConsoleInputBuffer(inh);
		    }
	    }
	    if (pipe) {
		    if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) {
			    return 1;
		    }
		    return dw;
	    } else {
		    GetNumberOfConsoleInputEvents(inh, &dw);
		    return dw <= 1 ? 0 : dw;
	    }
    #endif
#endif
}

/*get_ms() nos indica la hora actual en milisegundos desde el 1 de enero de 1970*/
int get_ms(void)
{
#ifndef WINDOWS
	gettimeofday (&tv, &tz);
	return(tv.tv_sec * 1000 + (tv.tv_usec / 1000));
#else
    return GetTickCount();
#endif
}
