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
Funciones de búsqueda del mejor movimiento
*/

/*Includes*/
#include <stdio.h>		/*prinft, scanf, sprinft, sscanf, fgets, fflush...*/
#include <string.h>     /*menset, strcmp, strcat ...*/
#include <stdlib.h>     /*abs*/
#include "definiciones.h"
#include "variables.h"
#include "funciones.h"

/*Esta función nos va a devolver el mejor movimiento posible para una profundidad
Este movimiento podría ser un movimiento de libro
La función llama de forma iterativa al algoritmo alphabeta o en nuestro caso PVS*/
movimiento	MotorPiensa(void)
{
	movimiento	m = no_move;
	int	i, alpha, beta, contar_legales = 0;
	int contador1, contador2;
	char s[4];
	int de, a, tipo;
	int contar;
    movimiento	lista[256];
	char EPD[100];
	char FROM[10],TO[10];
	char FROM1[20],FROM2[20],TO1[20],TO2[20];
	int moves;
	int tiempo_usado;

	tiempo_inicio = get_ms();					        /*tiempo actual*/

	if (estado == BUSCANDO) {
		/*ahora vamos a comprobar si hay un movimiento de libro, si el libro está cargado, se han juegado menos de 30
		movimientos y no se han jugado más de 4 movimientos fuera del libro*/
		if ((strcmp (variante,"normal") == 0) && libro_cargado && fuera_del_libro < 3 && njugadas < 60) {
			s[0] = '\0';
			Tablero_FEN(EPD);
			FIND_OPENING(EPD,FROM,TO,FROM1,FROM2,TO1,TO2,&moves);
			if (moves == 0)
				fuera_del_libro++;
			else
				fuera_del_libro = 0;
			strcat(s, FROM);
			strcat(s, TO);

			/*calculamos las coordenadas del movimiento, de, a y tipo y comparamos con una lista que generamos de todos
			los posibles movimientos, así sabemos que el movimiento que viene del libro es legal*/
			if (strcmp(s,"") != 0) {
				de = s[0] - 'a';
				de += 8 * (8 - (s[1] - '0'));
				a = s[2] - 'a';
				a += 8 * (8 - (s[3] - '0'));
				tipo = NORMAL;

				if (Comprobar_Tipo_Movimiento(de, a, s) != NINGUNO)
					tipo = Comprobar_Tipo_Movimiento(de, a, s);

				/*generamos una lista con todos los posibles movimientos que se podrían jugar*/
				contar = Generar(lista);
				/*recorre todos los movimientos para ver si es legal*/
				for (i = 0; i < contar; i++) {
					if (lista[i].de == de && lista[i].a == a && lista[i].tipo == tipo) {
						if (Hacer_Movimiento(lista[i])) {
							Deshacer();			            /*deshacemos el movimiento aunque sea bueno porque lo enviamos luego*/
							printf ("0 0 0 0 Book move\n");	/*indicamos que es un movimiento del libro*/
#if UNDER_CE
							fflush(stdout);
#endif
							return lista[i];	            /*movimiento que retornamos y se enviará al GUI*/
						}
						else {
							Deshacer();	                    /*si el movimiento del libro no es legal lo deshacemos*/
							continue;
						}
					}
				}
			}
		}
	}
#if UNDER_CE
	else if (estado == PONDERANDO) {
		printf ("0 0 0 0 pondering...\n");
		fflush(stdout);
	}
#endif

	aborta = FALSO;
	para_busqueda = FALSO;                              /*inicialmente no paramos la búsqueda*/

	ply = 0;					                        /*comenzamos en el ply 0 (otros empiezan en el 1)*/
	nodos = 0;					                        /*iniciamos el número de nodos*/
	contador1 = contador2 = 0;	                        /*iniciamos contadores para saber si se ha producido un fallo en la búsqueda*/

	material = piezasblanco+piezasnegro+peonesblanco+peonesnegro;
	hacer_lazy_eval = 1;

#if UNDER_CE
	if (material<FINAL)
		cknodos =  0x33;
	else
		cknodos =  0xCCC;
#else
	if (material<FINAL)
		cknodos =  0x400;
	else
		cknodos =  0x4000;
#endif

	/*borramos la pv, historia y los movimientos killer y matekiller*/
	memset(pv, 0, sizeof(pv));
	memset(Historia, 0, sizeof(Historia));
	memset(matekiller, 0, sizeof(matekiller));
	memset(killer1, 0, sizeof(killer1));
	memset(killer2, 0, sizeof(killer2));
	maxhistoria = 0;
	/*se guarda el material que hay en ese momento para utilizarlo luego en la evaluación*/
	old_piezasblanco = piezasblanco;
	old_piezasnegro = piezasnegro;
	old_peonesblanco = peonesblanco;
	old_peonesnegro = peonesnegro;

	/*definimos 2 ventanas para hacer búsqueda, en lugar de buscar desde -MATE hasta MATE que serían todas
	las posibles evaluaciones, buscamos desde -WINDOW1 a +WINDOW1, asi la búsqueda es más rápida, si la
	búsqueda sale de estos valores abriremos a una ventana más amplia y luego hasta -MATE y MATE*/
	#define WINDOW1	15
	#define WINDOW2	35
	#define WINDOW3	100
	#define WINDOW4	200

	/*para la primera profundidad hacemos una búsqueda completa*/
	alpha = -MATE;
	beta = MATE;

	contar_legales = Generar_legales();
	/*printf("El número de movimientos legales es: %d\n", contar_legales);*/

	/*llamamos a la búsqueda de una forma interactiva, primero buscamos completo la profundidad 1, luego la 2,
	etc, esto tiene 2 ventajas, si el tiempo se termina al menos tenemos una búsqueda completa en una profundidad
	y además cada nueva profundidad podemos resolverla más rápido ya que produce una mejor ordenación de la variante principal*/
	for (i = 1; i <= max_profundidad;) {

		li = i;	                /*guardamos el ply para utilizarlo en el acceso a las bitbases y otros*/
		sigue_Pv = VERDADERO;	/*inicialmente hacemos que siga la variante principa*/

		/*llamamos a la búsqueda, la profundidad que vamos a utilizar será fracional, PLY = 8,
		las extensiones fraccionales deberían permitir un mejor control*/
		valor1[i] = PVS(alpha, beta, i * PLY, VERDADERO);
		m = pv[0][0];		    /*este es el mejor movimiento para la profundidad i*/

		/*si la búsqueda ha terminado, retornamos el mejor movimiento*/
		if (para_busqueda == VERDADERO)
			break;

		/*abrimos nuevas ventanas si se produce un fallo en la búsqueda, no estoy seguro si tal como está definida
		las ventanas se podría producir inestablidad, parece funcionar bien y rápido*/
		if (valor1[i] <= alpha) {
			contador1++;
			if (contador1 == 1) {
				alpha = valor1[i] - WINDOW2;
				continue;
			}
			if (contador1 == 2) {
				alpha = valor1[i] - WINDOW3;
				continue;
			}
			if (contador1 == 3) {
				alpha = valor1[i] - WINDOW4;
				continue;
			}
			alpha =  -MATE;
			continue;
		}

		if (valor1[i] >= beta) {
			contador2++;
			if (contador2 == 1) {
				beta = valor1[i] + WINDOW2;
				continue;
			}
			if (contador2 == 2) {
				beta = valor1[i] + WINDOW3;
				continue;
			}
			if (contador2 == 3) {
				beta = valor1[i] + WINDOW4;
				continue;
			}
			beta =  MATE;
			continue;
		}

		/* si la cosa es clara reducimos el tiempo de búsqueda */
		if (estado == BUSCANDO && i >= 2) {
			/* ya tenemos una profundidad mínima para abandonar la búsqueda */
			 aborta = VERDADERO;
			/* un solo movimiento legal */
			if (contar_legales == 1 && i >= 4) break;
			/* no comenzamos una nueva iteración si ya hemos consumido al menos la mitad de tiempo */
			tiempo_usado = get_ms() - tiempo_inicio;
			if (tiempo_usado > no_nueva_iteracion) break;
		}

		alpha = valor1[i] - WINDOW1;       /*configuramos la ventana para el siguiente ciclo*/
	    beta = valor1[i] + WINDOW1;
		contador1 = contador2 = 0;
	    i++;
	}

	return m;       /*devolvemos el mejor movimiento*/
}


/* la siguiente función es la encargada de la búsqueda, es un algoritmo alphabeta, una variante
que se llama variante principal, está basada en un algoritmo de Bruce Moreland */
int	PVS(int alpha, int beta, int depth, int hacer_nulo)
{
#define R	2
	int i, jaque, da_jaque, contar = 0, nuMoves = 0, ev, total_piezas, amenaza_mate = 0, puntos = alpha, de, a, podemoscortar = 0;
	movimiento mejor_movimiento = no_move, lista[256];   /*218 es el máximo número de combinaciones posibles para un movimiento*/

    int nodo_pv =  ((beta - alpha) > 1);
	int ext;
	int mejor = -MATE;

	/*TTables*/
	int htipo;
	movimiento hmov;
	int hash_flag = hasfALPHA;

	/*si estamos en profundidad cero o en nuestro caso menor que un PLY llamamos al buscador de capturas y promociones*/
	/*lo mismo si hemos ido muy profundo*/
	if ((depth < PLY) || ply >= MAX_PROFUNDIDAD-1) {
		return Extiende_capturas(alpha, beta);
	}

	nodos++;
	if ((nodos % cknodos) == 0) 	        /*comprueba cada un cierto número de nodos que tenemos tiempo*/
		checkup(puntos);
	if (para_busqueda == VERDADERO)
		return 0;

	/* mate-distance pruning */
	if (ply) {
		alpha = MAX(alpha, -MATE + ply);
		beta = MIN(beta, MATE - ply);
		if (alpha >= beta)
			return alpha;
	}

	/*si la posición se repite 1 vez valora como tablas, aunque no las reclama*/
	if (ply && repeticion () > 0)
		return 0;

	/*comprobamos las tablas hash*/
	puntos = probe_hash(&htipo, depth+njugadas, beta, &hmov, &hacer_nulo);
	if (ply && puntos != valUNKNOWN)  {
		switch (htipo) {
			case hasfEXACT:
				if (!nodo_pv) {
					Actualiza_pv(hmov);
					return puntos;
				}
				break;
			case hasfALPHA:
				if (puntos <= alpha) {
					Actualiza_pv(hmov);
					return alpha;
				}
				break;
			case hasfBETA:
				if (puntos >= beta) {
					Actualiza_pv(hmov);
					return beta;
				}
				break;
			case hasfNULL:
				break;
		}
	}

	/*comprobamos el acceso a las bitbases*/
	if (ply && egbb_is_loaded) {
		total_piezas = peonesblancos + caballosblancos + alfilesblancos + torresblancas
						+ damasblancas + peonesnegros + caballosnegros + alfilesnegros + torresnegras + damasnegras + 2;

		if (total_piezas <= men) {
			int probe_depth = (2*((depth/PLY)+ply)/3);
			/* Cortamos el árbol solo después de una cierta profundidad o si hacemos progresos (capturas, movimientos de peón) */
			if (ply >= probe_depth || jugadas[njugadas-1].captura != VACIO || pieza[jugadas[njugadas-1].m.a] == PEON) {
				/* Probamos bitbases en las ramas solo si están cargadas en RAM */
				if (ply <= probe_depth || (egbb_load_type >= 1 && total_piezas <= 4) || egbb_load_type == 3) {
					puntos = 0;
					if (probe_bitbases(&puntos) == 1) {
						/* Preferimos ganadas cerca de la raíz */
						if (puntos > 0)
							puntos +=VAL_BITBASE - ply;
						else if (puntos < 0)
							puntos += ply - VAL_BITBASE;
						return puntos;
					}
				}
			}
		}
	}

	if (ply >= MAX_PROFUNDIDAD-1) {
		return Evaluar(alpha, beta);
	}

	jaque = EstaEnJaque(turno);		/*comprobamos si estamos en jaque*/
	
	/*Internal Iterative Deepening */
	if (ply && hmov.a == 0 && hmov.de == 0 && nodo_pv && depth >= 4*PLY) {
		puntos = PVS(alpha, beta, depth-2*PLY, VERDADERO);
		probe_hash(&htipo, depth+njugadas, beta, &hmov, &hacer_nulo);
	}

	if (ply && !jaque && !nodo_pv && beta < MATE-64) {

		ev = Evaluar(-MATE, MATE);
		
		if (hacer_nulo && ((turno == BLANCO && piezasblanco > VALOR_ALFIL) || (turno == NEGRO && piezasnegro > VALOR_ALFIL))){
			
			if (depth <= PLY) {
				/*static null move prunning*/
				if (ev >= beta + 150)
					return (beta + 150);
				/*razoring*/
				if (ev + 125 < beta){
					puntos = Extiende_capturas(alpha, beta);
					return MAX(puntos, ev + 125);				
				}
			}
			else if (depth <= 3*PLY) {
				/*razoring*/
				if (ev + 300 < beta){
					puntos = Extiende_capturas(alpha, beta);
					if (puntos < ev + 300)
						return MAX(puntos, ev + 300);				
				}	
			}

			/* probamos un movimiento nulo, solo lo podemos hacer si nos deja después de comprobar las tablas hash, sino estamos
			en jaque (ya que nos comerían el rey) y si el lado que mueve tiene al menos una pieza */
			if (depth > PLY && ev >= beta) {

				/*nosotros utilizamos un movimiento nulo más agresivo que el resto de programadores, nos da rapidez, no
				seguro si podemos perder algo*/
				material = piezasblanco+piezasnegro+peonesblanco+peonesnegro;
				depthnull = depth - (1+R+(material>MEDIOJUEGO)+(depth>6*PLY)+(depth>11*PLY)+(depth>16*PLY))*PLY;

				/*hacemos el movimiento nulo, actualizamos la llave hash*/
				jugadas[njugadas].m = no_move;
				jugadas[njugadas].captura = VACIO;
				jugadas[njugadas].enroque = enroque;
				jugadas[njugadas].alpaso = alpaso;
				jugadas[njugadas].regla50 = regla50;
				jugadas[njugadas].hash = hash;
				if (alpaso != -1)
					hash ^= hash_alpaso[alpaso];
				alpaso = -1;
				regla50++;
				ply++;
				njugadas++;
				hash ^= hash_turno[turno];
				turno = 1 - turno;
				hash ^= hash_turno[turno];

				puntos = -PVS(-beta, -beta+1, depthnull, FALSO);

				/*deshacemos el movmiento nulo*/
				turno = 1 - turno;
				njugadas--;
				ply--;
				enroque = jugadas[njugadas].enroque;
				alpaso = jugadas[njugadas].alpaso;
				regla50 = jugadas[njugadas].regla50;
				hash = jugadas[njugadas].hash;

				if (para_busqueda == VERDADERO)
					return 0;

				/*verificación movimiento nulo*/
				if (puntos >= beta && depthnull > 4*PLY) {
					if ((turno == BLANCO && piezasblanco < 1600) || (turno == NEGRO && piezasnegro < 1600)) {
						puntos = PVS(alpha, beta, depthnull-2*PLY, VERDADERO);
					}
				}

				if (puntos >= beta) {
					store_hash(hasfBETA, depth+njugadas, beta, mejor_movimiento);
					if (puntos >= MATE - 64) puntos = beta; /* no devuelve un mate */
						return puntos;	/*cutoff*/
				}
				else if (puntos <= (-MATE+ply+2))
					amenaza_mate = 1;
			}
		}

		/*cormprobamos si es posible hacer futility prunning*/
		if (depth < 4*PLY && alpha > -MATE+64) {
			int margen[4] = { 0, 120, 140, 160 };
			if (ev + margen[depth/PLY] < beta)
				podemoscortar = 1;
		}
	}
	
	pv_longuitud[ply] = ply;
	contar = Generar(lista);	    /*genera todos los movimientos para la posición actual*/

	/*ordenamos primero el movimiento que viene de la hash y luego el de la variante principal*/
	Ordena_hmove_pv(contar, lista, hmov);

	/*recorremos todo el ciclo de movimientos*/
    for (i = 0; i < contar; ++i) {
		OrdenaMovimiento(i, contar, lista);
		if (!Hacer_Movimiento(lista[i]))
			continue;
		nuMoves++;	    /*número de movimientos legales*/

		da_jaque = EstaEnJaque(turno);

		/*extensiones*/
		ext = 0;
		material = piezasblanco+piezasnegro+peonesblanco+peonesnegro;
		if (nodo_pv) {
			/*damos jaque*/
			if (da_jaque) ext += PLY;
			/*tenemos una amenaza*/
			else if (amenaza_mate) ext += PLY;
			/*recaptura*/
			else if (jugadas[njugadas-1].captura != VACIO && jugadas[njugadas-2].captura != VACIO
				&& jugadas[njugadas-1].m.a == jugadas[njugadas-2].m.a
				&& abs(valor_pieza[jugadas[njugadas-1].captura] - valor_pieza[jugadas[njugadas-2].captura] <= VALOR_PEON/2))
				ext += PLY;
			/*peon en septima*/
			else if (material < MEDIOJUEGO && pieza[jugadas[njugadas-1].m.a] == PEON
				&& (FILA(jugadas[njugadas-1].m.a) == 1 || FILA(jugadas[njugadas-1].m.a) == 6))
				ext += PLY;
			/*peon en sexta*/
			else if (material < FINAL && pieza[jugadas[njugadas-1].m.a] == PEON
				&& (FILA(jugadas[njugadas-1].m.a) == 2 || FILA(jugadas[njugadas-1].m.a) == 5))
				ext += PLY/2;
			/*final de peones*/
			if (material < FINAL && jugadas[njugadas-1].captura > PEON && jugadas[njugadas-1].captura < REY) {
				if (piezasblanco == 0 && piezasnegro == 0) {        /*entrando en un final de peones*/
					switch(jugadas[njugadas-1].captura) {
						case DAMA: ext = 2*PLY;
							break;
						case TORRE: ext = 3*PLY/2;
							break;
						default: ext = PLY;
							break;
					}
				}
			}
		}
		else {
			if (da_jaque) ext += PLY/2;
			/*else if (amenaza_mate) ext += PLY/2;
			else if (jugadas[njugadas-1].captura != VACIO && jugadas[njugadas-2].captura != VACIO
				&& jugadas[njugadas-1].m.a == jugadas[njugadas-2].m.a
				&& abs(valor_pieza[jugadas[njugadas-1].captura] - valor_pieza[jugadas[njugadas-2].captura] <= VALOR_PEON/2))
				ext += PLY/2;
			else if (material < FINAL && pieza[jugadas[njugadas-1].m.a] == PEON
				&& (FILA(jugadas[njugadas-1].m.a) == 1 || FILA(jugadas[njugadas-1].m.a) == 6))
				ext += PLY/2;
			else if (material < FINAL && pieza[jugadas[njugadas-1].m.a] == PEON
				&& (FILA(jugadas[njugadas-1].m.a) == 2 || FILA(jugadas[njugadas-1].m.a) == 5))
				ext += PLY/4;
			if (material < FINAL && jugadas[njugadas-1].captura > PEON && jugadas[njugadas-1].captura < REY) {
				if (piezasblanco == 0 && piezasnegro == 0) { 
					switch(jugadas[njugadas-1].captura) {
						case DAMA: ext = PLY;
							break;
						case TORRE: ext = 3*PLY/4;
							break;
						default: ext = PLY/2;
							break;
					}
				}
			}*/
		}

		/*futility prunning*/
		if (podemoscortar && !ext && nuMoves > 1 && lista[i].valor < 1800000) {
			Deshacer();
			continue;
		}

		/*para el primer movimiento hacemos una busqueda completa*/
		if (nuMoves == 1) {
			puntos = -PVS(-beta, -alpha, depth+ext-PLY, VERDADERO);
		}
		else {
			/*LMR prunning en las terminaciones*/
			if (nuMoves > 12 && depth < 2*PLY && !ext && !jaque && !nodo_pv) {
				if ((nuMoves > 20) || (lista[i].valor < 1800000)) {
					Deshacer();
					continue;
				}
			}
			/*probamos a hacer LMR si no hay extensiones, si hay un número mínimo de movimientos legales, si tenemos
			una determinada profundidad, si el valor del movmiento no es grande, que no sea el movimiento de la tabla
			hash, principal variante, matekiller, capturas, promociones y killers y que su historia no sea elevada, si
			todo esto se cumple probamos a reducir la prof& undidad*/
			if (nuMoves >= 4 && depth >= 2*PLY && !ext && !jaque && !sigue_Pv && (lista[i].valor < (maxhistoria*66/100) || (jugadas[njugadas-1].captura != VACIO && lista[i].valor < 2000000))) {
				if (nuMoves > 20 && depth >= 4*PLY && lista[i].valor < (maxhistoria*16/100))
					puntos = -PVS( -alpha-1, -alpha, depth-4*PLY, VERDADERO);	    
				else if (nuMoves > 12 && depth >= 3*PLY && lista[i].valor < (maxhistoria*33/100))
					puntos = -PVS( -alpha-1, -alpha, depth-3*PLY, VERDADERO);	    
				else
					puntos = -PVS( -alpha-1, -alpha, depth-2*PLY, VERDADERO);	    
			}			
			else
				puntos = alpha+1;       /*Hack to ensure that full-depth search is done*/
			if (puntos > alpha) {
				puntos = -PVS(-alpha-1, -alpha, depth+ext-PLY, VERDADERO);
				if (para_busqueda == FALSO && puntos > alpha && puntos < beta)
					puntos = -PVS(-beta, -alpha, depth+ext-PLY, VERDADERO);
			}
		}

		Deshacer();

		if (para_busqueda == VERDADERO)
			return 0;

		if (puntos > mejor) {
			mejor = puntos;
			mejor_movimiento = lista[i];

			if (mejor > alpha) {
				/*actualizamos la historia*/
				Historia[mejor_movimiento.de][mejor_movimiento.a] += (depth/PLY);
				if (Historia[mejor_movimiento.de][mejor_movimiento.a] > maxhistoria)
					maxhistoria = Historia[mejor_movimiento.de][mejor_movimiento.a];
				if (maxhistoria > 2000000) {
					maxhistoria /=2;
					for (de = 0; de < 64; de++)
						for (a = 0; a < 64; a++)
							Historia[de][a] /= 2;
				}
				if (mejor >= beta) {
					/* Los movimientos matekiller y killer son movimientos buenos, aprovechamos para guardar información de ellos */
					if (puntos > (MATE-64))
						matekiller[ply] = lista[i];		/*un movimiento matekiller, es un movimiento killer (asesino) que es un mate*/
					else if ((pieza[lista[i].a] == VACIO) && lista[i].tipo < PROMOCION_A_DAMA) {
						/*guardamos dos movimientos killer que no sean movimientos de captura o promoción*/
						if ((lista[i].a != killer1[ply].a) || (lista[i].de != killer1[ply].de)) {
							killer2[ply] = killer1[ply];
							killer1[ply] = lista[i];
						}
					}
					/*TTables*/
					store_hash(hasfBETA, depth+njugadas, beta, mejor_movimiento);
					return mejor;	    /*cutoff del alphabeta*/
				}
				alpha = mejor;
				hash_flag = hasfEXACT;
				Actualiza_pv(mejor_movimiento);	    /*actualizamos la variante principal*/
				/*mostrar valoración*/
#if UNDER_CE
				if (!ply && estado == BUSCANDO && li > 5) print_pv(li, puntos);
#else
				if (!ply && nodos > 250000) print_pv(li, puntos);
#endif
			}
		}
	}

	if (nuMoves == 0) {
		if (jaque) mejor = -MATE + ply; /*mate*/
		else mejor = 0; /*ahogado*/
	}
	else if (regla50 >= 100)
		return 0;

	/*TTables*/
	store_hash(hash_flag, depth+njugadas, mejor, mejor_movimiento);
	return mejor;
}


/*extensiones es similar a alphabeta salvo en que solo busca capturas y promociones, no lleva profundidad, esta función
siver para evitar el efecto horizonte*/
int Extiende_capturas(int alpha, int beta)
{
	int i, contar2, delta = 0, puntos = alpha, total_piezas, jaque, legal = 0;
	movimiento lista2[256], hmov = no_move;

	nodos++;
	if ((nodos % cknodos) == 0) 	        /*comprueba cada un cierto número de nodos que tenemos tiempo*/
		checkup(puntos);
	if (para_busqueda == VERDADERO)
		return 0;

	if (repeticion () > 0)
		return 0;

	if (ply >= MAX_PROFUNDIDAD-1)
		return Evaluar(alpha, beta);

	/*comprobamos el acceso a las bitbases*/
	if (egbb_is_loaded) {
		total_piezas = peonesblancos + caballosblancos + alfilesblancos + torresblancas
						+ damasblancas + peonesnegros + caballosnegros + alfilesnegros + torresnegras + damasnegras + 2;
		if (total_piezas <= men) {
			puntos = 0;
			if (probe_bitbases(&puntos) == 1) {
				/* Preferimos ganadas cerca de la raíz */
				if (puntos > 0)
					puntos +=VAL_BITBASE - ply;
				else if (puntos < 0)
					puntos += ply - VAL_BITBASE;
				return puntos;
			}
		}
	}
			
	pv_longuitud[ply] = ply;
	puntos = alpha;

	jaque = EstaEnJaque(turno);		/*comprobamos si estamos en jaque*/

	if (jaque) {
		contar2 = Generar(lista2);	/*aqui deberíamos generar evasiones*/
	}
	else {
		puntos = Evaluar(alpha, beta);
		if (puntos >= beta)
			return beta;
		if (puntos > alpha)
			alpha = puntos;

		/*delta prunning*/
		material = piezasblanco+piezasnegro+peonesblanco+peonesnegro;
		if (material > MEDIOJUEGO)
			delta = alpha - 200 - puntos;
		else if (material > FINAL)
			delta = alpha - 500 - puntos;
		else
			delta = alpha - 900 - puntos;
			
		contar2 = Generar_Capturas(lista2);
	}

	Ordena_hmove_pv(contar2, lista2, hmov);

    for (i = 0; i < contar2; ++i) {
		OrdenaMovimiento(i, contar2, lista2);
		/*si no estamos en jaque y el movimiento no es una promoción*/
		if (!jaque && lista2[i].tipo < PROMOCION_A_DAMA) {
			/* mala captura y delta prunning */
			if (lista2[i].valor == -1 || valor_pieza[pieza[lista2[i].a]] < delta /*|| see(turno, lista2[i].a, lista2[i].de) < 0*/)
				continue;
		}
		if (!Hacer_Movimiento(lista2[i]))
			continue;

		legal++;

		puntos = -Extiende_capturas(-beta, -alpha);
		Deshacer();

		if (para_busqueda == VERDADERO)
			return 0;

		if (puntos >= beta)
			return beta;

		if (puntos > alpha){
			alpha = puntos;
			Actualiza_pv(lista2[i]);
		}
	}
	if (jaque && legal == 0)
		alpha = -MATE + ply;

	return alpha;
}

/*ordena el movimiento hash en primer lugar, luego el movimiento que sigue la variante principal anterior*/
void Ordena_hmove_pv(int cuenta, movimiento *lista, movimiento hmove)
{
	int i;

	if (!sigue_Pv) {
		for (i = 0; i < cuenta; ++i) {
			if (hmove.de == lista[i].de && hmove.a == lista[i].a) {
				lista[i].valor += 20000000;
				return;
			}
		}
	}
	else {
		for(i = 0; i < cuenta; ++i) {
			if (hmove.de == lista[i].de && hmove.a == lista[i].a) {
				lista[i].valor += 20000000;
				break;
			}
		}
		sigue_Pv = FALSO;
		for(i = 0; i < cuenta; ++i) {
			if (lista[i].de == pv[0][ply].de && lista[i].a == pv[0][ply].a) {
				sigue_Pv = VERDADERO;
				lista[i].valor += 10000000;
				break;
			}
		}
	}
}

/*ponemos en primer lugar el movimiento con mayor valor*/
void OrdenaMovimiento(int de, int cuenta, movimiento *lista)
{
	int i, j, Max;
	movimiento Tmp;

	/*cerca del maximo*/
	Max = lista[de].valor;
	j = de;
	for (i = (de+1); i < cuenta; i++) {
		if (lista[i].valor > Max) {
			Max = lista[i].valor;
			j = i;
		}
	}
	if (Max != lista[de].valor) {
		Tmp = lista[de];
		lista[de] = lista[j];
		lista[j] = Tmp;
	}
}

/*actualizamos la variante principal con un movimiento*/
void Actualiza_pv(movimiento pvmov)
{
	int j;

	pv[ply][ply] = pvmov;
	for (j = ply + 1; j < pv_longuitud[ply + 1]; ++j)
		pv[ply][j] = pv[ply + 1][j];
	pv_longuitud[ply] = pv_longuitud[ply + 1];
}

/*comprueba si una posición se repite*/
int repeticion(void)
{
	int i;
	int r = 0;

	for (i = njugadas-regla50; i < njugadas; ++i)
		if (jugadas[i].hash == hash)
			++r;
	return r;
}

void checkup(int puntos)
{
	char linea[256], comando[256];	    /*para leer la información del GUI y saber que comando llega*/
	int tiempo_usado;

	tiempo_usado = get_ms() - tiempo_inicio;

	if (estado == BUSCANDO && aborta) {
		if (bioskey()) {
			para_busqueda = VERDADERO;
			motor = VACIO;
			return;
		}
		if (tiempo_usado >= limite_tiempo) {
			para_busqueda = VERDADERO;
			return;
		}
		if  ((tiempo_usado > no_nuevo_movimiento) && (puntos + MARGEN > valor1[li-1])) {
			para_busqueda = VERDADERO;
			return;
		}
	}
	else if (estado == PONDERANDO) {
		if (bioskey()) {
			para_busqueda = VERDADERO;
			return;
		}
	}
	else if (estado == ANALIZANDO) {
		if (bioskey()) {
#if UNDER_CE
			if (gets(linea) == NULL)
			return;
#else
			if (!fgets(linea, 256, stdin))
			return;
#endif
			sscanf(linea, "%s", comando);
			if (strcmp(comando, ".") == 0) {
				para_busqueda = FALSO;
				return;
			}
			if (strcmp(comando, "exit") == 0) {
				para_busqueda = VERDADERO;
				return;
			}
		}
		if (tiempo_usado >= limite_tiempo) {
			para_busqueda = VERDADERO;
			return;
		}
	}
	return;
}
