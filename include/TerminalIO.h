#ifndef TERMINALIO_H
#define TERMINALIO_H

/*
  Define funções de entrada e saída pelo terminal.
*/

#include <stdbool.h>

// Dimensões do terminal inteiro.
#define N_LINHAS 35
#define N_COLUNAS 80

// Tamanho das subjanelas para os estados que as utilizam.
#define N_LINHAS_TITULO 14
#define N_LINHAS_MENU (N_LINHAS - N_LINHAS_TITULO + 1)
#define N_LINHAS_CABECALHO 4
#define N_LINHAS_JOGO (N_LINHAS - N_LINHAS_CABECALHO + 1)

#if defined(_WIN32) || defined(WIN32)
/*
  Windows usa PDCurses como biblioteca do curses.
*/

#include <PDCurses.h>
#include <Panel.h>

/*
  Códigos de tecla para entrada assíncrona no Windows. Usadas quando
  a leitura com o curses não é adequada.
*/

#include <windows.h>

typedef enum enum_tecla_async
{
  W = 0x57,
  A = 0x41,
} TECLA_ASYNC;

#else
/*
  Linux usa o ncurses como biblioteca de curses.
*/

#include <ncurses.h>

#define XCURSES

/*
  Códigos de tecla para leitura assíncrona com o servidor X. Usadas
  quando a leitura com o curses não é adequada.
*/

#include <X11/Xlib.h>
#include <X11/keysym.h>

typedef enum enum_tecla_async
{
  W = XK_w,
  A = XK_a,
} TECLA_ASYNC;

#endif

/*
  Estrutura que contém o estado das teclas de controle pressionadas
  pelo usuário ou geradas pelo curses durante um quadro.
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
bool teclaPressionada(TECLA_ASYNC tecla);

#endif