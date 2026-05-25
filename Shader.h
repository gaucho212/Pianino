/**
 * @file Shader.h
 * @brief System zarządzania programami cieniującymi (Shaders) dla karty graficznej.
 */

#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <unordered_map>

/**
 * @brief Klasa odpowiedzialna za wczytywanie, kompilację i zarządzanie cyklem życia shadera.
 * * Zapewnia również liczne metody pomocnicze pozwalające na łatwe przesyłanie zmiennych 
 * (tzw. "Uniformów") bezpośrednio z procesora (C++) na kartę graficzną w trakcie działania programu.
 */
class Shader {
private:
    GLuint programID; ///< Identyfikator połączonego (zlinkowanego) programu shadera.
    
    /// Cache przechowujący zlokalizowane pozycje zmiennych uniform w pamięci karty graficznej w celu przyspieszenia działania pętli gry.
    mutable std::unordered_map<std::string, GLint> uniformLocationCache;

public:
    /**
     * @brief Wczytuje kod źródłowy z plików na dysku, kompiluje Vertex i Fragment shader i tworzy gotowy program.
     * @param vertexPath Ścieżka do pliku shadera wierzchołków (zwykle ".vert").
     * @param fragmentPath Ścieżka do pliku shadera fragmentów (zwykle ".frag").
     */
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    
    /**
     * @brief Usuwa zaprogramowany shader z pamięci karty graficznej po zakończeniu działania aplikacji.
     */
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    /**
     * @brief Aktywuje shader - wszystkie operacje rysowania po wywołaniu tej metody będą przez niego formatowane.
     */
    void use() const;

    /**
     * @brief Zwraca identyfikator programu shadera.
     * @return Identyfikator GLuint.
     */
    GLuint getID() const { return programID; }

    /**
     * @brief Wysyła zmienną typu logicznego (bool) do shadera.
     * @param name Nazwa zmiennej w kodzie GLSL (w pliku .vert lub .frag).
     * @param value Przesyłana wartość logiczna.
     */
    void setBool(const std::string& name, bool value) const;

    /**
     * @brief Wysyła zmienną całkowitoliczbową (int) do shadera.
     * @param name Nazwa zmiennej w kodzie GLSL.
     * @param value Przesyłana wartość całkowita.
     */
    void setInt(const std::string& name, int value) const;

    /**
     * @brief Wysyła zmienną zmiennoprzecinkową (float) do shadera.
     * @param name Nazwa zmiennej w kodzie GLSL.
     * @param value Przesyłana wartość zmiennoprzecinkowa.
     */
    void setFloat(const std::string& name, float value) const;

    /**
     * @brief Wysyła macierz matematyczną 4x4 (np. perspektywy, obrotu) do shadera.
     * @param name Nazwa zmiennej w kodzie GLSL.
     * @param mat Wzorzec struktury glm::mat4.
     */
    void setMat4(const std::string& name, const glm::mat4& mat) const;

    /**
     * @brief Wysyła trójelementowy wektor (np. wektor światła, kolor RGB) do shadera.
     * @param name Nazwa zmiennej w kodzie GLSL.
     * @param vec Wzorzec wektora glm::vec3.
     */
    void setVec3(const std::string& name, const glm::vec3& vec) const;

private:
    /**
     * @brief Odszukuje zoptymalizowany identyfikator zmiennej w pamięci za pomocą zaimplementowanego algorytmu pamięci podręcznej (Cache).
     * @param name Nazwa wyszukiwanego uniforma.
     * @return Adres pamięci typu GLint wewnątrz GPU.
     */
    GLint getUniformLocation(const std::string& name) const;
    
    /**
     * @brief Funkcja systemowa wykonująca walidację poprawności w trakcie kompilacji lub linkowania kodu w GLSL.
     * @param shader Identyfikator sprawdzanego elementu (Vertex, Fragment lub kompletnego programu).
     * @param type Typ (np. "VERTEX", "PROGRAM") używany do wyświetlania błędu tekstowego.
     */
    void checkCompileErrors(GLuint shader, const std::string& type) const;
};