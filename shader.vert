#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

// To wyślemy do Fragment Shadera
out vec3 FragPos;  
out vec3 Normal;   

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main() {
    // Obliczamy pozycję wierzchołka w świecie 3D (bez kamery i perspektywy)
    FragPos = vec3(M * vec4(aPos, 1.0));
    
    // Obliczamy macierz normalnych. 
    // Dlaczego nie samo "aNormal"? Ponieważ jeśli przeskalujesz model w C++ nieproporcjonalnie 
    // (np. spłaszczysz go), zwykłe wektory normalne się wykrzywią i oświetlenie będzie zepsute.
    // Używamy odwróconej, transponowanej macierzy 3x3, aby temu zapobiec.
    Normal = mat3(transpose(inverse(M))) * aNormal;  
    
    // Standardowa pozycja na ekranie
    gl_Position = P * V * vec4(FragPos, 1.0);
}