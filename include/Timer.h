#ifndef TIMER_H
#define TIMER_H

/*
  Define uma função que retorna quanto tempo se passou
  desde sua última invocação como uma fração de um segundo.
  Valor de retorno da sua primeira invocação é indefinido.
*/

/*
  Retorna o tempo desde a última invocação como uma fração de segundo.
  Retorna 0 na primeira invocação.
*/
double calcularDeltaTempo();

#endif