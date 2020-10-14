#ifndef LISTA_H
#define LISTA_H

/*
  Estrutura e funções para manipular listas
  duplamente ligadas de objetos do jogo.
*/

#include <Objeto.h>

typedef struct sNode
{
  struct sNode *anterior;
  struct sNode *proximo;
  Objeto objeto;
} Node;

Node *criarNodeAntesDe(Node **proximo, Objeto objeto);
void removerNode(Node *node);
void removerListaDoNode(Node **node);

#endif