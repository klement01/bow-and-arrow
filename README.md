# bow-and-arrow
Implementação do jogo [Bow and Arrow - In Search of the Greatest Archer](https://www.classicdosgames.com/game/Bow_and_Arrow_-_In_Search_of_the_Greatest_Archer.html), feita em C usando arte ASCII. Trabalho final da disciplina de Algoritmos e Programação da UFRGS, semestre 2020/1.

# Dependências
* Windows: usa a biblioteca [PDCursesMod](https://www.projectpluto.com/win32a.htm). O código pode ser encontrado na [página do GitHub do projeto](https://github.com/Bill-Gray/PDCursesMod). Esse repositório inclui a versão para wincon 64 bits, compilada com a versão 4.2.0.
* Linux: usa a biblioteca [ncurses](https://invisible-island.net/ncurses/), que vem pré-instalada na maioria das distribuições.

# Compilação:
```bash
git clone https://github.com/klement01/bow-and-arrow
cd bow-and-arrow
make
```
Isso irá gerar os executáveis para Linux do jogo e do editor no diretório do repositório. Para compilar para Windows (no Linux):<br>
```bash
make WINDOWS=1
```
Por padrão, usa o compilador de 64 bits do MinGW.