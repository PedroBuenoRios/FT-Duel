# Nome do programa
PROGRAM = main

MAKEFLAGS += --silent

# Compilador
CC = clang++

# Diretórios de arquivos headers .h, .hpp
INCLUDE_DIR := include

# Diretórios de arquivos fontes .c, .cpp
SRC_DIR := src

# Diretórios de bibliotecas .so, .a, .la
LIB_DIR := libs

# Diretório de saída de arquivos, onde o arquivo executável irá ficar e também onde os arquivos objs
OUTPUT_DIR := bin

# Opções do compilador
CFLAGS := -Wall -pedantic -std=c++20 -I$(INCLUDE_DIR) -Wc99-extensions

# Link para as bibliotecas necessárias
LFLAGS := -Llibs -lSDL2 -lSDL2main -lSDL2_image -lraylib 

# Arquivos fontes .c, .cpp
SRC := $(wildcard $(SRC_DIR)/*.cpp)

MAIN := $(SRC_DIR)/main.cpp
# Arquivos objetos .o
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OUTPUT_DIR)/%.o, $(filter-out $(MAIN), $(SRC)))

# Arquivos headers .h, .hpp
HEADERS := $(wildcard $(INCLUDE_DIR)*.h)

shared: libgame.so.2 $(LIB_DIR)
	mv -f $(LIB_DIR)/libgame.so.2 $(LIB_DIR)/libgame.so
	touch $(LIB_DIR)/libgame.so
	
libgame.so.2: $(filter-out $(MAIN), $(SRC))
	$(CC) $(CFLAGS) -fPIC -g $^ -shared -o $(LIB_DIR)/$@ 

# $^ indica todos os pre-requisitos
all: $(filter-out src/context_helper.cpp, $(SRC))
	$(CC) $(LFLAGS) $(CFLAGS) -g $^ -o $(OUTPUT_DIR)/$(PROGRAM)
	
# $@ indica o target atual, alvo
$(OBJS): $(SRC) $(HEADERS) $(OUTPUT_DIR)
	$(CC) $(patsubst $(OUTPUT_DIR)/%.o, $(SRC_DIR)/%.cpp, $@) -g -c -o $@ $(CFLAGS)

$(OUTPUT_DIR):
	mkdir -p $@

$(LIB_DIR):
	mkdir -p $@

OBJS: $(OBJS)
	echo $(OBJS)

run: all
	./$(OUTPUT_DIR)/$(PROGRAM)

cleanLib:
	rm -f $(OBJS) *~ $(LIB_DIR)/*

clean:
	rm -f $(OBJS) *~ $(OUTPUT_DIR)/$(PROGRAM) 
