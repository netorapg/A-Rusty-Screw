# Compilador e flags
CXX = g++
CXXFLAGS = -std=c++20 -I/usr/include/SDL2
LDFLAGS = `pkg-config --cflags --libs sdl2`

# Diretórios de fontes e objetos
SRC_DIR = .
OBJ_DIR = obj
PLAYER_OBJ_DIR = $(OBJ_DIR)/player
PLATFORMS_OBJ_DIR = $(OBJ_DIR)/platforms
WALL_OBJ_DIR = $(OBJ_DIR)/wall
GAME_OBJ_DIR = $(OBJ_DIR)/game
BIN_DIR = bin

# Arquivos de origem e objeto
SOURCES = $(wildcard $(SRC_DIR)/*.cpp $(SRC_DIR)/player/*.cpp $(SRC_DIR)/platforms/*.cpp $(SRC_DIR)/wall/*.cpp $(SRC_DIR)/game/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
PLAYER_OBJECTS = $(patsubst $(SRC_DIR)/player/%.cpp, $(PLAYER_OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/player/*.cpp))
PLATFORMS_OBJECTS = $(patsubst $(SRC_DIR)/platforms/%.cpp, $(PLATFORMS_OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/platforms/*.cpp))
WALL_OBJECTS = $(patsubst $(SRC_DIR)/wall/%.cpp, $(WALL_OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/wall/*.cpp))
GAME_OBJECTS = $(patsubst $(SRC_DIR)/game/%.cpp, $(GAME_OBJ_DIR)/%.o, $(wildcard $(SRC_DIR)/game/*.cpp))

# Nome do executável
EXEC = $(BIN_DIR)/main

# Regras de construção
all: $(EXEC)

$(EXEC): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(PLAYER_OBJ_DIR)/%.o: $(SRC_DIR)/player/%.cpp
	@mkdir -p $(PLAYER_OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(PLATFORMS_OBJ_DIR)/%.o: $(SRC_DIR)/platforms/%.cpp
	@mkdir -p $(PLATFORMS_OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(WALL_OBJ_DIR)/%.o: $(SRC_DIR)/wall/%.cpp
	@mkdir -p $(WALL_OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(GAME_OBJ_DIR)/%.o: $(SRC_DIR)/game/%.cpp
	@mkdir -p $(GAME_OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Utilitário para a configuração do mingw, se necessário
mingw:
	g++ -o $(BIN_DIR)/main.exe $(SOURCES) -I d:/mingw/SDL2-2.0.9/x86_64-w64-mingw32/include/SDL2 -L d:/mingw/SDL2-2.0.9/x86_64-w64-mingw32/lib -lSDL2 -lSDL2main -lmingw32 -w
