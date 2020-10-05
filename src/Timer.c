/*
  Requer POSIX.1b (real time extensions) através da "feature test
  macro" _POSIX_C_SOURCE >=199309L para expor funções da biblioteca
  <time.h> relacionadas ao uso do temporizador do processador. Leia:
  <https://man7.org/linux/man-pages/man2/clock_gettime.2.html>
  <https://man7.org/linux/man-pages/man7/feature_test_macros.7.html>
*/

#include <Timer.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double calcularDeltaTempo()
{
  static struct timespec t1, t2;

  // Tenta obter o tempo atual.
  if (clock_gettime(CLOCK_MONOTONIC_RAW, &t2) == -1)
  {
    perror("Erro obtendo valor do timer");
    exit(EXIT_FAILURE);
  }

  // Intervalos de tempo são separados em duas partes, segundos e
  // nanossegundos, cujas diferenças são calculadas separadamente.
  double dt_sec = t2.tv_sec - t1.tv_sec;
  double dt_nsec = t2.tv_nsec - t1.tv_nsec;
  double dt = dt_sec + dt_nsec / 1e9;

  t1 = t2;

  return dt;
}