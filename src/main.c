#include <Objeto.h>
#include <Grafico.h>
#include <TerminalIO.h>
#include <Timer.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#define N_LINHAS_TITULO 14
#define N_LINHAS_MENU (N_LINHAS - N_LINHAS_TITULO + 1)
#define N_LINHAS_CABECALHO 4
#define N_LINHAS_JOGO (N_LINHAS - N_LINHAS_CABECALHO + 1)

// Define os estados em que o jogo pode estar.
typedef enum enum_estado
{
  FIM,
  MENU,
  PLACAR,
  JOGO,
  GAMEOVER,
  GAMEOVER_HIGHSCORE
} ESTADO;

ESTADO atualizarMenu(CONTROLE *controle, double dt, bool trocaDeEstado);
ESTADO atualizarPlacar(CONTROLE *controle, double dt, bool trocaDeEstado);
ESTADO atualizarJogo(CONTROLE *controle, double dt, bool trocaDeEstado);
void carregarMateriais();
void descarregarMateriais();

int main()
{
  /*
    Setup: carrega todos os recursos e configura os dados
    dos objetos (jogador, flechas, balões, etc.)
  */

  // Define o estado inicial do jogo.
  ESTADO estado = MENU;
  ESTADO ultimoEstado = FIM;

  // Entra no modo curses.
  inicializarTerminal();

  // Carrega os materiais do jogo (gráficos, janelas, etc.)
  carregarMateriais();

  // Garante que o tamanho do terminal esteja correto.
  corrigirTamanhoDoTerminal();

  // Inicializa o timer.
  reiniciarTimer();

  /*
    Loop: executa continuamente o código principal do jogo,
    realizando alterações a cada tick de processamento.
  */

  while (estado != FIM)
  {
    // Coleta caracteres de controle geradas pelo usuário e pelo curses
    // durante o último frame.
    CONTROLE controle = verificarTeclasDeControle();

    // Corrige o tamanho do terminal se ele tiver mudado.
    if (controle.terminalRedimensionado)
    {
      corrigirTamanhoDoTerminal();
      reiniciarTimer();
    }

    // Checha se houve uma troca de estado desde o último quadro.
    bool trocaDeEstado = estado != ultimoEstado;
    ultimoEstado = estado;

    // Calcula o tempo transcorrido desde o último loop.
    double dt = calcularDeltaTempo();

    // Atualiza o jogo de acordo com o estado atual.
    switch (estado)
    {
    case MENU:
      estado = atualizarMenu(&controle, dt, trocaDeEstado);
      break;
    }

    // Atualiza a tela.
    doupdate();
  }

  /*
    Cleanup: libera qualquer memória que ainda não tenha
    sido liberada antes do fim do programa. Não é realmente
    necessário, pois o SO faz isso automaticamente, mas
    liberar manualmente facilita o debugging com o Valgrind.
  */

  // Descarrega os materiais.
  descarregarMateriais();

  // Sai do modo curses.
  fecharTerminal();

  // Fim do programa.
  return EXIT_SUCCESS;
}

// Menu principal.
struct
{
  GRAFICO gTitulo;
  GRAFICO gOpcoes;
  WINDOW *wTitulo;
  WINDOW *wOpcoes;
} menu;

ESTADO atualizarMenu(CONTROLE *controle, double dt, bool trocaDeEstado)
{
  // O estado para o qual o jogo mudará quando a função retornar.
  ESTADO estado = MENU;

  // O estado selecionado pelo cursor.
  static ESTADO selecao = JOGO;

  // A posição e passo do cursor.
  const int NUM_OPCOES = 3;
  const int OFFSET_X = 2;
  const int PASSO_Y = 1;
  int origem_y = (getmaxy(menu.wOpcoes) - menu.gOpcoes.numLinhas) / 2;
  int origem_x = (getmaxx(menu.wOpcoes) - menu.gOpcoes.numColunas) / 2;

  // Processa os controler do usuário, seguindo a hierarquia de prioridade
  // cima > baixo > retorna > confirma. Primeiro, altera a seleção de
  // acordo com a tecla de deslocamento pressionada.
  if (controle->cima)
  {
    if (selecao == PLACAR)
    {
      selecao = JOGO;
    }
    else if (selecao == FIM)
    {
      selecao = PLACAR;
    }
  }
  else if (controle->baixo)
  {
    if (selecao == JOGO)
    {
      selecao = PLACAR;
    }
    else if (selecao == PLACAR)
    {
      selecao = FIM;
    }
  }
  // Se o usuário tenta retornar, seleciona o fim. Se já tiver sido
  // selecionado, fecha o jogo.
  else if (controle->retorna)
  {
    if (selecao == FIM)
    {
      estado = FIM;
    }
    else
    {
      selecao = FIM;
    }
  }
  // Se o usuário confirmou a seleção, muda o estado para essa seleção.
  else if (controle->confirma)
  {
    estado = selecao;
  }

  // Limpa as janelas.
  werase(menu.wTitulo);
  werase(menu.wOpcoes);

  // Redesenha o título e as opções.
  desenharGrafico(&menu.gTitulo, menu.wTitulo, CENTRO, CENTRO);
  box(menu.wTitulo, 0, 0);
  desenharGrafico(&menu.gOpcoes, menu.wOpcoes, CENTRO, CENTRO);
  wborder(menu.wOpcoes, 0, 0, 0, 0, ACS_LTEE, ACS_RTEE, 0, 0);

  // Desenha o cursor de seleção em sua nova posição.
  int offset_y;
  switch (selecao)
  {
  case JOGO:
    offset_y = 0 * PASSO_Y;
    break;
  case PLACAR:
    offset_y = 1 * PASSO_Y;
    break;
  case FIM:
    offset_y = 2 * PASSO_Y;
    break;
  }
  mvwprintw(menu.wOpcoes, origem_y + offset_y, origem_x + OFFSET_X, "-->");

  // Adiciona as janelas à fila de janelas a serem atualizadas
  // no fim do quadro.
  wnoutrefresh(menu.wTitulo);
  wnoutrefresh(menu.wOpcoes);

  return estado;
}

void carregarMateriais()
{
  // Menu principal.
  menu.gTitulo = carregarGrafico("materiais/titulo.txt");
  menu.gOpcoes = carregarGrafico("materiais/menu.txt");
  menu.wTitulo = criarJanela(N_LINHAS_TITULO, N_COLUNAS, 0, 0);
  menu.wOpcoes = criarJanela(N_LINHAS_MENU, N_COLUNAS, N_LINHAS_TITULO - 1, 0);
}

void descarregarMateriais()
{
  // Menu principal.
  delwin(menu.wTitulo);
  delwin(menu.wOpcoes);
}