#include <Vetor.h>

/*
  Vetores fracionários.
*/

FVETOR vfSoma(FVETOR v1, FVETOR v2)
{
  FVETOR v;
  v.x = v1.x + v2.x;
  v.y = v1.y + v2.y;
  return v;
}

FVETOR vfSub(FVETOR v1, FVETOR v2)
{
  FVETOR v;
  v.x = v1.x - v2.x;
  v.y = v1.y - v2.y;
  return v;
}

FVETOR vfMult(FVETOR v1, float c)
{
  FVETOR v;
  v.x = v1.x * c;
  v.y = v1.y * c;
  return v;
}

FVETOR vfDiv(FVETOR v1, float c)
{
  FVETOR v;
  v.x = v1.x / c;
  v.y = v1.y / c;
  return v;
}

FVETOR vfNeg(FVETOR v1)
{
  FVETOR v;
  v.x = -v1.x;
  v.y = -v1.y;
  return v;
}

/*
  Vetores inteiros.
*/

IVETOR viSoma(IVETOR v1, IVETOR v2)
{
  IVETOR v;
  v.x = v1.x + v2.x;
  v.y = v1.y + v2.y;
  return v;
}

IVETOR viSub(IVETOR v1, IVETOR v2)
{
  IVETOR v;
  v.x = v1.x - v2.x;
  v.y = v1.y - v2.y;
  return v;
}

IVETOR viMult(IVETOR v1, int c)
{
  IVETOR v;
  v.x = v1.x * c;
  v.y = v1.y * c;
  return v;
}

IVETOR viDiv(IVETOR v1, int c)
{
  IVETOR v;
  v.x = v1.x / c;
  v.y = v1.y / c;
  return v;
}

IVETOR viNeg(IVETOR v1)
{
  IVETOR v;
  v.x = -v1.x;
  v.y = -v1.y;
  return v;
}