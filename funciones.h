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
Funciones que se van a utilizar en el programa
*/

/*principal*/
int __cdecl main(int argc, char *argv[]);

/*leer_archivo_ini*/
void leer_inifile(void);
void imprime_configuracion(int tamano1, int tamano2);

/*xboard*/
void xboard(void);
void Carga_libro(void);
void Carga_bitbases(void);

/*hash*/
void inicia_hash(void);
void poner_hash(void);
void clear_hash(void);
void store_hash(int type, int depth, int score, movimiento hmov);
int probe_hash(int *type, int depth, int beta, movimiento *hmov, int *hacer_nulo);
void store_eval(int score);
int probe_eval(void);
void alloc_hash(void);
void free_hash(void);

/*ataques*/
int	EstaEnJaque(int turno);
int atacado(int k, int turno);
int atacado_nr(int k, int turno);

/*generamovimientos*/
void Generar_Movimiento_PVS(int de, int a, int tipo, movimiento *plista, int *pcuenta);
void Generar_Movimiento_Normal_PVS(int de, int a, movimiento *plista, int *pcuenta);
void Generar_Movimiento_PEON_PVS(int de, int a, movimiento *plista, int *pcuenta);
void Generar_Movimiento_PEON2_PVS(int de, int a, movimiento *plista, int *pcuenta);
void Generar_Movimiento_Enroque_PVS(int de, int a, movimiento *plista, int *pcuenta);
void Generar_Movimiento_Alpaso_PVS(int de, int a, movimiento *plista, int *pcuenta);
void Generar_Movimiento_Capturas(int de, int a, int tipo, movimiento *plista, int *pcuenta);
void Generar_Movimiento_Normal_Capturas(int de, int a, movimiento *plista, int *pcuenta);
void Generar_Movimiento_PEON_Capturas(int de, int a, movimiento *plista, int *pcuenta);
void Generar_Movimiento_PEON2_Capturas(int de, int a, movimiento *plista, int *pcuenta);
void Generar_Movimiento_Enroque_Capturas(int de, int a, movimiento *plista, int *pcuenta);
void Generar_Movimiento_Alpaso_Capturas(int de, int a, movimiento *plista, int *pcuenta);
int	Generar(movimiento *plista);
int	Generar_Capturas(movimiento *plista);
int Hacer_Movimiento(movimiento m);
void Deshacer(void);
int Generar_legales(void);

/*see*/
int malaCaptura(movimiento mcmov);
void calcula_atacantes(int casilla);
int see(int turno, int casilla, int de);

/*busqueda*/
movimiento MotorPiensa(void);
int	PVS(int alpha, int beta, int depth, int hacer_nulo);
int Extiende_capturas(int alpha, int beta);
void Ordena_hmove_pv(int cuenta, movimiento *lista, movimiento hmove);
void OrdenaMovimiento(int de, int cuenta, movimiento *lista);
void Actualiza_pv(movimiento pvmov);
int repeticion(void);
void checkup(int puntos);

/*evalua*/
int Evaluar(int alpha, int beta);
void Calcula_material(void);
void Evalua_material(void);
void Busca_peones(void);
int rey_blanco(int sq);
int bpf(int f);
int rey_negro(int sq);
int npf(int f);
int peones_blancos_mediojuego(int i);
int peones_blancos_final(int i);
int peones_negros_mediojuego(int i);
int peones_negros_final(int i);
void Evalua_posicion_final(void);
void Evalua_posicion_mediojuego(void);
void Evalua_posicion_mediojuego_final(void);
void Evalua_peones_bloqueados(void);
void Evalua_alfiles_atrapados(void);
void Cambios_mediojuego(void);
void Cambios_final(void);
void Evalua_seguridad_rey();
int Movilidad_Caballo(int pos);
int Movilidad_Alfil(int pos);
int Movilidad_Torre(int pos);
int Movilidad_Dama(int pos);
int reconoce_tablas(void);
int reclama_tablas_material(void);

/*entradas*/
int Comprobar_Tipo_Movimiento(int de, int a, char *s);
void Comprobar_Movimiento(char *s);
int bioskey(void);
int get_ms(void);

/*salidas*/
void print_pv(int depth, int score);
void Enviar_Movimiento(movimiento m);
void imprime_resultado(int cuenta, movimiento *lista);

/*aleatorio (números)*/
U32 Rand32(void);
U64 Rand64(void);

/*fen*/
void SetBoard(char *string);
void Tablero_FEN(char *string);

/*libro*/
void MAKE_HASHKEY(void);
void BOEKIN(void);
void BOEK_BEGIN(void);
int INITIALIZE(const char *Toerfile,const char *Bookfile);
int FIND_OPENING(const char *EPD,char *From,char *To,char *From1,char *From2,char *To1,char *To2,int *bookmvs);
void CLOSE_BOOK(void);

/*bitbases*/
int LoadEgbbLibrary(char* main_path, int egbb_cache_size);
void unload_bitbases(void);
int probe_bitbases(int *score);

/*test*/
U64 perft(int depth);
void Test(void);
void bench(void);

#if UNDER_CE
    #include <stdarg.h>
    int __cdecl PrintF (const char *fmt, ...);
    int GetCh (void);
    typedef struct
    {
        int version;
        void *pIOInst;
        int (__stdcall *p_input_available) (void *pInst);
        char * (__stdcall *p_get_string) (void *pInst, char *s, int nmaxlen);
        void (__stdcall *p_put_string) (void *pInst, const char *s);
        int (__cdecl *p_print) (void *pInst, const char *fmt, ...);
        int (__cdecl *p_vprint) (void *pInst, const char *fmt, va_list args);
        unsigned int (__stdcall *p_available_memory) (void *pInst);
        void * (__stdcall *p_allocate_memory) (void *pInst, int nsize);
        void (__stdcall *p_free_memory) (void *pInst, void *pmem);
        int argc;
        char **argv;
    }
    TEngineIO;
    extern TEngineIO *EngineIO;
    #define malloc(n_) (EngineIO->p_allocate_memory(EngineIO->pIOInst,n_))
    #define free(p_) (EngineIO->p_free_memory(EngineIO->pIOInst,p_))
    #define printf PrintF
    #define puts(s_) printf("%s\n",s_)
    #define getch() GetCh()
    #define gets(s_) (EngineIO->p_get_string (EngineIO->pIOInst, s_, 255))
    #define fflush(f_)
    #define InputAvailable() (EngineIO->p_input_available(EngineIO->pIOInst))
#endif
