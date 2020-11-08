#ifndef CONFIGS_H
#define CONFIGS_H

/*
  Defines que podem ser partilhados por vários arquivos.
*/

/*
  Constantes sobre o tamanho de estruturas de score.
*/
#define TAM_NOME 40
#define NUM_MAX_SCORES 5

/*
  Constantes de formatação.
*/
#define PLACAR_NOME (TAM_NOME - 1)
#define PLACAR_SEPARADOR 6
#define PLACAR_SCORE 6
#define PLACAR_TOTAL (PLACAR_NOME + PLACAR_SEPARADOR + PLACAR_SCORE)

/*
  Pontuação máxima que pode ser obtida.
*/
#define MAX_SCORE (1e6 - 1)

#endif