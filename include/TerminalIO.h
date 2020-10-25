#ifndef TERMINALIO_H
#define TERMINALIO_H

/*
  Define funções de entrada e saída pelo terminal.
*/

#include <stdbool.h>

#if defined(_WIN32) || defined(WIN32)
/*
  Windows usa o PDCurses, uma implementação do domínio público
  do curses.
*/

#include <PDCurses.h>
#include <Panel.h>

/*
  Códigos de tecla pra Windows.
*/

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
  Linux usa o ncurses no lugar do PDCurses.
*/

#include <ncurses.h>

/*
  Códigos de tecla para sistemas com o gerenciador de janelas X11
  (como, por exemplo, a maioria das distribuições do Linux.)
*/

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

// Cria uma instância de um terminal.
void inicializarTerminal();

// Cria uma janela na posição desejada e com com o tamanho especificado.
WINDOW *criarJanela(int posy, int posx, int altura, int largura);

// Retorna true se $tecla está pressionada nesse momento.
bool teclaPressionada(Tecla tecla);

#endif