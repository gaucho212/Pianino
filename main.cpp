#include "Window.h"
#include "Shader.h"
#include "Model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

int main() {
    try {
        // 1. Inicjalizacja okna
        Window window(800, 600, "Projekt Pianino 3D - Lab 8 (Wczytywanie Modeli)");

        // 2. Ładowanie shaderów z plików
        Shader myShader("shader.vert", "shader.frag");

        // 3. Ładowanie modelu 3D 
        Model myModel("Piano.obj"); 

        // 4. Konfiguracja matryc 
        glm::mat4 V = glm::lookAt(
            glm::vec3(0.0f, 2.0f, 5.0f),  // Pozycja kamery
            glm::vec3(0.0f, 0.0f, 0.0f),  // Punkt na który patrzy 
            glm::vec3(0.0f, 1.0f, 0.0f)   // Wektor "w górę"
        );

        // Projekcja perspektywiczna (kąt widzenia 45 stopni)
        glm::mat4 P = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        // Zmienna do animacji obrotu
        float rotationAngle = 0.0f;

        while (!window.shouldClose()) {
            // Obsługa klawiszy 
            window.processInput();

            // Czyszczenie tła (Ciemny grafitowy)
            glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Aktywacja naszego shadera
            myShader.use();

            // Animacja - powolny obrót modelu z czasem
            rotationAngle = (float)glfwGetTime() * 0.5f; 

            // Tworzenie macierzy Modelu (M)
            glm::mat4 M = glm::mat4(1.0f);
            M = glm::translate(M, glm::vec3(0.0f, -1.0f, 0.0f)); // Przesuwamy lekko w dół
            M = glm::rotate(M, rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f)); // Obracamy wokół osi Y
            M = glm::rotate(M, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // Obrót spowodowany formatem róznica obrotu w OpenGL a .obj
            M = glm::scale(M, glm::vec3(0.02f)); 

            // Przesłanie macierzy do shadera
            myShader.setMat4("M", M);
            myShader.setMat4("V", V);
            myShader.setMat4("P", P);

            // Rysowanie wczytanego modelu!
            myModel.Draw(myShader);

            // Zamiana buforów i przetwarzanie zdarzeń systemu okien
            window.swapBuffers();
            window.pollEvents();
        }
    } catch (const std::exception& e) {
        std::cerr << "Blad krytyczny: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}