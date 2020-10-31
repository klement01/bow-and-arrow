#ifndef OBJETO_H
#define OBJETO_H

/*
  Define um estrutura e funções para lidar como objetos do jogo.
  Define também os parâmetros base de cada tipo de objeto.
*/

#include <Grafico.h>

#include <stdbool.h>

// Número máximo de objetos que podem estar na tela em um determinado
// momento.
#define MAX_OBJETOS 30

// Distância a qual um objeto pode estar fora da área do jogo sem
// desaparecer, em linhas / colunas.
#define BUFFER 5

/*
  Identificadores únicos para cada tipo de objeto.
*/
typedef enum id_objeto
{
  JOGADOR,
  FLECHA,
  BALAO,
  MONSTRO
} ID_OBJETO;

/*
  Identificador do estado do objeto.
*/
typedef enum estado_obj
{
  INATIVO,
  VIVO,
  MORTO
} ESTADO_OBJ;

/*
  Estrutura com informações sobre um objeto do jogo (seu tipo, posição,
  velocidade e gráfico.)
*/
typedef struct objeto
{
  ID_OBJETO id;
  ESTADO_OBJ estado;
  float x, y;
  float vx, vy;
  GRAFICO grafico;
} OBJETO;

/*
  Atualiza os objetos do jogo de acordo com o tempo transcorrido
  desde o último quadro.
*/
void atualizarObjeto(OBJETO *objeto, float dt);

/*
  Desenha os objetos do jogo na janela e realiza checagem de
  colisões.
*/
void desenharObjeto(OBJETO *objeto, WINDOW *win);

#endif