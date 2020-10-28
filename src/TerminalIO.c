#include <TerminalIO.h>

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

CONTROLE verificarTeclasDeControle()
{
  // Variável de controle inicializada com zeros (ou seja, false.)
  static CONTROLE controleNulo;
  CONTROLE controle = controleNulo;
  // Itera sobre a stream de entrada do curses até ela ser exausta.
  int ch;
  while ((ch = getch()) != ERR)
  {
    ch = tolower(ch);
    switch (ch)
    {
    // Teclas de controle geradas pelo usuário.
    case KEY_UP:
    case 'w':
      controle.cima = true;
      break;
    case KEY_DOWN:
    case 's':
      controle.baixo = true;
      break;
    case KEY_ENTER:
      controle.confirma = true;
      break;
    case KEY_BACKSPACE:
      controle.retorna = true;
      break;
    // Teclas de controle geradas pelo curses.
    case KEY_RESIZE:
      controle.terminalRedimensionado = true;
      break;
    }
  }
  return controle;
}

void inicializarTerminal()
{
  // Inicializa a janela.
  initscr();
  // Impede que teclas digitadas apareçam no terminal automaticamente.
  noecho();
  // Esconde o cursor.
  curs_set(0);
  // Passa as entradas sem esperar por newline.
  cbreak();
  // Permite que a janela receba teclas de função e do teclado.
  keypad(stdscr, true);
  // Desativa a espera por entradas.
  nodelay(stdscr, true);
  // No PDCurses, tenta redimensionar a tela.
#ifdef PDCURSES
  resize_term(N_LINHAS, N_COLUNAS);
#endif
  // Atualiza a tela.
  refresh();
}

WINDOW *criarJanela(int altura, int largura, int posy, int posx)
{
  WINDOW *win = newwin(altura, largura, posy, posx);
  keypad(win, true);
  nodelay(win, true);
  return win;
}

bool verificarTamanhoDoTerminal()
{
  // No PDCurses, o tamanho deve ser atualizado manualmente após
  // ser alterado.
#ifdef PDCURSES
  resize_term(0, 0);
#endif
  return getmaxy(stdscr) == N_LINHAS && getmaxx(stdscr) == N_COLUNAS;
}

void corrigirTamanhoDoTerminal()
{
  // Limpa a tela.
  clear();
  refresh();

  // Configurações da window de aviso.
  const int ALTURA = 5, LARGURA = 32;
  int posy = -1, posx = -1;
  WINDOW *win = NULL;

  while (!verificarTamanhoDoTerminal())
  {
    // Determina o tamanho atual do terminal e a posição da
    // janela dentro dele.
    int yterm = getmaxy(stdscr), xterm = getmaxx(stdscr);
    int nposy = (yterm - ALTURA) / 2, nposx = (xterm - LARGURA) / 2;

    // Se a janela tiver se movido, apaga sua posição antiga e
    // a move.
    if (nposy != posy || nposx != posx)
    {
      // Limpa a tela se a janela for movida e deleta a window.
      // TODO: corrigir flickering na versão de Windows.
      if (win)
      {
        delwin(win);
      }
      clear();
      refresh();

      // Se o terminal tiver o tamanho necessário, cria uma nova
      // window. Uso de 1 em vez de 0 criar um buffer que previne
      // alguns artefatos.
      if (nposy >= 1 && nposx >= 1)
      {
        posy = nposy;
        posx = nposx;
        win = criarJanela(ALTURA, LARGURA, posy, posx);
      }
      // Se não, torna o ponteiro da window nulo.
      else
      {
        posy = -1;
        posx = -1;
        win = NULL;
      }
    }

    // Se houver uma window, mostra informações sobre o estado do
    // terminal nela.
    if (win)
    {
      box(win, 0, 0);
      mvwprintw(win, 1, 2, "Tamanho incorreto da janela");
      mvwprintw(win, 2, 2, "Tamanho esperado: %3d x %3d", N_LINHAS, N_COLUNAS);
      mvwprintw(win, 3, 2, "Tamanho real:     %3d x %3d", yterm, xterm);
      wrefresh(win);
    }
  }

  // Deleta a janela.
  clear();
  refresh();
  delwin(win);
}

#if defined(_WIN32) || defined(WIN32)
/*
  Entrada assíncrona para Windows.
*/

#include <windows.h>

bool teclaPressionada(TECLA tecla)
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

bool teclaPressionada(TECLA tecla)
{
  // Abre conexão com servidor X principal.
  Display *dsp = XOpenDisplay(NULL);
  if (!dsp)
  {
    perror("Erro conectando ao servidor X");
    exit(EXIT_FAILURE);
  }

  // Gera vetor de estado do teclado.
  uint8_t kbs[32];
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
