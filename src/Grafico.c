#include <Grafico.h>
#include <Vetor.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GRAFICO carregarGrafico(char *caminho)
{
  // TODO: adicionar suporte para UTF-8.

  // Tenta abrir um arquivo de gráfico,
  // fecha o programa se falhar.
  FILE *arquivo = fopen(caminho, "r");
  if (!arquivo)
  {
    fputs("Erro carregando gráfico ", stderr);
    perror(caminho);
    exit(EXIT_FAILURE);
  }

  // Descobre as dimensões do gráfico.
  int numLinhas = 0;
  int numColunas = 0;
  int numColunasLinhaAtual = 0;
  int charAtual;
  while ((charAtual = fgetc(arquivo)) != EOF)
  {
    if (charAtual == '\n')
    {
      numLinhas++;
      if (numColunasLinhaAtual > numColunas)
      {
        numColunas = numColunasLinhaAtual;
      }
      numColunasLinhaAtual = 0;
    }
    // Conta o número de colunas da linha atual.
    // Número de colunas do gráfico é o valor máximo.
    else
    {
      numColunasLinhaAtual++;
    }
  }
  // Corrige a contagem de linhas no caso em que a última
  // linha não termina com um caractere '\n'.
  if (numColunasLinhaAtual != 0)
  {
    numLinhas++;
    if (numColunasLinhaAtual > numColunas)
    {
      numColunas = numColunasLinhaAtual;
    }
  }

  // Tenta alocar espaço para o gráfico, fecha o programa
  // se não conseguir. Enquanto aloca espaço, inicializa
  // todos os caracteres com ' '.
  char **imagem = (char **)malloc(numLinhas * sizeof(char *));
  if (!imagem)
  {
    fputs("Erro alocando espaço para gráfico ", stderr);
    perror(caminho);
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < numLinhas; i++)
  {
    imagem[i] = (char *)malloc(numColunas * sizeof(char));
    if (!imagem[i])
    {
      fputs("Erro alocando espaço para gráfico ", stderr);
      perror(caminho);
      exit(EXIT_FAILURE);
    }
    memset(imagem[i], ' ', numColunas);
  }

  // Retorna ao início do arquivo e preenche a matriz
  // de gráficos com os caracteres necessários.
  rewind(arquivo);
  int linhaAtual = 0;
  int colunaAtual = 0;
  // int charAtual; (definido acima.)
  while ((charAtual = fgetc(arquivo)) != EOF)
  {
    if (charAtual == '\n')
    {
      linhaAtual++;
      colunaAtual = 0;
    }
    else
    {
      imagem[linhaAtual][colunaAtual] = charAtual;
      colunaAtual++;
    }
  }

  // Fecha o arquivo, monta a estrutura do gráfico e retorna.
  fclose(arquivo);

  GRAFICO grafico = {
      .numLinhas = numLinhas,
      .numColunas = numColunas,
      .imagem = imagem};
  return grafico;
}

void descarregarGrafico(GRAFICO *grafico)
{
  // Libera o espaço alocado para o gráfico.
  for (int i = 0; i < grafico->numLinhas; i++)
  {
    free(grafico->imagem[i]);
    grafico->imagem[i] = NULL;
  }
  free(grafico->imagem);
  grafico->imagem = NULL;
  grafico->numLinhas = 0;
  grafico->numColunas = 0;
}

void desenharGrafico(GRAFICO *grafico, WINDOW *win, int y, int x)
{
  // Calcula as coordenadas para o gráfico centralizado.
  if (y == CENTRO)
  {
    y = (getmaxy(win) - grafico->numLinhas) / 2;
  }
  if (x == CENTRO)
  {
    x = (getmaxx(win) - grafico->numColunas) / 2;
  }
  // Itera sobre as linhas da imagem.
  for (int i = 0; i < grafico->numLinhas; i++)
  {
    // Calcula a coordenada y do char dentro de $win.
    int yChar = y + i;
    // Se essa coordenada fica fora de $win, não desenha o char.
    if (0 < yChar && yChar < getmaxy(win))
    {
      // Itera sobre as colunas da linha.
      for (int j = 0; j < grafico->numColunas; j++)
      {
        // Se o char não for espaço em branco e sua coordenada x ficar
        // dentro da janela, o desenha.
        int xChar = x + j;
        char esseChar = grafico->imagem[i][j];
        if (0 < xChar && xChar < getmaxx(win) && !isspace(esseChar))
        {
          mvwaddch(win, yChar, xChar, esseChar);
        }
      }
    }
  }
}

void desenharGraficoTemporario(char *caminho, WINDOW *win, int y, int x)
{
  GRAFICO tmp = carregarGrafico(caminho);
  desenharGrafico(&tmp, win, y, x);
  descarregarGrafico(&tmp);
}