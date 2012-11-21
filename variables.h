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
Copia de las variables globales para poder utilizarlas en otros archivos.
*/

extern char nombre[];
extern char fecha[];

extern int turno;
extern int motor;
extern int ponder;
extern int hacer_ponder;
extern int estado;

extern int post;

extern char variante[256];
extern char servidor[256];
extern char castling[256];

extern int max_profundidad;
extern int depthnull;
extern int sigue_Pv;

extern int max_tiempo;
extern int mps;
extern int base;
extern int inc;
extern int quedan;
extern int tiempo_inicio;
extern int limite_tiempo;
extern int no_nuevo_movimiento;
extern int no_nueva_iteracion;
extern int tiempo_total;

extern int aborta;
extern int para_busqueda;
extern int para_ponder;
extern int para_analisis;

extern int nodos;
extern int cknodos;
extern int ply;
extern int njugadas;

/*extern int puntos;*/
extern int puntosblanco;
extern int puntosnegro;
extern int puntosblanco_m, puntosblanco_f, puntosnegro_m, puntosnegro_f;
extern int material;
extern int hacer_lazy_eval;

extern int	piezasblanco;
extern int	piezasnegro;
extern int	peonesblanco;
extern int	peonesnegro;

extern int old_piezasblanco;
extern int old_piezasnegro;
extern int old_peonesblanco;
extern int old_peonesnegro;

extern int peonesblancos, peonesnegros;
extern int caballosblancos, caballosnegros;
extern int alfilesblancos, alfilesnegros;
extern int torresblancas, torresnegras;
extern int damasblancas, damasnegras;

extern int wbcolor, bbcolor, wpcolor, bpcolor, npcolor;
extern int alfilblanco, alfilnegro;

extern int rb;
extern int rn;

extern int tb1, tb2, tn1, tn2;

extern int distancia[64][64];

extern int fila_peon[2][10];
extern int aislado;
extern int retrasado;
extern int abierto;
extern int tmp;

extern int valor_pieza[7];
extern int valor_pieza2[7];

extern int pieza[64], color[64];

extern int enroque;

extern int alpaso;
extern int regla50;

extern U64 hash_piezas[2][6][64];
extern U64 hash_turno[2];
extern U64 hash_alpaso[64];
extern U64 hash_enroque[16];

extern U64 hash;

extern int TTSize;

extern int ETSize;

extern char ownbook[256];
extern int usa_libro;
extern int libro_cargado;
extern char libroprincipal[256], librotorneo[256];
extern int fuera_del_libro;

extern int egbb_is_loaded;
extern char egbb_path[256];
extern int men;
extern int egbb_cache_size;
extern int egbb_load_type;

extern char fenstring[256];

extern int lista_piezas[33];
extern int cuenta_piezas;

extern int enroque_mascara[64];

extern movimiento pv[MAX_PROFUNDIDAD][MAX_PROFUNDIDAD];
extern int pv_longuitud[MAX_PROFUNDIDAD];
extern jugada jugadas[STACK];

extern movimiento killer1[MAX_PROFUNDIDAD];
extern movimiento killer2[MAX_PROFUNDIDAD];
extern movimiento matekiller[MAX_PROFUNDIDAD];

extern movimiento no_move;

extern movimiento mp;
extern movimiento mc;
extern int li;

extern int Historia[64][64];
extern int maxhistoria;

extern int valor1[MAX_PROFUNDIDAD];
extern int valor2[MAX_PROFUNDIDAD];

extern tipo_see atacantes[2][16];
extern int num_atacantes[2];

extern int mailbox[120];
extern int mailbox64[64];
extern int offset[6][8];
