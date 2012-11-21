DanaSah es gratuito, es un motor de ajedrez compatible con el protocolo Winboard, escrito en lenguaje C, funciona con interface o GUIs como Arena, Winboard (reclama tablas y movimiento legal), Fritz mediante el convertidor wb2uci, Chessmater ...
Su nombre viene de dos palabras, Dana de Daniela y Sah que significa ajedrez en rumano.

Pedro Castro (pecastro@msn.com)

Pais Vasco. Spain.

****************************************************************************************

Para hacer funcionar a DanaSah necesitas instalar primero el motor en la GUI:
- Winboard: en el archivo winboard.ini -> /firstChessProgramNames={DanaSahxxx   "danasahXXX.exe xboard"
- ARENA: Motor -> Nuevo Motor -> Winboard -> danasahXXX.exe.
- ChessPartner: Extra -> Engine Import Wizard -> Winboard engines -> Install  -> danasahXXX.exe.
- Chessmaster: Game -> Import Winboard Engine -> danasahXXX.exe.
- Chessbase: Engine -> New UCI engine -> Browse -> danasahXXX_UCI.exe (Wb2uci).

Archivos:
- danasahXXX.exe -->ejecutable para Arena, Winboard ... (protocolo winboard).
- danasah.txt -- > archivo de configuracion para danasah (winboard y uci).
- mainbook.bin --> el gran libro principal.
- tourbook.bin --> seleccion de lineas favoritas de aperturas llamado libro de torneo.
- random1.bin --> necesario para la operacion de los libros.
- random2.bin --> necesario para la operacion de los libros.
- logo.bmp --> Logo de Jim Ablett para Fritz.

Hechos:
- Libro de aperturas de Prodeo/Rebel. Gracias a Ed Shroeder y Jeroen Noomen.
- Bitbases de Scorpio. Gracias a Daniel Shawul.
- Representacion del tablero: cadena de 64 elementos, basado en Firstchess (Pham Hong Nguyen).
- Busqueda de profundidad iterativa, con ventanas de valor 15 y 150.
- Funciones de busqueda PVS y horizonte (quiesce). Gracias a Bruce Moreland por su pagina web.
- Tablas de transposicion. Gracias a Miguel Izquierdo (Popochin).
- Movimiento nulo con valor de R entre 2 y 5.
- Profundidad iterativa interna (IID) para ordenacion de movimientos.
- Extensiones de busqueda (jaque, singular, peon en septima, recaptura, final de peones).
- Reduccion de los ultimos movimientos (LMR).
- Razoring.
- Modo editar (setboard), Mover ahora,  Ponder (pensamiento en tiempo del contrario) y - Analizar.
- Ordenacion de movmientos (hash, sigue_pv, capturas, promociones, movimientos killer, heuristic historia).
- Evaluacion rapida de solo material y evaluacion con tablas de piezas/casillas, movilidad,  seguridad del rey ...
 

Por hacer:

- Abandonar.
- Aprendizaje.