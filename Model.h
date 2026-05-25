/**
 * @file Model.h
 * @brief Deklaracja klasy Model odpowiadającej za wczytywanie i zarządzanie modelami 3D (Assimp).
 */

#pragma once
#include "Mesh.h"
#include "Texture.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory> // Dla std::shared_ptr

/**
 * @brief Klasa reprezentująca kompletny model 3D składający się z wielu siatek (Mesh).
 * * Klasa korzysta z biblioteki Assimp do wczytania plików obiektów przestrzennych
 * (np. .obj), a następnie dzieli je na mniejsze siatki, automatycznie parując je z teksturami.
 */
class Model
{
public:
    std::vector<Mesh> meshes; ///< Wektor przechowujący wszystkie siatki składające się na ten model.

    /**
     * @brief Konstruktor wczytujący model z pliku.
     * @param path Ścieżka do pliku modelu (np. "Tekstury_objekty/Piano.obj").
     */
    Model(const std::string &path);

    /**
     * @brief Rysuje cały model na ekranie.
     * * Wywołuje funkcję Draw() dla każdej podrzędnej siatki z uwzględnieniem
     * jej własnej macierzy transformacji (lokalnego obrotu lub przesunięcia).
     * * @param shader Skompilowany program shadera używany do renderowania.
     * @param baseMatrix Główna macierz transformacji całego obiektu.
     * @param localTransforms Mapa transformacji lokalnych dla poszczególnych siatek (np. ugięcie klawisza).
     */
    void Draw(const Shader &shader, glm::mat4 baseMatrix, std::unordered_map<std::string, glm::mat4> localTransforms = {}) const;

private:
    std::string directory; ///< Katalog, w którym znajduje się plik modelu (służy do wczytywania tekstur).

    /// Pamięć podręczna (cache) dla wczytanych tekstur, aby uniknąć wielokrotnego wczytywania tego samego pliku z dysku.
    std::unordered_map<std::string, std::shared_ptr<Texture>> loadedTextures;

    /**
     * @brief Główna funkcja inicjująca wczytywanie modelu przez bibliotekę Assimp.
     * @param path Ścieżka do pliku.
     */
    void loadModel(const std::string &path);

    /**
     * @brief Przetwarza węzeł w drzewie struktury Assimpa w sposób rekurencyjny.
     * @param node Aktualnie przetwarzany węzeł.
     * @param scene Wskaźnik na całą załadowaną scenę.
     */
    void processNode(aiNode *node, const aiScene *scene);

    /**
     * @brief Konwertuje strukturę siatki Assimpa na wewnętrzną klasę Mesh.
     * @param mesh Siatka wygenerowana przez bibliotekę Assimp.
     * @param scene Wskaźnik na załadowaną scenę (używany do powiązania materiałów).
     * @return Zbudowany obiekt klasy Mesh.
     */
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    /**
     * @brief Funkcja pomocnicza wczytująca teksturę przypisaną do materiału w Assimp.
     * @param mat Wskaźnik na materiał.
     * @param scene Wskaźnik na scenę.
     * @return Identyfikator wygenerowanej tekstury OpenGL.
     */
    GLuint loadMaterialTexture(aiMaterial *mat, const aiScene *scene);
};