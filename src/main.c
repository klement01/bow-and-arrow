#include <Grafico.h>
#include <Jogo.h>
#include <Placar.h>
#include <TerminalIO.h>
#include <Timer.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

// FPS que o jogo tentará manter, limitando a taxa de execução.
#define MAX_FPS 100

// Tamanho das subjanelas do menu principal.
#define N_LINHAS_TITULO 15
#define N_LINHAS_MENU (N_LINHAS - N_LINHAS_TITULO + 1)

// Define os estados em que o jogo pode estar.
typedef enum estado
{
  FIM,
  MENU,
  PLACAR,
  JOGO,
  GAMEOVER,
} ESTADO;

ESTADO atualizarMenu(ENTRADA *entrada, bool trocaDeEstado, double dt);
ESTADO atualizarPlacar(ENTRADA *entrada, bool trocaDeEstado, double dt);
ESTADO atualizarJogo(ENTRADA *entrada, bool trocaDeEstado, double dt);
ESTADO atualizarGameover(ENTRADA *entrada, bool trocaDeEstado, double dt);
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

  // Entra no modo curses e configura a função de fechar terminal
  // para ser executada na saída do programa.
  inicializarTerminal();
  atexit(&fecharTerminal);

  // Carrega os materiais do jogo (gráficos, janelas, etc.) e
  // configura a função de descarregar materiais para ser executada
  // na saída do programa.
  carregarMateriais();
  atexit(&descarregarMateriais);

  // Garante que o tamanho do terminal esteja correto.
  corrigirTamanhoDoTerminal();

  // Inicializa o timer.
  iniciarTimer();

  /*
    Loop: executa continuamente o código principal do jogo,
    realizando alterações a cada tick de processamento.
  */

  while (estado != FIM)
  {
    // Calcula o tempo transcorrido durante o último quadro e começa
    // a contagem do quadro atual.
    double dt = timerAtual();
    iniciarTimer();

    // Coleta uma cópia do buffer de entrada no início do quadro
    // e o estado de diversos eventos de controle.
    ENTRADA entrada = processarEntrada();

    // Corrige o tamanho do terminal se ele tiver mudado.
    if (entrada.terminalRedimensionado)
    {
      corrigirTamanhoDoTerminal();
    }

    // Checha se houve uma troca de estado desde o último quadro.
    bool trocaDeEstado = estado != ultimoEstado;
    ultimoEstado = estado;

    // Atualiza o jogo de acordo com o estado atual.
    switch (estado)
    {
    case MENU:
      estado = atualizarMenu(&entrada, trocaDeEstado, dt);
      break;
    case PLACAR:
      estado = atualizarPlacar(&entrada, trocaDeEstado, dt);
      break;
    case JOGO:
      estado = atualizarJogo(&entrada, trocaDeEstado, dt);
      break;
    case GAMEOVER:
      estado = atualizarGameover(&entrada, trocaDeEstado, dt);
      break;
    }

    // Reesconde o cursor.
    curs_set(0);

    // Atualiza a tela.
    doupdate();

    // Determina o tempo transcorrido durante o quadro atual e pausa
    // o programa para limitar o FPS, se necessário.
    dt = (1.0 / MAX_FPS) - timerAtual();
    if (dt > 0)
    {
      pause(dt);
    }
  }

  // Fim do programa.
  return EXIT_SUCCESS;
}

/***
 *      __  __                        
 *     |  \/  |                       
 *     | \  / |   ___   _ __    _   _ 
 *     | |\/| |  / _ \ | '_ \  | | | |
 *     | |  | | |  __/ | | | | | |_| |
 *     |_|  |_|  \___| |_| |_|  \__,_|
 *                                    
 *                                    
 */

struct
{
  GRAFICO gTitulo;
  WINDOW *wTitulo;
  WINDOW *wOpcoes;
} menu;

ESTADO atualizarMenu(ENTRADA *entrada, bool trocaDeEstado, double dt)
{
  // O estado para o qual o jogo mudará quando a função retornar.
  ESTADO estado = MENU;

  // O estado selecionado pelo cursor.
  static ESTADO selecao = JOGO;

  // Regenera as janelas após redimensionamento / troca de estado.
  if (entrada->terminalRedimensionado || trocaDeEstado)
  {
    destruirJanela(&menu.wTitulo);
    destruirJanela(&menu.wOpcoes);
  }
  if (!menu.wTitulo)
  {
    menu.wTitulo = criarJanela(N_LINHAS_TITULO, N_COLUNAS, 0, 0);
    wattr_on(menu.wTitulo, A_BOLD, NULL);
  }
  if (!menu.wOpcoes)
  {
    menu.wOpcoes = criarJanela(N_LINHAS_MENU, N_COLUNAS, N_LINHAS_TITULO - 1, 0);
  }

  // Processa os controles do usuário, seguindo a hierarquia de prioridade
  // cima > baixo > retorna > confirma. Primeiro, altera a seleção de
  // acordo com a tecla de deslocamento pressionada.
  if (entrada->cima)
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
  else if (entrada->baixo)
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
  // Se o usuário tenta retornar, seleciona o fim.
  else if (entrada->retorna)
  {
    selecao = FIM;
  }
  // Se o usuário confirmou a seleção, muda o estado para essa seleção.
  else if (entrada->confirma)
  {
    estado = selecao;
  }

  // Limpa as janelas.
  werase(menu.wTitulo);
  werase(menu.wOpcoes);

  // Calcula as coordenadas do texto do menu.
  const int ALTURA = 3;   // n° opções.
  const int LARGURA = 16; // n° colunas da opção mais longa.
  int origemY = centralizarY(menu.wOpcoes, ALTURA);
  int origemX = centralizarX(menu.wOpcoes, LARGURA);

  int jogoY = origemY;
  int placarY = origemY + 1;
  int sairY = origemY + 2;

  // Determina a posição da flecha de seleção na tela e os atributos
  // dos textos de opção.
  attr_t jogoAttr = 0;
  attr_t placarAttr = 0;
  attr_t sairAttr = 0;
  attr_t selecionadoAttr = A_BOLD;
  int flechaY;
  int flechaX = origemX - 2;
  switch (selecao)
  {
  case JOGO:
    flechaY = jogoY;
    jogoAttr = selecionadoAttr;
    break;
  case PLACAR:
    flechaY = placarY;
    placarAttr = selecionadoAttr;
    break;
  case FIM:
    flechaY = sairY;
    sairAttr = selecionadoAttr;
    break;
  }

  // Desenha o título, as opções e a flecha de seleção.
  desenharGrafico(menu.gTitulo, menu.wTitulo, CENTRO, CENTRO);
  box(menu.wTitulo, 0, 0);

  wattr_on(menu.wOpcoes, jogoAttr, NULL);
  mvwaddstr(menu.wOpcoes, jogoY, origemX, "NOVO JOGO");
  wattr_off(menu.wOpcoes, selecionadoAttr, NULL);

  wattr_on(menu.wOpcoes, placarAttr, NULL);
  mvwaddstr(menu.wOpcoes, placarY, origemX, "MAIORES PLACARES");
  wattr_off(menu.wOpcoes, selecionadoAttr, NULL);

  wattr_on(menu.wOpcoes, sairAttr, NULL);
  mvwaddstr(menu.wOpcoes, sairY, origemX, "SAIR");
  wattr_off(menu.wOpcoes, selecionadoAttr, NULL);

  wborder(menu.wOpcoes, 0, 0, 0, 0, ACS_LTEE, ACS_RTEE, 0, 0);

  wattr_on(menu.wOpcoes, selecionadoAttr, NULL);
  mvwaddstr(menu.wOpcoes, flechaY, flechaX, ">");
  wattr_off(menu.wOpcoes, selecionadoAttr, NULL);

  // Adiciona as janelas à fila de janelas a serem atualizadas
  // no fim do quadro.
  wnoutrefresh(menu.wTitulo);
  wnoutrefresh(menu.wOpcoes);

  return estado;
}

/***
 *      _____    _                               
 *     |  __ \  | |                              
 *     | |__) | | |   __ _    ___    __ _   _ __ 
 *     |  ___/  | |  / _` |  / __|  / _` | | '__|
 *     | |      | | | (_| | | (__  | (_| | | |   
 *     |_|      |_|  \__,_|  \___|  \__,_| |_|   
 *                                               
 *                                               
 */

struct
{
  FILE *arqScores;
  TIPO_JOGADOR scores[MAX_SCORES];
  int numScores;
} placar;

ESTADO atualizarPlacar(ENTRADA *entrada, bool trocaDeEstado, double dt)
{
  // O estado para o qual o jogo mudará quando a função retornar.
  ESTADO estado = PLACAR;

  // Volta ao menu se o jogador pressionar retorna.
  if (entrada->retorna)
  {
    estado = MENU;
  }

  // Limpa a janela.
  erase();

  // Coordenadas de base para o placar.
  int ALTURA = MAX_SCORES + 1;
  int LARGURA = PLACAR_TOTAL;
  int origemY = centralizarY(stdscr, ALTURA);
  int origemX = centralizarX(stdscr, LARGURA);

  // Desenha o cabeçalho.
  attr_t attrCabecalho = A_BOLD | A_UNDERLINE;
  attr_on(attrCabecalho, NULL);
  mvprintw(origemY, origemX, "%s", "Nome");
  printw("%*s", LARGURA - 4, "Score");
  attroff(attrCabecalho);

  // Desenha os scores em si.
  for (int i = 0; i < placar.numScores; i++)
  {
    move(origemY + i + 1, origemX);
    printw("%-*s", PLACAR_NOME, placar.scores[i].nome);
    printw("%*s", PLACAR_SEPARADOR, "");
    printw("%*d", PLACAR_SCORE, placar.scores[i].score);
  }

  // Desenha a mensagem para voltar.
  centralizarString(
      stdscr,
      centroY(stdscr) + ALTURA,
      "Pressione BACKSPACE [<--   ] para voltar ao menu");

  // Adiciona a janela padrão à fila de janelas a serem atualizadas
  // no fim do quadro.
  wnoutrefresh(stdscr);

  return estado;
}

/***
 *           _                         
 *          | |                        
 *          | |   ___     __ _    ___  
 *      _   | |  / _ \   / _` |  / _ \ 
 *     | |__| | | (_) | | (_| | | (_) |
 *      \____/   \___/   \__, |  \___/ 
 *                        __/ |        
 *                       |___/         
 */

struct
{
  int ultimoScore;
} jogo;

ESTADO atualizarJogo(ENTRADA *entrada, bool trocaDeEstado, double dt)
{
  // O estado para o qual o jogo mudará quando a função retornar.
  ESTADO estado = JOGO;

  // Atualiza o quadro e guarda o retorno.
  int retorno = atualizarQuadroDoJogo(
      entrada,
      trocaDeEstado,
      dt,
      placar.scores[0].score);

  // Se o retorno não for $JOGO_CONTINUA, o jogo acabou, e o retorno
  // é o score do jogador.
  if (retorno != JOGO_CONTINUA)
  {
    estado = GAMEOVER;
    jogo.ultimoScore = retorno;
  }

  return estado;
}

/***
 *       _____                                                          
 *      / ____|                                                         
 *     | |  __    __ _   _ __ ___     ___    ___   __   __   ___   _ __ 
 *     | | |_ |  / _` | | '_ ` _ \   / _ \  / _ \  \ \ / /  / _ \ | '__|
 *     | |__| | | (_| | | | | | | | |  __/ | (_) |  \ V /  |  __/ | |   
 *      \_____|  \__,_| |_| |_| |_|  \___|  \___/    \_/    \___| |_|   
 *                                                                      
 *                                                                      
 */

ESTADO atualizarGameover(ENTRADA *entrada, bool trocaDeEstado, double dt)
{
  // O estado para o qual o jogo mudará quando a função retornar.
  ESTADO estado = GAMEOVER;

  return estado;
}

/***
 *      __  __           _                   _           _       
 *     |  \/  |         | |                 (_)         (_)      
 *     | \  / |   __ _  | |_    ___   _ __   _    __ _   _   ___ 
 *     | |\/| |  / _` | | __|  / _ \ | '__| | |  / _` | | | / __|
 *     | |  | | | (_| | | |_  |  __/ | |    | | | (_| | | | \__ \
 *     |_|  |_|  \__,_|  \__|  \___| |_|    |_|  \__,_| |_| |___/
 *                                                               
 *                                                               
 */

void carregarMateriais()
{
  // Menu principal.
  menu.gTitulo = carregarGrafico("materiais/titulo.txt");

  // Placar.
  placar.arqScores = lerScores(
      "materiais/highscores.bin",
      placar.scores,
      &placar.numScores);

  // Jogo.
  carregarMateriaisDoJogo();
}

void descarregarMateriais()
{
  // Menu principal.
  descarregarGrafico(&menu.gTitulo);

  destruirJanela(&menu.wTitulo);
  destruirJanela(&menu.wOpcoes);

  // Placar.
  fclose(placar.arqScores);

  // Jogo.
  descarregarMateriaisDoJogo();
}