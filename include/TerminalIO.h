#ifndef TERMINALIO_H
#define TERMINALIO_H

/*
  Define funções de entrada e saída pelo terminal.
*/

#include <Configs.h>
#include <stdbool.h>

// Dimensões do terminal inteiro.
#define N_LINHAS 35
#define N_COLUNAS 80

// Número de chars que são guardados durante uma entrada.
#define MAX_ENTRADA 16

#ifdef WINDOWS
/*
  Windows usa PDCurses como biblioteca do curses.
*/

#define PDC_DLL_BUILD
#define PDC_WIDE
#define PDC_FORCE_UTF8

#include <PDCurses.h>
#include <Panel.h>

/*
  Códigos de tecla para entrada assíncrona no Windows. Usadas quando
  a leitura com o curses não é adequada.
*/

#include <windows.h>

typedef enum tecla_async
{
  W = 0x57,
  A = 0x41,
  S = 0x53,
  D = 0x44,
  ESPACO = VK_SPACE
} TECLA_ASYNC;

#endif

#ifdef LINUX
/*
  Linux usa o ncurses como biblioteca de curses.
*/

#include <ncurses.h>

/*
  Códigos de tecla para leitura assíncrona com o servidor X. Usadas
  quando a leitura com o curses não é adequada.
*/

#include <X11/keysym.h>

typedef enum tecla_async
{
  W = XK_w,
  A = XK_a,
  S = XK_s,
  D = XK_d,
  ESPACO = XK_space
} TECLA_ASYNC;

#endif

/*
  Estrutura que contém o estado das teclas de controle pressionadas
  pelo usuário ou geradas pelo curses durante um quadro, além de um
  cópia dos primeiros $MAX_ENTRADA chars do buffer de entrada.
*/
typedef struct entrada
{
  // Cópia do buffer de entrada.
  int buffer[MAX_ENTRADA];
  int tamBuffer;
  // Eventos processados.
  bool cima;
  bool baixo;
  bool confirma;
  bool confirmaSemEspaco;
  bool retorna;
  bool pause;
  bool terminalRedimensionado;
} ENTRADA;

/*
  Nomes dos pares de cores.
*/
typedef enum enum_cores
{
  PRETO = 1,
  AZUL,
  VERDE,
  CIANO,
  VERMELHO,
  MAGENTA,
  AMARELO,
  BRANCO
} PARES_DE_CORES;

/*
  Cria uma instância de um terminal.
*/
void inicializarTerminal(void);

/*
  Fecha o terminal.
*/
void fecharTerminal(void);

/*
  Cria uma janela na posição desejada e com com o tamanho especificado,
  usando algumas configurações extras.
*/
WINDOW *criarJanela(int altura, int largura, int posy, int posx);

/*
  Destrói uma janela, se ela existir.
*/
void destruirJanela(WINDOW **win);

/*
  Retorna a coordenada do centro Y de $win.
*/
int centroY(WINDOW *win);

/*
  Retorna a coordenada do centro X de $win.
*/
int centroX(WINDOW *win);

/*
  Retorna a coordenada Y inicial de algo com $altura centralizado
  no eixo Y em $win.
*/
int centralizarY(WINDOW *win, int altura);

/*
  Retorna a coordenada X inicial de algo com $largura centralizado
  no eixo X em $win.
*/
int centralizarX(WINDOW *win, int largura);

/*
  Retorna true se $ch for a continuacao de um byte anterior (UTF-8.)
*/
bool continuacao(unsigned int ch);

/*
  Desenha uma string no centro da linha especificada.
*/
void centralizarString(WINDOW *win, int y, const char *str);

/*
  Retorna uma estrutura com os estados das teclas de controle do
  buffer de entrada, e retorna quantos caracteres podem ser lidos
  do buffer de entrada.
*/
ENTRADA *processarEntrada(ENTRADA *entrada);

/*
  Retorna true se o tamanho do terminal estiver correto.*/
bool verificarTamanhoDoTerminal(void);

/*
  Pede para o usuário redimensionar o terminal se estiver com o tamanho
  incorreto, e retorna true se isso foi necessário.
*/
void corrigirTamanhoDoTerminal(void);

/*
  Retorna true se $tecla está pressionada nesse momento, sem esperar
  pelo terminal. Usada quando a entrada pelo curses não é adequada.
*/
bool teclaPressionada(TECLA_ASYNC tecla);

#endif