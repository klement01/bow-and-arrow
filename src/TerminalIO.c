#include <TerminalIO.h>

#define N_LINHAS 35
#define N_COLUNAS 80

void inicializarTerminal()
{
  initscr();
  // Muda o tamanho do terminal para o tamanho especificado pelo
  // enunciado do trabalho final.
  resize_term(N_LINHAS, N_COLUNAS);
  // Impede que teclas digitadas apareçam no terminal automaticamente.
  noecho();
  // Esconde o cursor.
  curs_set(0);
  // Passa as entradas diretamente para o programa.
  raw();
  // Permite que a janela receba teclas de função e do teclado.
  keypad(stdscr, true);
  // Desativa a espera por entradas.
  nodelay(stdscr, true);
  // Limpa o terminal.
  refresh();
}

WINDOW *criarJanela(int altura, int largura, int posy, int posx)
{
  WINDOW *win = newwin(altura, largura, posy, posx);
  keypad(win, true);
  nodelay(win, true);
  return win;
}

#if defined(_WIN32) || defined(WIN32)
/*
  Entrada assíncrona para Windows.
*/

#include <windows.h>

bool teclaPressionada(Tecla tecla)
{
  return (bool)GetAsyncKeyState(tecla);
}

#else
/*
  Entrada assíncrona para sistemas com o gerenciador de janelas
  X11 (por exemplo, a maioria das distribuições do Linux.)
*/

#include <X11/Xlib.h>
#include <X11/keysym.h>

bool teclaPressionada(Tecla tecla)
{
  // Abre conexão com servidor X principal.
  Display *dsp = XOpenDisplay(NULL);

  // Gera vetor de estado do teclado.
  char kbs[32];
  XQueryKeymap(dsp, kbs);

  // Transforma o valor $tecla em um código de tecla do X11 e checa
  // se o bit correspondente em $kbs está ligado. Os três bits menos
  // significativos indexam um bit dentro de um byte (2^3 = 8),
  // e o resto indexa um byte do vetor $kbs.
  KeyCode kc = XKeysymToKeycode(dsp, tecla);
  bool pressionada = kbs[kc >> 3] & (1 << (kc & 0b111));

  // Fecha a conexão com o servidor e retorna.
  XCloseDisplay(dsp);
  return pressionada;
}

#endif