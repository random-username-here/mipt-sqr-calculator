### Настройки

#DEPENDENCIES = gtk4
DEPENDENCIES = 

BUILD_DIR = build
SOURCE_DIR = src
OUT_FILE = sqe
THIS_DIR = $(shell pwd)

CC = g++

# DEDFLAGS не работает с gtk4.
DEDFLAGS = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

# Использовать здесь АБСОЛЮТНЫЕ пути, без пробелов.
# Иначе clangd ломается.
# TODO: автоматически преобразовывать их?
CFLAGS = -I$(THIS_DIR)/include -g $(DEDFLAGS) -Wno-unused-parameter
LDFLAGS = -lm $(DEDFLAGS) 
SOURCES = $(shell find $(SOURCE_DIR) -name '*.c')

all: run

### Флаги из pkg-config

CFLAGS += $(shell pkg-config $(DEPENDENCIES) --cflags)
LDFLAGS += $(shell pkg-config $(DEPENDENCIES) --libs)

### Компиляция


OBJ_FILES = $(foreach FILE,$(SOURCES),$(BUILD_DIR)/objects/$(basename $(FILE)).o)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)/objects

$(OBJ_FILES) : $(BUILD_DIR)/objects/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $^

$(OUT_FILE) : $(BUILD_DIR) $(OBJ_FILES)
	# 
	$(CC) $(LDFLAGS) -o $(BUILD_DIR)/$(OUT_FILE) $(OBJ_FILES)

### Отдельные команды.

.PHONY: run
run: $(OUT_FILE)
	$(BUILD_DIR)/$(OUT_FILE)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)/*


EMPTY = 
SPACE = $(EMPTY) $(EMPTY)
COMMA = ,

# Для языкового сервера
# https://clangd.llvm.org/config
.clangd:
	echo "CompileFlags:" > .clangd
	echo "  Add: [" $(subst $(SPACE), $(COMMA), $(CFLAGS)) "]" >> .clangd
	echo "  Compiler: " $(CC) >> .clangd
