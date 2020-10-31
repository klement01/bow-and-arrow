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

// Número de chars que são guardados durante uma entrada.
#define MAX_ENTRADA 8

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

typedef enum tecla_async
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

#include <X11/keysym.h>

typedef enum tecla_async
{
  W = XK_w,
  A = XK_a,
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
  bool retorna;
  bool espaco;
  bool terminalRedimensionado;
} ENTRADA;

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
  Desenha uma string no centro da linha especificada.
*/
void centralizarString(WINDOW *win, int y, char *str);

/*
  Retorna uma estrutura com os estados das teclas de controle do
  buffer de entrada, e retorna quantos caracteres podem ser lidos
  do buffer de entrada.
*/
ENTRADA processarEntrada();

/*
  Retorna true se o tamanho do terminal estiver correto.*/
bool verificarTamanhoDoTerminal();

/*
  Pede para o usuário redimensionar o terminal se estiver com o tamanho
  incorreto, e retorna true se isso foi necessário.
*/
void corrigirTamanhoDoTerminal();

/*
  Retorna true se $tecla está pressionada nesse momento, sem esperar
  pelo terminal. Usada quando a entrada pelo curses não é adequada.
*/
bool teclaPressionada(TECLA_ASYNC tecla);

#endif