prog_name = bow-and-arrow

# Baseado em:
# <https://cs.colby.edu/maxwell/courses/tutorials/maketutor/>

# Caminhos.
source_dir  = src
include_dir = include
build_dir   = build
mat_dir     = materiais
lib_dir     = libs

# Arquivos individuais.
sources = $(wildcard $(source_dir)/*.c)
headers = $(wildcard $(include_dir)/*.h)

# Configurações universais.
common_flags = -I$(include_dir) -std=c99 

# Configurações específicas do Linux.
linux_lib_dir = $(lib_dir)/x11
linux_compiler = c99
linux_flags = -D'XCURSES' -D'_POSIX_C_SOURCE=199309L' -L$(linux_lib_dir)
linux_libs = -lXCurses -lX11 -Wl,-rpath,'$$ORIGIN'

# Configurações específicas do Windows.
windows_lib_dir = $(lib_dir)/win
windows_compiler = x86_64-w64-mingw32-gcc
windows_flags = -D'PDC_DLL_BUILD' -L$(windows_lib_dir)
windows_libs = -lpdcurses 

# Escolhe quais flags de OS usar.
OS ?= LINUX
ifeq ($(OS), LINUX)
	CC = $(linux_compiler)
	REQ_FLAGS = $(common_flags) $(linux_flags) $(linux_libs)
	BIN_NAME = $(prog_name)
	LIB_DIR = $(linux_lib_dir)
else ifeq ($(OS), WINDOWS)
	CC = $(windows_compiler)
	REQ_FLAGS = $(common_flags) $(windows_flags) $(windows_libs)
	BIN_NAME = $(prog_name).exe
	LIB_DIR = $(windows_lib_dir)
else
	$(error Invalid OS)
endif

# Escolhe quais flags especiais usar.
BUILD_TYPE ?= DEBUG 
ifeq ($(BUILD_TYPE), DEBUG)
	SPECIAL_FLAGS = -ggdb -O0
	root = $(build_dir)/debug
else ifeq ($(BUILD_TYPE), RELEASE)
	SPECIAL_FLAGS = -DNDEBUG -g0 -O3
	root = $(build_dir)/release
else
	SPECIAL_FLAGS =
	root = $(build_dir)/default
endif

binary_dir = $(root)/bin
object_dir = $(root)/obj
objects = $(patsubst $(source_dir)/%.c,$(object_dir)/%.o,$(sources))

# Compila e linka o programa.
# Copia executável para raiz.
$(BIN_NAME): $(binary_dir)/$(BIN_NAME)
	cp $^ $@ 
	cp $(LIB_DIR)/* .

# Gera executável.
$(binary_dir)/$(BIN_NAME): $(objects)
	$(CC) -o $@ $^ $(REQ_FLAGS) $(SPECIAL_FLAGS)

# Gera arquivos objeto.
$(objects): $(object_dir)/%.o: $(source_dir)/%.c $(headers)
	$(CC) -o $@ -c $< $(REQ_FLAGS) $(SPECIAL_FLAGS)

#----------------------------------------------------------------------------

# Apaga todos os arquivos das builds.
# Usar 'make clean'
.PHONY: clean

clean:
	find $(build_dir) -type f -exec rm {} \;