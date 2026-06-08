#include "Mesh.h"
#include <cstddef> // Potrzebne dla offsetof

Mesh::Mesh(std::string name, std::vector<Vertex> vertices, std::vector<unsigned int> indices, GLuint textureID) {
    this->name = name; // Nazwa
    this->vertices = vertices;// Wierzchołki
    this->indices = indices;// Indexy
    this->textureID = textureID; // Zapisujemy ID tekstury

    setupMesh();
}

void Mesh::setupMesh()
{
    //Generujemy identyfikatory dla buforów
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Wiążemy VAO
    glBindVertexArray(VAO);

    // Ładujemy wierzchołki do VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // Ładujemy indeksy do EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Ustawiamy wskaźniki atrybutów wierzchołków 
    
    // Pozycja (layout(location = 0) w shaderze)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    // Wektor Normalny (layout(location = 1) w shaderze)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));

    // Współrzędne tekstury (layout(location = 2) w shaderze)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));

    // Odpinamy VAO
    glBindVertexArray(0);
}

void Mesh::Draw(const Shader & /*shader*/) const
{

    if (textureID != 0)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }
    // Przed rysowaniem wiążemy odpowiednie VAO
    glBindVertexArray(VAO);

    // Rysujemy elementy bazując na indeksach z EBO
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // Odpinamy VAO po narysowaniu
    glBindVertexArray(0);

    // Odpinamy teksturę po narysowaniu
    glBindTexture(GL_TEXTURE_2D, 0);
}