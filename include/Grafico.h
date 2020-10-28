#ifndef GRAFICO_H
#define GRAFICO_H

/*
  Estrutura de gráficos de um objeto do jogo e métodos
  para carregá-los e descarregá-los.
*/

#include <Vetor.h>

typedef struct
{
  int numLinhas;
  int numColunas;
  char **str;
} GRAFICO;

GRAFICO carregarGrafico(char *caminho);
void descarregarGrafico(GRAFICO *grafico);

#endif