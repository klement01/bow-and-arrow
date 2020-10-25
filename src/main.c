#include <Objeto.h>
#include <Grafico.h>
#include <TerminalIO.h>
#include <Timer.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
  /*
    Setup: carrega todos os recursos e configura os dados
    dos objetos (jogador, flechas, balões, etc.)
  */

  // Carrega os gráficos.
  Grafico gArqueiro = carregarGrafico("materiais/arqueiro.txt");
  Grafico gBalao = carregarGrafico("materiais/balao.txt");
  Grafico gFlecha = carregarGrafico("materiais/flecha.txt");
  Grafico gIconeFlecha = carregarGrafico("materiais/icone-flecha.txt");
  Grafico gMonstro = carregarGrafico("materiais/monstro.txt");

  // Define os estados em que o jogo pode estar e o estado atual.
  typedef enum
  {
    FIM,
    MENU,
    JOGO
  } Estado;
  Estado estado = MENU;
  Estado ultimoEstado = FIM;
  int nivel = 1;

  /*
    Loop: executa continuamente o código principal do jogo,
    realizando alterações a cada tick de processamento.
  */

  while (estado != FIM)
  {
    // Calcula o tempo transcorrido desde o último loop.
    double dt = calcularDeltaTempo();

    // Configura o novo estado quando há uma troca de estado.
    if (estado != ultimoEstado)
    {
    }
    ultimoEstado = estado;
  }

  /*
    Cleanup: libera qualquer memória que ainda não tenha
    sido liberada antes do fim do programa. Não é realmente
    necessário, pois o SO faz isso automaticamente, mas
    liberar manualmente facilita o debugging com o Valgrind.
  */

  // Descarrega os gráficos.
  descarregarGrafico(&gArqueiro);
  descarregarGrafico(&gBalao);
  descarregarGrafico(&gFlecha);
  descarregarGrafico(&gIconeFlecha);
  descarregarGrafico(&gMonstro);

  // Fim do programa.
  return EXIT_SUCCESS;
}