#ifndef VETOR_H
#define VETOR_H

/*
  Define um vetores bidimensionais com coordenadas
  inteiras e fracionárias e funções aritméticas
  entre eles com vetores.
*/

typedef struct
{
  float x;
  float y;
} fVetor;

fVetor vfSoma(fVetor v1, fVetor v2);
fVetor vfSub(fVetor v1, fVetor v2);

typedef struct
{
  int x;
  int y;
} iVetor;

iVetor viSoma(iVetor v1, iVetor v2);
iVetor viSub(iVetor v1, iVetor v2);

#endif