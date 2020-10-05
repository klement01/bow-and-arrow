#include <Objeto.h>
#include <Grafico.h>
#include <Timer.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
  /*
    Setup: carrega todos os recursos e configura os dados
    dos objetos (jogador, flechas, balões, etc.)
  */

  Grafico gArqueiro = carregarGrafico("materiais/arqueiro.txt");
  Grafico gBalao = carregarGrafico("materiais/balao.txt");
  Grafico gFlecha = carregarGrafico("materiais/flecha.txt");
  Grafico gIconeFlecha = carregarGrafico("materiais/icone-flecha.txt");
  Grafico gMonstro = carregarGrafico("materiais/monstro.txt");
  Grafico gQuadroJogo = carregarGrafico("materiais/quadro-jogo.txt");

  typedef enum
  {
    FIM,
    MENU,
    JOGO
  } Estado;
  Estado estado = MENU;

  /*
    Loop: executa continuamente o código principal do jogo,
    realizando alterações a cada tick de processamento.
  */

  // Inicializa o timer e descarta o seu primeiro valor,
  // que é indefinido.
  calcularDeltaTempo();

  while (estado != FIM)
  {
    double dt = calcularDeltaTempo();
  }

  /*
    Cleanup: libera qualquer memória que ainda não tenha
    sido liberada antes do fim do programa. Não é realmente
    necessário, pois o SO faz isso automaticamente, mas
    liberar manualmente deixa o Valgrind feliz :)
  */

  descarregarGrafico(&gArqueiro);
  descarregarGrafico(&gBalao);
  descarregarGrafico(&gFlecha);
  descarregarGrafico(&gIconeFlecha);
  descarregarGrafico(&gMonstro);
  descarregarGrafico(&gQuadroJogo);

  // Fim do programa.
  return EXIT_SUCCESS;
}