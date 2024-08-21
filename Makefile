### Настройки

#DEPENDENCIES = gtk4
DEPENDENCIES = 

BUILD_DIR  = build
SOURCE_DIR = src
TEST_DIR 	 = test
MAIN_FILE  = src/main.c

OUT_FILE 	    = build/sqe
TEST_OUT_FILE = build/tests

THIS_DIR 	 = $(shell pwd)

CC = gcc

# DEDFLAGS не работает с gtk4.
# DEDFLAGS =
# Оригинальные флаги
#DEDFLAGS = -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

# Адаптированные под gcc
DEDFLAGS = -D _DEBUG -ggdb3 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

# Использовать здесь АБСОЛЮТНЫЕ пути, без пробелов.
# Иначе clangd ломается.
# TODO: автоматически преобразовывать их?
CFLAGS = -I$(THIS_DIR)/include -g $(DEDFLAGS) -Wno-unused-parameter #-DGTK_UI
LDFLAGS = -lm $(DEDFLAGS) 

IMPL_SOURCES = $(filter-out $(MAIN_FILE), $(shell find $(SOURCE_DIR) -name '*.c'))
TEST_SOURCES = $(shell find $(TEST_DIR) -name '*.c')

all: test

### Флаги из pkg-config

CFLAGS += $(shell pkg-config $(DEPENDENCIES) --cflags)
LDFLAGS += $(shell pkg-config $(DEPENDENCIES) --libs)

### Компиляция

ALL_SOURCES = $(MAIN_FILE) $(SOURCES) $(TEST_SOURCES)

IMPL_OBJ_FILES = $(foreach FILE,$(IMPL_SOURCES),$(BUILD_DIR)/objects/$(basename $(FILE)).o)
TEST_OBJ_FILES = $(foreach FILE,$(TEST_SOURCES),$(BUILD_DIR)/objects/$(basename $(FILE)).o)
MAIN_OBJ_FILE = $(BUILD_DIR)/objects/$(basename $(MAIN_FILE)).o
ALL_OBJ_FILES = $(IMPL_OBJ_FILES) $(TEST_OBJ_FILES) $(MAIN_OBJ_FILE)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)/objects

$(ALL_OBJ_FILES) : $(BUILD_DIR)/objects/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $^

### Основная программа

$(OUT_FILE) : $(BUILD_DIR) $(IMPL_OBJ_FILES) $(MAIN_OBJ_FILE)
	$(CC) $(LDFLAGS) -o $(OUT_FILE) $(IMPL_OBJ_FILES) $(MAIN_OBJ_FILE)

### Тесты

$(TEST_OUT_FILE) : $(BUILD_DIR) $(IMPL_OBJ_FILES) $(TEST_OBJ_FILES)
	$(CC) $(LDFLAGS) -o $(TEST_OUT_FILE) $(IMPL_OBJ_FILES) $(TEST_OBJ_FILES)

### Отдельные команды.

.PHONY: run
run: $(OUT_FILE)
	$(OUT_FILE)

.PHONY: test
test: $(TEST_OUT_FILE)
	$(TEST_OUT_FILE)


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
