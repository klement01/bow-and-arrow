# Nomes padrão dos executáveis.
game_bin   = bow-and-arrow
editor_bin = editor

# Diretórios.
src_dir     = src
include_dir = include
lib_dir     = libs
obj_dir     = obj

# Configurações globais.
CFLAGS += -I$(include_dir) -lm -std=c99

# Configurações exclusivas dos sistemas operacionais.
WINDOWS ?= 0
ifeq ($(WINDOWS), 0)
	# Linux.
	default_cc = c99
	CFLAGS += -lncursesw -lX11

	obj_dir    := $(obj_dir)/linux
	game_bin   := $(game_bin)-linux
	editor_bin := $(editor_bin)-linux
else
	# Windows.
	default_cc = x86_64-w64-mingw32-gcc
	CFLAGS += -L.
	CFLAGS += -lpdcurses
	CFLAGS += -I$(lib_dir)

	obj_dir    := $(obj_dir)/windows
	game_bin   := $(game_bin)-windows.exe
	editor_bin := $(editor_bin)-windows.exe
endif

# Arquivos individuais (do jogo.)
SOURCES = $(wildcard $(src_dir)/*.c)
HEADERS = $(wildcard $(include_dir)/*.h)
OBJS    = $(patsubst $(src_dir)/%.c,$(obj_dir)/%.o,$(SOURCES))

# Configurações do alvo (debug, release.)
DEBUG   ?= 0
RELEASE ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -ggdb -O0
else ifeq ($(RELEASE), 1)
	CFLAGS += -D'NDEBUG' -g0 -O3
endif

# Usa os parâmetros padrão ou definidos pelo usuário.
ifeq ($(origin CC),default)
	CC = $(default_cc)
endif
GAME  ?= $(game_bin)
PAINT ?= $(editor_bin)

# Limpa e compila o jogo e o editor.
all: clean jogo editor

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

$(PAINT): $(obj_dir)/Editor.o $(obj_dir)/Grafico.o $(obj_dir)/TerminalIO.o $(obj_dir)/Timer.o
	$(CC) -o $@ $^ $(CFLAGS)

$(obj_dir)/Editor.o: $(src_dir)/editor/Editor.c $(HEADERS)
	$(CC) -o $@ -c $< $(CFLAGS)