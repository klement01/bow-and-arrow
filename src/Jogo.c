#include <Grafico.h>
#include <Jogo.h>
#include <Timer.h>

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Número máximo de objetos que podem existir no jogo em um determinado
// momento.
#define MAX_FLECHAS_ATIVAS 5
#define MAX_INIMIGOS_ATIVOS 30
#define MAX_JOGADOR 1
#define MAX_OBJETOS_ATIVOS (MAX_FLECHAS_ATIVAS + MAX_INIMIGOS_ATIVOS + MAX_JOGADOR)

// Tamanho das subjanelas.
#define N_LINHAS_CABECALHO 4
#define N_LINHAS_JOGO (N_LINHAS - N_LINHAS_CABECALHO + 1)

// Distância a qual um objeto pode estar fora da área do jogo sem
// desaparecer / reaparecer do outro lado, em linhas / colunas.
#define PADDING 1

// Tempo mínimo entre flechas (em segundos.)
#define T_FLECHAS 0.9

// Tempo de contagem da munição após fim do nível (em segundos.)
#define T_MUNICAO 0.1

// Tempo entre aparição de monstros (em segundos.)
#define T_MONSTROS 2.0

// Tempo que o jogo fica pausado após o jogador morrer (em segundos.)
#define T_GAMEOVER 1.5

// Nível máximo que o jogo pode atingir.
// TODO: adicionar mais níveis.
#define NIVEL_MAX 10

/***
 *      _______  _
 *     |__   __|(_)
 *        | |    _  _ __    ___   ___
 *        | |   | || '_ \  / _ \ / __|
 *        | |   | || |_) || (_) |\__ \
 *        |_|   |_|| .__/  \___/ |___/
 *                 | |
 *                 |_|
 */

/*
  Identificadores únicos para cada tipo de objeto.
*/
typedef enum id_objeto
{
  FLECHA,
  MONSTRO,
  BALAO,
  JOGADOR
} ID_OBJETO;

/*
  Identificador do estado do objeto.
*/
typedef enum estado_obj
{
  OBJ_INATIVO,
  OBJ_VIVO,
  OBJ_MORTO,
  OBJ_OOB // Out of bounds.
} ESTADO_OBJ;

/*
  Estrutura com informações sobre um objeto do jogo (seu tipo, posição,
  velocidade e gráfico.)
*/
typedef struct objeto
{
  ID_OBJETO id;
  ESTADO_OBJ estado;
  double x, y;
  double vx, vy;
  GRAFICO grafico;
} OBJETO;

/*
  Tipo do nível atual.
*/
typedef enum nivel
{
  NIVEL_BALOES,
  NIVEL_MONSTROS
} NIVEL;

/*
  Pontos dados ao jogador em diversos eventos.
*/
enum pontos
{
  BONUS_VITORIA = 10000,
  FLECHA_EXTRA = 50,
  BALAO_MORTO = 100,
  MONSTRO_MORTO = 200,
};

/***
 *     __      __           _    __               _
 *     \ \    / /          (_)  /_/              (_)
 *      \ \  / /__ _  _ __  _   __ _ __   __ ___  _  ___
 *       \ \/ // _` || '__|| | / _` |\ \ / // _ \| |/ __|
 *        \  /| (_| || |   | || (_| | \ V /|  __/| |\__ \
 *         \/  \__,_||_|   |_| \__,_|  \_/  \___||_||___/
 *
 *
 */

// Protótipos de objetos do jogo, criados durante o carregamento
// de materiais do jogo.
OBJETO objJogador;
OBJETO objFlecha;
OBJETO objBalao;
OBJETO objMonstro;

// Janelas do jogo.
WINDOW *wCabecalho;
WINDOW *wJogo;

// Vetor de objetos, ativos e inativos. Ele é separado em vários
// subvetores para facilitar detecção de colisão.
OBJETO vetObjetos[MAX_OBJETOS_ATIVOS];
OBJETO *vetFlechas = vetObjetos;
OBJETO *vetInimigos = vetObjetos + MAX_FLECHAS_ATIVAS;
OBJETO *jogador = vetObjetos + MAX_FLECHAS_ATIVAS + MAX_INIMIGOS_ATIVOS;

// Variáveis globais do jogo.
int scoreAtual;
int scorePendente;
int nivel;
NIVEL tipoDoNivel;
int municao;
int numInimigosRestantes;

// Posição dos objetos com os quais pode haver colisão.
bool posFlecha[N_LINHAS_JOGO * N_COLUNAS];
bool posMonstros[N_LINHAS_JOGO * N_COLUNAS];

/***
 *      ______   _____        _               _                _
 *     |  ____| |  __ \      (_)             (_)              | |
 *     | |__    | |__) |_ __  _  _ __    ___  _  _ __    __ _ | |
 *     |  __|   |  ___/| '__|| || '_ \  / __|| || '_ \  / _` || |
 *     | | _    | |    | |   | || | | || (__ | || |_) || (_| || |
 *     |_|(_)   |_|    |_|   |_||_| |_| \___||_|| .__/  \__,_||_|
 *                                              | |
 *                                              |_|
 */

SUBESTADO emJogo(ENTRADA *entrada, bool trocaDeSubestado, double dt);
SUBESTADO pausado(ENTRADA *entrada, bool trocaDeSubestado, double dt);
SUBESTADO trocandoNivel(ENTRADA *entrada, bool trocaDeSubestado, double dt);

void criarInimigos(bool trocaDeSubestado, double dt);

void atualizarTodosOsObjetos(double dt);
void desenharTodosOsObjetos(void);

OBJETO *inserirObjeto(OBJETO *objeto);
OBJETO *inserirObjetoEmSubvetor(OBJETO *objeto, OBJETO *vetor, int num);
void limparVetorPosicao(bool vetor[]);
bool limitarValor(double *valor, double vmin, double vmax, bool wrap);
bool limitarPosicaoDeObjeto(OBJETO *objeto, bool wrap, bool pad);

#define IGNORAR -127
bool limitarVelocidadeDeObjeto(OBJETO *objeto, int xmin, int xmax, int ymin, int ymax);

SUBESTADO atualizarQuadroDoJogo(
    ENTRADA *entrada,
    bool trocaDeEstado,
    double dt,
    int highscore,
    int *score)
{
  // Variáveis persistentes.
  static SUBESTADO subestado, ultimoSubestado;
  static double timerGameover;

  // Regenera as janelas após redimensionamento / troca de estado.
  if (entrada->terminalRedimensionado || trocaDeEstado)
  {
    destruirJanela(&wCabecalho);
    destruirJanela(&wJogo);
  }
  if (!wCabecalho)
  {
    wCabecalho = criarJanela(N_LINHAS_CABECALHO, N_COLUNAS, 0, 0);
  }
  if (!wJogo)
  {
    wJogo = criarJanela(N_LINHAS_JOGO, N_COLUNAS, N_LINHAS_CABECALHO - 1, 0);
  }

  // Configura os elementos iniciais do jogo após troca de estado
  // (novo jogo.)
  if (trocaDeEstado)
  {
    // Desativa todos os objetos.
    for (int i = 0; i < MAX_OBJETOS_ATIVOS; i++)
    {
      vetObjetos[i].estado = OBJ_INATIVO;
    }

    // Cria um objeto para o jogador.
    jogador = inserirObjeto(&objJogador);
    jogador->y = centralizarY(wJogo, jogador->grafico.linhas);

    // Configura variáveis iniciais.
    subestado = TROCANDO_NIVEL;
    ultimoSubestado = QUADRO_CONGELADO;
    scoreAtual = 0;
    scorePendente = 0;
    nivel = 0;
    municao = 0;
    timerGameover = T_GAMEOVER;
  }

  // Checa se houve troca de subestado desde o último quadro.
  // Não considera troca de subestado quando o jogo estava pausado.
  bool trocaDeSubestado = subestado != ultimoSubestado;
  trocaDeSubestado = trocaDeSubestado && ultimoSubestado != PAUSADO;
  ultimoSubestado = subestado;

  // Score exibido no cabeçalho (não reflete necessariamente o score
  // real.)
  int scoreCabecalho = scoreAtual;

  // Atualiza o subestado apropriado.
  switch (subestado)
  {
  case EM_JOGO:
    subestado = emJogo(entrada, trocaDeSubestado, dt);
    break;
  case PAUSADO:
    subestado = pausado(entrada, trocaDeSubestado, dt);
    break;
  case TROCANDO_NIVEL:
    subestado = trocandoNivel(entrada, trocaDeSubestado, dt);
    break;
  case QUADRO_CONGELADO:
    desenharTodosOsObjetos();
    timerGameover -= dt;
    // Efeito do score aumentando no fim do jogo.
    scoreCabecalho += scorePendente * 1.5 * (T_GAMEOVER - timerGameover) / T_GAMEOVER;
    if (scoreCabecalho > scoreAtual + scorePendente)
    {
      scoreCabecalho = scoreAtual + scorePendente;
    }
    // Muda o jogo para o estado correto e adiciona o score pendente de
    // de verdade.
    if (timerGameover < 0)
    {
      if (nivel > NIVEL_MAX)
      {
        subestado = FIM_VITORIA;
      }
      else
      {
        subestado = FIM_GAMEOVER;
      }
      scoreAtual += scorePendente;
    }
    break;
  }

  // Normaliza o score.
  if (scoreCabecalho > MAX_SCORE)
  {
    scoreCabecalho = MAX_SCORE;
  }
  if (scoreAtual > MAX_SCORE)
  {
    scoreAtual = MAX_SCORE;
  }

  // Desenha o cabeçalho.
  werase(wCabecalho);
  wborder(wCabecalho, 0, 0, 0, 0, 0, 0, ACS_LTEE, ACS_RTEE);

  // Determina o nível, munição e high score que devem ser desenhados.
  int municaoCabecalho = fmin(municao, 33);
  int nivelCabecalho = nivel;
  if (nivel > NIVEL_MAX)
  {
    nivelCabecalho = NIVEL_MAX;
    municaoCabecalho = 0;
  }
  if (scoreCabecalho > highscore)
  {
    highscore = scoreCabecalho;
  }

  // Escreve as informações do jogo no cabeçalho.
  mvwprintw(wCabecalho, 1, 1, "SCORE:     %*d", PLACAR_SCORE, scoreCabecalho);

  mvwprintw(wCabecalho, 2, 1, "HIGHSCORE: %*d", PLACAR_SCORE, highscore);

  mvwprintw(wCabecalho, 1, centralizarX(wCabecalho, 9), "NÍVEL %3d", nivelCabecalho);

  centralizarString(wCabecalho, 2, "FLECHAS");

  centralizarString(wCabecalho, 0, "> BOW AND ARROW <");

  // Desenha as flechas.
  const int LARGURA = getmaxx(wCabecalho);
  for (int i = 0; i < municaoCabecalho; i++)
  {
    mvwaddch(wCabecalho, 1, LARGURA - 2 - i, '|' | A_BOLD | COLOR_PAIR(AMARELO));
    mvwaddch(wCabecalho, 2, LARGURA - 2 - i, 'V' | A_BOLD);
  }

  // Adiciona as janelas à fila de janelas a serem atualizadas.
  wnoutrefresh(wJogo);
  wnoutrefresh(wCabecalho);

  *score = scoreAtual;
  return subestado;
}

/***
 *      ______                       _  _  _
 *     |  ____|                     (_)| |(_)
 *     | |__      __ _  _   _ __  __ _ | | _   __ _  _ __  ___  ___
 *     |  __|    / _` || | | |\ \/ /| || || | / _` || '__|/ _ \/ __|
 *     | | _    | (_| || |_| | >  < | || || || (_| || |  |  __/\__ \
 *     |_|(_)    \__,_| \__,_|/_/\_\|_||_||_| \__,_||_|   \___||___/
 *
 *
 */

/*
  Cria inimigos de acordo com o nível atual.
*/
void criarInimigos(bool trocaDeSubestado, double dt)
{
  static int numInimigosParaCriar;
  static double timerMonstros;
  if (trocaDeSubestado)
  {
    numInimigosParaCriar = numInimigosRestantes;
    timerMonstros = T_MONSTROS;
  }

  const int ALTURA = getmaxy(wJogo);
  const int LARGURA = getmaxx(wJogo);

  // Cria os balões alinhados na parte inferior da tela.
  if (tipoDoNivel == NIVEL_BALOES && numInimigosParaCriar > 0)
  {
    for (int i = 0; i < numInimigosRestantes; i++)
    {
      OBJETO *obj = inserirObjeto(&objBalao);
      obj->x = LARGURA - 7 - i * (obj->grafico.colunas + 1);
      // Aumenta a velocidade do balão dependendo do nível.
      obj->vy += -2 * ((nivel - 1) / 2);
      if (nivel >= 5)
      {
        obj->vy += -((rand() % (nivel - 1)) - (nivel - 1));
      }
      if (nivel >= 9)
      {
        obj->vx += (rand() % ((nivel - 1) / 2)) - ((nivel - 1) / 2);
      }
      numInimigosParaCriar--;
    }
  }

  // Cria um novo monstro em uma posição aleatória do eixo Y, de
  // acordo com o timer.
  if (tipoDoNivel == NIVEL_MONSTROS && numInimigosParaCriar > 0)
  {
    timerMonstros -= dt;
    if (timerMonstros < 0)
    {
      OBJETO *obj = inserirObjeto(&objMonstro);
      obj->y = 1 + rand() % (ALTURA - obj->grafico.linhas - 1);
      // Aumenta a velocidade do monstro dependendo do nível.
      obj->vx += -7 * (nivel / 2 - 1);
      if (nivel >= 6)
      {
        obj->vy += (rand() % nivel) - (nivel / 2);
      }
      // Reinicia o contado de tempo do monstro.
      timerMonstros += T_MONSTROS;
      numInimigosParaCriar--;
    }
  }
}

/*
  Atualiza o jogo enquanto estiver rodando.
*/
SUBESTADO emJogo(ENTRADA *entrada, bool trocaDeSubestado, double dt)
{
  // Subestado para o qual o jogo vai mudar quando a função retornar.
  SUBESTADO subestado = EM_JOGO;

  // Variáveis persistentes.
  static int flechasAtivas;
  static double timerFlecha;

  // Configura os elementos iniciais do jogo após troca de estado
  // (novo jogo.)
  if (trocaDeSubestado)
  {
    // Reseta variáveis persistentes para seus valores iniciais.
    flechasAtivas = 0;
    timerFlecha = T_FLECHAS;
  }

  // Invoca novos inimigos quando necessário.
  criarInimigos(trocaDeSubestado, dt);

  // Reinicia os arranjos de posição dos objetos.
  limparVetorPosicao(posFlecha);
  limparVetorPosicao(posMonstros);

  // Atualiza e desenha os objetos do jogo.
  atualizarTodosOsObjetos(dt);
  desenharTodosOsObjetos();

  // Se o usuário tiver pressionado espaço e o intervalo entre flechas
  // já tiver passado, cria uma nova flecha junto ao jogador.
  timerFlecha -= dt;
  if (timerFlecha < 0)
  {
    timerFlecha = 0;
  }
  if (teclaPressionada(ESPACO) && timerFlecha == 0 && municao > 0)
  {
    OBJETO *novaFlecha = inserirObjeto(&objFlecha);
    novaFlecha->y = jogador->y + 1;
    flechasAtivas++;
    municao--;
    timerFlecha += T_FLECHAS;
  }

  // Desativa os objetos mortos (com exceção do jogador, que fica
  // sempre ativo.)
  bool jogadorMorto = false;
  for (int i = 0; i < MAX_OBJETOS_ATIVOS; i++)
  {
    OBJETO *obj = &vetObjetos[i];
    if (obj->estado != OBJ_VIVO && obj->estado != OBJ_INATIVO)
    {
      switch (obj->id)
      {
      case JOGADOR:
        jogadorMorto = true;
        break;
      case BALAO:
        scoreAtual += BALAO_MORTO;
        numInimigosRestantes--;
        break;
      case MONSTRO:
        if (obj->estado == OBJ_MORTO)
        {
          scoreAtual += MONSTRO_MORTO;
        }
        numInimigosRestantes--;
        break;
      case FLECHA:
        flechasAtivas--;
        break;
      }
      if (obj->id != JOGADOR)
      {
        obj->estado = OBJ_INATIVO;
      }
    }
  }

  /*
    Troca de subestado. Quando mais de um evento ocorre que pode levar a
    uma troca de subestado, segue a precedência;
    FIM DE JOGO > TROCA DE NÍVEL > PAUSE.
  */

  // Pausa o jogo se o usuário tiver pressionado algum botão de pause.
  if (entrada->pause)
  {
    subestado = PAUSADO;
  }

  // Se não houverem mais inimigos, avança de nível.
  if (numInimigosRestantes <= 0)
  {
    subestado = TROCANDO_NIVEL;
  }
  // Se ainda houverem inimigos e o jogador estiver sem flechas em um
  // nível de balões, o jogo acaba.
  else if (flechasAtivas <= 0 && municao <= 0 && tipoDoNivel == NIVEL_BALOES)
  {
    subestado = QUADRO_CONGELADO;
  }

  // Se o jogador tiver morrido, acaba o jogo.
  if (jogadorMorto)
  {
    subestado = QUADRO_CONGELADO;
  }

  return subestado;
}

SUBESTADO pausado(ENTRADA *entrada, bool trocaDeSubestado, double dt)
{
  // Subestado para o qual o jogo vai mudar quando a função retornar.
  SUBESTADO subestado = PAUSADO;

  // Variáveis persistentes.
  const int ALTURA = 8, LARGURA = 24;
  static WINDOW *wPause;
  static SUBESTADO selecao;

  // Calcula a origem da janela.
  int origemY = centralizarY(stdscr, ALTURA);
  int origemX = centralizarX(stdscr, LARGURA);

  // Regenera a janela após uma troca de subestado ou redimensionamento
  // to terminal.
  if (entrada->terminalRedimensionado || trocaDeSubestado)
  {
    destruirJanela(&wPause);
  }
  if (!wPause)
  {
    wPause = criarJanela(ALTURA, LARGURA, origemY, origemX);
  }

  // Reinicia as variáveis após o jogador pausar.
  if (trocaDeSubestado)
  {
    selecao = EM_JOGO;
  }

  // Muda a seleção conforme a entrada do jogador.
  if (entrada->baixo && selecao == EM_JOGO)
  {
    selecao = FIM_GAMEOVER;
  }
  else if (entrada->cima && selecao == FIM_GAMEOVER)
  {
    selecao = EM_JOGO;
  }
  else if (entrada->retorna)
  {
    selecao = FIM_GAMEOVER;
  }

  // Desenha os objetos do jogo.
  desenharTodosOsObjetos();

  // Determina as posições dos textos.
  int mesY = 2;
  int voltarY = mesY + 2;
  int sairY = voltarY + 1;
  int opcaoX = 6;

  // Determina a posição da flecha de seleção na tela e os atributos
  // dos textos de opção.
  attr_t voltarAttr = 0;
  attr_t sairAttr = 0;
  attr_t selecionadoAttr = A_BOLD;
  int flechaY;
  int flechaX = opcaoX - 2;
  switch (selecao)
  {
  case EM_JOGO:
    voltarAttr = selecionadoAttr;
    flechaY = voltarY;
    break;
  case FIM_GAMEOVER:
    sairAttr = selecionadoAttr;
    flechaY = sairY;
    break;
  }

  // Desenha as opções e a flecha de seleção.
  werase(wPause);
  box(wPause, 0, 0);

  wattr_on(wPause, selecionadoAttr, NULL);
  centralizarString(wPause, mesY, "PAUSE");
  wattr_off(wPause, selecionadoAttr, NULL);

  wattr_on(wPause, voltarAttr, NULL);
  mvwaddstr(wPause, voltarY, opcaoX, "VOLTAR AO JOGO");
  wattr_off(wPause, voltarAttr, NULL);

  wattr_on(wPause, sairAttr, NULL);
  mvwaddstr(wPause, sairY, opcaoX, "SAIR DO JOGO");
  wattr_off(wPause, sairAttr, NULL);

  wattr_on(wPause, selecionadoAttr, NULL);
  mvwaddstr(wPause, flechaY, flechaX, ">");
  wattr_off(wPause, selecionadoAttr, NULL);

  wrefresh(wPause);

  // Adiciona a janela à lista de renderização.
  wnoutrefresh(wPause);

  // Se o jogador confirmar, muda o subestado para refletir a seleção.
  if (entrada->confirma)
  {
    subestado = selecao;
  }

  // Sai do pause se o jogador pressionar o botão de pause novamente.
  if (entrada->pause)
  {
    subestado = EM_JOGO;
  }

  return subestado;
}

SUBESTADO trocandoNivel(ENTRADA *entrada, bool trocaDeSubestado, double dt)
{
  // Subestado para o qual o jogo vai mudar quando a função retornar.
  SUBESTADO subestado = TROCANDO_NIVEL;

  static double timerMunicao = 0;

  // Reinicia o timer após uma troca de subestado.
  if (trocaDeSubestado)
  {
    timerMunicao = T_MUNICAO;
  }

  // Enquanto o jogador ainda tiver munição, remove dele enquanto
  // incrementa seus pontos.
  if (municao > 0)
  {
    timerMunicao -= dt;
    if (timerMunicao < 0)
    {
      municao--;
      scoreAtual += FLECHA_EXTRA;
      timerMunicao += T_MUNICAO;
    }
  }
  // Quando o jogador não tiver mais munição, configura o novo
  // nível.
  else
  {
    subestado = EM_JOGO;
    nivel++;

    // Níveis ímpares têm balões.
    if (nivel % 2 == 1)
    {
      tipoDoNivel = NIVEL_BALOES;
      municao = 14 + nivel;
      numInimigosRestantes = 15;
    }
    // Níveis pares têm monstros.
    else
    {
      tipoDoNivel = NIVEL_MONSTROS;
      numInimigosRestantes = 28 + nivel;
      municao = numInimigosRestantes;
    }
  }

  // Termina o jogo após o nível 10.
  if (nivel > NIVEL_MAX)
  {
    scorePendente = BONUS_VITORIA;
    subestado = QUADRO_CONGELADO;
  }

  // Atualiza e desenha os objetos do jogo.
  atualizarTodosOsObjetos(dt);
  desenharTodosOsObjetos();

  return subestado;
}

/*
  Atualiza a posição dos objetos o jogo de acordo com o tempo passado
  desde a última vez que foram atualizados.
*/
void atualizarTodosOsObjetos(double dt)
{
  for (int i = 0; i < MAX_OBJETOS_ATIVOS; i++)
  {
    OBJETO *obj = &vetObjetos[i];

    // Atualiza o objeto se ele estiver ativo.
    if (obj->estado != OBJ_INATIVO)
    {
      // Variáveis para tornar o códido mais conciso.
      double dy = obj->vy * dt;
      double dx = obj->vx * dt;

      // Jogador se move condicionalmente.
      if (obj->id == JOGADOR)
      {
        if (teclaPressionada(W))
        {
          obj->y -= dy;
        }
        else if (teclaPressionada(S))
        {
          obj->y += dy;
        }
        limitarPosicaoDeObjeto(obj, false, false);
      }
      // Outros objetos se movem de acordo com sua velocidade.
      else
      {
        obj->y += dy;
        obj->x += dx;
        switch (obj->id)
        {
        case BALAO:
          limitarPosicaoDeObjeto(obj, true, true);
          limitarVelocidadeDeObjeto(
              obj,
              jogador->x + jogador->grafico.colunas + 1,
              getmaxx(wJogo) - 1,
              IGNORAR,
              IGNORAR);
          break;
        case MONSTRO:
          if (limitarPosicaoDeObjeto(obj, false, true) && obj->x < 0)
          {
            obj->estado = OBJ_OOB;
          }
          limitarVelocidadeDeObjeto(
              obj,
              IGNORAR,
              IGNORAR,
              1,
              getmaxy(wJogo) - 1);
          break;
        case FLECHA:
          if (limitarPosicaoDeObjeto(obj, false, true) && obj->x > 0)
          {
            obj->estado = OBJ_OOB;
          }
          break;
        }
      }
    }
  }
}

/*
  Desenha os objetos ativos, checando colisão no processo.
*/
void desenharTodosOsObjetos(void)
{
  // Limpa a janela do jogo.
  werase(wJogo);

  for (int i = 0; i < MAX_OBJETOS_ATIVOS; i++)
  {
    OBJETO *obj = &vetObjetos[i];

    // Desenha o objeto se ele estiver ativo, checando colisões
    // no processo.
    if (obj->estado != OBJ_INATIVO)
    {
      // Variáveis para tornar o códido mais conciso.
      GRAFICO *g = &obj->grafico;
      int y = floor(obj->y);
      int x = floor(obj->x);

      bool vivo = true;
      switch (obj->id)
      {
      case JOGADOR:
        vivo = !desenharGraficoComColisao(g, wJogo, y, x, posMonstros, NULL);
        break;
      case FLECHA:
        // Limita a colisão da flecha para sua ponta.
        desenharGraficoComColisaoLimitada(
            g, wJogo, y, x, NULL, posFlecha, g->linhas - 1, g->colunas - 1);
        break;
      case BALAO:
        vivo = !desenharGraficoComColisao(g, wJogo, y, x, posFlecha, NULL);
        break;
      case MONSTRO:
        vivo = !desenharGraficoComColisao(g, wJogo, y, x, posFlecha, posMonstros);
        break;
      default:
        desenharGrafico(g, wJogo, y, x);
      }

      // Se o objeto não está mais vivo por ter sido atingido, muda
      // seu estado.
      if (!vivo)
      {
        obj->estado = OBJ_MORTO;
      }
    }
  }

  // Nas fases de monstros, é feito um segundo passe para checar colisões
  // dos monstros com as flechas. No primeiro passe, os monstros morrem
  // se colidem com as flechas. No segundo, as flechas são destruídas se
  // colididirem com um monstro.
  if (tipoDoNivel == NIVEL_MONSTROS)
  {
    for (int i = 0; i < MAX_FLECHAS_ATIVAS; i++)
    {
      OBJETO *obj = &vetFlechas[i];
      if (obj->estado == OBJ_VIVO)
      {
        bool vivo = !desenharGraficoComColisaoLimitada(
            &obj->grafico,
            wJogo,
            obj->y,
            obj->x,
            posMonstros,
            NULL,
            obj->grafico.linhas - 1,
            obj->grafico.colunas - 1);
        if (!vivo)
        {
          obj->estado = OBJ_MORTO;
        }
      }
    }
  }

  // Adiciona as linhas ao redor da janela.
  wborder(wJogo, 0, 0, 0, 0, ACS_LTEE, ACS_RTEE, 0, 0);
}

/*
  Determina em qual subvetor inserir o objeto e o insere nele. Retorna
  um ponteiro para a posição do objeto no vetor.
*/
OBJETO *inserirObjeto(OBJETO *obj)
{
  switch (obj->id)
  {
  case FLECHA:
    obj = inserirObjetoEmSubvetor(obj, vetFlechas, MAX_FLECHAS_ATIVAS);
    break;
  case BALAO:
  case MONSTRO:
    obj = inserirObjetoEmSubvetor(obj, vetInimigos, MAX_INIMIGOS_ATIVOS);
    break;
  case JOGADOR:
    obj = inserirObjetoEmSubvetor(obj, jogador, MAX_JOGADOR);
    break;
  }
  return obj;
}

/*
  Coloca uma cópia de objeto no subvetor $vetor que tem capacidade para
  $num objetos. Retorna um ponteiro para a posição do objeto no vetor.
*/
OBJETO *inserirObjetoEmSubvetor(OBJETO *objeto, OBJETO *vetor, int num)
{
  // Procura um objeto inativo que pode ser substituido pelo novo
  // objeto.
  int indice = 0;
  bool inserido = false;
  while (indice < num && !inserido)
  {
    if (vetor[indice].estado == OBJ_INATIVO)
    {
      vetor[indice] = *objeto;
      objeto = &vetor[indice];
      objeto->estado = OBJ_VIVO;
      inserido = true;
    }
    else
    {
      indice++;
    }
  }
  assert(inserido);
  // Retorna um ponteiro para o objeto inserido ou NULL se o objeto não
  // pode ser inserido.
  if (!inserido)
  {
    objeto = NULL;
  }
  return objeto;
}

/*
  Esvazia um vetor de posição de objetos, tornando todos os espaços falsos.
*/
void limparVetorPosicao(bool vetor[])
{
  memset(vetor, false, N_LINHAS_JOGO * N_COLUNAS);
}

/*
  Toma $valor e o converte para um número entre $vmin e $vmax.
  Se $wrap for verdadeiro, o manda para o outro extremo.
  Se não, escolhe o número mais próximo dentro do intervalo escolhido.
  Retorna true se &valor foi alterado.
*/
bool limitarValor(double *valor, double vmin, double vmax, bool wrap)
{
  // Primeiro, determina se &valor está dentro do intervalo.
  double novoValor = *valor;
  novoValor = fmax(novoValor, vmin);
  novoValor = fmin(novoValor, vmax);
  bool alterado = novoValor != *valor;

  // Se $wrap for falso, o $novoValor é usado.
  if (!wrap)
  {
    *valor = novoValor;
  }

  // Se for verdadeiro, calcula o valor correto de $novoValor.
  else if (alterado) // && wrap
  {
    double intervalo = vmax - vmin;
    while (*valor < vmin)
    {
      *valor += intervalo;
    }
    while (*valor > vmax)
    {
      *valor -= intervalo;
    }
  }

  return alterado;
}

/*
  Limita a posição de $objeto, deixando-o dentro da janela de jogo.
*/
bool limitarPosicaoDeObjeto(OBJETO *objeto, bool wrap, bool pad)
{
  const int ALTURA = objeto->grafico.linhas;
  const int LARGURA = objeto->grafico.colunas;

  // Determina os valores mínimo e máximo de x e y para o objeto,
  // levando em consideração o tamnaho de seu gráfico.
  double ymin = 1;
  double ymax = getmaxy(wJogo);
  double xmin = 1;
  double xmax = getmaxx(wJogo);

  if (!pad)
  {
    ymax -= ALTURA + 1;
    xmax -= LARGURA + 1;
  }
  // Se $pad, o objeto pode sair da tela, por um valor máximo
  // determinado por $PADDING.
  else
  {
    ymin -= PADDING + LARGURA;
    ymax += PADDING;
    xmin -= PADDING + LARGURA;
    xmax += PADDING;
  }

  // Limita o valor do objeto nos dois eixo e determina se
  // foi alterado.
  bool novoY = limitarValor(&objeto->y, ymin, ymax, wrap);
  bool novoX = limitarValor(&objeto->x, xmin, xmax, wrap);

  return novoY || novoX;
}

/*
  Ajuda a velocidade de um objeto para que ele fique dentro das coordenadas
  indicadas. Alguns testes podem ser ignorados com o valor IGNORAR..
*/
bool limitarVelocidadeDeObjeto(OBJETO *objeto, int xmin, int xmax, int ymin, int ymax)
{
  assert(xmin != IGNORAR || xmax != IGNORAR || ymin != IGNORAR || ymax != IGNORAR);
  // Limita a posição e a velocidade $x.
  if (xmin != IGNORAR && objeto->x < xmin)
  {
    objeto->x = xmin;
    objeto->vx = fabs(objeto->vx);
  }
  else if (xmax != IGNORAR && objeto->x + objeto->grafico.colunas > xmax)
  {
    objeto->x = xmax - objeto->grafico.colunas;
    objeto->vx = -fabs(objeto->vx);
  }
  // Limita a posição e a velocidade $y.
  if (ymin != IGNORAR && objeto->y < ymin)
  {
    objeto->y = ymin;
    objeto->vy = fabs(objeto->vy);
  }
  else if (ymax != IGNORAR && objeto->y + objeto->grafico.linhas > ymax)
  {
    objeto->y = ymax - objeto->grafico.linhas;
    objeto->vy = -fabs(objeto->vy);
  }
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
void carregarMateriaisDoJogo(void)
{
  objJogador.id = JOGADOR;
  objJogador.x = 4;
  objJogador.y = -127; // Deve ser definido quando criado.
  objJogador.vy = 15;
  carregarGrafico(&objJogador.grafico, "materiais/arqueiro.txt");

  objFlecha.id = FLECHA;
  objFlecha.x = objJogador.x + objJogador.grafico.colunas;
  objFlecha.y = -127; // Deve ser definido quando atirada.
  objFlecha.vx = 40;
  objFlecha.vy = 0;
  carregarGrafico(&objFlecha.grafico, "materiais/flecha.txt");

  objBalao.id = BALAO;
  objBalao.x = -127; // Deve ser definido quando criado.
  objBalao.y = N_LINHAS_JOGO + PADDING;
  objBalao.vx = 0;
  objBalao.vy = -5;
  carregarGrafico(&objBalao.grafico, "materiais/balao.txt");

  objMonstro.id = MONSTRO;
  objMonstro.x = N_COLUNAS + PADDING;
  objMonstro.y = -127; // Deve ser definido quando criado.
  objMonstro.vx = -15;
  objMonstro.vy = 0;
  carregarGrafico(&objMonstro.grafico, "materiais/monstro.txt");
}

void descarregarMateriaisDoJogo(void)
{
  descarregarGrafico(&objJogador.grafico);
  descarregarGrafico(&objFlecha.grafico);
  descarregarGrafico(&objBalao.grafico);
  descarregarGrafico(&objMonstro.grafico);

  destruirJanela(&wCabecalho);
  destruirJanela(&wJogo);
}
