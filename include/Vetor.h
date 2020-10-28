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
} FVETOR;

FVETOR vfSoma(FVETOR v1, FVETOR v2);
FVETOR vfSub(FVETOR v1, FVETOR v2);
FVETOR vfMult(FVETOR v1, float c);
FVETOR vfDiv(FVETOR v1, float c);
FVETOR vfNeg(FVETOR v1);

typedef struct
{
  int x;
  int y;
} IVETOR;

IVETOR viSoma(IVETOR v1, IVETOR v2);
IVETOR viSub(IVETOR v1, IVETOR v2);
IVETOR viMult(IVETOR v1, int c);
IVETOR viDiv(IVETOR v1, int c);
IVETOR viNeg(IVETOR v1);

#endif