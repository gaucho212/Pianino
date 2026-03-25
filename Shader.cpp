#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    // 1. Pobranie kodu źródłowego shaderów z plików
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // Upewnienie się, że obiekty ifstream mogą rzucać wyjątkami
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure& e) {
        throw std::runtime_error("BLAD::SHADER::NIE_UDALO_SIE_ODCZYTAC_PLIKU: " + vertexPath + " lub " + fragmentPath);
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. Kompilacja shaderów
    GLuint vertex, fragment;

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // 3. Linkowanie programu
    programID = glCreateProgram();
    glAttachShader(programID, vertex);
    glAttachShader(programID, fragment);
    glLinkProgram(programID);
    checkCompileErrors(programID, "PROGRAM");

    // 4. Sprzątanie (RAII dla samych obiektów shaderów - są już w programie, więc usuwamy je)
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader() {
    glDeleteProgram(programID);
    std::cout << "Zwolniono program shadera o ID: " << programID << "\n";
}

void Shader::use() const {
    glUseProgram(programID);
}

// --- Implementacja funkcji do Uniformów ---

GLint Shader::getUniformLocation(const std::string& name) const {
    // Sprawdzamy, czy lokalizacja jest już w naszym cache
    if (uniformLocationCache.find(name) != uniformLocationCache.end()) {
        return uniformLocationCache[name];
    }

    // Jeśli nie, pytamy OpenGL
    GLint location = glGetUniformLocation(programID, name.c_str());
    if (location == -1) {
        // Nie rzucamy wyjątku, bo czasem uniform zostaje "wycięty" przez kompilator GLSL 
        // jeśli nie jest używany (tzw. dead code elimination), ale warto o tym ostrzec
        std::cerr << "OSTRZEZENIE::SHADER::Nie znaleziono uniforma o nazwie: " << name << "\n";
    }
    
    // Zapisujemy do cache
    uniformLocationCache[name] = location;
    return location;
}

void Shader::setBool(const std::string& name, bool value) const {         
    glUniform1i(getUniformLocation(name), (int)value); 
}
void Shader::setInt(const std::string& name, int value) const { 
    glUniform1i(getUniformLocation(name), value); 
}
void Shader::setFloat(const std::string& name, float value) const { 
    glUniform1f(getUniformLocation(name), value); 
}
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::setVec3(const std::string& name, const glm::vec3& vec) const {
    glUniform3fv(getUniformLocation(name), 1, &vec[0]);
}

// --- Debugowanie błędów kompilacji/linkowania OpenGL ---

void Shader::checkCompileErrors(GLuint shader, const std::string& type) const {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            throw std::runtime_error("BLAD::SHADER_KOMPILACJA_TYPU: " + type + "\n" + infoLog);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            throw std::runtime_error("BLAD::PROGRAM_LINKOWANIE_TYPU: " + type + "\n" + infoLog);
        }
    }
}