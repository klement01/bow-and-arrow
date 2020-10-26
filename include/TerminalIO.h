#ifndef TERMINALIO_H
#define TERMINALIO_H

/*
  Define funções de entrada e saída pelo terminal.
*/

#include <stdbool.h>

#define N_LINHAS 35
#define N_COLUNAS 80

#if defined(_WIN32) || defined(WIN32)
/*
  Códigos de tecla pra Windows.
*/

#include <PDCurses.h>
#include <Panel.h>

#include <windows.h>

typedef enum
{
  ENTER = VK_RETURN,
  ESPACO = VK_SPACE,
  W = 0x57,
  A = 0x41,
  S = 0x53,
  D = 0x44
} Tecla;

#else
/*
  Códigos de tecla para sistemas com o gerenciador de janelas X11
  (como, por exemplo, a maioria das distribuições do Linux.)
*/

#include <ncurses.h>

#include <X11/keysym.h>

typedef enum
{
  ENTER = XK_Return,
  ESPACO = XK_space,
  W = XK_w,
  A = XK_a,
  S = XK_s,
  D = XK_d
} Tecla;

#endif

/*
  Cria uma instância de um terminal.
*/
void inicializarTerminal();

/*
  Cria uma janela na posição desejada e com com o tamanho especificado,
  usando algumas configurações extras.
*/
WINDOW *criarJanela(int altura, int largura, int posy, int posx);

/*
  Retorna true se o tamanho do terminal estiver correto.
*/
bool verificarTamanho();

/*
  Pede para o usuário redimensionar o terminal se estiver com o tamanho
  incorreto, e retorna true se isso foi necessário.
*/
bool corrigirTamanho();

/*
  Retorna true se $tecla está pressionada nesse momento.
*/
bool teclaPressionada(Tecla tecla);

#endif