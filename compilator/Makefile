# Имя конечного исполняемого файла
TARGET    = compilator

RM        := rm -rf

# Директории проекта
PRJ_DIR   := .
OBJ_DIR   := $(PRJ_DIR)/obj
BIN_DIR   := $(PRJ_DIR)

# Папки для поиска заголовочных файлов
INC_DIRS  := $(PRJ_DIR)
INC_DIRS  += $(PRJ_DIR)/../vm
# INC_DIRS  += $(PRJ_DIR)/test
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Компилятор и флаги компиляции
CC        := g++
CFLAGS    := -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline 
CFLAGS    += -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum 
CFLAGS    += -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual
CFLAGS    += -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers
CFLAGS    += -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual 
CFLAGS    += -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel
CFLAGS    += -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE
CFLAGS    += -g
CFLAGS    += $(INC_FLAGS)

# Сбор всех исходных файлов (включая main.c в корне, если он там)
SRC_FILES := $(wildcard $(PRJ_DIR)/*.cpp) \
#             $(wildcard $(PRJ_DIR)/src/*.cpp) \
#             $(wildcard $(PRJ_DIR)/test/*.cpp)

# Преобразование путей: превращаем каждый *.cpp в соответствующий obj/*.o
OBJ_FILES := $(patsubst $(PRJ_DIR)/%,$(OBJ_DIR)/%,$(SRC_FILES:.cpp=.o))

# Итоговый путь к бинарнику
EXEC      := $(BIN_DIR)/$(TARGET)

# Главное правило
all: $(EXEC)

# Правило сборки исполняемого файла
$(EXEC): $(OBJ_FILES)
	@echo ""
	@echo "----[ Linking ]----"
	@echo ""
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJ_FILES) -o $(EXEC)

# Универсальное правило компиляции .cpp файлов в объектные .o внутри папки obj
$(OBJ_DIR)/%.o: $(PRJ_DIR)/%.cpp
	@echo ""
	@echo "----[ Compile: $< ]----"
	@echo ""
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Очистка проекта
clean:
	@echo ""
	@echo "----[ Clean ]----"
	@echo ""
	$(RM) $(OBJ_DIR)

.PHONY: all clean