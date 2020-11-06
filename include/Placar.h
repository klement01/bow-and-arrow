#ifndef PLACAR_H
#define PLACAR_H

/*
  Define uma estrutura $TIPO_JOGADOR que guarda uma entrada no
  placar, além de funções e constantes para lidar com o placar e
  arquivos de placar.
*/

#include <stdbool.h>
#include <stdio.h>

/*
  Constantes sobre o tamanho de estruturas.
*/
#define TAM_NOME 40
#define MAX_SCORES 5

/*
  Constantes de formatação.
*/
#define PLACAR_NOME (TAM_NOME - 1)
#define PLACAR_SEPARADOR 3
#define PLACAR_SCORE 6
#define PLACAR_TOTAL (PLACAR_NOME + PLACAR_SEPARADOR + PLACAR_SCORE)

/*
  Estrutura com um nome de jogador e sua pontação.
*/
typedef struct tipo_jogador
{
  char nome[TAM_NOME];
  int score;
} TIPO_JOGADOR;

/*
  Tenta abrir o arquivo especificado por $caminho e lê até $MAX_SCORES
  scores, que são colocados no arranjo $placar. Returna o descritor do
  arquivo aberto. Coloca o número de scores efetivamente lidos no local
  apontado por $num_scores.
*/
FILE *lerScores(char *caminho, TIPO_JOGADOR placar[], int *numScores);

/*
  Retorna true se $novoScore se qualifica para entrar em $placar.
  Assume que $plcara está em ordem decrescente.
*/
bool checarScore(int novoScore, TIPO_JOGADOR placar[]);

/*
  Tenta inserir o $novoScore em $placar e retorna o índice onde ele foi
  colocado, ou -1 se ele não se qualificar. Assume que $placar está em
  ordem decrescente.
*/
int inserirScore(TIPO_JOGADOR novoScore, TIPO_JOGADOR placar[], int *numScores);

/*
  Salva o $placar em $arq.
*/
void salvarScores(FILE *arq, TIPO_JOGADOR placar[], int numScores);

#endif