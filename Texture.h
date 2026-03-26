#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>
#include <iostream>
#include "lodepng.h"

class Texture {
private:
    GLuint textureID;

public:
    // Konstruktor ładuje plik .png i wysyła na kartę graficzną
    Texture(const std::string& path);
    
    // Destruktor zwalnia pamięć na GPU
    ~Texture();

    // Blokada kopiowania (aby nie usunąć tekstury podwójnie)
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    GLuint getID() const { return textureID; }

    // Aktywuje teksturę w zadanym "slocie" karty graficznej
    void bind(GLuint slot = 0) const;
    void unbind() const;
};