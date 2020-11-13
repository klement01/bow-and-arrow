#include <Grafico.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GRAFICO *carregarGrafico(GRAFICO *grafico, const char *caminho)
{
  carregarImagem(grafico, caminho);

  // Altera a string para encontrar os atributos.
  int len = strlen(caminho);
  char *caminho_atr = (char *)malloc(sizeof(char) * (len + 1));
  strcpy(caminho_atr, caminho);
#ifdef WINDOWS
  const char extensao[] = {"pdc"};
#endif
#ifdef LINUX
  const char extensao[] = {"ncr"};
#endif
  strcpy(caminho_atr + len - 3, extensao);

  carregarAtributos(grafico, caminho_atr);

  free(caminho_atr);

  return grafico;
}

void carregarImagem(GRAFICO *grafico, const char *caminho)
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
    else if (byte != '\r')
    {
      bytesLinhaAtual++;
      // Caracteres começam com "0" ou "11". Bytes que começam com
      // "10" são uma continuação do último caractere.
      if (!continuacao(byte))
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
    else if (byte != '\r')
    {
      imagem[i][j] = byte;
      j++;
    }
  }

  // Fecha o arquivo, monta a estrutura do gráfico e retorna.
  fclose(arquivo);
  grafico->linhas = linhas;
  grafico->colunas = colunas;
  grafico->bytesPorLinha = bytesPorLinha;
  grafico->imagem = imagem;
}

void descarregarGrafico(GRAFICO *grafico)
{
  // Libera o espaço alocado para o gráfico e descarta seus valores
  // (ponteiros para a imagem, metadados.)
  if (grafico->imagem)
  {
    for (int i = 0; i < grafico->linhas; i++)
    {
      free(grafico->imagem[i]);
      grafico->imagem[i] = NULL;
    }
    free(grafico->imagem);
  }

  // Libera o espaço alocado para os atributos.
  if (grafico->atributos)
  {
    for (int i = 0; i < grafico->linhas; i++)
    {
      free(grafico->atributos[i]);
      grafico->atributos[i] = NULL;
    }
    free(grafico->atributos);
  }

  // Zera os valores do gráfico.
  grafico->imagem = NULL;
  grafico->atributos = NULL;
  grafico->linhas = 0;
  grafico->colunas = 0;
  grafico->bytesPorLinha = 0;
}

void carregarAtributos(GRAFICO *grafico, const char *caminho)
{
  // Tenta abrir um arquivo de atributos. Se falhar, inicializa
  // os atributos com zero.
  FILE *arquivo = fopen(caminho, "rb");

  // Aloca espaço para os atributos e lê do arquivo ou inicializa
  // com zero. Fecha o programa se falhar.
  chtype **atributos = (chtype **)malloc(grafico->linhas * sizeof(chtype *));
  if (!atributos)
  {
    fputs("Erro alocando espaço para atributos: ", stderr);
    perror(caminho);
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < grafico->linhas; i++)
  {
    // Aloca espaço para uma linha.
    int tamanhoLinha = grafico->colunas * sizeof(chtype);
    atributos[i] = (chtype *)malloc(tamanhoLinha);
    if (!atributos[i])
    {
      fputs("Erro alocando espaço para atributos: ", stderr);
      perror(caminho);
      exit(EXIT_FAILURE);
    }

    // Tenta ler atributos para preencher o vetor de atributos, fecha
    // o programa com um erro se não conseguir.
    if (arquivo)
    {
      int numLidos = fread(
          atributos[i],
          1,
          tamanhoLinha,
          arquivo);
      if (numLidos != tamanhoLinha)
      {
        fputs("Erro lendo atributos de: ", stderr);
        perror(caminho);
        exit(EXIT_FAILURE);
      }
    }

    // Se o arquivo não foi aberto, inicializa todos os atributos com 0.
    else
    {
      memset(atributos[i], 0, tamanhoLinha);
    }
  }

  // Fecha o arquivo se ele foi aberto.
  if (arquivo)
  {
    fclose(arquivo);
  }

  // Guarda os atributos no gráfico.
  grafico->atributos = atributos;
}

// Wrapper para gráfico sem colisão.
void desenharGrafico(GRAFICO *grafico, WINDOW *win, int y, int x)
{
  desenharGraficoComColisao(grafico, win, y, x, NULL, NULL);
}

// Wrapper para gráfico com colisão em todos os seus chars;
bool desenharGraficoComColisao(
    GRAFICO *grafico,
    WINDOW *win,
    int y,
    int x,
    bool fonte[],
    bool destino[])
{
  return desenharGraficoComColisaoLimitada(
      grafico,
      win,
      y,
      x,
      fonte,
      destino,
      -1,
      -1);
}

// Gráfico com colisão em apenas um char (ex.: flechas.)
bool desenharGraficoComColisaoLimitada(
    GRAFICO *grafico,
    WINDOW *win,
    int y,
    int x,
    bool fonte[],
    bool destino[],
    int jColisao,
    int iColisao)
{
  // Obtém as dimensões da janela.
  const int ALTURA = getmaxy(win);
  const int LARGURA = getmaxx(win);

  // Calcula as coordenadas para o gráfico centralizado.
  if (y == CENTRO)
  {
    y = centralizarY(win, grafico->linhas);
  }
  if (x == CENTRO)
  {
    x = centralizarX(win, grafico->colunas);
  }

  // Guarda o estado das colisões.
  bool houveColisao = false;

  // Itera sobre as linhas da imagem. Desenha cada caractere individualmente
  // e não desenha sobre as bordas da janela ou fora da janela.
  for (int j = 0; j < grafico->linhas; j++)
  {
    int charY = y + j;
    // Se $charY fica dentro de $win, desenha essa linha.
    if (0 < charY && charY < ALTURA)
    {
      // Se o x inicial estiver fora da tela, avança pelos bytes
      // até chegar ao início.
      int iByte = 0;
      int charX = x;
      while (charX <= 0 && iByte < grafico->bytesPorLinha)
      {
        char byte = grafico->imagem[j][iByte];
        if (!continuacao(byte))
        {
          charX++;
        }
        iByte++;
      }
      // Move o cursor para o início da linha.
      wmove(win, charY, charX);
      // Enquanto x estiver dentro da tela, imprime os caracteres,
      // pulando os espaços.
      while (charX < LARGURA - 1 && iByte < grafico->bytesPorLinha)
      {
        char byte = grafico->imagem[j][iByte];
        int iChar = charX - x;
        // Pula espaços em branco.
        chtype atributo = grafico->atributos[j][iChar];
        if (isspace(byte))
        {
          wmove(win, charY, charX + 1);
        }
        // Imprime o char e checa colisões;
        else
        {
          waddch(win, byte | atributo);
          // Se a colisão foi limitada, checa se o caractere atual é o
          // caractere desejado.
          if (
              (jColisao < 0 || iColisao < 0) ||
              (j == jColisao && iChar == iColisao))
          {
            // Configura a posição do char como verdadeira no vetor
            // de destino.
            if (destino)
            {
              destino[charY * N_COLUNAS + charX] = true;
            }
            // Checa se a posição do char é verdadeira no vetor de
            // origem.
            if (fonte && fonte[charY * N_COLUNAS + charX])
            {
              houveColisao = true;
            }
          }
        }
        // Atualiza a posição.
        iByte++;
        charX = getcurx(win);
      }
    }
  }
  return houveColisao;
}