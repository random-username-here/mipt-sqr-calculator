### Настройки

DEPENDENCIES = gtk4

BUILD_DIR = build
SOURCE_DIR = src
OUT_FILE = sqe
THIS_DIR = $(shell pwd)

CC = gcc

# Использовать здесь АБСОЛЮТНЫЕ пути, без пробелов.
# Иначе clangd ломается.
# TODO: автоматически преобразовывать их?
CFLAGS = -I$(THIS_DIR)/include -DGTK_UI -g
LDFLAGS = -lm 
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
