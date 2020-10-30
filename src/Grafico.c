#include <Grafico.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BYTES_UTF_8 6

// TODO: diferenciar entre chars com largura simples (halfwidth)
// e largura dupla (fullwidth.)

GRAFICO carregarGrafico(char *caminho)
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

  // Descobre as dimensões do gráfico, que é interpretado
  // como um arquivo de texto codificado em UTF-8.
  int linhas = 0;
  int colunas = 0;
  int colunasLinhaAtual = 0;
  int bytesPorColuna = 0;
  int bytesColunaAtual = 0;
  int byteAtual; // int para acomodar EOF.
  while ((byteAtual = fgetc(arquivo)) != EOF)
  {
    // Nova linha foi encontrada.
    if (byteAtual == '\n')
    {
      linhas++;
      if (colunasLinhaAtual > colunas)
      {
        colunas = colunasLinhaAtual;
      }
      if (bytesColunaAtual > bytesPorColuna)
      {
        bytesPorColuna = bytesColunaAtual;
      }
      colunasLinhaAtual = 0;
      bytesColunaAtual = 0;
    }
    // Incrementa o número de bytes lidos nessa coluna e o
    // número de caracteres lidos, se for o caso.
    else
    {
      bytesColunaAtual++;
      // Caracteres começam com "0" ou "11". Bytes que começam com
      // "10" são uma continuação do último caractere.
      if ((byteAtual >> 6) != 0b10)
      {
        colunasLinhaAtual++;
      }
    }
  }
  // Corrige a contagem de linhas no caso em que a última
  // linha não termina com um caractere '\n'.
  if (colunasLinhaAtual != 0)
  {
    linhas++;
    if (colunasLinhaAtual > colunas)
    {
      colunas = colunasLinhaAtual;
    }
    if (bytesColunaAtual > bytesPorColuna)
    {
      bytesPorColuna = bytesColunaAtual;
    }
  }

  // Tenta alocar espaço para o gráfico, fecha o programa
  // se não conseguir. Enquanto aloca espaço, inicializa
  // todos os caracteres com ' '.
  char **imagem = (char **)malloc(linhas * sizeof(char *));
  if (!imagem)
  {
    fputs("Erro alocando espaço para gráfico ", stderr);
    perror(caminho);
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < linhas; i++)
  {
    imagem[i] = (char *)malloc(bytesPorColuna * sizeof(char));
    if (!imagem[i])
    {
      fputs("Erro alocando espaço para gráfico ", stderr);
      perror(caminho);
      exit(EXIT_FAILURE);
    }
    memset(imagem[i], ' ', bytesPorColuna);
  }

  // Retorna ao início do arquivo e preenche a matriz
  // de gráficos com os caracteres necessários.
  rewind(arquivo);
  int linhaAtual = 0;
  int colunaAtual = 0;
  while ((byteAtual = fgetc(arquivo)) != EOF)
  {
    if (byteAtual == '\n')
    {
      linhaAtual++;
      colunaAtual = 0;
    }
    else
    {
      imagem[linhaAtual][colunaAtual] = byteAtual;
      colunaAtual++;
    }
  }

  // Fecha o arquivo, monta a estrutura do gráfico e retorna.
  fclose(arquivo);
  GRAFICO grafico = {
      .linhas = linhas,
      .colunas = colunas,
      .imagem = imagem};
  return grafico;
}

void descarregarGrafico(GRAFICO *grafico)
{
  // Libera o espaço alocado para o gráfico.
  for (int i = 0; i < grafico->linhas; i++)
  {
    free(grafico->imagem[i]);
    grafico->imagem[i] = NULL;
  }
  free(grafico->imagem);
  grafico->imagem = NULL;
  grafico->linhas = 0;
  grafico->colunas = 0;
}

void desenharGrafico(GRAFICO *grafico, WINDOW *win, int y, int x)
{
  // Obtém as dimensões da janela.
  int altura = getmaxy(win);
  int largura = getmaxx(win);

  // Calcula as coordenadas para o gráfico centralizado.
  if (y == CENTRO)
  {
    y = (altura - grafico->linhas) / 2;
  }
  if (x == CENTRO)
  {
    x = (largura - grafico->colunas) / 2;
  }

  // Itera sobre as linhas da imagem.
  for (int i = 0; i < grafico->linhas; i++)
  {
    int charY = y + i;
    // Se $charY fica dentro de $win, desenha essa linha.
    if (0 <= charY && charY <= altura)
    {
      int bytesLidos = 0;
      for (int j = 0; j < grafico->colunas; j++)
      {
        int charX = x + j;
        // Extrai um caractere UTF-8 da linha.
        char primeiroByte = grafico->imagem[i][bytesLidos];
        int numBytesChar;
        // Primeiro byte é um header e é seguido por um ou mais
        // bytes de continuação.
        if (primeiroByte & 0x80)
        {
          numBytesChar = 2;
          while ((primeiroByte << numBytesChar) & 0x80)
          {
            numBytesChar++;
          }
        }
        // Primeiro byte é um char ASCII.
        else
        {
          numBytesChar = 1;
        }
        char esseChar[MAX_BYTES_UTF_8 + 1] = {0};
        memcpy(esseChar, &grafico->imagem[i][bytesLidos], numBytesChar);
        // Desenha o caractere extraído se estiver dentro da janela
        // e não for espaço em branco.
        if (0 <= charX && charX <= largura && !isspace(primeiroByte))
        {
          mvwaddstr(win, charY, charX, esseChar);
        }
        // Avança o contador de bytes lidos.
        bytesLidos += numBytesChar;
      }
    }
  }
}