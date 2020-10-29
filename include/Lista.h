#ifndef LISTA_H
#define LISTA_H

/*
  Estrutura e funções para manipular listas
  duplamente ligadas de objetos do jogo.
*/

#include <Objeto.h>

typedef struct struct_node
{
  struct struct_node *anterior;
  struct struct_node *proximo;
  OBJETO objeto;
} NODE;

NODE *criarNodeEm(NODE **proximo, OBJETO objeto);
void removerNode(NODE *node);
void removerListaDoNode(NODE **node);

#endif