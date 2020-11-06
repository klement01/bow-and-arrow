#ifndef TIMER_H
#define TIMER_H

/*
  Define funções para marcar a passagem de tempo real.
*/

/*
  Reinicia / inicializa o timer.
*/
void iniciarTimer();

/*
  Retorna o tempo em segundos desde a última inicialização do timer.
*/
double timerAtual();

/*
  Pausa a execução do programa por alguns segundos.
*/
void pause(double segundos);

#endif