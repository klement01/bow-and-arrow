#include <Grafico.h>
#include <Jogo.h>
#include <Timer.h>

#include <assert.h>
#include <stdbool.h>

// Número máximo de objetos que podem existir no jogo em um determinado
// momento.
#define MAX_OBJETOS_ATIVOS 30

// Distância a qual um objeto pode estar fora da área do jogo sem
// desaparecer / reaparecer do outro lado, em linhas / colunas.
#define PADDING 5

// Pontuação máxima que pode ser obtida.
#define MAX_SCORE (1e6 - 1)

// Tamanho das subjanelas.
#define N_LINHAS_CABECALHO 4
#define N_LINHAS_JOGO (N_LINHAS - N_LINHAS_CABECALHO + 1)

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
  MORTO
} ESTADO_OBJ;

/*
  Estrutura com informações sobre um objeto do jogo (seu tipo, posição,
  velocidade e gráfico.)
*/
typedef struct objeto
{
  ID_OBJETO id;
  ESTADO_OBJ estado;
  float x, y;
  float vx, vy;
  GRAFICO grafico;
} OBJETO;

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

/*
  Protótipos de objetos do jogo, criados durante o carregamento
  de materiais do jogo.
*/
OBJETO objJogador;
OBJETO objFlecha;
OBJETO objBalao;
OBJETO objMonstro;

/*
  Janelas do jogo.
*/
WINDOW *wCabecalho;
WINDOW *wJogo;

/*
  Vetor de objetos ativos.
*/
OBJETO objetos[MAX_OBJETOS_ATIVOS];

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

void atualizarObjeto(OBJETO *objeto, float dt);
void desenharObjeto(OBJETO *objeto, WINDOW *win);
OBJETO *inserirObjeto(OBJETO *objeto);

int atualizarQuadroDoJogo(ENTRADA *entrada, bool trocaDeEstado, int highscore)
{
  // O jogo continua até algo forçar um gameover (o jogador morre,
  // fica sem flechas, o último nível é completo, etc.)
  bool fimDeJogo = false;

  // Regenera as janelas e reinicia o timer após redimensionamento /
  // troca de estado.
  if (entrada->terminalRedimensionado || trocaDeEstado)
  {
    destruirJanela(&wCabecalho);
    destruirJanela(&wJogo);
    reiniciarTimer();
  }
  if (!wCabecalho)
  {
    wCabecalho = criarJanela(N_LINHAS_CABECALHO, N_COLUNAS, 0, 0);
  }
  if (!wJogo)
  {
    wJogo = criarJanela(N_LINHAS_JOGO, N_COLUNAS, N_LINHAS_CABECALHO - 1, 0);
  }

  // Calcula quando tempo passou desde o último quadro.
  float dt = calcularDeltaTempo();

  // Variáveis persistentes.
  static int score;
  static OBJETO *jogador;
  static int nivel, ultimoNivel;
  static int numInimigos;
  static int numFlechas;
  static bool flechaAtiva;

  // Configura os elementos do jogo após troca de estado (novo jogo.)
  if (trocaDeEstado)
  {
    // Desativa todos os objetos.
    for (int i = 0; i < MAX_OBJETOS_ATIVOS; i++)
    {
      objetos[i].estado = INATIVO;
    }
    // Cria um objeto para o jogador.
    jogador = inserirObjeto(&objJogador);
    // Reseta variáveis persistentes para seus valores iniciais.
    score = 0;
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
    OBJETO *novaFlecha = inserirObjeto(&objFlecha);
    novaFlecha->y = jogador->y + 1;
  }

  // Atualiza e desenha os objetos do jogo.
  werase(wJogo);
  for (int i = 0; i < MAX_OBJETOS_ATIVOS; i++)
  {
    atualizarObjeto(&objetos[i], dt);
  }
  for (int i = 0; i < MAX_OBJETOS_ATIVOS; i++)
  {
    desenharObjeto(&objetos[i], wJogo);
  }
  wborder(wJogo, 0, 0, 0, 0, ACS_LTEE, ACS_RTEE, 0, 0);

  // Desativa os objetos mortos.
  for (int i = 0; i < MAX_OBJETOS_ATIVOS; i++)
  {
    OBJETO *esseObj = &objetos[i];
    if (esseObj->estado == MORTO)
    {
      esseObj->estado = INATIVO;
      switch (esseObj->id)
      {
        // TODO: adicionar lógica para objetos mortos.
      }
    }
  }

  // Desenha o cabeçalho.
  werase(wCabecalho);
  box(wCabecalho, 0, 0);
  // TODO: cabeçalho.

  // Adiciona as janelas à fila de janelas a serem atualizadas
  // no fim do quadro.
  wnoutrefresh(wCabecalho);
  wnoutrefresh(wJogo);

  // Se não houverem mais inimigos, avança de nível.
  if (numInimigos == 0)
  {
    nivel++;
  }
  // Se ainda houverem inimigos e o jogador estiver sem flechas,
  // o jogo acaba.
  else if (!flechaAtiva && numFlechas == 0) // && numInimigos != 0
  {
    fimDeJogo = true;
  }

  // Se o jogador estiver morto, o jogo acabar.
  if (jogador->estado != VIVO)
  {
    fimDeJogo = true;
  }

  // Retorna o score se o jogo chegar ao fim ou $JOGO_CONTINUA se não.
  return fimDeJogo ? score : JOGO_CONTINUA;
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
  Atualiza a posição de um objeto do jogo de acordo com o tempo passado
  desde a última vez que foi atualizado.
*/
void atualizarObjeto(OBJETO *objeto, float dt)
{
}

/*
  Desenha um objeto do jogo na tela e checa colisões.
*/
void desenharObjeto(OBJETO *objeto, WINDOW *win)
{
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
    indice++;
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
void carregarMateriaisDoJogo()
{
  objJogador.id = JOGADOR;
  objJogador.x = 5;
  objJogador.y = (N_LINHAS_JOGO / 2);
  objJogador.vx = 0;
  objJogador.vy = 10;
  objJogador.grafico = carregarGrafico("materiais/arqueiro.txt");

  objFlecha.id = FLECHA;
  objFlecha.x = objJogador.x + objJogador.grafico.colunas;
  objFlecha.y = -127; // Deve ser definido quando atirada.
  objFlecha.vx = 0;
  objFlecha.vy = 0;
  objFlecha.grafico = carregarGrafico("materiais/flecha.txt");

  objBalao.id = BALAO;
  objBalao.x = -127; // Deve ser definido quando criado.
  objBalao.y = N_LINHAS_JOGO + PADDING;
  objBalao.vx = 0;
  objBalao.vy = 10;
  objBalao.grafico = carregarGrafico("materiais/balao.txt");

  objMonstro.id = MONSTRO;
  objMonstro.x = N_COLUNAS + PADDING;
  objMonstro.y = -127; // Deve ser definido quando criado.
  objMonstro.vx = -10;
  objMonstro.vy = 0;
  objMonstro.grafico = carregarGrafico("materiais/monstro.txt");
}

void descarregarMateriaisDoJogo()
{
  descarregarGrafico(&objJogador.grafico);
  descarregarGrafico(&objFlecha.grafico);
  descarregarGrafico(&objBalao.grafico);
  descarregarGrafico(&objMonstro.grafico);

  destruirJanela(&wCabecalho);
  destruirJanela(&wJogo);
}