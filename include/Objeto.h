#ifndef OBJETO_H
#define OBJETO_H

/*
  Define um estrutura e funções para lidar como objetos do jogo.
*/

#include <Grafico.h>

#include <stdbool.h>

/*
  Identificadores únicos para cada tipo de objeto.
*/
typedef enum enum_id_objeto
{
  JOGADOR,
  FLECHA,
  BALAO,
  MONSTRO
} ID_OBJETO;

typedef struct struct_objeto
{
  ID_OBJETO id;
  float x, y;
  float vx, vy;
  GRAFICO *grafico;
} OBJETO;

/*
  Atualiza os objetos do jogo de acordo com o tempo transcorrido
  desde o último quadro.
*/
bool atualizarObjeto(OBJETO *objeto, float dt);

/*
  Desenha os objetos do jogo na janela.
*/
bool desenharObjeto(OBJETO *objeto, WINDOW *win);

#endif