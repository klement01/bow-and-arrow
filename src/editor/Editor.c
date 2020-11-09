/*
  Editor de atributos de gráficos.
*/

#include <Configs.h>
#include <Grafico.h>
#include <TerminalIO.h>

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
  Salva os atribudos de $grafico em $caminho, criando um novo arquivo
  se necessário.
*/
void salvarAtributos(GRAFICO *grafico, const char *caminho);

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    fprintf(stderr, "Uso: %s nome_do_arquivo.txt\n", argv[0]);
    return EXIT_FAILURE;
  }

  char *caminho = argv[1];

  // Inicializa o terminal.
  inicializarTerminal();
  curs_set(1);
  raw();
  nodelay(stdscr, false);

  // Carrega o gráfico entrado pelo usuário.
  GRAFICO grafico;
  carregarGrafico(&grafico, caminho);

  // Determina o caminho e carregada os atributos do gráfico.
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

  // Loop principal.
  chtype ch;
  move(1, 1);
  do
  {
    // Salva o ponto atual do cursor e desenha o gráfico.
    int y = getcury(stdscr), x = getcurx(stdscr);
    desenharGrafico(&grafico, stdscr, 1, 1);
    chtype *at = &grafico.atributos[y - 1][x - 1];
    
    // Mostra instruções.
    int origY = grafico.linhas + 5;
    int origX = 3;
    int deltaX = 20;

    mvaddstr(origY + 0, origX, "WASD: MOVIMENTO");

    mvaddstr(origY + 2, origX, "1: PRETO");
    mvaddstr(origY + 3, origX, "2: AZUL");
    mvaddstr(origY + 4, origX, "3: VERDE");
    mvaddstr(origY + 5, origX, "4: CIANO");
    mvaddstr(origY + 6, origX, "5: VERMELHO");
    mvaddstr(origY + 7, origX, "6: MAGENTA");
    mvaddstr(origY + 8, origX, "7: AMARELO");
    mvaddstr(origY + 9, origX, "8: BRANCO");

    mvaddstr(origY + 0, origX + deltaX, "R: REVERSE");
    mvaddstr(origY + 1, origX + deltaX, "I: ITÁLICO");
    mvaddstr(origY + 2, origX + deltaX, "L: STANDOUT");
    mvaddstr(origY + 3, origX + deltaX, "U: UNDERLINE");
    mvaddstr(origY + 4, origX + deltaX, "B: BOLD");
    mvaddstr(origY + 5, origX + deltaX, "N: NORMAL");

    mvaddstr(origY + 7, origX + deltaX, "Q: SAIR");
    
    // Move o cursor ao ponto original.
    move(y, x);

    // Espera uma tecla.
    ch = getch();

    chtype atributosAtuais = *at & (~A_COLOR);
    switch (ch)
    {
    // Cores;
    case '1':
      *at = COLOR_PAIR(PRETO);
      break;
    case '2':
      *at = COLOR_PAIR(AZUL);
      break;
    case '3':
      *at = COLOR_PAIR(VERDE);
      break;
    case '4':
      *at = COLOR_PAIR(CIANO);
      break;
    case '5':
      *at = COLOR_PAIR(VERMELHO);
      break;
    case '6':
      *at = COLOR_PAIR(MAGENTA);
      break;
    case '7':
      *at = COLOR_PAIR(AMARELO);
      break;
    case '8':
      *at = COLOR_PAIR(BRANCO);
      break;
    }
    *at |= atributosAtuais;

    ch = tolower(ch);
    switch (ch)
    {
    // Movimentação.
    case 'a':
      x--;
      break;
    case 'd':
      x++;
      break;
    case 'w':
      y--;
      break;
    case 's':
      y++;
      break;

    // Atributos gerais.
    case 'r':
      *at ^= A_REVERSE;
      break;
    case 'i':
      *at ^= A_ITALIC;
      break;
    case 'l':
      *at ^= A_STANDOUT;
      break;
    case 'u':
      *at ^= A_UNDERLINE;
      break;
    case 'b':
      *at ^= A_BOLD;
      break;
    case 'n':
      *at = 0;
      break;
    }

    // Salva as mudanças em disco.
    salvarAtributos(&grafico, caminho_atr);

    // Limita a posição do cursor.
    if (x < 1)
    {
      x = 1;
    }
    if (x > grafico.colunas)
    {
      x = grafico.colunas;
    }
    if (y < 1)
    {
      y = 1;
    }
    if (y > grafico.linhas)
    {
      y = grafico.linhas;
    }
    move(y, x);
  } while (ch != 'q');

  // Libera a memória e retorna.
  free(caminho_atr);
  fecharTerminal();
  descarregarGrafico(&grafico);

  return EXIT_SUCCESS;
}

void salvarAtributos(GRAFICO *grafico, const char *caminho)
{
  // Tenta abrir o arquivo para escrita. Fecha o programa com erro se
  // falhar.
  FILE *arquivo = fopen(caminho, "wb");
  if (!arquivo)
  {
    fputs("Erro abrindo arquivo para escrita: ", stderr);
    perror(caminho);
    exit(EXIT_FAILURE);
  }

  // Salva os atributos.
  for (int i = 0; i < grafico->linhas; i++)
  {
    int tamanhoLinha = grafico->colunas * sizeof(chtype);
    int numSalvos = fwrite(
        grafico->atributos[i],
        1,
        tamanhoLinha,
        arquivo);
    // Fecha o programa com erro se houver alguma falhar escrevendo no
    // arquivo.
    if (numSalvos != tamanhoLinha)
    {
      fputs("Erro escrevendo atributos no arquivo: ", stderr);
      perror(caminho);
      exit(EXIT_FAILURE);
    }
  }

  fclose(arquivo);
}