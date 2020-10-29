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

typedef enum enum_tecla
{
  ENTER = VK_RETURN,
  ESPACO = VK_SPACE,
  W = 0x57,
  A = 0x41,
  S = 0x53,
  D = 0x44
} TECLA;

#else
/*
  Bibliotecas para entrada assíncrona com um servidor X.
  (como, por exemplo, a maioria das distribuições do Linux.)
*/

#include <ncurses.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#define XCURSES

/*
  Códigos de tecla para o servidor X.
*/

typedef enum enum_tecla
{
  ENTER = XK_Return,
  ESPACO = XK_space,
  W = XK_w,
  A = XK_a,
  S = XK_s,
  D = XK_d
} TECLA;

#endif

/*
  Estrutura que contém o estado das teclas de controle pressionadas
  pelo usuário ou geradas pelo curses.
*/
typedef struct struct_controle
{
  bool cima;
  bool baixo;
  bool confirma;
  bool retorna;
  bool terminalRedimensionado;
} CONTROLE;

/*
  Retorna uma estrutura com os estados das teclas de controle do
  buffer de entrada.
*/
CONTROLE verificarTeclasDeControle();

/*
  Cria uma instância de um terminal.
*/
void inicializarTerminal();

/*
  Fecha o terminal.
*/
void fecharTerminal();

/*
  Cria uma janela na posição desejada e com com o tamanho especificado,
  usando algumas configurações extras.
*/
WINDOW *criarJanela(int altura, int largura, int posy, int posx);

/*
  Retorna true se o tamanho do terminal estiver correto.
*/
bool verificarTamanhoDoTerminal();

/*
  Pede para o usuário redimensionar o terminal se estiver com o tamanho
  incorreto, e retorna true se isso foi necessário.
*/
void corrigirTamanhoDoTerminal();

/*
  Retorna true se $tecla está pressionada nesse momento, sem esperar
  pelo terminal.
*/
bool teclaPressionada(TECLA tecla);

#endif