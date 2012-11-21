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
La función principal aquí va a ser comprobar si una casilla es atacada por una pieza,
en base a ello comprobamos si el rey está en jaque, la función atacado está basado en
las características de generación de movimientos de FirstChess, el generador más simple
posible, está función es muchísimo más rápida que la utilizada con mailbox por TSCP.

Está por ver si con otro generador de movimientos diferentes se conseguiría mucha más
velocidad, también mantengo este sistema un poco en honor a FirstChess.
*/

/*Includes*/
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

/* comprueba si el rey está en jaque, tenemos guardadas la posición de los reyes rb (rey
blanco) y rn ya que vamos actualizando su posición en hacer_movimiento y deshacer */
int	EstaEnJaque(int turno)
{
	if (turno == BLANCO)
		return atacado(rb, BLANCO);
	else
		return atacado(rn, NEGRO);
}

/*comprueba si una casilla k es atacada por una casilla de color contraria al turno que toca
retorna 1 (VERDADERO) si es atacada y un 0 (FALSO) si no lo es, esta función la vamos a utilizar
para saber si el rey está en jaque, también nos permitirá controlar la seguridad del rey sabiendo
si una casilla cercana al rey está siendo atacada, incluso planea utilizarla para saber si puede
haber piezas atrapadas*/
int atacado(int k, int turno)
{
	int				h,
                    y,
                    fila,
                    columna,
                    cturno;

	cturno = (BLANCO + NEGRO) - turno;     /*quien nos da jaque tiene el turno contrario*/
    fila = FILA(k), columna = COLUMNA(k);

    /*Chequea los ataques del caballo*/
    if (columna > 0 && fila > 1 && color[k - 17] == cturno && pieza[k - 17] == CABALLO)
        return 1;
    if (columna < 7 && fila > 1 && color[k - 15] == cturno && pieza[k - 15] == CABALLO)
        return 1;
    if (columna > 1 && fila > 0 && color[k - 10] == cturno && pieza[k - 10] == CABALLO)
        return 1;
    if (columna < 6 && fila > 0 && color[k - 6] == cturno && pieza[k - 6] == CABALLO)
        return 1;
    if (columna > 1 && fila < 7 && color[k + 6] == cturno && pieza[k + 6] == CABALLO)
        return 1;
    if (columna < 6 && fila < 7 && color[k + 10] == cturno && pieza[k + 10] == CABALLO)
        return 1;
    if (columna > 0 && fila < 6 && color[k + 15] == cturno && pieza[k + 15] == CABALLO)
        return 1;
    if (columna < 7 && fila < 6 && color[k + 17] == cturno && pieza[k + 17] == CABALLO)
        return 1;

	/*Chequea las lineas horizontales y verticales para los ataques de dama, torre y rey*/
    /*abajo*/
    y = k + 8;
    if (y < 64) {
        if (color[y] == cturno && (pieza[y] == REY || pieza[y] == DAMA || pieza[y] == TORRE))
            return 1;
        if (pieza[y] == VACIO)
            for (y += 8; y < 64; y += 8) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }
    /*izquierda*/
    y = k - 1;
    h = k - columna;
    if (y >= h) {
        if (color[y] == cturno && (pieza[y] == REY || pieza[y] == DAMA || pieza[y] == TORRE))
            return 1;
        if (pieza[y] == VACIO)
            for (y--; y >= h; y--) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }
    /*derecha*/
    y = k + 1;
    h = k - columna + 7;
    if (y <= h) {
        if (color[y] == cturno && (pieza[y] == REY || pieza[y] == DAMA || pieza[y] == TORRE))
            return 1;
        if (pieza[y] == VACIO)
            for (y++; y <= h; y++) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }
    /*arriba*/
    y = k - 8;
    if (y >= 0) {
        if (color[y] == cturno && (pieza[y] == REY || pieza[y] == DAMA || pieza[y] == TORRE))
            return 1;
        if (pieza[y] == VACIO)
            for (y -= 8; y >= 0; y -= 8) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }

	/*Chequea las diagonales para los ataques de dama, alfil, rey y peon*/
    /*abajo derecha*/
    y = k + 9;
    if (y < 64 && COLUMNA(y) != 0) {
        if (color[y] == cturno) {
            if (pieza[y] == REY || pieza[y] == DAMA || pieza[y] == ALFIL)
                return 1;
            if (turno == NEGRO && pieza[y] == PEON)
                return 1;
        }
        if (pieza[y] == VACIO)
            for (y += 9; y < 64 && COLUMNA(y) != 0; y += 9) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == ALFIL))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }
    /*abajo izquierda*/
    y = k + 7;
    if (y < 64 && COLUMNA(y) != 7) {
        if (color[y] == cturno) {
            if (pieza[y] == REY || pieza[y] == DAMA || pieza[y] == ALFIL)
                return 1;
            if (turno == NEGRO && pieza[y] == PEON)
                return 1;
        }
        if (pieza[y] == VACIO)
            for (y += 7; y < 64 && COLUMNA(y) != 7; y += 7) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == ALFIL))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }
    /*arriba izquierda*/
    y = k - 9;
    if (y >= 0 && COLUMNA(y) != 7) {
        if (color[y] == cturno) {
            if (pieza[y] == REY || pieza[y] == DAMA || pieza[y] == ALFIL)
                return 1;
            if (turno == BLANCO && pieza[y] == PEON)
                return 1;
        }
        if (pieza[y] == VACIO)
            for (y -= 9; y >= 0 && COLUMNA(y) != 7; y -= 9) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == ALFIL))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }
    /*arriba derecha*/
    y = k - 7;
    if (y >= 0 && COLUMNA(y) != 0) {
        if (color[y] == cturno) {
            if (pieza[y] == REY || pieza[y] == DAMA || pieza[y] == ALFIL)
                return 1;
            if (turno == BLANCO && pieza[y] == PEON)
                return 1;
        }
        if (pieza[y] == VACIO)
            for (y -= 7; y >= 0 && COLUMNA(y) != 0; y -= 7) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == ALFIL))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }
    return 0;
}

/*ahora vamos a tener una variante de la función atacado, en este caso es para utilizarla
en la seguridad del rey y saber si una casilla cercana al rey está defendida por las piezas
del mismo color, en este caso no vamos a comprobar si está defendido por el propio rey.*/
int atacado_nr(int k, int turno)
{
	int				h,
                    y,
                    fila,
                    columna,
                    cturno;

	cturno = (BLANCO + NEGRO) - turno;     /*quien nos da jaque tiene el turno contrario*/
    fila = FILA(k), columna = COLUMNA(k);

    /*Chequea los ataques del caballo*/
    if (columna > 0 && fila > 1 && color[k - 17] == cturno && pieza[k - 17] == CABALLO)
        return 1;
    if (columna < 7 && fila > 1 && color[k - 15] == cturno && pieza[k - 15] == CABALLO)
        return 1;
    if (columna > 1 && fila > 0 && color[k - 10] == cturno && pieza[k - 10] == CABALLO)
        return 1;
    if (columna < 6 && fila > 0 && color[k - 6] == cturno && pieza[k - 6] == CABALLO)
        return 1;
    if (columna > 1 && fila < 7 && color[k + 6] == cturno && pieza[k + 6] == CABALLO)
        return 1;
    if (columna < 6 && fila < 7 && color[k + 10] == cturno && pieza[k + 10] == CABALLO)
        return 1;
    if (columna > 0 && fila < 6 && color[k + 15] == cturno && pieza[k + 15] == CABALLO)
        return 1;
    if (columna < 7 && fila < 6 && color[k + 17] == cturno && pieza[k + 17] == CABALLO)
        return 1;

	/*Chequea las lineas horizontales y verticales para los ataques de dama, torre*/
    /*abajo*/
    y = k + 8;
    if (y < 64) {
        if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
            return 1;
        if (pieza[y] == VACIO)
            for (y += 8; y < 64; y += 8) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }
    /*izquierda*/
    y = k - 1;
    h = k - columna;
    if (y >= h) {
        if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
            return 1;
        if (pieza[y] == VACIO)
            for (y--; y >= h; y--) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }
    /*derecha*/
    y = k + 1;
    h = k - columna + 7;
    if (y <= h) {
        if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
            return 1;
        if (pieza[y] == VACIO)
            for (y++; y <= h; y++) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }
    /*arriba*/
    y = k - 8;
    if (y >= 0) {
        if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
            return 1;
        if (pieza[y] == VACIO)
            for (y -= 8; y >= 0; y -= 8) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == TORRE))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }

	/*Chequea las diagonales para los ataques de dama, alfil, y peon*/
    /*abajo derecha*/
    y = k + 9;
    if (y < 64 && COLUMNA(y) != 0) {
        if (color[y] == cturno) {
            if (pieza[y] == DAMA || pieza[y] == ALFIL)
                return 1;
            if (turno == NEGRO && pieza[y] == PEON)
                return 1;
        }
        if (pieza[y] == VACIO)
            for (y += 9; y < 64 && COLUMNA(y) != 0; y += 9) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == ALFIL))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }
    /*abajo izquierda*/
    y = k + 7;
    if (y < 64 && COLUMNA(y) != 7) {
        if (color[y] == cturno) {
            if (pieza[y] == DAMA || pieza[y] == ALFIL)
                return 1;
            if (turno == NEGRO && pieza[y] == PEON)
                return 1;
        }
        if (pieza[y] == VACIO)
            for (y += 7; y < 64 && COLUMNA(y) != 7; y += 7) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == ALFIL))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }
    /*arriba izquierda*/
    y = k - 9;
    if (y >= 0 && COLUMNA(y) != 7) {
        if (color[y] == cturno) {
            if (pieza[y] == DAMA || pieza[y] == ALFIL)
                return 1;
            if (turno == BLANCO && pieza[y] == PEON)
                return 1;
        }
        if (pieza[y] ==	VACIO)
            for (y -= 9; y >= 0 && COLUMNA(y) != 7; y -= 9) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == ALFIL))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }
    /*arriba derecha*/
    y = k - 7;
    if (y >= 0 && COLUMNA(y) != 0) {
        if (color[y] == cturno) {
            if (pieza[y] == DAMA || pieza[y] == ALFIL)
                return 1;
            if (turno == BLANCO && pieza[y] == PEON)
                return 1;
        }
        if (pieza[y] == VACIO)
            for (y -= 7; y >= 0 && COLUMNA(y) != 0; y -= 7) {
                if (color[y] == cturno && (pieza[y] == DAMA || pieza[y] == ALFIL))
                    return 1;
                if (pieza[y] != VACIO)
                    break;
            }
    }
    return 0;
}
