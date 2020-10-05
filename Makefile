CC = c99
_PROG = bow-and-arrow
_DEPS = Grafico.h Objeto.h Timer.h Vetor.h
_OBJ = main.o Grafico.o Objeto.o Timer.o Vetor.o

# Baseado em:
# <https://cs.colby.edu/maxwell/courses/tutorials/maketutor/>

# Usado com todas as configurações de compilação.
SDIR = src
IDIR = include
BDIR = build
MDIR = materiais
CFLAGS = -D'_POSIX_C_SOURCE=199309L' -std=c99 -I$(IDIR)

# Configurações específicas de debug.
# Usar 'make DEBUG=1'
DEBUG_ROOT = $(BDIR)/debug
DEBUG_FLAGS = -ggdb -O0

# Configurações específicas de release.
RELEASE_ROOT = $(BDIR)/release
RELEASE_FLAGS = -DNDEBUG -g0 -O3

# Escolhe quais flags especiais usar.
DEBUG ?= 1
ifeq ($(DEBUG), 1)
	SFLAGS = $(DEBUG_FLAGS)
	ROOT = $(DEBUG_ROOT)
else
	SFLAGS = $(RELEASE_FLAGS)
	ROOT = $(RELEASE_ROOT)
endif
ODIR = $(ROOT)/obj
PROG = $(ROOT)/bin/$(_PROG)

# Deriva os caminhos dos arquivos a partir dos nomes e raizes.
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

# Compila e linka o programa.
$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(SFLAGS)

$(PROG): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(SFLAGS)
	cp $(PROG) .

# Apaga todos os arquivos das builds.
# Usar 'make clean'
.PHONY: clean

clean:
	find $(BDIR) -type f -exec rm {} \;