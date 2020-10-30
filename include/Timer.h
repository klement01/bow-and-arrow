#ifndef TIMER_H
#define TIMER_H

/*
  Define funções para marcar a passagem de tempo real.
*/

/*
  Reinicia / inicializa o timer.
*/
void reiniciarTimer();

/*
  Retorna o tempo desde a última invocação ou reset como uma fração de
  segundo. Retorna um valor indefinido se chamado sem ser resetado.
*/
float calcularDeltaTempo();

#endif