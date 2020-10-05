#include <Vetor.h>

// Vetores fracionários.
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

// Vetores inteiros.
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