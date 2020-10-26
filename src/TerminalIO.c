#include <TerminalIO.h>

#include <stdio.h>
#include <stdlib.h>

void inicializarTerminal()
{
  initscr();
  // Impede que teclas digitadas apareçam no terminal automaticamente.
  noecho();
  // Esconde o cursor.
  curs_set(0);
  // Passa as entradas diretamente para o programa.
  nocbreak();
  // Permite que a janela receba teclas de função e do teclado.
  keypad(stdscr, true);
  // Desativa a espera por entradas.
  nodelay(stdscr, true);
}

WINDOW *criarJanela(int altura, int largura, int posy, int posx)
{
  WINDOW *win = newwin(altura, largura, posy, posx);
  keypad(win, true);
  nodelay(win, true);
  return win;
}

bool verificarTamanho()
{
  return getmaxy(stdscr) == N_LINHAS && getmaxx(stdscr) == N_COLUNAS;
}

bool corrigirTamanho()
{
  bool houveResize = false;

  // Se o tamanho do terminal não está correto, mostra uma mensagem de
  // erro até ele estar.
  if (!verificarTamanho())
  {
    houveResize = true;
    clear();

    // Dimensóes e posição da janela.
    const int ALTURA = 5, LARGURA = 32;
    int posy = -1, posx = -1;

    WINDOW *win = NULL;

    do
    {
      // Determina o tamanho atual do terminal e a posição da
      // janela dentro dele.
      int yterm = getmaxy(stdscr), xterm = getmaxx(stdscr);
      int nposy = (yterm - ALTURA) / 2, nposx = (xterm - LARGURA) / 2;

      // Se a janela tiver se movido, apaga sua posição antiga e
      // a move.
      if (nposy != posy || nposx != posx)
      {
        // Limpa a tela se a janela for movida.
        delwin(win);
        clear();
        refresh();
        // Se o terminal tiver o tamanho necessário, cria uma nova
        // janela.
        if (nposy >= 0 && nposx >= 0)
        {
          posy = nposy;
          posx = nposx;
          win = criarJanela(ALTURA, LARGURA, posy, posx);
        }
        // Se não, torna o ponteiro da janela nulo.
        else
        {
          posy = -1;
          posx = -1;
          win = NULL;
        }
      }

      // Se houver uma janela, mostra informações sobre o estado do
      // terminal nela.
      if (win != NULL)
      {
        box(win, 0, 0);
        mvwprintw(win, 1, 2, "Tamanho incorreto da janela");
        mvwprintw(win, 2, 2, "Tamanho esperado: %3d x %3d", N_LINHAS, N_COLUNAS);
        mvwprintw(win, 3, 2, "Tamanho real:     %3d x %3d", yterm, xterm);
        wrefresh(win);
      }
    } while (!verificarTamanho());

    // Deleta a janela.
    clear();
    refresh();
    delwin(win);
  }

  return houveResize;
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
  if (!dsp)
  {
    perror("Erro conectado ao servidor X");
    exit(EXIT_FAILURE);
  }

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