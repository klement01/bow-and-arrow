#ifndef TIMER_H
#define TIMER_H

/*
  Define funções para marcar a passagem de tempo real.
*/

#include <Configs.h>

/*
  Reinicia / inicializa o timer.
*/
void iniciarTimer(void);

/*
  Retorna o tempo em segundos desde a última inicialização do timer.
*/
double timerAtual(void);

/*
  Pausa a execução do programa por alguns segundos.
*/
void pause(double segundos);

#endif