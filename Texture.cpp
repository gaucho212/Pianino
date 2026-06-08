#include "Texture.h"
#include <stdexcept>

Texture::Texture(const std::string& path) {
    std::vector<unsigned char> image;
    unsigned width, height;
    
    // Dekodowanie pliku za pomocą LodePNG
    unsigned error = lodepng::decode(image, width, height, path);

    if (error) {
        throw std::runtime_error("Blad wczytywania tekstury " + path + ": " + lodepng_error_text(error));
    }

    // Generowanie bufora tekstury 
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Przesłanie pikseli z RAM na GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());

    // Automatyczne generowanie Mipmap 
    glGenerateMipmap(GL_TEXTURE_2D);

    // Ustawienia filtrowania 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Ustawienia zawijania
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Odpięcie tekstury
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture() {
    glDeleteTextures(1, &textureID);
    std::cout << "Zwolniono teksture o ID: " << textureID << "\n";
}

void Texture::bind(GLuint slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}