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
Aquí tenemos una función que lee el archivo de configuración danasah.ini donde tenemos configurados
los libros de aperturas, el tamaño de las tablas hash y evaluación caché y la direccióny tamaño del
caché para las bitbases */

/*Includes*/
/*#include <windows.h>*/
#include <stdio.h>		/*prinft, scanf, sprinft, sscanf, fgets...*/
#include <string.h>
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

void leer_inifile(void)
{
#if UNDER_CE
	TTSize = (8 * 1024 * 1024) / sizeof(TType);
	ETSize = (1 * 1024 * 1024) / sizeof(EType);
	strcpy(librotorneo, "tourbook.bin");
	strcpy(libroprincipal, "mainbook.bin");
	return;
#else
	FILE *inifile;
	char linea[256], comandos[256], temp[256];
	char temp1[256] = "", temp2[256] = "", temp3[256] = "", temp4[256] = "";
	int tamano1 = 0, tamano2 = 0, tamano3 = 0, tamano4 = 0, tamano5 = 0;

	if ((inifile = fopen ("danasah.txt", "r")) == NULL) {
		printf("# Cfg file danasah.txt no found.\n");
		tamano1 = 128;
		tamano2 = 16;
		TTSize = (tamano1 * 1024 * 1024) / sizeof(TType);
		ETSize = (tamano2 * 1024 * 1024) / sizeof(EType);
		strcpy(librotorneo, "tourbook.bin");
		strcpy(libroprincipal, "mainbook.bin");
		imprime_configuracion(tamano1, tamano2);
	}
	else {
		printf("# Cfg file danasah.txt found.\n");

		fseek(inifile, 0, SEEK_SET);
		while (fgets(linea, 256, inifile)) {
			if (linea[0] != '#') {
				sscanf(linea, "%s", comandos);
				/*Libro de aperturas*/
				if (strcmp(comandos, "ownbook") == 0) {
					sscanf(linea, "ownbook %s", temp);
					strcpy(ownbook, temp);
				}
				if (strcmp(comandos, "mainbook") == 0) {
					sscanf(linea, "mainbook %s", temp1);
					strcpy(libroprincipal, temp1);
				}
				if (strcmp(comandos, "tourbook") == 0) {
					sscanf(linea, "tourbook %s", temp2);
					strcpy(librotorneo, temp2);
				}
				/*Transposition table*/
				if (strcmp(comandos, "hash") == 0) {
					sscanf(linea, "hash %d", &tamano1);
					TTSize = (tamano1 * 1024 * 1024) / sizeof(TType);
					tamano2 = tamano1 / 8;
					ETSize = (tamano2 * 1024 * 1024) / sizeof(EType);
				}
				/*Bitbases*/
				if (strcmp(comandos, "path") == 0) {
					sscanf(linea, "path %s", temp3);
					strcpy(egbb_path, temp3);
				}
				if (strcmp(comandos, "men") == 0) {
					sscanf(linea, "men %d", &tamano3);
					men = tamano3;
				}
				if (strcmp(comandos, "cache_size") == 0) {
					sscanf(linea, "cache_size %d", &tamano4);
					egbb_cache_size = tamano4;
				}
				if (strcmp(comandos, "load_type") == 0) {
					sscanf(linea, "load_type %d", &tamano5);
					egbb_load_type = tamano5;
				}
				/*Chess960 (FRC)*/
				if (strcmp(comandos, "castling") == 0) {
					sscanf(linea, "castling %s", temp4);
					strcpy(castling, temp4);
				}
			}
		}
		imprime_configuracion(tamano1, tamano2);
	}
	fclose(inifile);
	return;
#endif
}

void imprime_configuracion(int tamano1, int tamano2) {
	printf("# Ownbook: %s\n", ownbook);
	printf("# Mainbook: %s\n", libroprincipal);
	printf("# Tourbook: %s\n", librotorneo);
	printf("# Table hash size: %d Mb.\n", tamano1);
	printf("# Eval cache size: %d Mb.\n", tamano2);
	printf("# Bitbases path: %s\n", egbb_path);
	printf("# Bitbases men: %d\n", men);
	printf("# Bitbases cache size: %d\n", egbb_cache_size);
	printf("# Bitbases load type: %d\n", egbb_load_type);
	printf("# Chess960 (FRC) - Castling: %s\n", castling);	
}
