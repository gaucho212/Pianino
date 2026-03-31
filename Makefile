# Kompilator
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2


#BIBLIOTEKI
LDFLAGS = -lglfw -lGLEW -lGL -lassimp -lsfml-audio -lsfml-system

# Plik wynikowego
TARGET = piano


SRCS = main.cpp Shader.cpp Mesh.cpp Model.cpp lodepng.cpp Texture.cpp
OBJS = $(SRCS:.cpp=.o)

# Główna reguła budowania
all: $(TARGET)

# Linkowanie programu
$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

# Kompilacja poszczególnych plików .cpp do obiektów .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Czyszczenie 
clean:
	rm -f $(OBJS) $(TARGET)

# Reguła pomocnicza do szybkiego odpalenia
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run