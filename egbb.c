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
Funciones para acceder a las bitbases. Gracias a Daniel Shawul.
Para más información sobre esta funciones deberíamos recurrir a Scorpio.
Solo lo he hecho funcionar para windows, en linux tendriamos que llamar
en lugar de a egbbdll.dll a otro archivo.
*/

/*Includes*/
#include <stdio.h>
#include "definiciones.h"
#include "variables.h"

enum {_WHITE,_BLACK};
enum {_EMPTY,_WKING,_WQUEEN,_WROOK,_WBISHOP,_WKNIGHT,_WPAWN,
_BKING,_BQUEEN,_BROOK,_BBISHOP,_BKNIGHT,_BPAWN};

#define _NOTFOUND 99999

/*
Load the dll and get the address of the load and probe functions.
*/

#if defined (_WIN32) || defined(_WIN64)
    #ifndef WINDOWS
        #define WINDOWS
    #endif
    #include <windows.h>
    #undef CDECL
    #define CDECL __cdecl
  	#define EGBB_NAME STRING("egbbdll.dll")
#else
    #include <dlfcn.h>
    #include <string.h>
    #define CDECL
	#define EGBB_NAME "egbbso.so"
	#define HMODULE void*
	#define LoadLibrary(x) dlopen(x,RTLD_LAZY)
	#define FreeLibrary(x) dlclose(x)
	#define GetProcAddress dlsym
#endif

typedef int (CDECL *PPROBE_EGBB) (int player, int w_king, int b_king,
							int piece1, int square1,
							int piece2, int square2,
							int piece3, int square3);
typedef void (CDECL *PLOAD_EGBB) (char* path,int cache_size,int load_options);

static PPROBE_EGBB probe_egbb;
static HMODULE hmod;

int LoadEgbbLibrary(char* main_path, int egbb_cache_size) {
	PLOAD_EGBB load_egbb;
    #if _UNICODE
	    WCHAR path[256];
	    mbstowcs(path,main_path,strlen(main_path)+1);
	    wcscat(path,EGBB_NAME);
    #else
	    char path[256];
	    strcpy(path,main_path);
	    strcat(path,EGBB_NAME);
    #endif

	if ((hmod = LoadLibrary(path)) != 0) {
	    /*ISO C prohibe la conversión de objeto apuntador a un tipo de apuntador a función*/
	    load_egbb = (PLOAD_EGBB) GetProcAddress(hmod,STRING("load_egbb_5men"));
     	probe_egbb = (PPROBE_EGBB) GetProcAddress(hmod,STRING("probe_egbb_5men"));
#if UNDER_CE
		egbb_load_type = LOAD_NONE;
        load_egbb(main_path, egbb_cache_size, egbb_load_type);
		puts ("Bitbases loaded!");
		MessageBox (NULL, STRING ("Bitbases loaded!\r\n"), _T("Engine DanaSah 5.00"), MB_OK);

#else
        load_egbb(main_path, egbb_cache_size, egbb_load_type);
#endif
		return VERDADERO;
	} else {
		printf("# EgbbProbe not Loaded!\n");
		return FALSO;
	}
}

/*Unload the endgame bitbase object/library*/
void unload_bitbases(void)
{
        if (hmod == NULL)
            return;

#ifdef WINDOWS
         if (!FreeLibrary(hmod))
#else   /*not WINDOWS*/
         if (dlclose(hmod))
#endif
            printf("# Can't unload bitbases");
         hmod = NULL;
         probe_egbb = NULL;
         egbb_is_loaded = FALSO;
}

/*
Probe:
TSCP uses A8 = 0, H1 = 63 but we use A1 = 0, H8 = 63
board representation and then probe bitbase.
*/

int probe_bitbases(int *score) {

	int i, piece[3],square[3],count;
	int player, w_ksq, b_ksq;

	player = 1-turno;
	count = 0;
	piece[0] = 0;
	piece[1] = 0;
	piece[2] = 0;
	square[0] = 0;
	square[1] = 0;
	square[2] = 0;


#define FLIP(sq) ((sq) ^ 0x38)

#define ADD_PIECE(type) {\
	    piece[count] = type;\
	    square[count] = FLIP(i);\
        count++;\
};

    w_ksq = FLIP(rb);
    b_ksq = FLIP(rn);

	for (i = 0; i < 64; ++i) {
		if (color[i] == VACIO)
			continue;

		if(color[i] == BLANCO) {
			switch (pieza[i]) {
				case PEON: ADD_PIECE(_WPAWN); break;
				case CABALLO: ADD_PIECE(_WKNIGHT); break;
				case ALFIL: ADD_PIECE(_WBISHOP); break;
				case TORRE: ADD_PIECE(_WROOK); break;
				case DAMA: ADD_PIECE(_WQUEEN); break;
				case REY: break;
			}
		} else {
			switch(pieza[i]) {
				case PEON: ADD_PIECE(_BPAWN); break;
				case CABALLO: ADD_PIECE(_BKNIGHT); break;
				case ALFIL: ADD_PIECE(_BBISHOP); break;
				case TORRE: ADD_PIECE(_BROOK); break;
				case DAMA: ADD_PIECE(_BQUEEN); break;
				case REY: break;
			}
		}
	}

	*score = probe_egbb(player, w_ksq, b_ksq,
		piece[0],square[0],piece[1],square[1],piece[2],square[2]);

	if(*score != _NOTFOUND) {
		return 1;
	}

    return 0;
}
