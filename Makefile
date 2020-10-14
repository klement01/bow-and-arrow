CC = c99
prog_name = bow-and-arrow

# Baseado em:
# <https://cs.colby.edu/maxwell/courses/tutorials/maketutor/>

# Usado com todas as configurações de compilação.
source_dir = src
include_dir = include
build_dir = build
mat_dir = materiais
base_flags = -D'_POSIX_C_SOURCE=199309L' -std=c99 -I$(include_dir)

sources = $(wildcard $(source_dir)/*.c)
headers = $(wildcard $(include_dir)/*.h)

# Configurações específicas de debug.
# Usar 'make DEBUG=1'
debug_root = $(build_dir)/debug
debug_flags = -ggdb -O0

# Configurações específicas de release.
release_root = $(build_dir)/release
release_flags = -DNDEBUG -g0 -O3

# Escolhe quais flags especiais usar.
DEBUG ?= 1
ifeq ($(DEBUG), 1)
	special_flags = $(debug_flags)
	root = $(debug_root)
else
	special_flags = $(release_flags)
	root = $(release_root)
endif

binary_dir = $(root)/bin
object_dir = $(root)/obj
objects = $(patsubst $(source_dir)/%.c,$(object_dir)/%.o,$(sources))
CFLAGS = $(base_flags) $(special_flags)

# Compila e linka o programa.
# Copia executável para raiz.
$(prog_name): $(binary_dir)/$(prog_name)
	cp $^ $@

# Gera executável.
$(binary_dir)/$(prog_name): $(objects)
	$(CC) -o $@ $^ $(CFLAGS)

# Gera arquivos objeto.
$(objects): $(object_dir)/%.o: $(source_dir)/%.c $(headers)
	$(CC) -o $@ -c $< $(CFLAGS)

#----------------------------------------------------------------------------

# Apaga todos os arquivos das builds.
# Usar 'make clean'
.PHONY: clean

clean:
	find $(build_dir) -type f -exec rm {} \;