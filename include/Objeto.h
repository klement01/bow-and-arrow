#ifndef OBJETO_H
#define OBJETO_H

/*
  Define uma enumeração com identificadores numéricos únicos
  para cada tipo de objeto do jogo e um estrutura de informações
  comuns a todos os objetos.
*/

#include <Grafico.h>
#include <Vetor.h>

#include <stdbool.h>

typedef enum enum_id_objeto
{
  JOGADOR,
  FLECHA,
  BALAO
} ID_OBJETO;

typedef struct struct_objeto
{
  ID_OBJETO id;
  FVETOR pos;
  FVETOR velocidade;
  GRAFICO *grafico;
} OBJETO;

bool atualizarObjeto(float dt);
bool desenharObjeto(float dt);

#endif