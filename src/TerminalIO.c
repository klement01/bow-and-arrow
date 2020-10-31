#include <TerminalIO.h>

#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef XCURSES
/*
  Conexão com servidor X.
*/
#include <X11/Xlib.h>
Display *xServer = NULL;

#endif

void inicializarTerminal()
{
#ifdef XCURSES
  // Abre conexão com servidor X principal.
  xServer = XOpenDisplay(NULL);
  if (!xServer)
  {
    perror("Erro conectando ao servidor X");
    exit(EXIT_FAILURE);
  }
#endif
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

void fecharTerminal()
{
  // Fecha janela do curses.
  endwin();
#ifdef XCURSES
  // Fecha conexão com servidor X principal.
  assert(xServer);
  XCloseDisplay(xServer);
#endif
}

WINDOW *criarJanela(int altura, int largura, int posy, int posx)
{
  WINDOW *win = newwin(altura, largura, posy, posx);
  if (!win)
  {
    perror("Erro criando janela");
    exit(EXIT_FAILURE);
  }
  keypad(win, true);
  nodelay(win, true);
  return win;
}

void destruirJanela(WINDOW **win)
{
  if (*win)
  {
    delwin(*win);
    *win = NULL;
  }
}

int centroY(WINDOW *win)
{
  return getmaxy(win) / 2;
}

int centroX(WINDOW *win)
{
  return getmaxx(win) / 2;
}

int centralizarY(WINDOW *win, int altura)
{
  return (getmaxy(win) - altura) / 2;
}

int centralizarX(WINDOW *win, int largura)
{
  return (getmaxx(win) - largura) / 2;
}

void centralizarString(WINDOW *win, int y, char *str)
{
  int comprimento = strlen(str);
  mvwaddstr(win, y, centralizarX(win, comprimento), str);
}

ENTRADA processarEntrada()
{
  // Variável de entrada inicializada com zeros.
  ENTRADA entrada = {0};

  // Itera sobre a stream de entrada do curses até ela ser exausta.
  int ch;
  while ((ch = getch()) != ERR)
  {
    // Se houver espaço, guarda o char atual na cópia do buffer de
    // entrada.
    if (entrada.tamBuffer < MAX_ENTRADA && (ch & 0xFF))
    {
      entrada.buffer[entrada.tamBuffer] = (ch & 0xFF);
      entrada.tamBuffer++;
    }

    // A diferença entre maiúsculas e minúsculas (ex.: Q, Shift+Q)
    // não é considerada para caracteres de controle.
    ch = tolower(ch);
    switch (ch)
    {
    // Teclas de controle geradas pelo usuário.
    case KEY_UP:
    case 'w':
      entrada.cima = true;
      break;
    case KEY_DOWN:
    case 's':
      entrada.baixo = true;
      break;
    case KEY_ENTER:
    case '\n':
    case '\r':
      entrada.confirma = true;
      break;
    case KEY_BACKSPACE:
    case '\b':
      entrada.retorna = true;
      break;
    case ' ':
      entrada.espaco = true;
      break;
    // Teclas de controle geradas pelo curses.
    case KEY_RESIZE:
      entrada.terminalRedimensionado = true;
      break;
    }
  }

  return entrada;
}

bool verificarTamanhoDoTerminal()
{
#ifdef PDCURSES
  // No PDCurses, o tamanho deve ser atualizado
  // manualmente após ser alterado pelo usuário.
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
  int ultimaOrigemY = -1, ultimaOrigemX = -1;
  WINDOW *win = NULL;

  while (!verificarTamanhoDoTerminal())
  {
    // Calcula origem da janela de mensagem.
    int origemY = centralizarY(stdscr, ALTURA);
    int origemX = centralizarX(stdscr, LARGURA);

    // Se a origem tiver mudado, destrói a janela e limpa a tela.
    if (origemY != ultimaOrigemY || origemX != ultimaOrigemX)
    {
      destruirJanela(&win);
      clear();
      refresh();
    }
    ultimaOrigemY = origemY;
    ultimaOrigemX = origemX;

    // Se o tamanho da janela for suficiente, mostra informações nela.
    if (origemY >= 1 && origemX >= 1)
    {
      // Cria uma janela se ela não existir.
      if (!win)
      {
        win = criarJanela(ALTURA, LARGURA, origemY, origemX);
      }

      // Mostra a mensagem.
      wmove(win, 1, 2);
      wprintw(win, "Tamanho incorreto da janela");

      wmove(win, 2, 2);
      wprintw(win, "Tamanho esperado: %3d x %3d", N_COLUNAS, N_LINHAS);

      wmove(win, 3, 2);
      wprintw(win, "Tamanho real:     %3d x %3d", getmaxx(stdscr), getmaxy(stdscr));

      box(win, 0, 0);

      wrefresh(win);
    }
  }

  // Deleta a janela.
  destruirJanela(&win);

  // Limpa a tela.
  clear();
  refresh();

  // Reesconde o cursor.
  curs_set(0);
}

#if defined(_WIN32) || defined(WIN32)
/*
  Entrada assíncrona para Windows.
*/

#include <windows.h>

bool teclaPressionada(TECLA_ASYNC tecla)
{
  return (bool)GetAsyncKeyState(tecla);
}

#elif defined(XCURSES)
/*
  Entrada assíncrona para sistemas com o gerenciador de janelas
  X11 (por exemplo, a maioria das distribuições do Linux.)
*/

bool teclaPressionada(TECLA_ASYNC tecla)
{
  assert(xServer);

  // Gera vetor de estado do teclado.
  uint8_t kbs[32];
  XQueryKeymap(xServer, kbs);

  // Transforma o valor $tecla em um código de tecla do X11 e checa
  // se o bit correspondente em $kbs está ligado. Os três bits menos
  // significativos indexam um bit dentro de um byte (2^3 = 8),
  // e o resto indexa um byte do vetor $kbs.
  KeyCode kc = XKeysymToKeycode(xServer, tecla);
  bool pressionada = kbs[kc >> 3] & (1 << (kc & 0b111));

  return pressionada;
}

#else
#error Nenhum método de entrada assíncrona definido

#endif