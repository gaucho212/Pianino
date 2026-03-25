#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <unordered_map>

class Shader {
private:
    GLuint programID;
    // Cache dla lokalizacji zmiennych uniform, poprawia wydajność
    mutable std::unordered_map<std::string, GLint> uniformLocationCache;

public:
    // Konstruktor wczytuje kod z plików, kompiluje i linkuje program
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    
    // Destruktor zwalnia program z pamięci GPU (RAII)
    ~Shader();

    // Blokada kopiowania
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    // Aktywacja shadera
    void use() const;

    // Pobieranie ID programu (przydatne rzadko, ale warto mieć)
    GLuint getID() const { return programID; }

    // Funkcje pomocnicze do przesyłania danych do shadera (Uniformy)
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    void setVec3(const std::string& name, const glm::vec3& vec) const;

private:
    // Funkcja pobierająca lokalizację uniforma z wykorzystaniem cache'u
    GLint getUniformLocation(const std::string& name) const;
    
    // Funkcja pomocnicza do sprawdzania błędów kompilacji/linkowania
    void checkCompileErrors(GLuint shader, const std::string& type) const;
};