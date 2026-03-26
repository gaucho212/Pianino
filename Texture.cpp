#include "Texture.h"
#include <stdexcept>

Texture::Texture(const std::string& path) {
    std::vector<unsigned char> image;
    unsigned width, height;
    
    // 1. Dekodowanie pliku za pomocą LodePNG
    unsigned error = lodepng::decode(image, width, height, path);

    if (error) {
        throw std::runtime_error("Blad wczytywania tekstury " + path + ": " + lodepng_error_text(error));
    }

    // 2. Generowanie bufora tekstury w OpenGL
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // 3. Przesłanie pikseli z RAM na GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());

    // 4. Automatyczne generowanie Mipmap (mniejszych wersji tekstury dla obiektów w oddali)
    glGenerateMipmap(GL_TEXTURE_2D);

    // 5. Ustawienia filtrowania (Trilinearing - najlepsza jakość z wykładu)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Ustawienia zawijania (powtarzanie tekstury)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // 6. Odpięcie tekstury
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