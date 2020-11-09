#ifndef OBJETO_H
#define OBJETO_H

/*
  Define funções para lidar com o carregamento de materiais do jogo
  e atualizar o estado do jogo em si.
*/

#include <Configs.h>
#include <TerminalIO.h>

/*
  Subestados em que o jogo pode estar.
*/
typedef enum subestados
{
  FIM_VITORIA,
  FIM_GAMEOVER,
  QUADRO_CONGELADO,
  EM_JOGO,
  PAUSADO,
  TROCANDO_NIVEL,
} SUBESTADO;

/*
  Constante retornada por atualizarQuadroDoJogo se o jogo deve
  continuar.
*/
#define JOGO_CONTINUA -1

/*
  Atualiza um quadro do jogo baseado nas entradas do usuário, no tempo
  passado desde o último quadro e no fato do jogo ter começado nesse
  quadro ou em um quadro passado. Usa o highscore no cabeçalho.
  Devolve o score atual através de score.
*/
SUBESTADO atualizarQuadroDoJogo(
    ENTRADA *entrada,
    bool trocaDeEstado,
    double dt,
    int highscore,
    int *score);

/*
  Carrega os materiais que serão usado pelo jogo em si. Deve ser chamada
  antes do início do jogo.
*/
void carregarMateriaisDoJogo(void);

/*
  Descarrega os materiais que serão usado pelo jogo em si. Deve ser
  chamada no final do programa.
*/
void descarregarMateriaisDoJogo(void);

#endif