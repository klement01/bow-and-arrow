#ifndef GRAFICO_H
#define GRAFICO_H

/*
  Estrutura de gráficos de um objeto do jogo e métodos
  para carregá-los, descarregá-los e desenhá-los.
*/

#include <Configs.h>
#include <TerminalIO.h>

/*
  Constante usada para definir quando um gráfico é centralizado.
  Em teoria, nenhum objeto do jogo vai realmente precisar ser desenhado
  nessa coordenada.
*/
#define CENTRO -128

/*
  Estrutura com ponteiro para uma matriz com os caracteres de um
  gráficos e informações sobre esse gráfico.
*/
typedef struct struct_grafico
{
  int linhas;
  int colunas;
  int bytesPorLinha;
  char **imagem;
  chtype **atributos;
} GRAFICO;

/*
  Cria uma instância de um gráfico e guarda nela o gráfico no arquivo
  especificado por $caminho. Também tenta achar o arquivo de atributos
  (mesmo caminho que o gráfico, trocando .txt por .att).
*/
GRAFICO *carregarGrafico(GRAFICO *grafico, const char *caminho);

/*
  Libera a memória ocupada pelo gráfico e torna seus ponteiros nulos.
*/
void descarregarGrafico(GRAFICO *grafico);

/*
  Carrega a imagem de um gráfico.
*/
void carregarImagem(GRAFICO *grafico, const char *caminho);

/*
  Carrega os atributos de um gráfico. Assume que sua imagem já foi
  carregada.
*/
void carregarAtributos(GRAFICO *grafico, const char *caminho);

/*
  Desenha o $grafico nas coordenadas de $janela.
  Se o argumento $CENTRO for passado para uma das coordenadas,
  centraliza o gráfico dentro da janela nessa coordenada.
*/
void desenharGrafico(GRAFICO *grafico, WINDOW *win, int y, int x);

/*
  Desenha o $grafico nas coordenadas de $janela.
  Se o argumento $CENTRO for passado para uma das coordenadas,
  centraliza o gráfico dentro da janela nessa coordenada.
  Se $dst não for nulo, prenche o arranjo com true onde algum char do
  gráfico for desenhado.
  Se $src não for nulo, retorna true se algum char do gráfico for
  desenhado em algum índice com valor verdadeiro desse arranjo.
*/
bool desenharGraficoComColisao(
    GRAFICO *grafico,
    WINDOW *win,
    int y,
    int x,
    bool fonte[],
    bool destino[]);

/*
  Desenha o $grafico nas coordenadas de $janela.
  Se o argumento $CENTRO for passado para uma das coordenadas,
  centraliza o gráfico dentro da janela nessa coordenada.
  Se $dst não for nulo, prenche o arranjo com true onde algum char do
  gráfico for desenhado.
  Se $src não for nulo, retorna true se algum char do gráfico for
  desenhado em algum índice com valor verdadeiro desse arranjo.
  As checagems de colisão só ocorrem com o char na posição indicada por
  $jColisão e $iColisão.
*/
bool desenharGraficoComColisaoLimitada(
    GRAFICO *grafico,
    WINDOW *win,
    int y,
    int x,
    bool fonte[],
    bool destino[],
    int jColisao,
    int iColisao);


#endif