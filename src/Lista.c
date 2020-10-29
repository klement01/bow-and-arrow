#include <Lista.h>

#include <stdio.h>
#include <stdlib.h>

NODE *criarNodeEm(NODE **pProximo, OBJETO objeto)
{
  /*
    Cria espaço na memória para um node de objeto e o coloca
    na lista antes do node apontado pelo ponteiro apontado por
    $pProximo. Atualiza $proximo para ser o novo node e o retorna.
  */

  NODE *novoNode = NULL;

  if (pProximo)
  {
    // Pointer que aponta para o próximo node.
    NODE *proximo = *pProximo;

    // Tenta alocar espaço para o objeto no heap.
    novoNode = malloc(sizeof(NODE));
    if (!novoNode)
    {
      perror("Erro alocando memória para objeto.");
      exit(EXIT_FAILURE);
    }

    // Configura o novo node.
    novoNode->proximo = proximo;
    novoNode->objeto = objeto;
    if (proximo)
    {
      // Se $proximo não for nulo, usa seu valor de anterior
      // e corrige seus ponteiros.
      novoNode->anterior = proximo->anterior;
      proximo->anterior = novoNode;
    }
    else
    {
      novoNode->anterior = NULL;
    }

    // Atualiza $proximo para apontar ao novo objeto.
    *pProximo = novoNode;
  }

  return novoNode;
}

void removerNode(NODE *node)
{
  /*
    Remove o node de objeto apontado por $node e corrige
    os ponteiros dos nodes vizinhos. 
  */

  // Checa so o node passado não é nulo.
  if (node)
  {
    // Salva os ponteiros do node e o exclui.
    NODE *anterior = node->anterior;
    NODE *proximo = node->proximo;
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

void removerListaDoNode(NODE **pNode)
{
  /*
    Remove todos os objetos na lista do node apontado
    pelo ponteiro apontado por $pNode e o torna nulo.
  */

  if (pNode)
  {
    // Ponteiro para o node.
    NODE *node = *pNode;

    if (node)
    {
      // Primeiro, anda para trás removendo nodes.
      NODE *anterior = node->anterior;
      while (anterior)
      {
        NODE *temp = anterior;
        anterior = anterior->anterior;
        free(temp);
      }

      // Depois, anda para frente removendo nodes.
      NODE *proximo = node->proximo;
      while (proximo)
      {
        NODE *temp = proximo;
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