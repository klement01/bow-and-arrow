#include <Objeto.h>
#include <Grafico.h>
#include <Placar.h>
#include <TerminalIO.h>
#include <Timer.h>

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

// Define os estados em que o jogo pode estar.
typedef enum estado
{
  FIM,
  MENU,
  PLACAR,
  JOGO,
  GAMEOVER,
} ESTADO;

ESTADO atualizarMenu(ENTRADA *entrada, float dt, bool trocaDeEstado);
ESTADO atualizarPlacar(ENTRADA *entrada, float dt, bool trocaDeEstado);
ESTADO atualizarJogo(ENTRADA *entrada, float dt, bool trocaDeEstado);
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
  reiniciarTimer();

  /*
    Loop: executa continuamente o código principal do jogo,
    realizando alterações a cada tick de processamento.
  */

  while (estado != FIM)
  {
    // Calcula o tempo transcorrido desde o último quadro.
    float dt = calcularDeltaTempo();

    // Coleta uma cópia do buffer de entrada no início do quadro
    // e o estado de diversos eventos de controle.
    ENTRADA entrada = processarEntrada();

    // Corrige o tamanho do terminal se ele tiver mudado.
    if (entrada.terminalRedimensionado)
    {
      corrigirTamanhoDoTerminal();
      reiniciarTimer();
    }

    // Checha se houve uma troca de estado desde o último quadro.
    bool trocaDeEstado = estado != ultimoEstado;
    ultimoEstado = estado;

    // Atualiza o jogo de acordo com o estado atual.
    switch (estado)
    {
    case MENU:
      estado = atualizarMenu(&entrada, dt, trocaDeEstado);
      break;
    case PLACAR:
      estado = atualizarPlacar(&entrada, dt, trocaDeEstado);
      break;
    case JOGO:
      estado = atualizarJogo(&entrada, dt, trocaDeEstado);
      break;
    }

    // Atualiza a tela.
    doupdate();
  }

  // Fim do programa.
  return EXIT_SUCCESS;
}

/*
 *      __  __                 
 *     |  \/  | ___  _ _  _  _ 
 *     | |\/| |/ -_)| ' \| || |
 *     |_|  |_|\___||_||_|\_,_|
 *
 */

struct
{
  GRAFICO gTitulo;
  WINDOW *wTitulo;
  WINDOW *wOpcoes;
} menu;

ESTADO atualizarMenu(ENTRADA *entrada, float dt, bool trocaDeEstado)
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
  const int ALTURA = 3;
  int origemY = centralizarY(menu.wOpcoes, ALTURA);
  int origemX = centroX(menu.wOpcoes);

  int jogoY = origemY;
  int placarY = origemY + 1;
  int sairY = origemY + 2;
  int opcoesX = origemX - 8;

  // Determina a posição da flecha de seleção na tela e os atributos
  // dos textos de opção.
  attr_t jogoAttr = 0;
  attr_t placarAttr = 0;
  attr_t sairAttr = 0;
  attr_t selecionadoAttr = A_BOLD;
  int flechaY;
  int flechaX = opcoesX - 2;
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
  mvwaddstr(menu.wOpcoes, jogoY, opcoesX, "NOVO JOGO");
  wattr_off(menu.wOpcoes, selecionadoAttr, NULL);
  wattr_on(menu.wOpcoes, placarAttr, NULL);
  mvwaddstr(menu.wOpcoes, placarY, opcoesX, "MAIORES PLACARES");
  wattr_off(menu.wOpcoes, selecionadoAttr, NULL);
  wattr_on(menu.wOpcoes, sairAttr, NULL);
  mvwaddstr(menu.wOpcoes, sairY, opcoesX, "SAIR");
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

/*
 *      ___  _                      
 *     | _ \| | __ _  __  __ _  _ _ 
 *     |  _/| |/ _` |/ _|/ _` || '_|
 *     |_|  |_|\__,_|\__|\__,_||_|  
 *
 */

struct
{
  FILE *arqScores;
  TIPO_JOGADOR scores[MAX_SCORES];
  int numScores;
} placar;

ESTADO atualizarPlacar(ENTRADA *entrada, float dt, bool trocaDeEstado)
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

/*
 *         _                 
 *      _ | | ___  __ _  ___ 
 *     | || |/ _ \/ _` |/ _ \
 *      \__/ \___/\__, |\___/
 *                |___/      
 *
 */

struct
{
  // Protótipos de objetos do jogo.
  OBJETO objJogador;
  OBJETO objFlecha;
  OBJETO objBalao;
  OBJETO objMonstro;
  // Janelas.
  WINDOW *wCabecalho;
  WINDOW *wJogo;
  // Variáveis globais do jogo.
  OBJETO objetos[MAX_OBJETOS];
  int score;
} jogo;

OBJETO *inserirObjeto(OBJETO *objeto)
{
  // Procura um objeto inativo que pode ser substituido pelo novo
  // objeto.
  int indice = 0;
  bool inserido = false;
  while (indice < MAX_OBJETOS && !inserido)
  {
    if (jogo.objetos[indice].estado == INATIVO)
    {
      jogo.objetos[indice] = *objeto;
      jogo.objetos[indice].estado = VIVO;
      inserido = true;
    }
    indice++;
  }
  assert(inserido);
  // Retorna um ponteiro para o objeto inserido.
  return &jogo.objetos[indice];
}

ESTADO atualizarJogo(ENTRADA *entrada, float dt, bool trocaDeEstado)
{
  // O estado para o qual o jogo mudará quando a função retornar.
  ESTADO estado = JOGO;

  // Regenera as janelas após redimensionamento / troca de estado.
  if (entrada->terminalRedimensionado || trocaDeEstado)
  {
    destruirJanela(&jogo.wCabecalho);
    destruirJanela(&jogo.wJogo);
  }
  if (!jogo.wCabecalho)
  {
    jogo.wCabecalho = criarJanela(N_LINHAS_CABECALHO, N_COLUNAS, 0, 0);
  }
  if (!jogo.wJogo)
  {
    jogo.wJogo = criarJanela(N_LINHAS_JOGO, N_COLUNAS, N_LINHAS_CABECALHO - 1, 0);
  }

  // Variáveis persistentes.
  static OBJETO *jogador;
  static int nivel, ultimoNivel;
  static int numInimigos;
  static int numFlechas;
  static bool flechaAtiva;

  // Configura os elementos do jogo após troca de estado (novo jogo.)
  if (trocaDeEstado)
  {
    // Desativa todos os objetos.
    for (int i = 0; i < MAX_OBJETOS; i++)
    {
      jogo.objetos[MAX_OBJETOS].estado = INATIVO;
    }
    // Cria um objeto para o jogador.
    jogador = inserirObjeto(&jogo.objJogador);
    // Reseta variáveis persistentes para seus valores iniciais.
    jogo.score = 0;
    nivel = 1;
    ultimoNivel = 0;
    flechaAtiva = false;
  }

  // Checa se houve uma troca de nível.
  bool trocaDeNivel = nivel != ultimoNivel;
  ultimoNivel = nivel;

  // Se houve trocar de nível, reseta as variáveis de acordo com
  // o nível.
  if (trocaDeNivel)
  {
    // TODO: lógica de troca de nível.
  }

  // Se o usuário tiver pressionado espaço e não houver uma flecha na
  // tela, cria uma nova flecha junto ao jogador.
  if (entrada->espaco && !flechaAtiva)
  {
    OBJETO *novaFlecha = inserirObjeto(&jogo.objFlecha);
    novaFlecha->y = jogador->y + 3;
  }

  // Atualiza e desenha os objetos do jogo.
  werase(jogo.wJogo);
  for (int i = 0; i < MAX_OBJETOS; i++)
  {
    atualizarObjeto(&jogo.objetos[i], dt);
  }
  for (int i = 0; i < MAX_OBJETOS; i++)
  {
    desenharObjeto(&jogo.objetos[i], jogo.wJogo);
  }
  wborder(jogo.wJogo, 0, 0, 0, 0, ACS_LTEE, ACS_RTEE, 0, 0);

  // Desativa os objetos mortos.
  for (int i = 0; i < MAX_OBJETOS; i++)
  {
    OBJETO *esseObj = &jogo.objetos[i];
    if (esseObj->estado == MORTO)
    {
      esseObj->estado = INATIVO;
      switch (esseObj->id)
      {
      case JOGADOR:
        estado = GAMEOVER;
        break;
        // TODO: adicionar lógica para outros objetos mortos.
      }
    }
  }

  // Se não houverem mais inimigos, avança de nível.
  if (numInimigos == 0)
  {
    nivel++;
  }
  // Se ainda houverem inimigos e o jogador estiver sem flechas,
  // o jogo acaba.
  else if (!flechaAtiva && numFlechas == 0) // && numInimigos != 0
  {
    estado = GAMEOVER;
  }

  // Desenha o cabeçalho.
  werase(jogo.wCabecalho);
  box(jogo.wCabecalho, 0, 0);
  // TODO: cabeçalho.

  // Adiciona as janelas à fila de janelas a serem atualizadas
  // no fim do quadro.
  wnoutrefresh(jogo.wCabecalho);
  wnoutrefresh(jogo.wJogo);

  return estado;
}

/*
 *       ___                  _ 
 *      / __| ___  _ _  __ _ | |
 *     | (_ |/ -_)| '_|/ _` || |
 *      \___|\___||_|  \__,_||_|
 *
 */

void carregarMateriais()
{
  // Menu principal.
  menu.gTitulo = carregarGrafico("materiais/titulo.txt");

  // Placar.
  placar.arqScores = lerScores("materiais/highscores.bin", placar.scores, &placar.numScores);

  // Jogo.
  jogo.objJogador.id = JOGADOR;
  jogo.objJogador.x = 5;
  jogo.objJogador.y = (N_LINHAS_JOGO / 2);
  jogo.objJogador.vx = 0;
  jogo.objJogador.vy = 10;
  jogo.objJogador.grafico = carregarGrafico("materiais/arqueiro.txt");

  jogo.objFlecha.id = FLECHA;
  jogo.objFlecha.x = jogo.objJogador.x + jogo.objJogador.grafico.colunas;
  jogo.objFlecha.y = -127; // Deve ser definido quando atirada.
  jogo.objFlecha.vx = 0;
  jogo.objFlecha.vy = 0;
  jogo.objFlecha.grafico = carregarGrafico("materiais/flecha.txt");

  jogo.objBalao.id = BALAO;
  jogo.objBalao.x = -127; // Deve ser definido quando criado.
  jogo.objBalao.y = N_LINHAS_JOGO + BUFFER;
  jogo.objBalao.vx = 0;
  jogo.objBalao.vy = 10;
  jogo.objBalao.grafico = carregarGrafico("materiais/balao.txt");

  jogo.objMonstro.id = MONSTRO;
  jogo.objMonstro.x = N_COLUNAS + BUFFER;
  jogo.objMonstro.y = -127; // Deve ser definido quando criado.
  jogo.objMonstro.vx = -10;
  jogo.objMonstro.vy = 0;
  jogo.objMonstro.grafico = carregarGrafico("materiais/monstro.txt");
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
  descarregarGrafico(&jogo.objJogador.grafico);
  descarregarGrafico(&jogo.objFlecha.grafico);
  descarregarGrafico(&jogo.objBalao.grafico);
  descarregarGrafico(&jogo.objMonstro.grafico);

  destruirJanela(&jogo.wCabecalho);
  destruirJanela(&jogo.wJogo);
}