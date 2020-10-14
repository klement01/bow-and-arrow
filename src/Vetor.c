#include <Vetor.h>

/*
  Vetores fracionários.
*/

fVetor vfSoma(fVetor v1, fVetor v2)
{
  fVetor v;
  v.x = v1.x + v2.x;
  v.y = v1.y + v2.y;
  return v;
}

fVetor vfSub(fVetor v1, fVetor v2)
{
  fVetor v;
  v.x = v1.x - v2.x;
  v.y = v1.y - v2.y;
  return v;
}

fVetor vfMult(fVetor v1, float c)
{
  fVetor v;
  v.x = v1.x * c;
  v.y = v1.y * c;
  return v;
}

fVetor vfDiv(fVetor v1, float c)
{
  fVetor v;
  v.x = v1.x / c;
  v.y = v1.y / c;
  return v;
}

fVetor vfNeg(fVetor v1)
{
  fVetor v;
  v.x = -v1.x;
  v.y = -v1.y;
  return v;
}

/*
  Vetores inteiros.
*/

iVetor viSoma(iVetor v1, iVetor v2)
{
  iVetor v;
  v.x = v1.x + v2.x;
  v.y = v1.y + v2.y;
  return v;
}

iVetor viSub(iVetor v1, iVetor v2)
{
  iVetor v;
  v.x = v1.x - v2.x;
  v.y = v1.y - v2.y;
  return v;
}

iVetor viMult(iVetor v1, int c)
{
  iVetor v;
  v.x = v1.x * c;
  v.y = v1.y * c;
  return v;
}

iVetor vfDiv(iVetor v1, int c)
{
  iVetor v;
  v.x = v1.x / c;
  v.y = v1.y / c;
  return v;
}

iVetor vfNeg(iVetor v1)
{
  iVetor v;
  v.x = -v1.x;
  v.y = -v1.y;
  return v;
}