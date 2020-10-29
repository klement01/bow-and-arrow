#ifndef GRAFICO_H
#define GRAFICO_H

/*
  Estrutura de gráficos de um objeto do jogo e métodos
  para carregá-los, descarregá-los e desenhá-los.
*/

#include <TerminalIO.h>

/*
  Constante usada para definir quando um gráfico é centralizado.
  Em teoria, nenhum objeto do jogo vai realmente precisar ser desenhado
  nessa coordenada.
*/
#define CENTRO -128

typedef struct struct_grafico
{
  int numLinhas;
  int numColunas;
  char **imagem;
} GRAFICO;

/*
  Cria uma instância de um gráfico e guarda nela o gráfico no arquivo
  especificado por $caminho.
*/
GRAFICO carregarGrafico(char *caminho);

/*
  Libera a memória ocupada pelo gráfico e torna seus ponteiros nulos.
*/
void descarregarGrafico(GRAFICO *grafico);

/*
  Desenha o $grafico nas coordenadas de $janela. Se o argumento
  $CENTRO for passado para uma das coordenadas, centraliza o gráfico
  dentro da janela nessa coordenada.
*/
void desenharGrafico(GRAFICO *grafico, WINDOW *win, int y, int x);

/*
  Wrapper para $carregarGrafico, $desenharGrafico e $descarregarGrafico.
  Usado para desenhar um gráfico que só precisa ser usado uma vez.
*/
void desenharGraficoTemporario(char *caminho, WINDOW *win, int y, int x);

#endif