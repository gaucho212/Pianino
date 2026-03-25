#include "Model.h"
#include <stdexcept>

Model::Model(const std::string &path)
{
    loadModel(path);
}

void Model::Draw(const Shader &shader) const
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i].Draw(shader);
    }
}

void Model::loadModel(const std::string &path)
{
    Assimp::Importer importer;

    // Wczytujemy plik. Flagi konfiguracyjne to magia Assimpa, która robi za nas brudną robotę:
    // aiProcess_Triangulate - zamienia wszystkie wielokąty na trójkąty (OpenGL kocha trójkąty!)
    // aiProcess_GenSmoothNormals - jeśli model nie ma wektorów normalnych, Assimp je za nas wyliczy
    // aiProcess_FlipUVs - odwraca tekstury na osi Y (OpenGL ma punkt (0,0) w lewym dolnym rogu obrazka, a nie górnym)
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

    // Sprawdzamy, czy plik się wczytał i czy scena nie jest niekompletna
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        throw std::runtime_error(std::string("BLAD::ASSIMP::") + importer.GetErrorString());
    }

    // Zapisujemy ścieżkę do katalogu, przyda się na Lab 11 do tekstur
    directory = path.substr(0, path.find_last_of('/'));

    // Zaczynamy przetwarzanie od głównego węzła
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // 1. Przetwarzamy wszystkie siatki przypisane do aktualnego węzła
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // node->mMeshes zawiera tylko indeksy do głównej tablicy siatek w scenie
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    // 2. Rekurencyjnie przetwarzamy wszystkie dzieci tego węzła
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene * /* scene */)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // KROK 1: Wierzchołki
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        // Pozycja
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        // Wektor normalny (upewniamy się, że istnieje)
        if (mesh->HasNormals())
        {
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
        else
        {
            vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
        }

        // Współrzędne tekstury (model może mieć do 8 zestawów tekstur, my bierzemy pierwszy: [0])
        if (mesh->mTextureCoords[0])
        {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // KROK 2: Indeksy
    // Assimp przechowuje indeksy w "twarzach" (faces). Skoro daliśmy flagę aiProcess_Triangulate, każda twarz ma 3 indeksy.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // KROK 3: Zwracamy naszą klasę Mesh
    // (Na tym etapie na razie pomijamy materiały/tekstury - zrobimy to przed Lab 11)
    return Mesh(vertices, indices);
}