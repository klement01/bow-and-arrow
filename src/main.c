#include <Objeto.h>
#include <Grafico.h>
#include <TerminalIO.h>
#include <Timer.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

int main()
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

  // Entra no modo curses.
  inicializarTerminal();

  /*
    Loop: executa continuamente o código principal do jogo,
    realizando alterações a cada tick de processamento.
  */

  while (estado != FIM)
  {
    // Exibe um erro se o tamanho do terminal está incorreto e
    // reinicia um timer nesse caso.
    if (corrigirTamanho())
    {
      reiniciarTimer();
    }

    // Configura o novo estado quando há uma troca de estado.
    if (estado != ultimoEstado)
    {
      // TODO: Código de mudança de estado.
      reiniciarTimer();
    }
    ultimoEstado = estado;
    
    // Calcula o tempo transcorrido desde o último loop.
    double dt = calcularDeltaTempo();

    // Atualiza e desenha os objetos do jogo.
    // TODO: Código para atualizar objetos.

    // Atualiza a tela.
    doupdate();
  }

  /*
    Cleanup: libera qualquer memória que ainda não tenha
    sido liberada antes do fim do programa. Não é realmente
    necessário, pois o SO faz isso automaticamente, mas
    liberar manualmente facilita o debugging com o Valgrind.
  */

  // Sai do modo curses.
  endwin();

  // Descarrega os gráficos.
  descarregarGrafico(&gArqueiro);
  descarregarGrafico(&gBalao);
  descarregarGrafico(&gFlecha);
  descarregarGrafico(&gIconeFlecha);
  descarregarGrafico(&gMonstro);

  // Fim do programa.
  return EXIT_SUCCESS;
}