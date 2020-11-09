#include <TerminalIO.h>
#include <Timer.h>

#include <assert.h>
#include <ctype.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef LINUX
/*
  Conexão com servidor X.
*/
#include <X11/Xlib.h>
Display *xServer = NULL;

#endif

void inicializarTerminal(void)
{
#ifdef LINUX
  // Abre conexão com servidor X principal.
  xServer = XOpenDisplay(NULL);
  if (!xServer)
  {
    perror("Erro conectando ao servidor X");
    exit(EXIT_FAILURE);
  }
#endif
  // Configura locale para chars.
  setlocale(LC_CTYPE, "");
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
  // Habilita cores no terminal, se disponível.
  if (has_colors())
  {
    // Inicializa os pares de cores.
    start_color();
    init_pair(PRETO, COLOR_BLACK, COLOR_BLACK);
    init_pair(AZUL, COLOR_BLUE, COLOR_BLACK);
    init_pair(VERDE, COLOR_GREEN, COLOR_BLACK);
    init_pair(CIANO, COLOR_CYAN, COLOR_BLACK);
    init_pair(VERMELHO, COLOR_RED, COLOR_BLACK);
    init_pair(MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(AMARELO, COLOR_YELLOW, COLOR_BLACK);
    init_pair(BRANCO, COLOR_WHITE, COLOR_BLACK);
  }

#ifdef WINDOWS
  // No PDCurses, tenta redimensionar a tela.
  resize_term(N_LINHAS, N_COLUNAS);
#endif
  // Atualiza a tela.
  refresh();
}

void fecharTerminal(void)
{
  // Fecha janela do curses.
  endwin();
#ifdef LINUX
  // Fecha conexão com servidor X principal.
  assert(xServer);
  XCloseDisplay(xServer);
#endif
}

WINDOW *criarJanela(int altura, int largura, int posy, int posx)
{
  WINDOW *win = subwin(stdscr, altura, largura, posy, posx);
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

bool continuacao(unsigned int ch)
{
  // No UTF-8, um byte que começa com 0x8 (0b10xxxxxx) é
  // continuação de um code point anterior.
  return (ch >> 6) == 0x2;
}

void centralizarString(WINDOW *win, int y, const char *str)
{
  // Conta o número de caracteres, levando em conta que
  // caracteres UTF-8 têm comprimento variável.
  int comprimento = 0;
  int i = 0;
  while (str[i] != '\0')
  {
    if (!continuacao(str[i]))
    {
      comprimento++;
    }
    i++;
  }
  mvwaddstr(win, y, centralizarX(win, comprimento), str);
}

ENTRADA *processarEntrada(ENTRADA *entrada)
{
  // Limpa a struct de entrada.
  memset(entrada, false, sizeof(ENTRADA));

  // Itera sobre a stream de entrada do curses até ela ser exausta.
  int ch;
  while ((ch = getch()) != ERR)
  {
    // Se houver espaço, guarda o char atual na cópia do buffer de
    // entrada.
    if (entrada->tamBuffer < MAX_ENTRADA)
    {
      entrada->buffer[entrada->tamBuffer] = ch;
      entrada->tamBuffer++;
    }

    // A diferença entre maiúsculas e minúsculas (ex.: Q, Shift+Q)
    // não é considerada para caracteres de controle.
    ch = tolower(ch);
    switch (ch)
    {
    // Teclas de controle geradas pelo usuário.
    case KEY_UP:
    case 'w':
      entrada->cima = true;
      break;
    case KEY_DOWN:
    case 's':
      entrada->baixo = true;
      break;
    case KEY_ENTER:
    case '\n':
    case '\r':
      entrada->confirmaSemEspaco = true;
    case ' ':
      entrada->confirma = true;
      break;
    case KEY_BACKSPACE:
    case '\b':
      entrada->retorna = true;
      break;
    case 'p':
      entrada->pause = true;
      break;
    // Teclas de controle geradas pelo curses.
    case KEY_RESIZE:
      entrada->terminalRedimensionado = true;
      break;
    }
  }
  return entrada;
}

bool verificarTamanhoDoTerminal(void)
{
#ifdef WINDOWS
  // No PDCurses, o tamanho deve ser atualizado
  // manualmente após ser alterado pelo usuário.
  resize_term(0, 0);
#endif
  return getmaxy(stdscr) == N_LINHAS && getmaxx(stdscr) == N_COLUNAS;
}

void corrigirTamanhoDoTerminal(void)
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

      // Reesconde o cursor.
      curs_set(0);

      wrefresh(win);
    }

    // Limita a taxa de execução.
    pause(0.05);
  }

  // Deleta a janela.
  destruirJanela(&win);

  // Limpa a tela.
  clear();
  refresh();
}

#ifdef WINDOWS
/*
  Entrada assíncrona para Windows.
*/

#include <windows.h>

bool teclaPressionada(TECLA_ASYNC tecla)
{
  return (bool)GetAsyncKeyState(tecla);
}

#endif

#ifdef LINUX
/*
  Entrada assíncrona para sistemas com o gerenciador de janelas
  X11 (por exemplo, a maioria das distribuições do Linux.)
*/

bool teclaPressionada(TECLA_ASYNC tecla)
{
  assert(xServer);

  // Gera vetor de estado do teclado.
  char kbs[32];
  XQueryKeymap(xServer, kbs);

  // Transforma o valor $tecla em um código de tecla do X11 e checa
  // se o bit correspondente em $kbs está ligado. Os três bits menos
  // significativos indexam um bit dentro de um byte (2^3 = 8),
  // e o resto indexa um byte do vetor $kbs.
  KeyCode kc = XKeysymToKeycode(xServer, tecla);
  bool pressionada = kbs[kc >> 3] & (1 << (kc & 0x7));

  return pressionada;
}

#endif