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
    // 1. Generujemy identyfikatory dla buforów
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // 2. Wiążemy VAO (Vertex Array Object) - to on "zapamięta" konfigurację poniższych buforów
    glBindVertexArray(VAO);

    // 3. Ładujemy wierzchołki do VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Używamy std::vector, więc przesyłamy wskaźnik na pierwszy element i rozmiar całego wektora w bajtach
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // 4. Ładujemy indeksy do EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // 5. Ustawiamy wskaźniki atrybutów wierzchołków (Vertex Attributes)
    // UWAGA: Trzymamy się tutaj standardowego layoutu z nowoczesnego OpenGL.
    // Zamiast szukać atrybutu po nazwie, zakładamy stałe lokalizacje: 0=Pozycja, 1=Normalna, 2=Tekstura

    // Atrybut 0: Pozycja (layout(location = 0) w shaderze)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    // Atrybut 1: Wektor Normalny (layout(location = 1) w shaderze)
    glEnableVertexAttribArray(1);
    // offsetof liczy, ile bajtów od początku struktury Vertex znajduje się zmienna Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));

    // Atrybut 2: Współrzędne tekstury (layout(location = 2) w shaderze)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));

    // 6. Odpinamy VAO, aby przypadkowo go nie zmodyfikować w innej części programu
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
    // GL_TRIANGLES - rysujemy trójkąty
    // indices.size() - ile wierzchołków rysujemy
    // GL_UNSIGNED_INT - typ danych w naszym EBO
    // 0 - offset (od początku bufora)
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // Odpinamy VAO po narysowaniu
    glBindVertexArray(0);

    // Odpinamy teksturę po narysowaniu
    glBindTexture(GL_TEXTURE_2D, 0);
}