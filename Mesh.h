#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Shader.h"

// Struktura reprezentująca pojedynczy wierzchołek w modelu 3D
struct Vertex {
    glm::vec3 Position;  // Pozycja w przestrzeni (x, y, z)
    glm::vec3 Normal;    // Wektor normalny (kierunek, w którym "patrzy" ściana) - potrzebne do cieniowania
    glm::vec2 TexCoords; // Współrzędne tekstury (u, v) - potrzebne do teksturowania
};

class Mesh {
public:
    // Dane siatki (przechowywane w pamięci RAM)
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Konstruktor: przyjmuje dane i od razu wysyła je na kartę graficzną
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

    // Funkcja rysująca ten konkretny fragment modelu
    void Draw(const Shader& shader) const;

private:
    // Identyfikatory buforów na karcie graficznej (VRAM)
    GLuint VAO; // Vertex Array Object - zapamiętuje konfigurację VBO i EBO
    GLuint VBO; // Vertex Buffer Object - trzyma surowe dane wierzchołków
    GLuint EBO; // Element Buffer Object - trzyma indeksy (kolejność łączenia wierzchołków w trójkąty)

    // Funkcja inicjalizująca bufory OpenGL (wywoływana w konstruktorze)
    void setupMesh();
};