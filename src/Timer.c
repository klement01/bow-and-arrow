#include <Timer.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/*
  Implementação do timer para Linux (e outros sistemas que
  sigam os padrões POSIX.) Requer POSIX.1b (real time extensions)
  através da "feature test macro" _POSIX_C_SOURCE >= 199309L para expor
  funções da biblioteca <time.h> relacionadas ao uso do temporizador
  do processador em tempo real. Leia:
  <https://man7.org/linux/man-pages/man2/clock_gettime.2.html>
  <https://man7.org/linux/man-pages/man7/feature_test_macros.7.html>
*/
#ifndef _WIN32
#ifndef _POSIX_C_SOURCE
#error "Sistemas não-Windows requerem macro _POSIX_C_SOURCE >= 199309L"
#else
/*
  Retorna o tempo desde a última invocação como uma fração de segundo.
  Retorna 0 na primeira invocação.
*/
double calcularDeltaTempo()
{
  static struct timespec t1, t2;
  static bool inicializado = false;

  // Tenta obter o tempo atual.
  if (clock_gettime(CLOCK_MONOTONIC_RAW, &t2) == -1)
  {
    perror("Erro obtendo valor do timer");
    exit(EXIT_FAILURE);
  }

  // Inicializa o timer se for sua primeira vez rodando.
  if (!inicializado)
  {
    t1 = t2;
    inicializado = true;
  }

  // Intervalos de tempo são separados em duas partes, segundos e
  // nanossegundos, cujas diferenças são calculadas separadamente.
  double dt_sec = t2.tv_sec - t1.tv_sec;
  double dt_nsec = t2.tv_nsec - t1.tv_nsec;
  double dt = dt_sec + dt_nsec / 1e9;

  // Salva o tempo da invocação atual para a próxima invocação.
  t1 = t2;

  return dt;
}
#endif

/*
  Implementação do timer para Windows. Usa funções da
  API do Windows, declaradas no header <windows.h>.
*/
#else
#include <windows.h>
/*
  Retorna o tempo desde a última invocação como uma fração de segundo.
  Retorna 0 na primeira invocação.
*/
double calcularDeltaTempo()
{
  static LARGE_INTEGER t1, t2;
  static LARGE_INTEGER frequency;
  static bool inicializado = false;

  // Checa o valor do timer em ticks.
  if (!QueryPerformanceCounter(&t2))
  {
    perror("Erro obtendo valor do timer");
    exit(EXIT_FAILURE);
  }

  // Inicializa o timer se for sua primeira vez rodando.
  if (!inicializado)
  {
    // Checa a frequência em ticks por segundo.
    if (!QueryPerformanceFrequency(&frequency))
    {
      perror("Erro obtendo frequência do timer");
      exit(EXIT_FAILURE);
    }
    t1 = t2;
    inicializado = true;
  }

  // Calcula a diferença e converte para segundos.
  double dt = ((double)(t2.QuadPart - t1.QuadPart)) / frequency.QuadPart;

  // Salva o tempo da invocação atual para a próxima invocação.
  t1 = t2;

  return dt;
}

#endif