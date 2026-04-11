CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 -I"C:\msys64\ucrt64\include"

# Linkowanie bibliotek dla środowiska Windows (UCRT64)
LDFLAGS = -lglew32 -lglfw3 -lopengl32 -lgdi32 -lsfml-audio -lsfml-system -lassimp
# Zakładam, że takie masz pliki .cpp na podstawie Twoich nagłówków. 
# Jeśli masz inne, po prostu je tu dopisz (np. glad.c)
SRC = main.cpp Shader.cpp Model.cpp Texture.cpp Mesh.cpp lodepng.cpp
OUT = Pianino3D.exe

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	del $(OUT)