#include "Window.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

int main()
{
    try
    {
        // 1. Inicjalizacja okna
        Window window(800, 600, "Projekt Pianino 3D - Lab 8 (Wczytywanie Modeli)");

        // 2. Ładowanie shaderów z plików
        Shader myShader("shader.vert", "shader.frag");

        // 3. Ładowanie modelu 3D
        Model myModel("Tekstury_objekty/Piano.obj");

        // Mówimy shaderowi, że zmienna "tex" używa slotu numer 0
        myShader.use();

        

        // Zmienne kamery
        glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, 5.0f);     // Gdzie stoimy
        glm::vec3 cameraFront = glm::vec3(0.0f, -0.3f, -1.0f); // Gdzie patrzymy (lekko w dół i w głąb ekranu)
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);      // Gdzie jest "sufit"

        float cameraSpeed = 3.5f; // Szybkość poruszania się

        // ZMIENNE DO MYSZKI I KĄTÓW EULERA
        window.disableCursor(); // Ukrywamy i blokujemy kursor!

        float yaw = -90.0f; // Yaw ustawiamy na -90.0, aby na starcie kamera patrzyła w stronę ujemnej osi Z
        float pitch = 0.0f; // Na starcie patrzymy prosto przed siebie
        
        double lastX = 400.0, lastY = 300.0; // Środek okna (zakładając 800x600)
        bool firstMouse = true; // Flaga, żeby kamera nie "skoczyła" przy pierwszym ruchu


        // Zmienne obrotu
        float angleX = 0.0f;
        float angleY = 0.0f;
        float rotationSpeed = 0.5f;
        float lastFrameTime = glfwGetTime();

        // Zmienna przechowująca nasz Field of View (domyślnie 45 stopni)
        float fov = 45.0f;

        // Obliczanie fps
        float time_second = 0.0f;
        int frames = 0;

        while (!window.shouldClose())
        {
            // Obsługa klawiszy
            window.processInput();

            // Obliczanie czasu
            float currentFrameTime = glfwGetTime();
            float deltaTime = currentFrameTime - lastFrameTime;
            time_second += deltaTime;
            frames++;

            // Sekunda
            if (time_second >= 1.0f)
            {
                // Wypisanie klatek na sekunde
                std::string newTitle = "Projekt Pianino 3D - " + std::to_string(frames) + " FPS";
                window.setTitle(newTitle);
                // Reset klatek po sekundzie
                frames = 0;
                time_second = 0.0f;
            }
            lastFrameTime = currentFrameTime;

            // --- OBSŁUGA ROZGLĄDANIA SIĘ (MYSZKA) ---
            double mouseX, mouseY;
            window.getCursorPos(mouseX, mouseY);

            if (firstMouse) {
                // W pierwszej klatce po prostu synchronizujemy stare pozycje z nowymi
                lastX = mouseX;
                lastY = mouseY;
                firstMouse = false;
            }

            float xoffset = mouseX - lastX;
            float yoffset = lastY - mouseY; // Odwrócone
            lastX = mouseX;
            lastY = mouseY;

            float sensitivity = 0.005f; // Czułość myszki (możesz zmienić, jeśli jest za szybka/wolna)
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            yaw   += xoffset;
            pitch += yoffset;

            // Zabezpieczenie przed "złamaniem karku" - nie pozwalamy spojrzeć wyżej niż pionowo w górę i dół
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;

            // OBLICZANIE NOWEGO WEKTORA FRONT
            glm::vec3 front;
            // Magia trygonometrii - przeliczenie kątów Eulera na wektor kierunkowy 3D
            front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            front.y = sin(glm::radians(pitch));
            front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            cameraFront = glm::normalize(front);

            // Obsługa klawiszy
            if (window.isKeyPressed(GLFW_KEY_RIGHT))
                angleX += rotationSpeed * deltaTime;
            if (window.isKeyPressed(GLFW_KEY_LEFT))
                angleX -= rotationSpeed * deltaTime;
            if (window.isKeyPressed(GLFW_KEY_UP))
                angleY += rotationSpeed * deltaTime;
            if (window.isKeyPressed(GLFW_KEY_DOWN))
                angleY -= rotationSpeed * deltaTime;


            // Obliczamy "płaski" wektor patrzenia (tylko na osiach X i Z)
            glm::vec3 cameraFrontFlat = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
            float currentSpeed = cameraSpeed * deltaTime;

            // Chodzenie do przodu i do tyłu po PŁASKIM terenie
            if (window.isKeyPressed(GLFW_KEY_W))
                cameraPos += currentSpeed * cameraFrontFlat;
            if (window.isKeyPressed(GLFW_KEY_S))
                cameraPos -= currentSpeed * cameraFrontFlat;

            // Klawisze A i D są już naturalnie płaskie, bo używają iloczynu wektorowego (cross) z osią pionową Y
            if (window.isKeyPressed(GLFW_KEY_A))
                cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * currentSpeed;
            if (window.isKeyPressed(GLFW_KEY_D))
                cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * currentSpeed;

            float zoomSpeed = 30.0f * deltaTime; // Szybkość zoomowania (stopnie na sekundę)

            // Z - Przybliżanie (zmniejszamy kąt widzenia FOV)
            if (window.isKeyPressed(GLFW_KEY_Z)) {
                fov -= zoomSpeed; 
            }
            // X - Oddalanie (zwiększamy kąt widzenia FOV)
            if (window.isKeyPressed(GLFW_KEY_X)) {
                fov += zoomSpeed; 
            }

            // Zabezpieczenia: nie pozwalamy odwrócić ekranu na lewą stronę (FOV < 1) 
            // i nie pozwalamy na zbyt duże zniekształcenie "rybie oko" (FOV > 45)
            if (fov < 1.0f)  fov = 1.0f;
            if (fov > 45.0f) fov = 45.0f;

            // OBLICZAMY NOWĄ MACIERZ PROJEKCJI CO KLATKĘ
            glm::mat4 P = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

            // Czyszczenie tła (Ciemny grafitowy)
            glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Aktywacja naszego shadera
            myShader.use();

            // pozycję kamery
            // 1. OBLICZAMY NOWĄ MACIERZ WIDOKU (KAMERY)
            // lookAt bierze: pozycję kamery, punkt w który patrzymy (pozycja + kierunek), oraz górę
            glm::mat4 V = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

            
            // Kolor obiektu wczytana grafika
            // myShader.setVec3("objectColor", glm::vec3(0.25f, 0.15f, 0.08f));

            // USTAWIENIA ŚWIATŁA 1 (Kierunkowe - chłodne)
            // Wektor kierunku - świeci z góry, z prawej, lekko z tyłu
            myShader.setVec3("dirLightDir", glm::vec3(-0.2f, -1.0f, -0.3f));
            myShader.setVec3("dirLightColor", glm::vec3(0.4f, 0.4f, 0.5f)); // Chłodny błękit/szarość

            // USTAWIENIA ŚWIATŁA 2 (Punktowe - ciepła lampka na pianinie)
            // Ustawiamy lampkę blisko modelu (np. trochę wyżej na osi Y i przesunięte w lewo na X)
            myShader.setVec3("pointLightPos", glm::vec3(-1.0f, 1.5f, 1.0f));
            myShader.setVec3("pointLightColor", glm::vec3(1.0f, 0.8f, 0.4f)); // Ciepły żółty/pomarańcz

            // Tworzenie macierzy Modelu (M)
            glm::mat4 M = glm::mat4(1.0f);
            M = glm::translate(M, glm::vec3(0.0f, -1.0f, 0.0f));
            M = glm::rotate(M, angleX, glm::vec3(0.0f, 1.0f, 0.0f));
            M = glm::rotate(M, angleY, glm::vec3(1.0f, 0.0f, 0.0f));
            M = glm::rotate(M, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            M = glm::scale(M, glm::vec3(0.01f)); // Skalowanie ze wzgledu na duzy plik .obj

            // Przesłanie macierzy do shadera
            myShader.setMat4("M", M);
            myShader.setMat4("V", V);
            myShader.setVec3("viewPos", cameraPos); // Bardzo ważne dla oświetlenia Blinna-Phonga!
            myShader.setMat4("P", P);               // P wysyłamy jak zwykle

            // Rysowanie modelu
            myModel.Draw(myShader);

            // Zamiana buforów i przetwarzanie zdarzeń systemu okien
            window.swapBuffers();
            window.pollEvents();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Blad krytyczny: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}