#include <Grafico.h>
#include <Vetor.h>

#include <stdio.h>
#include <stdlib.h>

Grafico carregarGrafico(char *caminho)
{
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
  char **str = (char **)malloc(numLinhas * sizeof(char *));
  if (!str)
  {
    fputs("Erro alocando espaço para gráfico ", stderr);
    perror(caminho);
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < numLinhas; i++)
  {
    str[i] = (char *)malloc(numColunas * sizeof(char));
    if (!str[i])
    {
      fputs("Erro alocando espaço para gráfico ", stderr);
      perror(caminho);
      exit(EXIT_FAILURE);
    }
    for (int j = 0; j < numColunas; j++)
    {
      str[i][j] = ' ';
    }
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
      str[linhaAtual][colunaAtual] = charAtual;
      colunaAtual++;
    }
  }

  // Fecha o arquivo, monta a estrutura do gráfico e retorna.
  fclose(arquivo);

  Grafico grafico = {numLinhas, numColunas, str};
  return grafico;
}

void descarregarGrafico(Grafico *grafico)
{
  // Libera o espaço alocado para o gráfico.
  for (int i = 0; i < grafico->numLinhas; i++)
  {
    free(grafico->str[i]);
  }
  free(grafico->str);
}