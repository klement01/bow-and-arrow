#include <Configs.h>
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
#define MAX_OBJETOS_ATIVOS 30

// Tamanho das subjanelas.
#define N_LINHAS_CABECALHO 4
#define N_LINHAS_JOGO (N_LINHAS - N_LINHAS_CABECALHO + 1)

// Distância a qual um objeto pode estar fora da área do jogo sem
// desaparecer / reaparecer do outro lado, em linhas / colunas.
#define PADDING 5

// Tempo mínimo entre flechas (em segundos.)
#define T_FLECHAS 1.0

// Tempo de contagem da munição após fim do nível (em segundos.)
#define T_MUNICAO 0.4

// Tempo entre aparição de monstros (em segundos.)
#define T_MONSTROS 2.0

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
  JOGADOR,
  FLECHA,
  BALAO,
  MONSTRO
} ID_OBJETO;

/*
  Identificador do estado do objeto.
*/
typedef enum estado_obj
{
  INATIVO,
  VIVO,
  MORTO,
  OOB // Out of bounds.
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
  BALOES,
  MONSTROS
} NIVEL;

/*
  Pontos dados ao jogador em diversos eventos.
*/
enum pontos
{
  FLECHA_EXTRA = 50,
  BALAO_MORTO = 100,
  MONSTRO_MORTO = 200,
};

/*
  Subestados em que o jogo pode estar.
*/
typedef enum subestados
{
  SUB_FIM,
  EM_JOGO,
  PAUSADO,
  TROCANDO_NIVEL,
} SUBESTADO;

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

// Objeto do jogardor.
OBJETO *jogador;

// Protótipos de objetos do jogo, criados durante o carregamento
// de materiais do jogo.
OBJETO objJogador;
OBJETO objFlecha;
OBJETO objBalao;
OBJETO objMonstro;

// Janelas do jogo.
WINDOW *wCabecalho;
WINDOW *wJogo;

// Vetor de objetos, ativos e inativos.
OBJETO objetos[MAX_OBJETOS_ATIVOS];

// Variáveis globais do jogo.
int score;
int nivel;
NIVEL tipoDoNivel;
int municao;
int numInimigos;

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

void atualizarObjetos(double dt);
void desenharQuadroDoJogo(void);

OBJETO *inserirObjeto(OBJETO *objeto);
void limparVetorPosicao(bool vetor[]);
bool limitarValor(double *valor, double vmin, double vmax, bool wrap);
bool limitarPosicaoDeObjeto(OBJETO *objeto, bool wrap, bool pad);

int atualizarQuadroDoJogo(ENTRADA *entrada, bool trocaDeEstado, double dt, int highscore)
{
  // Variáveis persistentes.
  static SUBESTADO subestado, ultimoSubestado;

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
      objetos[i].estado = INATIVO;
    }

    // Cria um objeto para o jogador.
    jogador = inserirObjeto(&objJogador);
    jogador->y = centralizarY(wJogo, jogador->grafico.linhas);

    // Configura variáveis iniciais.
    subestado = TROCANDO_NIVEL;
    ultimoSubestado = SUB_FIM;
    score = 0;
    nivel = 0;
    municao = 0;
  }

  // Checa se houve troca de subestado desde o último quadro.
  bool trocaDeSubestado = subestado != ultimoSubestado;
  ultimoSubestado = subestado;

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
  }

  // Normaliza o score.
  if (score > MAX_SCORE)
  {
    score = MAX_SCORE;
  }

  // Desenha o cabeçalho.
  werase(wCabecalho);
  wborder(wCabecalho, 0, 0, 0, 0, 0, 0, ACS_LTEE, ACS_RTEE);

  mvwprintw(wCabecalho, 1, 1, "SCORE:     %*d", PLACAR_SCORE, score);
  if (score > highscore)
  {
    highscore = score;
  }
  mvwprintw(wCabecalho, 2, 1, "HIGHSCORE: %*d", PLACAR_SCORE, highscore);

  mvwprintw(wCabecalho, 1, centralizarX(wCabecalho, 9), "NÍVEL %3d", nivel);

  centralizarString(wCabecalho, 2, "FLECHAS");

  centralizarString(wCabecalho, 0, "> BOW AND ARROW <");

  // Desenha as flechas.
  const int LARGURA = getmaxx(wCabecalho);
  for (int i = 0; i < municao; i++)
  {
    mvwaddch(wCabecalho, 1, LARGURA - 2 - i, '|');
    mvwaddch(wCabecalho, 2, LARGURA - 2 - i, 'V');
  }

  // Adiciona as janelas à fila de janelas a serem atualizadas.
  wnoutrefresh(wJogo);
  wnoutrefresh(wCabecalho);

  // Retorna o score ou $JOGO_CONTINUA.
  int retorno;
  if (subestado == SUB_FIM)
  {
    retorno = score;
  }
  else
  {
    retorno = JOGO_CONTINUA;
  }
  return retorno;
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
  Atualiza o jogo enquanto estiver rodando.
*/
SUBESTADO emJogo(ENTRADA *entrada, bool trocaDeSubestado, double dt)
{
  // Subestado para o qual o jogo vai mudar quando a função retornar.
  SUBESTADO subestado = EM_JOGO;

  // Variáveis persistentes.
  static int flechasAtivas;
  static double timerFlecha;
  static double timerMonstro;
  static int numInimigosPotenciais;

  // Configura os elementos iniciais do jogo após troca de estado
  // (novo jogo.)
  if (trocaDeSubestado)
  {
    // Reseta variáveis persistentes para seus valores iniciais.
    flechasAtivas = 0;
    timerFlecha = 0;
    timerMonstro = 0;
    numInimigosPotenciais = numInimigos;
  }

  // Se o usuário tiver pressionado espaço e o intervalo entre flechas
  // já tiver passado, cria uma nova flecha junto ao jogador.
  timerFlecha -= dt;
  if (timerFlecha < 0)
  {
    timerFlecha = 0;
  }
  if (teclaPressionada(ESPACO) && timerFlecha == 0)
  {
    OBJETO *novaFlecha = inserirObjeto(&objFlecha);
    novaFlecha->y = jogador->y + 1;
    flechasAtivas++;
    municao--;
    timerFlecha += T_FLECHAS;
  }

  // Cria um novo monstro em uma posição aleatória do eixo Y, de
  // acordo com o timer.
  if (tipoDoNivel == MONSTROS && numInimigosPotenciais > 0)
  {
    timerMonstro -= dt;
    if (timerMonstro < 0)
    {
      OBJETO *obj = inserirObjeto(&objMonstro);
      obj->y = 1 + rand() % (getmaxy(wJogo) - obj->grafico.linhas - 1);
      numInimigosPotenciais--;
      timerMonstro += T_MONSTROS;
    }
  }

  // Atualiza e desenha os objetos do jogo.
  atualizarObjetos(dt);
  desenharQuadroDoJogo();

  // Desativa os objetos mortos.
  for (int i = 0; i < MAX_OBJETOS_ATIVOS; i++)
  {
    OBJETO *obj = &objetos[i];
    if (obj->estado != VIVO && obj->estado != INATIVO)
    {
      switch (obj->id)
      {
      case BALAO:
        score += BALAO_MORTO;
        numInimigos--;
        break;
      case MONSTRO:
        if (obj->estado == MORTO)
        {
          score += MONSTRO_MORTO;
        }
        numInimigos--;
        break;
      case FLECHA:
        flechasAtivas--;
        break;
      }
      obj->estado = INATIVO;
    }
  }

  // Se não houverem mais inimigos, avança de nível.
  if (numInimigos <= 0)
  {
    subestado = TROCANDO_NIVEL;
  }
  // Se ainda houverem inimigos e o jogador estiver sem flechas,
  // o jogo acaba.
  else if (flechasAtivas <= 0 && municao <= 0) // && numInimigos != 0
  {
    subestado = SUB_FIM;
  }

  // Se o jogador estiver morto, o jogo acaba.
  if (jogador->estado != VIVO)
  {
    subestado = SUB_FIM;
  }

  return subestado;
}

SUBESTADO pausado(ENTRADA *entrada, bool trocaDeSubestado, double dt)
{
  // Subestado para o qual o jogo vai mudar quando a função retornar.
  SUBESTADO subestado = PAUSADO;

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
      score += FLECHA_EXTRA;
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
      tipoDoNivel = BALOES;
      municao = 15;

      const int LARGURA = getmaxx(wJogo);
      numInimigos = 10;
      for (int i = 0; i < numInimigos; i++)
      {
        OBJETO *obj = inserirObjeto(&objBalao);
        obj->x = LARGURA - 1 - (i + 1) * (obj->grafico.colunas + 1);
      }
    }
    // Níveis pares têm monstros.
    else
    {
      tipoDoNivel = MONSTROS;
      municao = 30;
      numInimigos = 30;
    }
  }

  // Atualiza e desenha os objetos do jogo.
  atualizarObjetos(dt);
  desenharQuadroDoJogo();

  return subestado;
}

/*
  Atualiza a posição dos objetos o jogo de acordo com o tempo passado
  desde a última vez que foram atualizados.
*/
void atualizarObjetos(double dt)
{
  for (int i = 0; i < MAX_OBJETOS_ATIVOS; i++)
  {
    OBJETO *obj = &objetos[i];

    // Atualiza o objeto se ele estiver ativo.
    if (obj->estado != INATIVO)
    {
      // Variáveis para tornar o códido mais conciso.
      GRAFICO *g = &obj->grafico;
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
        case MONSTRO:
          if (limitarPosicaoDeObjeto(obj, false, true) && obj->x < 0)
          {
            obj->estado = OOB;
          }
        case FLECHA:
          if (limitarPosicaoDeObjeto(obj, false, true) && obj->x > 0)
          {
            obj->estado = OOB;
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
void desenharQuadroDoJogo(void)
{
  // Limpa a janela do jogo.
  werase(wJogo);

  for (int i = 0; i < MAX_OBJETOS_ATIVOS; i++)
  {
    OBJETO *obj = &objetos[i];

    // Desenha o objeto se ele estiver ativo, checando colisões
    // no processo.
    if (obj->estado != INATIVO)
    {
      // Variáveis para tornar o códido mais conciso.
      GRAFICO *g = &obj->grafico;
      int y = floor(obj->y);
      int x = floor(obj->x);

      // TODO: corrigir bug quando mais de uma flecha é atirada.
      bool vivo = true;
      switch (obj->id)
      {
      case JOGADOR:
        vivo = !desenharGraficoComColisao(g, wJogo, y, x, posMonstros, NULL);
        limparVetorPosicao(posMonstros);
        break;
      case FLECHA:
        limparVetorPosicao(posFlecha);
        desenharGraficoComColisao(g, wJogo, y, x, NULL, posFlecha);
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
        obj->estado = MORTO;
      }
    }
  }

  // Adiciona as linhas ao redor da janela.
  wborder(wJogo, 0, 0, 0, 0, ACS_LTEE, ACS_RTEE, 0, 0);
}

/*
  Desenha os objetos do jogo na janela e realiza checagem de
  colisões. Retorna true se o objeto continua vivo.
*/
OBJETO *inserirObjeto(OBJETO *objeto)
{
  // Procura um objeto inativo que pode ser substituido pelo novo
  // objeto.
  int indice = 0;
  bool inserido = false;
  while (indice < MAX_OBJETOS_ATIVOS && !inserido)
  {
    if (objetos[indice].estado == INATIVO)
    {
      objetos[indice] = *objeto;
      objetos[indice].estado = VIVO;
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
  if (inserido)
  {
    objeto = &objetos[indice];
  }
  else
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
  objJogador.x = 5;
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