#ifndef OBJETO_H
#define OBJETO_H

/*
  Define funções para lidar com o carregamento de materiais do jogo
  e atualizar o estado do jogo em si.
*/

#include <TerminalIO.h>

/*
  Constante retornada por atualizarQuadroDoJogo se o jogo deve
  continuar.
*/
#define JOGO_CONTINUA -1

/*
  Atualiza um quadro do jogo. Retorna jogo continua se o jogo deve
  continuar, retorna o score (>= 0) em caso de game over.
*/
int atualizarQuadroDoJogo(ENTRADA *entrada, bool trocaDeEstado, int highscore);

/*
  Carrega os materiais que serão usado pelo jogo em si. Deve ser chamada
  antes do início do jogo.
*/
void carregarMateriaisDoJogo();

/*
  Descarrega os materiais que serão usado pelo jogo em si. Deve ser
  chamada no final do programa.
*/
void descarregarMateriaisDoJogo();

#endif