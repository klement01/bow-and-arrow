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

typedef enum
{
  JOGADOR,
  FLECHA,
  BALAO
} idObjeto;

typedef struct
{
  idObjeto id;
  fVetor pos;
  fVetor velocidade;
  Grafico *grafico;
} Objeto;

bool atualizarObjeto(float dt);
bool desenharObjeto(float dt);

#endif