#include <Configs.h>
#include <Grafico.h>
#include <Jogo.h>
#include <Placar.h>
#include <TerminalIO.h>
#include <Timer.h>

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// TODO: som.

// FPS que o jogo tentará manter, limitando a taxa de execução.
#define MAX_FPS 120

// Tamanho das subjanelas do menu principal.
#define N_LINHAS_TITULO 15
#define N_LINHAS_MENU (N_LINHAS - N_LINHAS_TITULO + 1)

// Tempo que o jogo fica na tela de gameover (em segundos.)
#define T_GAMEOVER 3

// Define os estados em que o jogo pode estar.
typedef enum estado
{
  FIM,
  MENU,
  PLACAR,
  JOGO,
  GAMEOVER,
  VITORIA,
  NOVO_SCORE
} ESTADO;

ESTADO atualizarMenu(ENTRADA *entrada, bool trocaDeEstado, double dt);
ESTADO atualizarPlacar(ENTRADA *entrada, bool trocaDeEstado, double dt);
ESTADO atualizarJogo(ENTRADA *entrada, bool trocaDeEstado, double dt);
ESTADO atualizarGameover(ENTRADA *entrada, bool trocaDeEstado, double dt);
ESTADO atualizarVitoria(ENTRADA *entrada, bool trocaDeEstado, double dt);
ESTADO atualizarNovoScore(ENTRADA *entrada, bool trocaDeEstado, double dt);

void desenharPlacar(int *origy, int *origx, int *altura, int *largura);
ESTADO desenharTelaDeFim(ESTADO estado, bool trocaDeEstado, double dt);
bool validarChar(unsigned int ch);

void carregarMateriais(void);
void descarregarMateriais(void);

int main(void)
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
    ENTRADA entrada = {0};
    processarEntrada(&entrada);

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
    case VITORIA:
      estado = atualizarVitoria(&entrada, trocaDeEstado, dt);
      break;
    case NOVO_SCORE:
      estado = atualizarNovoScore(&entrada, trocaDeEstado, dt);
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
  desenharGrafico(&menu.gTitulo, menu.wTitulo, CENTRO, CENTRO);
  wborder(menu.wTitulo, 0, 0, 0, 0, 0, 0, ACS_LTEE, ACS_RTEE);

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
  TIPO_JOGADOR scores[NUM_MAX_SCORES];
  int numScores;
} placar;

/*
  Mostra o placar no centro da tela. Coloca os valores de $origemY,
  $origemX, $ALTURA e $LARGURA se os ponteiros correspondentes não
  forem nulos.
*/
void desenharPlacar(int *origy, int *origx, int *altura, int *largura)
{
  // Coordenadas de base para o placar.
  int ALTURA = NUM_MAX_SCORES + 1;
  int LARGURA = PLACAR_TOTAL;
  int origemY = centralizarY(stdscr, ALTURA);
  int origemX = centralizarX(stdscr, LARGURA);

  // Coloca os valores solicitados nos locais indicador para retorno.
  if (altura)
  {
    *altura = ALTURA;
  }
  if (largura)
  {
    *largura = LARGURA;
  }
  if (origy)
  {
    *origy = origemY;
  }
  if (origx)
  {
    *origx = origemX;
  }

  // Desenha o cabeçalho.
  attr_t attrCabecalho = A_BOLD | A_UNDERLINE;
  attr_on(attrCabecalho, NULL);
  mvaddstr(origemY, origemX, "Nome");
  printw("%*s", LARGURA - 4, "Score");
  attroff(attrCabecalho);

  // Desenha os scores em si.
  for (int i = 0; i < placar.numScores; i++)
  {
    mvaddstr(origemY + i + 1, origemX, placar.scores[i].nome);
    mvprintw(
        origemY + i + 1,
        origemX + LARGURA - PLACAR_SCORE,
        "%*d",
        PLACAR_SCORE,
        placar.scores[i].score);
  }
}

ESTADO atualizarPlacar(ENTRADA *entrada, bool trocaDeEstado, double dt)
{
  // O estado para o qual o jogo mudará quando a função retornar.
  ESTADO estado = PLACAR;

  // Volta ao menu se o jogador pressionar retorna.
  if (entrada->retorna || entrada->confirma)
  {
    estado = MENU;
  }

  // Limpa a janela.
  erase();

  // Desenha o placar.
  int altura;
  desenharPlacar(NULL, NULL, &altura, NULL);

  // Desenha a mensagem para voltar.
  centralizarString(
      stdscr,
      centroY(stdscr) + altura,
      "Aperte ENTER, ESPAÇO ou BACKSPACE para voltar ao menu");

  box(stdscr, 0, 0);

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
  SUBESTADO retorno = atualizarQuadroDoJogo(
      entrada,
      trocaDeEstado,
      dt,
      placar.scores[0].score,
      &jogo.ultimoScore);

  // Se o retorno não for $JOGO_CONTINUA, o jogo acabou, e o retorno
  // é o score do jogador.
  if (retorno == FIM_GAMEOVER)
  {
    estado = GAMEOVER;
  }
  else if (retorno == FIM_VITORIA)
  {
    estado = VITORIA;
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

struct
{
  GRAFICO gGameover;
  GRAFICO gVitoria;
} fimDoJogo;

ESTADO desenharTelaDeFim(ESTADO estado, bool trocaDeEstado, double dt)
{
  // Variáveis persistentes.
  static double gameoverTimer;
  static GRAFICO *grafico;

  // Reinicia as variáveis no início do gameover.
  if (trocaDeEstado)
  {
    gameoverTimer = T_GAMEOVER;
    if (estado == GAMEOVER)
    {
      grafico = &fimDoJogo.gGameover;
    }
    else
    {
      grafico = &fimDoJogo.gVitoria;
    }
  }

  // Avança o timer e checa o score do jogador.
  gameoverTimer -= dt;
  if (gameoverTimer < 0)
  {
    if (checarScore(jogo.ultimoScore, placar.scores))
    {
      estado = NOVO_SCORE;
    }
    else
    {
      estado = MENU;
    }
  }

  // Desenha o gráfico correto.
  erase();
  box(stdscr, 0, 0);
  wattr_on(stdscr, A_BOLD, NULL);
  desenharGrafico(
      grafico,
      stdscr,
      centralizarY(stdscr, grafico->linhas),
      centralizarX(stdscr, grafico->colunas));
  wattr_off(stdscr, A_BOLD, NULL);
  mvprintw(
      centroY(stdscr) + grafico->linhas,
      centralizarX(stdscr, 18),
      "Score final: %*d",
      PLACAR_SCORE,
      jogo.ultimoScore);

  // Adiciona a janela à fila para atualização.
  wnoutrefresh(stdscr);

  return estado;
}

ESTADO atualizarGameover(ENTRADA *entrada, bool trocaDeEstado, double dt)
{
  // O estado para o qual o jogo mudará quando a função retornar.
  ESTADO estado = GAMEOVER;

  estado = desenharTelaDeFim(estado, trocaDeEstado, dt);

  return estado;
}

ESTADO atualizarVitoria(ENTRADA *entrada, bool trocaDeEstado, double dt)
{
  // O estado para o qual o jogo mudará quando a função retornar.
  ESTADO estado = VITORIA;

  estado = desenharTelaDeFim(estado, trocaDeEstado, dt);

  return estado;
}

/***
 *      _   _                    _____                    
 *     | \ | |                  / ____|                   
 *     |  \| | _____   _____   | (___   ___ ___  _ __ ___ 
 *     | . ` |/ _ \ \ / / _ \   \___ \ / __/ _ \| '__/ _ \
 *     | |\  | (_) \ V / (_) |  ____) | (_| (_) | | |  __/
 *     |_| \_|\___/ \_/ \___/  |_____/ \___\___/|_|  \___|
 *                                                        
 *                                                        
 */

bool validarChar(unsigned int ch)
{
  bool valido = true;
  // Checa se $ch é ASCII.
  if (ch >> 7 != 0)
  {
    valido = false;
  }
  // Checa se $ch é imprimível.
  else if (!isprint(ch))
  {
    valido = false;
  }
  return valido;
}

ESTADO atualizarNovoScore(ENTRADA *entrada, bool trocaDeEstado, double dt)
{
  // O estado para o qual o jogo mudará quando a função retornar.
  ESTADO estado = NOVO_SCORE;

  // Variáveis persistentes.
  static char *novoNome;
  static int posicaoPlacar;
  static int cursor;
  static int lenNome;

  if (trocaDeEstado)
  {
    // Cria um score sem nome e o coloca no arranjo de score. Guarda um
    // ponteiro para seu nome para o jogador poder editá-lo.
    TIPO_JOGADOR novoScore = {.nome = {0}, .score = jogo.ultimoScore};
    posicaoPlacar = inserirScore(novoScore, placar.scores, &placar.numScores);
    assert(posicaoPlacar >= 0);
    novoNome = placar.scores[posicaoPlacar].nome;
    // Ponto da array nome onde a entrada no jogador será inserida.
    cursor = 0;
    // Número de chars digitados.
    lenNome = 0;
  }

  // TODO: suporte UTF-8.
  // Processa a entrada do usuário na ordem em que foi digitada.
  // Processamento é feito manualmente para 1) não bloquear
  // 2) impedir que caracteres excessivos sejam impressos.
  for (int i = 0; i < entrada->tamBuffer; i++)
  {
    int ch = entrada->buffer[i];

    switch (ch)
    {
    // Move o cursor para a esquerda ou direita.
    case KEY_LEFT:
      if (cursor > 0)
      {
        cursor--;
      }
      break;
    case KEY_RIGHT:
      if (cursor < lenNome)
      {
        cursor++;
      }
      break;
    // Move o cursor para a esquerda e deleta o caratere selecionado.
    case KEY_BACKSPACE:
    case '\b':
      if (cursor <= 0)
      {
        break;
      }
      cursor--;
    // Deleta o caractere selecionado.
    case KEY_DC:
      if (cursor < lenNome)
      {
        memmove(
            novoNome + cursor,
            novoNome + cursor + 1,
            lenNome - cursor - 1);
        lenNome--;
      }
      break;
    // Insere o caractere digitado.
    default:
      if (lenNome < TAM_NOME - 1 && validarChar(ch))
      {
        // Move os chars do nome para dar espaço ao novo char.
        memmove(novoNome + cursor + 1, novoNome + cursor, lenNome - cursor);
        novoNome[cursor] = ch;
        cursor++;
        lenNome++;
      }
    }
  }

  // Preenche a porção não utilizada do vetor nome com \0.
  memset(novoNome + lenNome, '\0', TAM_NOME - lenNome);

  if (entrada->confirmaSemEspaco)
  {
    // Se o jogador confirmar sem ter entrado seu nome, dá um nome padrão
    // a ele.
    if (lenNome == 0)
    {
      strncpy(novoNome, "Jogador sem nome", TAM_NOME);
    }
    salvarScores(placar.arqScores, placar.scores, placar.numScores);
    estado = MENU;
  }

  // Mostra o placar para o jogador poder ver sua posição e seu nome
  // enquanto estiver digitando.
  erase();

  int origemY, origemX, altura;
  desenharPlacar(&origemY, &origemX, &altura, NULL);

  // Desenha o caractere selecionado com um underline.
  int ch = novoNome[cursor];
  if (ch == '\0')
  {
    ch = ' ';
  }
  mvaddch(
      origemY + posicaoPlacar + 1,
      origemX + cursor,
      ch | A_UNDERLINE);

  centralizarString(
      stdscr,
      centroY(stdscr) + altura,
      "Digite seu nome e aperte ENTER para entrar para o placar!");

  box(stdscr, 0, 0);

  wnoutrefresh(stdscr);

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

void carregarMateriais(void)
{
  // Menu principal.
  carregarGrafico(&menu.gTitulo, "materiais/titulo.txt");

  // Placar.
  placar.arqScores = lerScores(
      "materiais/highscores.bin",
      placar.scores,
      &placar.numScores);

  // Jogo.
  carregarMateriaisDoJogo();

  // Gameover.
  carregarGrafico(&fimDoJogo.gGameover, "materiais/gameover.txt");
  carregarGrafico(&fimDoJogo.gVitoria, "materiais/vitoria.txt");
}

void descarregarMateriais(void)
{
  // Menu principal.
  descarregarGrafico(&menu.gTitulo);

  destruirJanela(&menu.wTitulo);
  destruirJanela(&menu.wOpcoes);

  // Placar.
  fclose(placar.arqScores);

  // Jogo.
  descarregarMateriaisDoJogo();

  // Gameover.
  descarregarGrafico(&fimDoJogo.gGameover);
  descarregarGrafico(&fimDoJogo.gVitoria);
}