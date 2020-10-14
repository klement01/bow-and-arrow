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
fVetor vfMult(fVetor v1, float c);
fVetor vfDiv(fVetor v1, float c);
fVetor vfNeg(fVetor v1);

typedef struct
{
  int x;
  int y;
} iVetor;

iVetor viSoma(iVetor v1, iVetor v2);
iVetor viSub(iVetor v1, iVetor v2);
iVetor viMult(iVetor v1, int c);
iVetor viDiv(iVetor v1, int c);
iVetor viNeg(iVetor v1);

#endif