#!/usr/bin/sh

# Encontra o emulador de terminal para executar o programa.
TERMINAL=$(command -v x-terminal-emulator)
if [ ! "$TERMINAL" ]
then
  echo 'Emulador de terminal não encontrado'
  exit 1
fi

# Encontra o arquivo do jogo.
# TODO: passar caminho do programa como argumento.
ARQUIVO=$(readlink -f bow-and-arrow)
if [ ! "$ARQUIVO" ]
then
  echo 'Jogo não encontrado'
  exit 1
fi

COMANDO="$TERMINAL -T 'Bow and Arrow' -geometry 80x35 -e $ARQUIVO"

# TODO: criar atalho com essa informação.