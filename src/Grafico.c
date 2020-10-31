#include <Grafico.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  // Dimensões.
  int linhas = 0;
  int colunas = 0;
  int bytesPorLinha = 0;
  // Valores da linha sendo analisada no momento.
  int colunasLinhaAtual = 0;
  int bytesLinhaAtual = 0;
  int byte; // int para acomodar EOF.
  while ((byte = fgetc(arquivo)) != EOF)
  {
    // Nova linha foi encontrada.
    if (byte == '\n')
    {
      linhas++;
      if (colunasLinhaAtual > colunas)
      {
        colunas = colunasLinhaAtual;
      }
      if (bytesLinhaAtual > bytesPorLinha)
      {
        bytesPorLinha = bytesLinhaAtual;
      }
      colunasLinhaAtual = 0;
      bytesLinhaAtual = 0;
    }
    // Incrementa o número de bytes lidos nessa coluna e o
    // número de caracteres lidos, se for o caso.
    else
    {
      bytesLinhaAtual++;
      // Caracteres começam com "0" ou "11". Bytes que começam com
      // "10" são uma continuação do último caractere.
      if ((byte >> 6) != 0b10)
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
    if (bytesLinhaAtual > bytesPorLinha)
    {
      bytesPorLinha = bytesLinhaAtual;
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
    imagem[i] = (char *)malloc(bytesPorLinha * sizeof(char));
    if (!imagem[i])
    {
      fputs("Erro alocando espaço para gráfico ", stderr);
      perror(caminho);
      exit(EXIT_FAILURE);
    }
    memset(imagem[i], ' ', bytesPorLinha);
  }

  // Retorna ao início do arquivo e preenche a matriz
  // de gráficos com os caracteres necessários.
  rewind(arquivo);
  int i = 0;
  int j = 0;
  while ((byte = fgetc(arquivo)) != EOF)
  {
    if (byte == '\n')
    {
      i++;
      j = 0;
    }
    else
    {
      imagem[i][j] = byte;
      j++;
    }
  }

  // Fecha o arquivo, monta a estrutura do gráfico e retorna.
  fclose(arquivo);
  GRAFICO grafico = {
      .linhas = linhas,
      .colunas = colunas,
      .bytesPorLinha = bytesPorLinha,
      .imagem = imagem};
  return grafico;
}

void descarregarGrafico(GRAFICO *grafico)
{
  // Libera o espaço alocado para o gráfico e descarta seus valores
  // (ponteiros para a imagem, metadados.)
  for (int i = 0; i < grafico->linhas; i++)
  {
    free(grafico->imagem[i]);
    grafico->imagem[i] = NULL;
  }
  free(grafico->imagem);
  grafico->imagem = NULL;
  grafico->linhas = 0;
  grafico->colunas = 0;
  grafico->bytesPorLinha = 0;
}

void desenharGrafico(GRAFICO grafico, WINDOW *win, int y, int x)
{
  // Obtém as dimensões da janela.
  int alturaWin = getmaxy(win);
  int larguraWin = getmaxx(win);

  // Calcula as coordenadas para o gráfico centralizado.
  if (y == CENTRO)
  {
    y = centralizarY(win, grafico.linhas);
  }
  if (x == CENTRO)
  {
    x = centralizarX(win, grafico.colunas);
  }

  // Itera sobre as linhas da imagem. Desenha cada caractere individualmente
  // e não desenha sobre as bordas da janela ou fora da janela.
  for (int i = 0; i < grafico.linhas; i++)
  {
    int charY = y + i;
    // Se $charY fica dentro de $win, desenha essa linha.
    if (0 < charY && charY < alturaWin)
    {
      // Move o cursor para o início da linha.
      wmove(win, charY, x);
      for (int j = 0; j < grafico.bytesPorLinha; j++)
      {
        // Determina a posição atual do cursor.
        int charX = getcurx(win);

        // Coleta o byte atual e checa se é um byte de continuação
        // (em UTF-8, bytes do tipo 0b10xxxxxx são continuação de
        // um code point anterior.)
        char esseByte = grafico.imagem[i][j];
        bool continuacao = (esseByte >> 6) == 0b10;

        // Se o cursor estiver fora da tela para a esquerda e o byte
        // atual não for a continuação de um caractere anterior, ou o
        // byte atual for um caractere de espaço vazio, move o cursor
        // para a direita manualmente, sem desenhar o byte.
        if ((charX <= 0 && !continuacao) || isspace(esseByte))
        {
          wmove(win, charY, charX + 1);
        }

        // Se o cursor estiver dentro da tela, desenha o byte (o que
        // também avança o cursor.)
        else if (charX < larguraWin)
        {
          waddch(win, esseByte);
        }
      }
    }
  }
}