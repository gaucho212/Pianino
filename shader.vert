#version 330 core

// Nasze atrybuty, które ustawiliśmy w Mesh.cpp (layout)
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

// Macierze transformacji z GLM
uniform mat4 M; // Macierz Modelu (przesunięcie, obrót pianina)
uniform mat4 V; // Macierz Widoku (kamera)
uniform mat4 P; // Macierz Projekcji (perspektywa)

void main() {
    // W rdzennym OpenGL mnożymy od prawej do lewej: Projekcja * Widok * Model * Pozycja
    gl_Position = P * V * M * vec4(aPos, 1.0);
}