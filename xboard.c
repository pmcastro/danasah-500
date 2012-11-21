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
Función xboard(), sustituye a la función principal si recibimos el parámetro xboard,
gracias a estos comandos, podemos hacer jugar a DanaSah con interfaces de usuario como
Winboard, Arena, Fritz, Chessmaster, etc.
Para comprender dicho protocolo se debe examinar la siguiente página:
http://home.hccnet.nl/h.g.muller/engine-intf.html
*/

/*Includes*/
#if UNDER_CE
	#include <windows.h>
#endif
#include <stdio.h>		/*prinft, scanf, sprinft, sscanf, fgets...*/
#include <string.h>
#include <stdlib.h>		/*malloc, free, exit ...*/
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

void xboard(void) /*protocolo xboard*/
{
	char linea[256], comando[256], comando2[256];	/*para leer la información del GUI y saber que comando llega*/
	char args[4][64];								/*para el comando setboard*/
	int entrada = 0;                                /*recibe el número de movimientos para el control de tiempo*/
	char minutos[4];	                            /*recibe el número de minutos base para el control de tiempo*/
	int incremento;	                                /*recibe el número de segundos de incremento para el control de tiempo*/
	int contar;		                                /*numero de movimientos posibles en una posición*/
	movimiento lista[256], mejormovimiento;	        /*lista de movimientos posibles y mejormovimiento*/

/*En windows o linux podemos intentar eliminar el problema de buffering al enviar algo a xboard*/
#if !UNDER_CE
	setbuf(stdout, NULL);
#endif

	printf("feature done=0\n");

	variante[0] = '\0';
	strcpy(variante,"normal");
	strcpy(fenstring, "");
	strcat(fenstring, STARTPOS);
	SetBoard(fenstring);
	motor = NEGRO;

	Carga_libro();
	Carga_bitbases();

	/*bucle esperando recibir un comando del GUI*/
	for (;;) {
/*En el Pocket Pc tenemos que hacer fflush después de cada línea de salida*/
#if UNDER_CE
		fflush(stdout);
#endif
        if (turno == motor) {                       /*turno del programa*/
			estado = BUSCANDO;
			mejormovimiento = MotorPiensa();	    /*piensa la jugada*/
			Hacer_Movimiento(mejormovimiento);	    /*guarda el movimiento en la lista de jugadas*/

			Enviar_Movimiento(mejormovimiento);     /*envia el movimiento al GUI*/
			contar = Generar(lista);		  	    /*genera los movimientos del contrario para saber si ha recibido mate*/
			imprime_resultado(contar, lista);	    /*si es final de partida imprime el resultado*/
#if UNDER_CE
			if (max_tiempo > 1000)
				hacer_ponder = VERDADERO;
			else
				hacer_ponder = FALSO;
#else
			hacer_ponder = VERDADERO;
#endif
			continue;
        }
		else if (motor != VACIO) {
    		if (ponder && hacer_ponder) {		    /*pondera si el GUI le envía una señal*/
#if UNDER_CE
				if (fuera_del_libro) {				/*lo hacemos si no estamos en el libro*/
					estado = PONDERANDO;
					mejormovimiento = MotorPiensa();    /*piensa la jugada*/
       			}
#else
				estado = PONDERANDO;
				mejormovimiento = MotorPiensa();    /*piensa la jugada*/
#endif
				hacer_ponder = FALSO;			    /*deja de ponderar*/
    			continue;
            }
        }
		/*almacenamos lo que nos llega y lo guardamos en la variable comando*/
#if UNDER_CE
		if (gets(linea) == NULL)
			return;
#else
		if (!fgets(linea, 256, stdin))
			return;
#endif
		if (linea[0] == '\n')
			continue;
		sscanf(linea, "%s", comando);

		/*si llega xboard simplemente continuamos*/
		if (strcmp(comando, "xboard") == 0) {
			continue;
		}
		/*comprueba si tenemos protocolo winboard 2 y definimos algunas características*/
		if (strcmp(comando,"protover") == 0) {
			printf("feature colors=0 draw=0 ics=1 myname=\"%s\" ping=1 setboard=1 sigint=0 sigterm=0 variants=\"normal,nocastle,fischerandom\"\n", nombre);
			printf("feature done=1\n");
			continue;
		}
		/*nos indica si las características definidas son aceptadas o no*/
		if (strcmp(comando, "accepted") == 0) {
			/*printf("feature accepted\n");*/
			continue;
		}
		if (strcmp(comando, "rejected") == 0) {
			printf("feature rejected\n");
			continue;
		}
		/*comprueba si hay que empezar nueva partida*/
		if (strcmp(comando, "new") == 0) {
			variante[0] = '\0';
			strcpy(variante,"normal");
			strcpy(fenstring, "");
			strcat(fenstring, STARTPOS);
			SetBoard(fenstring);
			motor = NEGRO;
			continue;
		}
		/*nos indica si jugamos una variante que no es la normal*/
		if (strcmp(comando, "variant") == 0) {
			sscanf(linea, "variant %s", comando2);
			variante[0] = '\0';
			strcpy(variante,comando2);
			continue;
		}
		/*comando para cerrar el motor */
		if (strcmp(comando, "quit") == 0)
			return;
		/*comando que en GNUChess añadía un pequeño valor aleatorio a la evaluación*/
		if (strcmp(comando, "random") == 0)
			continue;
		/*el motor deja de jugar*/
		if (strcmp(comando, "force") == 0) {
			motor = VACIO;
			continue;
		}
		/*fuerza al motor a jugar*/
		if (strcmp(comando, "go") == 0) {
			motor = turno;
			continue;
		}
		/*turno para las blancas, el motor juega negras*/
		if (strcmp(comando, "white") == 0) {
			turno = BLANCO;
			motor = NEGRO;
			continue;
		}
		/*turno para las negras, el motor juega blancas*/
		if (strcmp(comando, "black") == 0) {
			turno = NEGRO;
			motor = BLANCO;
			continue;
		}
		/*parámetros para configurar el nivel de juego del programa	level 40 5 0 --> 40 movimientos 5 minutos incremento 0
		si el número de movimientos es 0 la partida es a finish*/
		if (strcmp(comando, "level") == 0) {
			sscanf(linea, "level %d %s %d", &entrada, minutos, &incremento);
			mps = entrada;	    /*nos interesa saber cuantos movimientos para el control*/
			inc = incremento * 1000;
			continue;
		}
		/*configura el nivel de juego indicando cuantos segundos por movimiento	st 10 --> 10 segundos por jugada*/
		if (strcmp(comando, "st") == 0) {
			sscanf(linea, "st %d", &max_tiempo);
			tiempo_total = max_tiempo *= 1000;		/*internamente trabajamos con milisegundos*/
			max_profundidad = MAX_PROFUNDIDAD;
			/*en este modo de tiempo no gestionamos el tiempo para cambios de iteración o nuevos movimeintos*/
			no_nueva_iteracion = tiempo_total;
			no_nuevo_movimiento = tiempo_total;
			limite_tiempo = tiempo_total;
			continue;
		}
		/*configura el nivel de juego indicando la profundidad a jugar sd 8 --> juega hasta profundidad principal 8*/
		if (strcmp(comando, "sd") == 0) {
			sscanf(linea, "sd %d", &max_profundidad);
			if (max_profundidad >  MAX_PROFUNDIDAD)
				max_profundidad =  MAX_PROFUNDIDAD;
			tiempo_total = 1 << 25;	/*es como poner tiempo infinito*/
			/*en este modo de tiempo no gestionamos el tiempo*/
			no_nueva_iteracion = tiempo_total;
			no_nuevo_movimiento = tiempo_total;
			limite_tiempo = tiempo_total;
			continue;
		}
		/*recibimos del GUI el tiempo que nos queda*/
		if (strcmp(comando, "time") == 0) {
			sscanf(linea, "time %d", &max_tiempo);
			/*pasamos a milisegundos que es como trabajamos internamente*/
			tiempo_total = max_tiempo *= 10;
			/*mantenemos un margen de seguridad de 100 ms*/
			tiempo_total -= 100;
			/*partidas con incremento de tiempo*/
			if (mps == 0 && inc > 0) {
				if (tiempo_total < 3 * inc) {
					tiempo_total = 75*inc/100;
					no_nueva_iteracion = tiempo_total/2;
					no_nuevo_movimiento = tiempo_total;
					limite_tiempo = tiempo_total;
				}
				else {
					tiempo_total = tiempo_total / 30 + inc;
					no_nueva_iteracion = tiempo_total/2;
					no_nuevo_movimiento = (3 * tiempo_total / 2);
					limite_tiempo = (4 * tiempo_total);
				}
				continue;
			}
			/*movimientos que quedarán para llegar al control si no es partida con incremento*/
			if (motor == BLANCO)
				quedan = (mps-njugadas/2)-1;
			else
				quedan = (mps-njugadas/2);
			if (mps == 0) {
#if UNDER_CE
				if (njugadas < 40) quedan = 40;
				else if (njugadas < 80) quedan = 35;
				else if (njugadas < 120) quedan = 30;
				else quedan = 25;
#else
				quedan = 40;
#endif
			}
			else {
				while (quedan <= 0)
					quedan += mps;
			}
			no_nueva_iteracion = (tiempo_total/2) / quedan;
			no_nuevo_movimiento = (3*tiempo_total/2) / (quedan + (1/2));
			limite_tiempo = (10*tiempo_total) / (quedan + 9);
			continue;
		}
		/*tiempo del contrario, no lo utilizamos de momento*/
		if (strcmp(comando, "otim") == 0) {
			continue;
		}
		/*control para winboard, recibimos un ping y enviamos un pong*/
		if (strcmp(comando,"ping") == 0){
			int n;
			sscanf(linea, "ping %d", &n);
			printf("pong %d\n",n);
			continue;
		}
		/*si recibimos un resultado de un juego el motor debe parar*/
		if (strcmp(comando, "result") == 0) {
			motor = VACIO;
			continue;
		}
		if (strcmp(comando, "?") == 0) {
			motor = VACIO;
			continue;
		}
		if (strcmp(comando, ".") == 0) {
			continue;
		}
		if (strcmp(comando, "exit") == 0) {
			continue;
		}
		/*configuramos el tablero con una posición en formato FEN*/
		if (strcmp(comando, "setboard") == 0) {
			strcpy(fenstring, "");
			sscanf(linea, "setboard %s %s %s %s", args[0],args[1],args[2],args[3]);
			strcat(fenstring, args[0]);
			strcat(fenstring, args[1]);
			strcat(fenstring, args[2]);
			strcat(fenstring, args[3]);
			SetBoard(fenstring);
			continue;
		}
		/*viejo comando edit de winboard para configurar una posición*/
		if (strcmp(comando, "edit") == 0) {
			continue;
		}
		/*Damos una pista del mejor movimiento al usuario si nos lo pide*/
		if (strcmp(comando, "hint") == 0) {
			char c;
			movimiento mh;
			tiempo_total = 5000;
			max_profundidad = MAX_PROFUNDIDAD;
			no_nueva_iteracion = tiempo_total;
			no_nuevo_movimiento = tiempo_total;
			limite_tiempo = tiempo_total;
			estado = BUSCANDO;
			mejormovimiento = MotorPiensa();
			mh = pv[0][0];
			if (!(mh.de == 0 && mh.a == 0)) {
				/*letra a imprimir si hay una promoción*/
				switch (mh.tipo) {
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
				printf("Hint: %c%d%c%d%c",
					'a' + COLUMNA(mh.de),
					8 - FILA(mh.de),
					'a' + COLUMNA(mh.a),
					8 - FILA(mh.a),
					c);
			}
			continue;
		}
		/*si el usuario selecciona Book en el menú*/
		if (strcmp(comando, "bk") == 0) {
			continue;
		}
		/*deshacemos un movimiento*/
		if (strcmp(comando, "undo") == 0) {
			if (njugadas == 0)
				continue;
			Deshacer();
			continue;
		}
		/*deshacemos los 2 últimos movimientos y continua el mismo color*/
		if (strcmp(comando, "remove") == 0) {
			if (njugadas < 2)
				continue;
			Deshacer();
			Deshacer();
			continue;
		}
		/*ponder, pensar con el tiempo del contrario*/
		if (strcmp(comando, "hard") == 0) {
			ponder = VERDADERO;
			continue;
		}
		/*no ponder*/
		if (strcmp(comando, "easy") == 0) {
			ponder = FALSO;
			continue;
		}
		/*imprimimos el pensamiento*/
		if (strcmp(comando, "post") == 0) {
			post = VERDADERO;
			continue;
		}
		/*no imprimir el pensamiento*/
		if (strcmp(comando, "nopost") == 0) {
			post = FALSO;
			continue;
		}
		/*modo de análisis con GUI*/
		if (strcmp(comando, "analyze") == 0) {
			tiempo_total = 1 << 25;
			max_profundidad = MAX_PROFUNDIDAD;
			no_nueva_iteracion = tiempo_total;
			no_nuevo_movimiento = tiempo_total;
			limite_tiempo = tiempo_total;
			estado = ANALIZANDO;
			mejormovimiento = MotorPiensa();
			motor = VACIO;
			continue;
		}
		/*nombre del oponente*/
		if (strcmp(comando, "name") == 0) {
			continue;
		}
		/*rating del oponente cuando estamos en modo ICS*/
		if (strcmp(comando, "rating") == 0) {
			continue;
		}
		/*nombre del servidor de Internet donde jugamos*/
		if (strcmp(comando, "ics") == 0) {
			sscanf(linea, "ics %s", comando2);
			servidor[0] = '\0';
			strcpy(servidor,comando2);
			continue;
		}
		/*el oponente es un motor*/
		if (strcmp(comando, "computer") == 0) {
			continue;
		}
#if UNDER_CE
		if (strcmp(comando, "log") == 0) {
			continue;
		}
		if (strcmp(comando, "output") == 0) {
			continue;
		}
		if (strcmp(comando, "unrated") == 0) {
			continue;
		}
		if (strcmp(comando, "rated") == 0) {
			continue;
		}
#endif
		Comprobar_Movimiento(comando);			/*comprueba si llega un movimiento del usuario y es legal*/
		contar = Generar(lista);		  	    /*genera los movimientos del contrario para saber si ha recibido mate*/
		imprime_resultado(contar, lista);	    /*si es final de partida imprime el resultado*/
	}
}

void Carga_libro(void) 
{
	int error;
	
	if (libro_cargado)
		CLOSE_BOOK();
	
	if (strcmp (ownbook,"on") == 0) 
		usa_libro = VERDADERO;	/*utilizamos libro de aperturas?*/
	else
		usa_libro = FALSO;	
	
	if (usa_libro) {
		error=INITIALIZE(librotorneo,libroprincipal);		/*Inicia libro de Rebel*/
/*Pocket Pc*/
#if UNDER_CE
		if (error == 0) {
			libro_cargado = VERDADERO;
			puts ("Rebel book found!");
			//MessageBox (NULL, STRING ("Rebel book found!\r\n"), _T("Engine DanaSah 5.00"), MB_OK);
		}
		else if (error == 1) {
			libro_cargado = VERDADERO;
			puts ("Rebel book found but tourbook is missed, using only mainbook");
			MessageBox (NULL, STRING ("Rebel book found but tourbook is missed, using only mainbook\r\n"), _T("Engine DanaSah 5.00"), MB_OK);
		}
		else if (error == 2) {
			puts ("Missing obliged file(s)random1.bin and/or random2.bin");
			MessageBox (NULL, STRING ("Missing obliged file(s) random1.bin and/or random2.bin\r\n"), _T("Engine DanaSah 5.00"), MB_OK);
		}
		else if (error == 3) {
			puts ("Missing obliged file mainbook");
			MessageBox (NULL, STRING ("Missing obliged file mainbook\r\n"), _T("Engine DanaSah 5.00"), MB_OK);
		}
		else {
			puts ("Error loading Rebel book!");
			MessageBox (NULL, STRING ("Error loading Rebel book!\r\n"), _T("Engine DanaSah 5.00"), MB_OK);
		}
	}
/*Windows o linux*/
#else
		if (error == 0) {
			libro_cargado = VERDADERO;
			printf("# Rebel book loaded!\n");
		}
		else if (error == 1) {
			libro_cargado = VERDADERO;
			printf("# Rebel book loaded but tourbook is missed, using only mainbook\n");
		}
		else if (error == 2) {
			printf("# Missing obliged file(s)random1.bin and/or random2.bin. Rebel book not loaded!\n");
		}
		else if (error == 3) {
			printf("# Missing obliged file mainbook. Rebel book not loaded!\n");
		}
		else {
			printf("# Error loading Rebel book!\n");
		}
	}
#endif
}

void Carga_bitbases(void)
{
	if (egbb_is_loaded)
    	unload_bitbases();
	egbb_cache_size = (egbb_cache_size * 1024 *1024);
	egbb_is_loaded = LoadEgbbLibrary(egbb_path, egbb_cache_size);	
}
