# Nomes padrão dos executáveis.
game_bin   = bow-and-arrow
editor_bin = editor

# Diretórios.
src_dir     = src
include_dir = include
lib_dir     = libs
obj_dir     = obj

# Arquivos individuais (do jogo.)
SOURCES = $(wildcard $(src_dir)/*.c)
HEADERS = $(wildcard $(include_dir)/*.h)
OBJS    = $(patsubst $(src_dir)/%.c,$(obj_dir)/%.o,$(SOURCES))

# Configurações globais.
CFLAGS += -I$(include_dir) -lm

# Configurações exclusivas dos sistemas operacionais.
WINDOWS ?= 0
ifeq ($(WINDOWS), 0)
	default_cc = c99
	CFLAGS += -lncursesw -lX11
else
	default_cc = x86_64-w64-mingw32-gcc
	CFLAGS += -I$(lib_dir)
	CFLAGS += -L$(lib_dir)
	CFLAGS += -lpdcurses

	game_bin   = $(game_bin).exe
	editor_bin = $(editor_bin).exe
endif

# Configurações do alvo (debug, release.)
DEBUG   ?= 0
RELEASE ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -ggdb -O0
else ifeq ($(RELEASE), 1)
	CFLAGS += -D'NDEBUG' -g0 -O4
endif

# Usa os parâmetros padrão ou definidos pelo usuário.
CC    ?= $(default_cc)
GAME  ?= $(game_bin)
PAINT ?= $(editor_bin)

# Compila o jogo e o editor.
all: jogo editor

# Limpa.
clean:
	find $(obj_dir) -type f -exec rm {} \;
	rm -f $(GAME)
	rm -f $(PAINT)

# Compila o jogo.
jogo: $(GAME) 

$(GAME): $(OBJS) 
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJS): $(obj_dir)/%.o: $(src_dir)/%.c $(HEADERS)
	$(CC) -o $@ -c $< $(CFLAGS)

# Compila o editor.
editor: $(PAINT) 

$(PAINT): $(obj_dir)/Editor.o $(obj_dir)/Grafico.o $(obj_dir)/TerminalIO.o
	$(CC) -o $@ $^ $(CFLAGS)

$(obj_dir)/Editor.o: $(src_dir)/editor/Editor.c $(HEADERS)
	$(CC) -o $@ -c $< $(CFLAGS)