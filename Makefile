# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++20 -I/usr/include/SDL2
LDFLAGS = `pkg-config --cflags --libs sdl2 SDL2_ttf`

# Diretórios de fontes e objetos
SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin

# Subdiretórios
SUBDIRS = player platforms wall game
OBJ_SUBDIRS = $(patsubst %, $(OBJ_DIR)/%, $(SUBDIRS))

# Arquivos de origem e objetos
SOURCES = $(wildcard $(SRC_DIR)/*.cpp $(foreach dir, $(SUBDIRS), $(SRC_DIR)/$(dir)/*.cpp))
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))

# Nome do executável
EXEC = $(BIN_DIR)/main

# Regras de construção
all: $(EXEC)

$(EXEC): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_SUBDIRS) $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Utilitário para a configuração do mingw, se necessário
mingw:
	g++ -o $(BIN_DIR)/main.exe $(SOURCES) -I d:/mingw/SDL2-2.0.9/x86_64-w64-mingw32/include/SDL2 -L d:/mingw/SDL2-2.0.9/x86_64-w64-mingw32/lib -lSDL2 -lSDL2main -lmingw32 -w
