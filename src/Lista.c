#include <Lista.h>

#include <stdio.h>
#include <stdlib.h>

Node *criarNodeAntesDe(Node **pProximo, Objeto objeto)
{
  /*
    Cria espaço na memória para um node de objeto e o coloca
    na lista antes do node apontado pelo ponteiro apontado por
    $pProximo. Atualiza $proximo para ser o novo node e o retorna.
  */

  Node *novoObjeto = NULL;

  if (pProximo)
  {
    // Pointer que aponta para o próximo node.
    Node *proximo = *pProximo;

    // Tenta alocar espaço para o objeto no heap.
    novoObjeto = malloc(sizeof(Node));
    if (!novoObjeto)
    {
      perror("Erro alocando memória para objeto.");
      exit(EXIT_FAILURE);
    }

    // Configura o novo node.
    novoObjeto->proximo = proximo;
    novoObjeto->objeto = objeto;
    if (proximo)
    {
      // Se $proximo não for nulo, usa seu valor de anterior
      // e corrige seus ponteiros.
      novoObjeto->anterior = proximo->anterior;
      proximo->anterior = novoObjeto;
    }
    else
    {
      novoObjeto->anterior = NULL;
    }

    // Atualiza $proximo para apontar ao novo objeto.
    *pProximo = novoObjeto;
  }

  return novoObjeto;
}

void removerNode(Node *node)
{
  /*
    Remove o node de objeto apontado por $node e corrige
    os ponteiros dos nodes vizinhos. 
  */

  // Checa so o node passado não é nulo.
  if (node)
  {
    // Salva os ponteiros do node e o exclui.
    Node *anterior = node->anterior;
    Node *proximo = node->proximo;
    free(node);
    // Corrige os ponteiros dos nodes adjacentes.
    if (anterior)
    {
      anterior->proximo = proximo;
    }
    if (proximo)
    {
      proximo->anterior = anterior;
    }
  }
}

void removerListaDoNode(Node **pNode)
{
  /*
    Remove todos os objetos na lista do node apontado
    pelo ponteiro apontado por $pNode e o torna nulo.
  */

  if (pNode)
  {
    // Ponteiro para o node.
    Node *node = *pNode;

    if (node)
    {
      // Primeiro, anda para trás removendo nodes.
      Node *anterior = node->anterior;
      while (anterior)
      {
        Node *temp = anterior;
        anterior = anterior->anterior;
        free(temp);
      }

      // Depois, anda para frente removendo nodes.
      Node *proximo = node->proximo;
      while (proximo)
      {
        Node *temp = proximo;
        proximo = proximo->proximo;
        free(temp);
      }

      // Finalmente, deleta o $node em si.
      free(node);
    }

    // Torna o ponteiro apontado por $pNode nulo.
    *pNode = NULL;
  }
}