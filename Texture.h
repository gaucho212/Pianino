/**
 * @file Texture.h
 * @brief Klasa zarządzająca cyklem życia i ładowaniem tekstur na kartę graficzną.
 */

#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>
#include <iostream>
#include "lodepng.h"

/**
 * @brief Zapewnia obiektowy interfejs do generowania i używania tekstur w OpenGL.
 * * Wykorzystuje zewnętrzną bibliotekę LodePNG do odkodowywania plików graficznych (.png)
 * i automatycznie przygotowuje je do wyświetlenia nakładając mipmapy oraz filtry trójliniowe.
 */
class Texture {
private:
    GLuint textureID; ///< Wewnętrzny identyfikator wygenerowanej tekstury OpenGL.

public:
    /**
     * @brief Konstruktor ładujący plik obrazu z dysku i wysyłający go do pamięci VRAM.
     * @param path Ścieżka do pliku graficznego (np. "Tekstury_objekty/metal.png").
     * @throw std::runtime_error Wyrzucany w przypadku błędnej ścieżki lub uszkodzonego pliku.
     */
    Texture(const std::string& path);
    
    /**
     * @brief Destruktor sprzątający obiekt tekstury z pamięci karty graficznej.
     */
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    /**
     * @brief Zwraca identyfikator przypisany tej teksturze przez sterownik OpenGL.
     * @return Identyfikator GLuint.
     */
    GLuint getID() const { return textureID; }

    /**
     * @brief Aktywuje teksturę przed narysowaniem obiektu.
     * @param slot Gniazdo tekstury (Textur Unit), w którym ma zostać zbindowana (domyślnie GL_TEXTURE0).
     */
    void bind(GLuint slot = 0) const;

    /**
     * @brief Wyłącza aktywną teksturę, aby nie nakładała się na inne, niespokrewnione obiekty.
     */
    void unbind() const;
};