#ifndef TIMER_H
#define TIMER_H

/*
  Define uma função que retorna quanto tempo se passou
  desde sua última invocação como uma fração de um segundo.
  Valor de retorno da sua primeira invocação é indefinido.
*/

/*
  Reinicia / inicializa o timer.
*/
void reiniciarTimer();

/*
  Retorna o tempo desde a última invocação ou reset como uma fração de
  segundo. Retorna um valor indefinido se chamado sem ser resetado.
*/
double calcularDeltaTempo();

#endif