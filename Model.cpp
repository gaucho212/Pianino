#include "Model.h"
#include <stdexcept>

Model::Model(const std::string &path)
{
    loadModel(path);
}

void Model::Draw(const Shader& shader, glm::mat4 baseMatrix, std::unordered_map<std::string, glm::mat4> localTransforms) const {
    for (unsigned int i = 0; i < meshes.size(); i++) {
        glm::mat4 finalM = baseMatrix; // Domyślnie pozycja całego pianina
        
        // Sprawdzamy, czy dla tej siatki mamy zaplanowaną animację
        if (localTransforms.find(meshes[i].name) != localTransforms.end()) {
            // Mnożymy macierz pianina przez lokalny ruch klawisza
            finalM = finalM * localTransforms[meshes[i].name];
        }

        // Przesyłamy macierz do shadera
        shader.setMat4("M", finalM);

        meshes[i].Draw(shader);
    }
}

void Model::loadModel(const std::string &path)
{
    Assimp::Importer importer;

    // Wczytujemy plik
    // aiProcess_Triangulate - zamienia wszystkie wielokąty na trójkąty 
    // aiProcess_GenSmoothNormals - jeśli model nie ma wektorów normalnych, Assimp je za nas wyliczy
    // aiProcess_FlipUVs - odwraca tekstury na osi Y 
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

    // Sprawdzamy czy plik się wczytał
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        throw std::runtime_error(std::string("BLAD::ASSIMP::") + importer.GetErrorString());
    }

    // Zapisujemy ścieżkę do katalogu
    directory = path.substr(0, path.find_last_of('/'));

    // Zaczynamy przetwarzanie od głównego węzła
    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    // Przetwarzamy wszystkie siatki przypisane do aktualnego węzła
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    // Rekurencyjnie przetwarzamy wszystkie dzieci tego węzła
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene * scene )
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::string meshName = mesh->mName.C_Str();

    // Wierzchołki
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;

        // Pozycja
        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        // Wektor normalny 
        if (mesh->HasNormals())
        {
            vertex.Normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
        else
        {
            vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
        }

        // Współrzędne tekstury 
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

    // Indeksy
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    //MATERIAŁY I TEKSTURY 
    GLuint meshTextureID = 0; // Domyślnie brak tekstury
    
    if (mesh->mMaterialIndex < scene->mNumMaterials) {
        // Pobieramy materiał z głównej sceny
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // Ładujemy teksturę rozproszenia dla tego materiału
        meshTextureID = loadMaterialTexture(material, scene);
    }

    // Zwracamy Mesh z przypisanym ID tekstury
    return Mesh(meshName, vertices, indices, meshTextureID);
}
GLuint Model::loadMaterialTexture(aiMaterial* mat, const aiScene* /*scene*/) {
    // Sprawdzamy czy materiał ma przypisaną teksturę typu "Diffuse" 
    if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
        aiString str;
        mat->GetTexture(aiTextureType_DIFFUSE, 0, &str);
        
        //Uniwersalne ścieżki
        std::string filename = std::string(str.C_Str());
        std::replace(filename.begin(), filename.end(), '\\', '/');
        
        // Wyciągamy samą nazwę pliku
        std::string justName = filename.substr(filename.find_last_of('/') + 1);
        std::string fullPath = directory + "/" + justName;

        // Sprawdzamy czy już wcześniej załadowaliśmy ten obrazek
        if (loadedTextures.find(justName) == loadedTextures.end()) {
            try {
                // Jeśli nie, tworzymy nową teksturę i zapisujemy w Cache
                // Używamy std::make_shared dla zachowania RAII
                loadedTextures[justName] = std::make_shared<Texture>(fullPath);
                std::cout << "Zaladowano teksture modelu: " << fullPath << "\n";
            } catch (const std::exception& e) {
                std::cerr << "Ostrzezenie: Nie udalo sie zaladowac tekstury " << fullPath << " - uzywam koloru domyslnego.\n";
                return 0; // Zwracamy 0 w razie awarii
            }
        }
        
        // Zwracamy surowe ID tekstury 
        return loadedTextures[justName]->getID(); 
    }
    
    return 0;
}