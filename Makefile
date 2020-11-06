prog_name = bow-and-arrow

# Caminhos.
source_dir  = src
include_dir = include
build_dir   = build
mat_dir     = materiais
lib_dir     = libs
script_dir  = etc

# Arquivos individuais.
sources = $(wildcard $(source_dir)/*.c)
headers = $(wildcard $(include_dir)/*.h)

# Configurações universais.
common_flags = -I$(include_dir) -I$(lib_dir) -std=c99 -L. -lm
# common_flags += -std=c99 -Wpedantic
# common_flags += -pedantic-errors
# common_flags += -Werror
# common_flags += -Wall
# common_flags += -Wextra
# common_flags += -Waggregate-return
# common_flags += -Wbad-function-cast
# common_flags += -Wcast-align
# common_flags += -Wcast-qual
# common_flags += -Wfloat-equal
# common_flags += -Wformat=2
# common_flags += -Wlogical-op
# common_flags += -Wmissing-declarations
# common_flags += -Wmissing-include-dirs
# common_flags += -Wmissing-prototypes
# common_flags += -Wnested-externs
# common_flags += -Wpointer-arith
# common_flags += -Wredundant-decls
# common_flags += -Wsequence-point
# common_flags += -Wshadow
# common_flags += -Wstrict-prototypes
# common_flags += -Wswitch
# common_flags += -Wundef
# common_flags += -Wunreachable-code
# common_flags += -Wunused-but-set-parameter
# common_flags += -Wwrite-strings

# Configurações específicas do Linux 64 bits.
linux64_compiler = c99
linux64_lib_dir  = $(lib_dir)/linux64
linux64_libs     = -lncursesw -lX11
linux64_flags    = -D'_POSIX_C_SOURCE=199309L' -L$(linux64_lib_dir) \
	-Wl,-rpath,'$$ORIGIN'

# Configurações específicas do Windows 64 bits.
windows64_compiler = x86_64-w64-mingw32-gcc
windows64_lib_dir  = $(lib_dir)/win64
windows64_libs     = -lpdcurses 
windows64_flags    = -D'PDC_DLL_BUILD' -D'PDC_WIDE' -D'PDC_FORCE_UTF8' \
	-L$(windows64_lib_dir)

# Escolhe quais flags de OS usar.
OS ?= LINUX64
ifeq ($(OS), LINUX64)
	CC = $(linux64_compiler)
	REQ_FLAGS    = $(common_flags) $(linux64_flags) $(linux64_libs)
	BIN_NAME     = $(prog_name)
	os_lib_dir   = $(linux64_lib_dir)
	os_build_dir = $(build_dir)/linux64
else ifeq ($(OS), WIN64)
	CC           = $(windows64_compiler)
	REQ_FLAGS    = $(common_flags) $(windows64_flags) $(windows64_libs)
	BIN_NAME     = $(prog_name).exe
	os_lib_dir   = $(windows64_lib_dir)
	os_build_dir = $(build_dir)/win64
else
	$(error Invalid OS)
endif

# Escolhe quais flags especiais usar.
DEBUG ?= 1 
ifeq ($(DEBUG), 1)
	SPECIAL_FLAGS = -ggdb -O0
	root          = $(os_build_dir)/debug
else
	SPECIAL_FLAGS = -D'NDEBUG' -g0 -O3
	root          = $(os_build_dir)/release
endif

binary_dir = $(root)/bin
object_dir = $(root)/obj
objects    = $(patsubst $(source_dir)/%.c,$(object_dir)/%.o,$(sources))

# Compila e linka o programa.
# Copia executável para raiz.
$(BIN_NAME): $(binary_dir)/$(BIN_NAME)
	cp $^ $@ 2> /dev/null || :
	cp $(os_lib_dir)/* . 2> /dev/null || :
	$(script_dir)/atalho.sh $(OS) $(BIN_NAME)

# Gera executável.
$(binary_dir)/$(BIN_NAME): $(objects)
	$(CC) -o $@ $^ $(REQ_FLAGS) $(SPECIAL_FLAGS)

# Gera arquivos objeto.
$(objects): $(object_dir)/%.o: $(source_dir)/%.c $(headers)
	$(CC) -o $@ -c $< $(REQ_FLAGS) $(SPECIAL_FLAGS)

#--------------------------------------------------------------------------

# Apaga todos os arquivos das builds.
# Usar 'make clean'
.PHONY: clean

clean:
	find $(build_dir) -type f -exec rm {} \;