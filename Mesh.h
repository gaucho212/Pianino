#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Shader.h"

/**
 * @brief Struktura reprezentująca pojedynczy wierzchołek w modelu 3D.
 * * Zawiera informacje o pozycji, wektorze normalnym (do oświetlenia) 
 * oraz współrzędnych tekstury.
 */
struct Vertex
{
    glm::vec3 Position;  ///< Pozycja w przestrzeni (x, y, z)
    glm::vec3 Normal;    ///< Wektor normalny (kierunek, w którym "patrzy" ściana)
    glm::vec2 TexCoords; ///< Współrzędne tekstury (u, v)
};

/**
 * @brief Klasa odpowiedzialna za przechowywanie i rysowanie pojedynczej siatki trójkątów.
 * * Klasa ładuje dane wierzchołków do buforów OpenGL (VBO, VAO, EBO) 
 * i pozwala na ich wyrenderowanie z użyciem odpowiedniego shadera i tekstury.
 */
class Mesh
{
public:
    std::string name;                  ///< Nazwa siatki wyciągnięta z pliku .obj
    std::vector<Vertex> vertices;      ///< Lista wierzchołków
    std::vector<unsigned int> indices; ///< Lista indeksów tworzących trójkąty
    GLuint textureID;                  ///< ID tekstury w pamięci VRAM GPU

    /**
     * @brief Konstruktor siatki.
     * * Inicjalizuje siatkę i od razu wysyła jej dane na kartę graficzną.
     * * @param name Nazwa siatki (np. "key49").
     * @param vertices Wektor struktur Vertex.
     * @param indices Wektor indeksów określających kolejność rysowania.
     * @param textureID ID załadowanej tekstury (0 jeśli brak).
     */
    Mesh(std::string name, std::vector<Vertex> vertices, std::vector<unsigned int> indices, GLuint textureID);

    /**
     * @brief Rysuje siatkę na ekranie.
     * @param shader Referencja do skompilowanego shadera, który ma zostać użyty.
     */
    void Draw(const Shader &shader) const;

private:
    GLuint VAO; ///< Vertex Array Object
    GLuint VBO; ///< Vertex Buffer Object
    GLuint EBO; ///< Element Buffer Object

    /**
     * @brief Inicjalizuje bufory OpenGL.
     */
    void setupMesh();
};