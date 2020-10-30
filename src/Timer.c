#include <Timer.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if defined(_WIN32) || defined(WIN32)
/*
  Implementação do timer para Windows. Usa funções da
  API do Windows, declaradas no header <windows.h>.
*/

#include <windows.h>

// Valor de tempo usado após reinicialização e entre invocações.
LARGE_INTEGER t1;

// Valor de frequência, só precisa ser obtido uma vez.
bool inicializado = false;
LARGE_INTEGER frequencia;

LARGE_INTEGER obterTempoAtual()
{
  // Fecha o programa se houver um erro obtendo o tempo atual.
  LARGE_INTEGER t;
  if (!QueryPerformanceCounter(&t))
  {
    perror("Erro obtendo valor do timer");
    exit(EXIT_FAILURE);
  }
  return t;
}

void reiniciarTimer()
{
  t1 = obterTempoAtual();
  // Se o timer ainda não foi inicializado, tenta obter o valore
  // de frequência para conversão entre ticks e segundos.
  if (!inicializado)
  {
    if (!QueryPerformanceFrequency(&frequencia))
    {
      perror("Erro obtendo frequência do timer");
      exit(EXIT_FAILURE);
    }
  }
  inicializado = true;
}

float calcularDeltaTempo()
{
  assert(inicializado);

  // Checa o valor do timer em ticks.
  LARGE_INTEGER t2 = obterTempoAtual();

  // Calcula a diferença e converte para segundos.
  float dt = ((double)(t2.QuadPart - t1.QuadPart)) / frequencia.QuadPart;

  // Salva o tempo da invocação atual para a próxima invocação.
  t1 = t2;

  return dt;
}

#elif _POSIX_C_SOURCE >= 199309L
/*
  Implementação do timer para Linux (e outros sistemas que
  sigam os padrões POSIX.) Requer POSIX.1b (real time extensions)
  através da "feature test macro" _POSIX_C_SOURCE >= 199309L para expor
  funções da biblioteca <time.h> relacionadas ao uso do temporizador
  do processador em tempo real. Leia:
  <https://man7.org/linux/man-pages/man2/clock_gettime.2.html>
  <https://man7.org/linux/man-pages/man7/feature_test_macros.7.html>
*/

// Valor de tempo usado após reinicialização e entre invocações.
struct timespec t1;

// Valor de inicialização, usado em debugging para checar se o timer
// é usado sem ser inicializado.
bool inicializado = false;

struct timespec obterTempoAtual()
{
  // Fecha o programa se houver um erro obtendo o tempo atual.
  struct timespec t;
  if (clock_gettime(CLOCK_MONOTONIC_RAW, &t) == -1)
  {
    perror("Erro obtendo valor do timer");
    exit(EXIT_FAILURE);
  }
  return t;
}

void reiniciarTimer()
{
  t1 = obterTempoAtual();
  inicializado = true;
}

float calcularDeltaTempo()
{
  assert(inicializado);

  // Tenta obter o tempo atual.
  struct timespec t2 = obterTempoAtual();

  // Intervalos de tempo são separados em duas partes, segundos e
  // nanossegundos, cujas diferenças são calculadas separadamente.
  float dt_sec = t2.tv_sec - t1.tv_sec;
  float dt_nsec = t2.tv_nsec - t1.tv_nsec;
  float dt = dt_sec + dt_nsec / 1e9;

  // Salva o tempo da invocação atual para a próxima invocação.
  t1 = t2;

  return dt;
}

#else
#error Nenhum método de timer definido

#endif