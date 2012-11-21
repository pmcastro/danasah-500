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
Funciones para generar y realizar un movimiento, están basadas en el generador de
movimientos del programa FirstChess, se ha añadido un generador de capturas para
evitar el efecto horizonte y la generación de enroques y al paso.
*/

/*Includes*/
#include <string.h>
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

/*Generamos movimientos para la PVS*/
void Generar_Movimiento_PVS(int de, int a, int tipo, movimiento *plista, int *pcuenta)
{
	movimiento	m;

	/*de un movimiento guardamos la casilla inicial, final, tipo de movimiento y valor
	de lo bueno que podría ser el movimiento para luego poder ordenarlo*/
	m.de = de;
	m.a = a;
	m.tipo = tipo;
	m.valor = 0;

	/*hacemos que puntuen más el matekiller, las capturas de ganancia, promociones a dama con
	y sin captura, capturas buenas y malas, movimientos killer y demás movimientos por historia*/
	if (m.tipo >= PROMOCION_A_DAMA) {                                                   /*si hay promocion*/
		if (m.tipo == PROMOCION_A_DAMA) m.valor = 4000000;	                            /*promocionamos a dama*/
		else if (m.tipo == PROMOCION_A_CABALLO) m.valor = 2300000;
		else if (m.tipo == PROMOCION_A_TORRE) m.valor = 2200000;
		else if (m.tipo == PROMOCION_A_ALFIL) m.valor = 2100000;
	}
	else {                                                                              /*no hay promoción*/
		if ((color[a] != VACIO)) {                                                      /*hay captura*/
			if (malaCaptura(m))
				m.valor = 1900000 + (pieza[a]*100) - (pieza[de]);									/*malas capturas por detrás*/
			else
				m.valor = 3000000 + (pieza[a]*100) - (pieza[de]);						/*ordenamos por MVV/LVA*/
		}
		else {
			if ((m.de == matekiller[ply].de) && (m.a == matekiller[ply].a))             /*los movimientos killer*/
				m.valor = 5000000;
			else if ((m.de == killer1[ply].de) && (m.a == killer1[ply].a))
				m.valor += 2800000;
			else if (ply >= 2 && (m.de == killer1[ply-2].de) && (m.a == killer1[ply-2].a))
				m.valor += 2700000;
			else if ((m.de == killer2[ply].de) && (m.a == killer2[ply].a))
				m.valor += 2600000;
			else if (ply >= 2 && (m.de == killer2[ply-2].de) && (m.a == killer2[ply-2].a))
				m.valor += 2500000;
			else
				m.valor += Historia[de][a];                                             /*history heuristic*/
		}
	}

	plista[*pcuenta] = m;
	*pcuenta = *pcuenta + 1;
}

/*genera los movimientos vamos a llamar normales, es decir movimientos que no son de peón, enroques y al paso*/
void Generar_Movimiento_Normal_PVS(int de, int a, movimiento *plista, int *pcuenta)
{
    Generar_Movimiento_PVS(de, a, NORMAL, plista, pcuenta);
}

/*genera los movimientos de peón incluido las promociones*/
void Generar_Movimiento_PEON_PVS(int de, int a, movimiento *plista, int *pcuenta)
{
    if (a > 7 && a < 56) {
		Generar_Movimiento_PVS(de, a, NORMAL, plista, pcuenta);
	} else {
		Generar_Movimiento_PVS(de, a, PROMOCION_A_DAMA, plista, pcuenta);
        Generar_Movimiento_PVS(de, a, PROMOCION_A_TORRE, plista, pcuenta);
        Generar_Movimiento_PVS(de, a, PROMOCION_A_ALFIL, plista, pcuenta);
        Generar_Movimiento_PVS(de, a, PROMOCION_A_CABALLO, plista, pcuenta);
	}
}

/*genera los movimientos que avanzan 2 casillas, es un caso especial por posible comidas al paso*/
void Generar_Movimiento_PEON2_PVS(int de, int a, movimiento *plista, int *pcuenta)
{
	Generar_Movimiento_PVS(de, a, PEON_AVANZA_2, plista, pcuenta);
}

/*general los movmientos posibles de enroque si es posible*/
void Generar_Movimiento_Enroque_PVS(int de, int a, movimiento *plista, int *pcuenta)
{
	Generar_Movimiento_PVS(de, a, ENROQUE, plista, pcuenta);
}

/*genera los movmientos posibles de comer al paso si es posible*/
void Generar_Movimiento_Alpaso_PVS(int de, int a, movimiento *plista, int *pcuenta)
{
	Generar_Movimiento_PVS(de, a, AL_PASO, plista, pcuenta);
}

/*ahora generamos movimientos pero solo para las capturas y promoción a dama*/
void Generar_Movimiento_Capturas(int de, int a, int tipo, movimiento *plista, int *pcuenta)
{
	movimiento	m;

	m.de = de;	    /*de un movimiento guardamos la casilla inicial, final, tipo de movimiento y valor*/
	m.a = a;
	m.tipo = tipo;
	m.valor = 0;
	if (m.tipo == PROMOCION_A_DAMA) m.valor = 4000000;
	else {														/*no hay promoción es captura */
		if (malaCaptura(m))
			m.valor = -1;										/*malas capturas por detrás*/
		else
			m.valor = 3000000 + (pieza[a]*100) - (pieza[de]);	/*ordenamos por MVV/LVA*/
	}
	plista[*pcuenta] = m;
	*pcuenta = *pcuenta + 1;
}

/*Ahora hacemos lo mismo para el generador de capturas y promociones*/
void    Generar_Movimiento_Normal_Capturas(int de, int a, movimiento *plista, int *pcuenta)
{
    Generar_Movimiento_Capturas(de, a, NORMAL, plista, pcuenta);
}

void    Generar_Movimiento_PEON_Capturas(int de, int a, movimiento *plista, int *pcuenta)
{
    if (a > 7 && a < 56) {
		Generar_Movimiento_Capturas(de, a, NORMAL, plista, pcuenta);
	} else {
		/*en el generador de capturas solo comprobamos promociones a dama (más rápido*/
		Generar_Movimiento_Capturas(de, a, PROMOCION_A_DAMA, plista, pcuenta);
	}
}

void    Generar_Movimiento_Alpaso_Capturas(int de, int a, movimiento *plista, int *pcuenta)
{
	Generar_Movimiento_Capturas(de, a, AL_PASO, plista, pcuenta);
}

/*Generar todos los pseudo-movimientos y retornar el número de ellos. No comprueba si nos quedamos en jaque*/
int	Generar(movimiento *plista)
{
    int i, j, k, y,
        fila, columna,
		cuenta, total_piezas;

	/*vamos a calcular el número de piezas totales para cada color*/
	if (turno == BLANCO)
		total_piezas = peonesblancos + caballosblancos + alfilesblancos + torresblancas + damasblancas + 1;
	else
		total_piezas = peonesnegros + caballosnegros + alfilesnegros + torresnegras + damasnegras + 1;

	cuenta = 0;
    for (i = 0, j = 1; j <= total_piezas; i++) {
        if (color[i] == turno) {
			j++;
            switch (pieza[i]) {
            case PEON:
                columna = COLUMNA(i);
                fila = FILA(i);
                if (turno == NEGRO) {
                    if (columna && color[i + 7] == BLANCO)
                        Generar_Movimiento_PEON_PVS(i, i + 7, plista, &cuenta);     /*come en diagonal - izquierda*/
                    if (columna < 7 && color[i + 9] == BLANCO)
                        Generar_Movimiento_PEON_PVS(i, i + 9, plista, &cuenta);	    /*come en diagonal - derecha*/
					if (color[i + 8] == VACIO) {                                    /*avanza una casilla*/
                        Generar_Movimiento_PEON_PVS(i, i + 8, plista, &cuenta);
						if (fila == 1 && color[i + 16] == VACIO)                    /*avanza dos casillas*/
							Generar_Movimiento_PEON2_PVS(i, i + 16, plista, &cuenta);
					}
                } else {
                    if (columna && color[i - 9] == NEGRO)
                        Generar_Movimiento_PEON_PVS(i, i - 9, plista, &cuenta);
                    if (columna < 7 && color[i - 7] == NEGRO)
                        Generar_Movimiento_PEON_PVS(i, i - 7, plista, &cuenta);
					if (color[i - 8] == VACIO) {
                        Generar_Movimiento_PEON_PVS(i, i - 8, plista, &cuenta);
						if (fila == 6 && color[i - 16] == VACIO)
							Generar_Movimiento_PEON2_PVS(i, i - 16, plista, &cuenta);
					}
                }
                break;

            case CABALLO:
                columna = COLUMNA(i);
                y = i - 6;
                if (y >= 0 && columna < 6 && color[y] != turno)
                    Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                y = i - 10;
                if (y >= 0 && columna > 1 && color[y] != turno)
                    Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                y = i - 15;
                if (y >= 0 && columna < 7 && color[y] != turno)
                    Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                y = i - 17;
                if (y >= 0 && columna > 0 && color[y] != turno)
                    Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                y = i + 6;
                if (y < 64 && columna > 1 && color[y] != turno)
                    Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                y = i + 10;
                if (y < 64 && columna < 6 && color[y] != turno)
                    Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                y = i + 15;
                if (y < 64 && columna > 0 && color[y] != turno)
                    Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                y = i + 17;
                if (y < 64 && columna < 7 && color[y] != turno)
                    Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                break;

            case DAMA:         /*La torre se mueve como ALFIL+TORRE*/
            case ALFIL:
                for (y = i - 9; y >= 0 && COLUMNA(y) != 7; y -= 9) {        /*abajo izquierda*/
                    if (color[y] != turno)
                        Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                    if (color[y] != VACIO)
                        break;
                }
                for (y = i - 7; y >= 0 && COLUMNA(y) != 0; y -= 7) {        /*abajo derecha*/
                    if (color[y] != turno)
                        Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                    if (color[y] != VACIO)
                        break;
                }
                for (y = i + 9; y < 64 && COLUMNA(y) != 0; y += 9) {        /*arriba derecha*/
                    if (color[y] != turno)
                        Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                    if (color[y] != VACIO)
                        break;
                }
                for (y = i + 7; y < 64 && COLUMNA(y) != 7; y += 7) {        /*arriba izquierda*/
                    if (color[y] != turno)
                        Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                    if (color[y] != VACIO)
                        break;
                }
                if (pieza[i] == ALFIL)
                    break;

            case TORRE:
                columna = COLUMNA(i);
                for (k = i - columna, y = i - 1; y >= k; y--) {					/*izquierda*/
                    if (color[y] != turno)
                        Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                    if (color[y] != VACIO)
                        break;
                }
                for (k = i - columna + 7, y = i + 1; y <= k; y++) {				/*derecha*/
                    if (color[y] != turno)
                        Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                    if (color[y] != VACIO)
                        break;
                }
                for (y = i - 8; y >= 0; y -= 8) {								/*arriba*/
                    if (color[y] != turno)
                        Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                    if (color[y] != VACIO)
                        break;
                }
                for (y = i + 8; y < 64; y += 8) {								/*abajo*/
                    if (color[y] != turno)
                        Generar_Movimiento_Normal_PVS(i, y, plista, &cuenta);
                    if (color[y] != VACIO)
                        break;
                }
                break;

            case REY:
                columna = COLUMNA(i);
                if (columna && color[i - 1] != turno)
                    Generar_Movimiento_Normal_PVS(i, i - 1, plista, &cuenta);   /*izquierda*/
                if (columna < 7 && color[i + 1] != turno)
                    Generar_Movimiento_Normal_PVS(i, i + 1, plista, &cuenta);   /*derecha*/
                if (i > 7 && color[i - 8] != turno)
                    Generar_Movimiento_Normal_PVS(i, i - 8, plista, &cuenta);   /*arriba*/
                if (i < 56 && color[i + 8] != turno)
                    Generar_Movimiento_Normal_PVS(i, i + 8, plista, &cuenta);   /*abajo*/
                if (columna && i > 7 && color[i - 9] != turno)
                    Generar_Movimiento_Normal_PVS(i, i - 9, plista, &cuenta);   /*izquierda arriba*/
                if (columna < 7 && i > 7 && color[i - 7] != turno)
                    Generar_Movimiento_Normal_PVS(i, i - 7, plista, &cuenta);   /*derecha arriba*/
                if (columna && i < 56 && color[i + 7] != turno)
                    Generar_Movimiento_Normal_PVS(i, i + 7, plista, &cuenta);   /*izquierda abajo*/
                if (columna < 7 && i < 56 && color[i + 9] != turno)
                    Generar_Movimiento_Normal_PVS(i, i + 9, plista, &cuenta);   /*derecha abajo*/
                break;
            }
        }
	}

	/*generamos los enroques*/
	if (turno == BLANCO) {
		if (enroque & 1)
			Generar_Movimiento_Enroque_PVS(rb, 62, plista, &cuenta);
		if (enroque & 2)
			Generar_Movimiento_Enroque_PVS(rb, 58, plista, &cuenta);
	}
	if (turno == NEGRO) {
		if (enroque & 4)
			Generar_Movimiento_Enroque_PVS(rn, 6, plista, &cuenta);
		if (enroque & 8)
			Generar_Movimiento_Enroque_PVS(rn, 2, plista, &cuenta);
	}

	/*generamos al paso*/
	if (alpaso != -1) {
		if (turno == BLANCO) {
			if ((COLUMNA(alpaso) != 0) && (color[alpaso + 7] == BLANCO) && (pieza[alpaso + 7] == PEON))
				Generar_Movimiento_Alpaso_PVS(alpaso + 7, alpaso, plista, &cuenta);
			if ((COLUMNA(alpaso) != 7) && (color[alpaso + 9] == BLANCO) && (pieza[alpaso + 9] == PEON))
				Generar_Movimiento_Alpaso_PVS(alpaso + 9, alpaso, plista, &cuenta);
		}
		else {
			if ((COLUMNA(alpaso) != 0) && (color[alpaso - 9] == NEGRO) && (pieza[alpaso - 9] == PEON))
				Generar_Movimiento_Alpaso_PVS(alpaso - 9, alpaso, plista, &cuenta);
			if ((COLUMNA(alpaso) != 7) && (color[alpaso - 7] == NEGRO) && (pieza[alpaso - 7] == PEON))
				Generar_Movimiento_Alpaso_PVS(alpaso - 7, alpaso, plista, &cuenta);
		}
	}
    return cuenta;
}

/*Generar solo movimientos de captura y promociones. Es básicamente una copia de Generar*/
int	Generar_Capturas(movimiento *plista)
{
    int i, j, k, y,
        columna,
		cuenta, cturno, total_piezas;

	if (turno == BLANCO)
		total_piezas = peonesblancos + caballosblancos + alfilesblancos + torresblancas + damasblancas + 1;
	else
		total_piezas = peonesnegros + caballosnegros + alfilesnegros + torresnegras + damasnegras + 1;

	cturno = (BLANCO+NEGRO)-turno;
	cuenta = 0;
    for (i = 0, j = 1; j <= total_piezas; i++) {
        if (color[i] == turno) {
			j++;
            switch (pieza[i]) {
            case PEON:
                columna = COLUMNA(i);
                if (turno == NEGRO) {
                    if ((color[i + 8] == VACIO) && (i>=48))     /*promoción*/
                        Generar_Movimiento_PEON_Capturas(i, i + 8, plista, &cuenta);
                    if (columna && color[i + 7] == BLANCO)
                        Generar_Movimiento_PEON_Capturas(i, i + 7, plista, &cuenta);
                    if (columna < 7 && color[i + 9] == BLANCO)
                        Generar_Movimiento_PEON_Capturas(i, i + 9, plista, &cuenta);
                } else
				if (turno == BLANCO) {
                    if ((color[i - 8] == VACIO) && (i<=15))
                        Generar_Movimiento_PEON_Capturas(i, i - 8, plista, &cuenta);
                    if (columna && color[i - 9] == NEGRO)
                        Generar_Movimiento_PEON_Capturas(i, i - 9, plista, &cuenta);
                    if (columna < 7 && color[i - 7] == NEGRO)
                        Generar_Movimiento_PEON_Capturas(i, i - 7, plista, &cuenta);
                }
                break;

            case CABALLO:
                columna = COLUMNA(i);
                y = i - 6;
                if (y >= 0 && columna < 6 && (color[y] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                y = i - 10;
                if (y >= 0 && columna > 1 && (color[y] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                y = i - 15;
                if (y >= 0 && columna < 7 && (color[y] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                y = i - 17;
                if (y >= 0 && columna > 0 && (color[y] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                y = i + 6;
                if (y < 64 && columna > 1 && (color[y] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                y = i + 10;
                if (y < 64 && columna < 6 && (color[y] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                y = i + 15;
                if (y < 64 && columna > 0 && (color[y] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                y = i + 17;
                if (y < 64 && columna < 7 && (color[y] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                break;

            case DAMA:
            case ALFIL:
                for (y = i - 9; y >= 0 && COLUMNA(y) != 7; y -= 9) {
					if (color[y] != VACIO) {
						if (color[y] != turno)
							Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                        break;
					}
                }
                for (y = i - 7; y >= 0 && COLUMNA(y) != 0; y -= 7) {
					if (color[y] != VACIO) {
						if (color[y] != turno)
							Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                        break;
					}
                }
                for (y = i + 9; y < 64 && COLUMNA(y) != 0; y += 9) {
					if (color[y] != VACIO) {
						if (color[y] != turno)
							Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                        break;
					}
                }
                for (y = i + 7; y < 64 && COLUMNA(y) != 7; y += 7) {
					if (color[y] != VACIO) {
						if (color[y] != turno)
							Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                        break;
					}
                }
                if (pieza[i] == ALFIL)
                    break;

            case TORRE:
                columna = COLUMNA(i);
                for (k = i - columna, y = i - 1; y >= k; y--) {
					if (color[y] != VACIO) {
						if (color[y] != turno)
							Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                        break;
					}
                }
                for (k = i - columna + 7, y = i + 1; y <= k; y++) {
					if (color[y] != VACIO) {
						if (color[y] != turno)
							Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                        break;
					}
                }
                for (y = i - 8; y >= 0; y -= 8) {
					if (color[y] != VACIO) {
						if (color[y] != turno)
							Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                        break;
						}
                }
                for (y = i + 8; y < 64; y += 8) {
					if (color[y] != VACIO) {
						if (color[y] != turno)
							Generar_Movimiento_Normal_Capturas(i, y, plista, &cuenta);
                        break;
					}
                }
                break;

            case REY:
                columna = COLUMNA(i);
                if ((columna > 1) && (color[i - 1] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, i - 1, plista, &cuenta);
                if ((columna < 7) && (color[i + 1] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, i + 1, plista, &cuenta);
                if ((i > 7) && (color[i - 8] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, i - 8, plista, &cuenta);
                if ((i < 56) && (color[i + 8] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, i + 8, plista, &cuenta);
                if (columna > 1 && i > 7 && (color[i - 9] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, i - 9, plista, &cuenta);
                if (columna < 7 && i > 7 && (color[i - 7] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, i - 7, plista, &cuenta);
                if (columna > 1 && i < 56 && (color[i + 7] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, i + 7, plista, &cuenta);
                if (columna < 7 && i < 56 && (color[i + 9] == cturno))
                    Generar_Movimiento_Normal_Capturas(i, i + 9, plista, &cuenta);
                break;
            }
        }
	}

	/*generamos al paso*/
	if (alpaso != -1) {
		if (turno == BLANCO) {
			if ((COLUMNA(alpaso) != 0) && (color[alpaso + 7] == BLANCO) && (pieza[alpaso + 7] == PEON))
				Generar_Movimiento_Alpaso_Capturas(alpaso + 7, alpaso, plista, &cuenta);
			if ((COLUMNA(alpaso) != 7) && (color[alpaso + 9] == BLANCO) && (pieza[alpaso + 9] == PEON))
				Generar_Movimiento_Alpaso_Capturas(alpaso + 9, alpaso, plista, &cuenta);
		}
		else {
			if ((COLUMNA(alpaso) != 0) && (color[alpaso - 9] == NEGRO) && (pieza[alpaso - 9] == PEON))
				Generar_Movimiento_Alpaso_Capturas(alpaso - 9, alpaso, plista, &cuenta);
			if ((COLUMNA(alpaso) != 7) && (color[alpaso - 7] == NEGRO) && (pieza[alpaso - 7] == PEON))
				Generar_Movimiento_Alpaso_Capturas(alpaso - 7, alpaso, plista, &cuenta);
		}
	}
    return cuenta;
}

/* esta función es la encargada de realizar un movimiento y guardarlo en la lista de jugadas */
int Hacer_Movimiento(movimiento m)
{
	int de = -1, a = -1, i, viejo_enroque, viejo_alpaso;

	if (m.tipo == ENROQUE) { /*en caso de enroque movemos la torre*/
		if (EstaEnJaque(turno))
			return FALSO;
		if (strcmp (variante,"normal") == 0) {
			switch (m.a) {
				case 62:
					if ((color[61] != VACIO) || (color[62] != VACIO) ||
						(atacado (61, BLANCO)))
						return FALSO;
					de = 63;
					a = 61;
					break;
				case 58:
					if ((color[59] != VACIO) || (color[58] != VACIO) || (color[57] != VACIO) ||
						(atacado (59, BLANCO)))
						return FALSO;
					de = 56;
					a = 59;
					break;
				case 6:
					if ((color[5] != VACIO) || (color[6] != VACIO) ||
						(atacado (5, NEGRO)))
						return FALSO;
					de = 7;
					a = 5;
					break;
				case 2:
					if ((color[3] != VACIO) || (color[2] != VACIO) || (color[1] != VACIO) ||
					(atacado (3, NEGRO)))
						return FALSO;
					de = 0;
					a = 3;
					break;
				default: 	/*no debería llegar aquí*/
					de = -1;
					a = -1;
					break;
			}
		}
		else if (strcmp (variante,"fischerandom") == 0) {
			switch (m.a) {
				case 62:
					if (rb < 62 ) {
						for (i = rb+1; i <= 62; i++) {
							if (color[i] != VACIO) {
								if ((pieza[i] == TORRE) && (color[i] == BLANCO) && (i == tb1))
									continue;
								else
									return FALSO;
							}
						}
						for (i = rb+1; i < 62; i++) {
							if (atacado (i, BLANCO))
								return FALSO;
						}
					}
					else if (rb == 62) {
						if (color[61] != VACIO)
							return FALSO;
					}
					de = tb1;
					a = 61;
					break;
				case 58:
					if (rb > 58) {
						if ((tb2 == 56) && (color[57] != VACIO))
							return FALSO;
						for (i = rb-1; i >= 58; i--) {
							if (color[i] != VACIO) {
								if ((pieza[i] == TORRE) && (color[i] == BLANCO) && (i == tb2))
									continue;
								else
									return FALSO;
							}
						}
						for (i = rb-1; i > 58; i--) {
							if (atacado (i, BLANCO))
								return FALSO;
						}
					}
					else if (rb == 58) {
						if (tb2 == 57) {
							if (color[59] != VACIO)
								return FALSO;
						}
						else if (tb2 == 56) {
							if ((color[57] != VACIO) || (color[59] != VACIO))
								return FALSO;
						}
					}
					else if (rb == 57) {
						if ((color[58] != VACIO) || (color[59] != VACIO))
							return FALSO;
					}
					de = tb2;
					a = 59;
					break;
				case 6:
					if (rn < 6) {
						for (i = rn+1; i <= 6; i++) {
							if (color[i] != VACIO) {
								if ((pieza[i] == TORRE) && (color[i] == NEGRO) && (i == tn1))
									continue;
								else
									return FALSO;
							}
						}
						for (i = rn+1; i < 6; i++) {
							if (atacado (i, NEGRO))
								return FALSO;
						}
					}
					else if (rn == 6) {
						if(color[5] != VACIO)
							return FALSO;
					}
					de = tn1;
					a = 5;
					break;
				case 2:
					if (rn > 2) {
						if ((tn2 == 0) && (color[1] != VACIO))
							return FALSO;
						for (i = rn-1; i >= 2; i--) {
							if (color[i] != VACIO) {
								if ((pieza[i] == TORRE) && (color[i] == NEGRO) && (i == tn2))
									continue;
								else
									return FALSO;
							}
						}
						for (i = rn-1; i > 2; i--) {
							if (atacado (i, NEGRO))
								return FALSO;
						}
					}
					else if (rn == 2) {
						if (tn2 == 1) {
							if (color[3] != VACIO)
								return FALSO;
						}
						else if (tn2 == 0) {
							if ((color[1] != VACIO) || (color[3] != VACIO))
								return FALSO;
						}
					}
					else if (rn == 1) {
						if ((color[2] != VACIO) || (color[3] != VACIO))
							return FALSO;
					}
					de = tn2;
					a = 3;
					break;
				default: 	/*no debería llegar aquí*/
					de = -1;
					a = -1;
					break;
			}
		}
	}

	/*guardamos el movimiento, pieza capturada, enroques y al paso*/
    jugadas[njugadas].m = m;
	if (m.tipo == ENROQUE)
		jugadas[njugadas].captura = VACIO;
	else
		jugadas[njugadas].captura = pieza[m.a];
	jugadas[njugadas].enroque = viejo_enroque = enroque;
	jugadas[njugadas].alpaso = viejo_alpaso = alpaso;
	jugadas[njugadas].regla50 = regla50;
	jugadas[njugadas].hash = hash;

	/*eliminamos la pieza de la casilla inicial para la llave*/
	hash ^= hash_piezas[turno][pieza[m.de]][m.de];

	/* si la pieza está en su sitio no la debemos borrar, esto es pensando en el rey en chess960 */
	if (m.tipo == ENROQUE && m.a == m.de) {
		pieza[m.a] = pieza[m.de];
		color[m.a] = color[m.de];
		/*desactivamos los enroques correspondientes aunque no hayamos movido el rey*/
		if (m.a == 62 || m.a == 58) enroque &= 12;
		if (m.a == 6 || m.a == 2) enroque &= 3;
	}
	else {
		pieza[m.a] = pieza[m.de];
		color[m.a] = color[m.de];
		pieza[m.de] = VACIO;
		color[m.de] = VACIO;
		/*actualizar enroques despues de mover un rey o torre*/
		enroque &= enroque_mascara[m.de] & enroque_mascara[m.a];
	}

	/*actualizar la posibilidad alpaso después de avanzar un peon 2 casillas*/
	if (m.tipo == PEON_AVANZA_2) {
		if (turno == BLANCO) {
			alpaso = m.a + 8;
		}
		else {
			alpaso = m.a - 8;
		}
	}
	else
		alpaso = -1;

	/*actualizar la regla de los 50 movimientos si no ha habido captura o movimiento de peon*/
	if ((jugadas[njugadas].captura != VACIO) || (pieza[m.a] == PEON))
		regla50 = 0;
	else
		regla50++;

	/* actualizamos el número de piezas y la situación de los reyes */
	if (turno == NEGRO) {
		if (pieza[m.a] == REY)
			rn = m.a;
		/* si capturamos una pieza, la eliminamos de la lista */
		if (jugadas[njugadas].captura != VACIO) {
			/* eliminamos la pieza capturada de la llave */
			hash ^= hash_piezas[BLANCO][jugadas[njugadas].captura][m.a];
			switch(jugadas[njugadas].captura) {
				case PEON:
					peonesblancos--;
					break;
				case CABALLO:
					caballosblancos--;
					break;
				case ALFIL:
					alfilesblancos--;
					break;
				case TORRE:
					torresblancas--;
					break;
				case DAMA:
					damasblancas--;
					break;
			}
		}
	}
	else {
		if (pieza[m.a] == REY)
			rb = m.a;
		if (jugadas[njugadas].captura != VACIO) {
			/* eliminamos la pieza capturada de la llave */
			hash ^= hash_piezas[NEGRO][jugadas[njugadas].captura][m.a];
			switch(jugadas[njugadas].captura) {
				case PEON:
					peonesnegros--;
					break;
				case CABALLO:
					caballosnegros--;
					break;
				case ALFIL:
					alfilesnegros--;
					break;
				case TORRE:
					torresnegras--;
					break;
				case DAMA:
					damasnegras--;
					break;
			}
		}
	}

	/* si estamos haciendo un enroque debemos mover la torre a su sitio */
	if (m.tipo == ENROQUE) {
		if (a != de) {
			pieza[a] = TORRE;
			color[a] = color[de];
			if ((m.a == 62 && tb1 != 62) || (m.a == 6 && tn1 != 6) || (m.a == 58 && tb2 != 58) || (m.a == 2 && tn2 != 2)) {
				/* quitamos la torre del punto inicial en la llave si corresponde */
				hash ^= hash_piezas[color[de]][TORRE][de];
				color[de] = VACIO;
				pieza[de] = VACIO;
				/* al mover la torre del enroque la ponemos en la llave */
				hash ^= hash_piezas[color[a]][TORRE][a];
			}
		}
	}

    /* si coronamos cambia el peon por pieza */
	if (m.tipo >= PROMOCION_A_DAMA) {
        switch (m.tipo) {
			case PROMOCION_A_DAMA:
				pieza[m.a] = DAMA;
				if (turno == BLANCO) {
					peonesblancos--;
					damasblancas++;
				}
				else {
					peonesnegros--;
					damasnegras++;
				}
				break;

			case PROMOCION_A_TORRE:
				pieza[m.a] = TORRE;
				if (turno == BLANCO) {
					peonesblancos--;
					torresblancas++;
				}
				else {
					peonesnegros--;
					torresnegras++;
				}
				break;

			case PROMOCION_A_ALFIL:
				pieza[m.a] = ALFIL;
				if (turno == BLANCO) {
					peonesblancos--;
					alfilesblancos++;
				}
				else {
					peonesnegros--;
					alfilesnegros++;
				}
				break;

			case PROMOCION_A_CABALLO:
				pieza[m.a] = CABALLO;
				if (turno == BLANCO) {
					peonesblancos--;
					caballosblancos++;
				}
				else {
					peonesnegros--;
					caballosnegros++;
				}
				break;
        }
    }

	/* borra el peon si estamos comiendo alpaso */
	if (m.tipo == AL_PASO) {
		if (turno == BLANCO) {
			color[m.a + 8] = VACIO;
			pieza[m.a + 8] = VACIO;
			peonesnegros--;
			/* eliminamos el peon que comemos al paso de la llave */
			hash ^= hash_piezas[NEGRO][PEON][m.a + 8];
		}
		else {
			color[m.a - 8] = VACIO;
			pieza[m.a - 8] = VACIO;
			peonesblancos--;
			hash ^= hash_piezas[BLANCO][PEON][m.a - 8];
		}
	}

	/* ponemos la pieza de la casilla final en la llave */
	hash ^= hash_piezas[turno][pieza[m.a]][m.a];

	hash ^= hash_turno[turno];

	ply++;
    njugadas++; /*nos preparamos para guardar la siguiente jugada*/
    turno = 1 - turno;    /*da el turno al oponente*/
	/*comprobamos si estamos en jaque después de mover*/
	if (EstaEnJaque(1 - turno)) {
		Deshacer();
		return FALSO;
	}
	hash ^= hash_turno[turno];
	if (viejo_alpaso != -1)
		hash ^= hash_alpaso[viejo_alpaso];
	if (alpaso != -1)
		hash ^= hash_alpaso[alpaso];
	if (viejo_enroque != enroque) {
		hash ^= hash_enroque[viejo_enroque];
		hash ^= hash_enroque[enroque];
	}
	Calcula_material();
	return VERDADERO;
}

void Deshacer(void) /* lo contrario a hacer un movimiento */
{
	movimiento	m;
	int de = -1, a = -1, td = FALSO;

	turno = 1 - turno;
    njugadas--;
	ply--;
    m = jugadas[njugadas].m;
	enroque = jugadas[njugadas].enroque;
	alpaso = jugadas[njugadas].alpaso;
	regla50 = jugadas[njugadas].regla50;
	hash = jugadas[njugadas].hash;

	if (m.tipo == ENROQUE) { /* colocamos la torre en su sitio si deshacemos el enroque */
		switch(m.a) {
			case 62:
				de = 61;
				a = tb1;
				break;
			case 58:
				de = 59;
				a = tb2;
				break;
			case 6:
				de = 5;
				a = tn1;
				break;
			case 2:
				de = 3;
				a = tn2;
				break;
			default:  /* no debería llegar aquí */
				de = -1;
				a = -1;
				break;
		}
		/*si estamos deshaciendo un enroque, primero probamos a llevar la torre a su sitio original
		 siempre y cuando no vaya a colocarse donde todavía está el rey*/
		if (a != de && a != 62 && a != 6 && a != 2 && a != 58) {
			pieza[a] = pieza[de];
			color[a] = color[de];
			color[de] = VACIO;
			pieza[de] = VACIO;
			td = VERDADERO;
		}
	}

	/*en chess960 si el rey al deshacer el enroque no hay que moverlo*/
	if (m.tipo == ENROQUE && m.a == m.de) {
		pieza[m.de] = pieza[m.a];
		color[m.de] = color[m.a];
	}
	else {
		pieza[m.de] = pieza[m.a];
		color[m.de] = turno;
		pieza[m.a] = jugadas[njugadas].captura;
		if (jugadas[njugadas].captura != VACIO)
			color[m.a] = (BLANCO + NEGRO) - turno;
		else
			color[m.a] = VACIO;
	}

	/* actualizamos la posición de los reyes */
	if (pieza[m.de] == REY && color[m.de] == BLANCO)
		rb = m.de;
	else if (pieza[m.de] == REY && color[m.de] == NEGRO)
		rn = m.de;

	/* actualizamos el número de piezas */
	if (color[m.a] == BLANCO) {
		switch(pieza[m.a]) {
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
				torresblancas++;
				break;
			case DAMA:
				damasblancas++;
				break;
		}
	}
	else if (color[m.a] == NEGRO) {
		switch(pieza[m.a]) {
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
				torresnegras++;
				break;
			case DAMA:
				damasnegras++;
				break;
		}
	}

	if (m.tipo >= PROMOCION_A_DAMA) { /* si deshacemos una promoción colocamos el peon en sitio */
        pieza[m.de] = PEON;
        switch (m.tipo) {
			case PROMOCION_A_DAMA:
				if (turno == BLANCO) {
					peonesblancos++;
					damasblancas--;
				}
				else {
					peonesnegros++;
					damasnegras--;
				}
				break;

			case PROMOCION_A_TORRE:
				if (turno == BLANCO) {
					peonesblancos++;
					torresblancas--;
				}
				else {
					peonesnegros++;
					torresnegras--;
				}
				break;

			case PROMOCION_A_ALFIL:
				if (turno == BLANCO) {
					peonesblancos++;
					alfilesblancos--;
				}
				else {
					peonesnegros++;
					alfilesnegros--;
				}
				break;

			case PROMOCION_A_CABALLO:
				if (turno == BLANCO) {
					peonesblancos++;
					caballosblancos--;
				}
				else {
					peonesnegros++;
					caballosnegros--;
				}
				break;
        }
	}

	if (m.tipo == ENROQUE) { /* colocamos la torre en su sitio si deshacemos el enroque y todavía no lo hemos hecho */
		if (!td && a != de) {
			pieza[a] = TORRE;
			color[a] = color[de];
			if (m.de != de) {
				color[de] = VACIO;
				pieza[de] = VACIO;
			}
		}
	}

	if (m.tipo == AL_PASO) { /*coloca el peon contrario en su sitio si deshacemos alpaso*/
		if (turno == BLANCO) {
			color[m.a + 8] = NEGRO;
			pieza[m.a + 8] = PEON;
			peonesnegros++;
		}
		else {
			color[m.a - 8] = BLANCO;
			pieza[m.a - 8] = PEON;
			peonesblancos++;
		}
	}
	Calcula_material();
}

/* esta función retorna el número de movimientos legales */
int Generar_legales(void) {
    int i, legales = 0, contar2 = 0;
	movimiento lista2[256];

	contar2 = Generar(lista2);

	for (i = 0; i < contar2; ++i) {
		if (!Hacer_Movimiento(lista2[i]))
			continue;
        ++legales;
 		Deshacer();
    }
	return legales;
}
