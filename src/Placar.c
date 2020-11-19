#include <Placar.h>

#include <stdlib.h>
#include <string.h>

FILE *lerScores(const char *caminho, TIPO_JOGADOR placar[], int *numScores)
{
  // Tenta abrir o arquivo especificado pelo caminho.
  FILE *arq = fopen(caminho, "r+");
  if (!arq)
  {
    // Se falhar, tenta criar um arquivo nesse local.
    perror("Erro abrindo arquivo de scores para atualização");
    arq = fopen(caminho, "w");
    if (!arq)
    {
      // Se isso também falhar, fechar o programa.
      perror("Erro criando arquivo de scores");
      exit(EXIT_FAILURE);
    }
    // Tenta reabrir o arquivo criado para leitura.
    fclose(arq);
    arq = fopen(caminho, "r+");
    // Se falhar, fecha o programa.
    if (!arq)
    {
      perror("Erro reabrindo arquivo de scores para leitura");
      exit(EXIT_FAILURE);
    }
  }
  // Tenta ler $NUM_MAX_SCORES, inicializa os scores que não puderam
  // ser lidos com zeros.
  int scoresLidos = fread(placar, sizeof(TIPO_JOGADOR), NUM_MAX_SCORES, arq);
  memset(
      placar + scoresLidos,
      0,
      sizeof(TIPO_JOGADOR) * (NUM_MAX_SCORES - scoresLidos));
  // Atualiza $numScores.
  *numScores = scoresLidos;
  return arq;
}

bool checarScore(int novoScore, TIPO_JOGADOR placar[])
{
  return novoScore > placar[NUM_MAX_SCORES - 1].score;
}

int inserirScore(TIPO_JOGADOR novoScore, TIPO_JOGADOR placar[], int *numScores)
{
  // Itera sobre os elemento do $placar até achar algum com score menor
  // que $novoScore.
  int indice = 0;
  bool incluido = false;
  while (indice < NUM_MAX_SCORES && !incluido)
  {
    // Se for encontrado um elemento com score menor que o de $novoScore,
    // move os outros elementos para frente e insere $novoScore.
    if (placar[indice].score < novoScore.score)
    {
      memmove(
          placar + indice + 1,
          placar + indice,
          sizeof(TIPO_JOGADOR) * (NUM_MAX_SCORES - indice - 1));
      placar[indice] = novoScore;
      incluido = true;
    }
    else
    {
      indice++;
    }
  }
  // Se $novoScore não tiver sido incluido, retorna -1.
  if (!incluido)
  {
    indice = -1;
  }
  // Se tiver sido incluído e o número prévio de scores for menor que
  // o número máximo (ou seja, $novoScore foi incluído sem remover
  // um score antigo) incrementa o contador de scores.
  else if (*numScores < NUM_MAX_SCORES) // && incluido
  {
    // *numScores++ não funciona (?)
    *numScores = *numScores + 1;
  }
  return indice;
}

void salvarScores(FILE *arq, TIPO_JOGADOR placar[], int numScores)
{
  // Tenta escrever $placar no disco. Fecha o programa se falhar.
  rewind(arq);
  int scoresSalvos = fwrite(
      placar,
      sizeof(TIPO_JOGADOR),
      numScores,
      arq);
  if (scoresSalvos != numScores)
  {
    perror("Erro salvando scores");
    exit(EXIT_FAILURE);
  }
}