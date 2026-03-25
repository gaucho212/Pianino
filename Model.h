#pragma once

#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <vector>
#include <iostream>

class Model {
public:
    // Wektor przechowujący wszystkie siatki składające się na ten model
    std::vector<Mesh> meshes;

    // Konstruktor: przyjmuje ścieżkę do pliku i ładuje model
    Model(const std::string& path);

    // Rysuje cały model (wywołuje Draw dla każdej siatki z osobna)
    void Draw(const Shader& shader) const;

private:
    // Katalog, w którym znajduje się plik (przydatne później do wczytywania tekstur)
    std::string directory;

    // Główne funkcje wczytujące
    void loadModel(const std::string& path);
    
    // Rekurencyjne przetwarzanie węzłów z Assimp
    void processNode(aiNode* node, const aiScene* scene);
    
    // Konwersja siatki Assimp na naszą klasę Mesh
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};