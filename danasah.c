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
Función principal del programa, entra en modo consola y espera un comando para inciar
el modo xboard o para hacer algún test, perft o bench.
*/

/*Includes*/
#if _WIN32 || _WIN64
	#include <windows.h>
	#define U64_FORMAT "I64u"
#else
	#define U64_FORMAT "llu"
#endif
#include <stdio.h>		/*prinft, scanf, sprinft, sscanf, fgets...*/
#include <string.h>     /*strcmp, strcpy, strcat*/
#include <stdlib.h>     /*atoi*/
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

int __cdecl main(int argc, char *argv[])
{
    char	linea[256], s[256];
	int		depth, i, tiempo_total;
	U64 nodes;

	/*presentamos alguna información en la consola*/
	printf("# %s is a chess engine with xboard protocol.\n", nombre);
	printf("# Copyright (C) <%s>  <Pedro Castro Elgarresta> \n", fecha);
	printf("# Basque Contry, Spain \n");
	printf("# Web Page: http://sites.google.com/site/danasah/ \n");
	printf("# \n");
	printf("# \"help\" displays a list of commands.\n");
	printf("# \n");

	leer_inifile();			/*leemos el fichero de configuración de danasah (danasah.ini)*/

	/*existe la posibilidad de introducir algunos parámetros en la línea de comando*/
	/*de esta forma no es necesario un archivo danasah.ini*/
    for(i=1; i<argc; i++) {
        if(!strcmp(argv[i], "hash")) {
            i++;
            if(i < argc) {
				TTSize = (atoi(argv[i]) * 1024 * 1024) / sizeof(TType);
				ETSize = (atoi(argv[i]) / 8 * 1024 * 1024) / sizeof(EType);
            }
        }
        if(!strcmp(argv[i], "ownbook")) {
            i++;
            if(i < argc) {
				strcpy(ownbook, argv[i]);
            }
        }
	}

	alloc_hash();	/*reservamos memoria para las tablas hash*/
	inicia_hash();	/*inicia las tablas hash*/


	/*entramos en un cliclo esperando un comando del usuario*/
    for (;;) {
#if UNDER_CE
        if (gets(linea) == NULL)						/*cierra el programa*/
            return 0;
#else
        printf("danasah> ");
        if (fgets(linea, 256, stdin) == NULL)			/*cierra el programa*/
            return 0;
#endif

        sscanf (linea,"%s",s);

		if (!strcmp(s, "quit")) {                           /*sale del programa*/
			break;
		}
		if (!strcmp(s, "perft")) {                          /*perft: comprueba el correcto funcionamiento del generador de movimientos*/
			sscanf(linea,"%*s %d", &depth);
			variante[0] = '\0';
			strcpy(variante,"normal");
			printf("# Variante = %s\n", variante);
			strcpy(fenstring, "");
			strcat(fenstring, STARTPOS);
			SetBoard(fenstring);
			printf("# Performance Test\n");
			printf("# Ply\tSeconds\tNodes\n");
			for (i = 0; i <= depth; i++) {
				tiempo_inicio = get_ms();
				nodes = perft(i);
				tiempo_total = get_ms()-tiempo_inicio;
				printf("# %d:\t%d\t%" U64_FORMAT "\n",i,tiempo_total/1000, nodes);
			}
			printf("# \n");
			continue;
		}
		if (!strcmp(s, "bench")) {
			bench();		                                /*bench: comprueba la velocidad del equipo*/
			printf("# \n");
			continue;
		}
		if (!strcmp(s, "test")) {
			Test();			                                /*test: reaaliza un test para generar un "profile guided optimization"*/
			printf("# \n");
			continue;
		}
		if (!strcmp(s, "xboard")) {
			xboard();		                                /*pasmos el control a la función xboard*/
			break;
		}
		if (!strcmp(s, "help")) {							/*ayuda en pantalla*/
			printf("# \n");
			printf("# perft number - Total nodes account for a given depth from the position Start\n");
			printf("# test - Test 25 positions 20 s, used to build a profile guided optimization\n");
			printf("# bench - Run the built-in benchmark\n");
			printf("# quit - Exit the program\n");
			printf("# xboard - Switch to XBoard mode\n");
			printf("# \n");
			continue;
		}
	}

	/*Liberamos la memoria de las bitbases*/
    if (egbb_is_loaded)
        unload_bitbases();

	/*Liberamos la memoria del libro de aperturas*/
	if (libro_cargado)
		CLOSE_BOOK();

    /*Liberamos la memoria de las tablas hash*/
	free_hash();

	/*Salimos*/
	return 0;
}

#if UNDER_CE
    TEngineIO *EngineIO;
    int GetCh(void)
    {
        char s[2];
        EngineIO->p_get_string (EngineIO->pIOInst, s, 1);
        return s[0];
    }
    int __cdecl PrintF(const char *fmt, ...)
    {
        int n;
        va_list args;
        va_start (args, fmt);
        n = EngineIO->p_vprint (EngineIO->pIOInst, fmt, args);
        va_end (args);
        return n;
    }
    BOOL WINAPI DllMain (HANDLE hinstDLL, DWORD dwReason, LPVOID lpvReserved)
    {
        return TRUE;
    }
    #ifdef __cplusplus
    extern "C"
    #endif
    __declspec(dllexport) void __stdcall Run (TEngineIO *pEngineIO)
    {
        EngineIO = pEngineIO;

        //puts ("DanaSah Started");
        //MessageBox (NULL, STRING ("DanaSah Started\r\n"), _T("Engine"), MB_OK);

        main (EngineIO->argc, EngineIO->argv);
    }
#endif
