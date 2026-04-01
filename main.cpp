#include "Window.h"
#include "Shader.h"
#include "Model.h"
#include "Texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <unordered_map>

#include <fstream>
#include <sstream>

int main()
{
    try
    {
        // 1. Inicjalizacja okna
        Window window(1200, 800, "Projekt Pianino 3D - Final");

        // 2. Ładowanie shaderów z plików
        Shader myShader("shader.vert", "shader.frag");

        // 3. Ładowanie modelu 3D
        Model myModel("Tekstury_objekty/Piano.obj");

        // Mówimy shaderowi, że zmienna "tex" używa slotu numer 0
        myShader.use();

        // Zmienne kamery
        glm::vec3 cameraPos = glm::vec3(3.0f, 1.0f, 0.0f);     // Gdzie stoimy
        glm::vec3 cameraFront = glm::vec3(0.0f, -0.3f, -1.0f); // Gdzie patrzymy (lekko w dół i w głąb ekranu)
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);      // Gdzie jest "sufit"

        float cameraSpeed = 3.5f; // Szybkość poruszania się

        // ZMIENNE DO MYSZKI I KĄTÓW EULERA
        window.disableCursor(); // Ukrywamy i blokujemy kursor!

        float yaw = -180.0f; // Yaw na start jestsmy tylem sie sie odwracamy
        float pitch = -20.0f; // Na starcie patrzymy w dol fortepianu

        double lastX = 600.0, lastY = 400.0; // Środek okna (zakładając 1200x800)
        bool firstMouse = true;              // Flaga, żeby kamera nie "skoczyła" przy pierwszym ruchu

        // Zmienne obrotu
        float angleX = 0.0f;
        float angleY = 0.0f;
        float rotationSpeed = 0.5f;
        float lastFrameTime = glfwGetTime();

        // Zmienna przechowująca nasz Field of View (domyślnie 45 stopni)
        float fov = 45.0f;

        // --- ZMIENNE DO ANIMACJI KLAPY PIANINA ---
        bool isPianoOpen = false;     // Stan: otwarte/zamknięte
        float currentLidAngle = 0.0f; // Aktualny kąt otwarcia
        float maxLidAngle = 55.0f;    // Maksymalny kąt otwarcia w stopniach
        float lidSpeed = 80.0f;       // Prędkość otwierania (stopnie na sekundę)
        bool oKeyPressed = false;     // Blokada (żeby klawisz nie "migał")

        // Obliczanie fps
        float time_second = 0.0f;
        int frames = 0;

        // --- KONFIGURACJA DŹWIĘKÓW I KLAWISZY ---
        struct PianoKey {
            int keyCode;             // Przycisk na klawiaturze (GLFW)
            std::string meshName;    // Nazwa siatki z logów Assimpa
            std::string noteName;    // NOWE: Nazwa muzyczna nuty (np. "C4")
            std::string soundPath;   // Ścieżka do pliku audio
            bool isPressed;          // Stan wciśnięcia (blokada przed zacięciem)
            sf::SoundBuffer buffer;  // Pamięć RAM dla dźwięku
            sf::Sound sound;         // Odtwarzacz SFML

            // Zaktualizowany Konstruktor
            PianoKey(int k, std::string m, std::string n, std::string s, bool p) 
                : keyCode(k), meshName(m), noteName(n), soundPath(s), isPressed(p) {}
        };

        // Tworzenie listy klawiszy z nazwami nut!
        // (Możesz tu wpisać dowolne nazwy, np. "C4", "C#4", "D4" itd.)
        std::vector<PianoKey> keys = {
            // ========================================================
            // 1. KLAWISZE TYLKO DLA PIANOLI (Z pliku tekstowego)
            // ========================================================
            
            PianoKey(GLFW_KEY_UNKNOWN, "key58", "A0", "dzwieki/A0.wav", false),
            PianoKey(GLFW_KEY_UNKNOWN, "key57", "B0", "dzwieki/B0.wav", false),
            
            PianoKey(GLFW_KEY_UNKNOWN, "key56", "C1", "dzwieki/C1.wav", false),
            PianoKey(GLFW_KEY_UNKNOWN, "key55", "D1", "dzwieki/D1.wav", false),
            PianoKey(GLFW_KEY_UNKNOWN, "key54", "E1", "dzwieki/E1.wav", false),
            PianoKey(GLFW_KEY_UNKNOWN, "key53", "F1", "dzwieki/F1.wav", false),
            PianoKey(GLFW_KEY_UNKNOWN, "key52", "G1", "dzwieki/G1.wav", false),
            PianoKey(GLFW_KEY_UNKNOWN, "key51", "A1", "dzwieki/A1.wav", false),
            PianoKey(GLFW_KEY_UNKNOWN, "key51", "B1", "dzwieki/B1.wav", false),//Brak

            PianoKey(GLFW_KEY_UNKNOWN, "key50", "C2", "dzwieki/C2.wav", false),
            PianoKey(GLFW_KEY_UNKNOWN, "key49", "D2", "dzwieki/D2.wav", false),
            PianoKey(GLFW_KEY_UNKNOWN, "key48", "E2", "dzwieki/E2.wav", false),//Brak
            PianoKey(GLFW_KEY_UNKNOWN, "key47", "F2", "dzwieki/F2.wav", false),
            PianoKey(GLFW_KEY_UNKNOWN, "key46", "G2", "dzwieki/G2.wav", false),//Brak
            PianoKey(GLFW_KEY_UNKNOWN, "key45", "A2", "dzwieki/A2.wav", false),
            PianoKey(GLFW_KEY_UNKNOWN, "key44", "B2", "dzwieki/B2.wav", false),//Brak

             PianoKey(GLFW_KEY_UNKNOWN,"key43", "C3", "dzwieki/C3.wav", false),
            PianoKey(GLFW_KEY_UNKNOWN, "key42", "D3", "dzwieki/D3.wav", false),//Brak
            PianoKey(GLFW_KEY_UNKNOWN, "key41", "E3", "dzwieki/E3.wav", false),//Brak
            PianoKey(GLFW_KEY_UNKNOWN, "key40", "F3", "dzwieki/F3.wav", false),//Brak
            PianoKey(GLFW_KEY_UNKNOWN, "key39", "G3", "dzwieki/G3.wav", false),//Brak
            PianoKey(GLFW_KEY_UNKNOWN, "key38", "A3", "dzwieki/A3.wav", false),
            PianoKey(GLFW_KEY_UNKNOWN, "key37", "B3", "dzwieki/B3.wav", false),//Brak

           

            // ========================================================
            // 2. KLAWISZE RĘCZNE (A4 - G5) - Graj za pomocą 1-7
            // ========================================================
            PianoKey(GLFW_KEY_1, "key30", "A4", "dzwieki/A4.wav", false),
            PianoKey(GLFW_KEY_2, "key29", "B4", "dzwieki/B4.wav", false),
            PianoKey(GLFW_KEY_3, "key28", "C5", "dzwieki/C5.wav", false),
            PianoKey(GLFW_KEY_4, "key27", "D5", "dzwieki/D5.wav", false),
            PianoKey(GLFW_KEY_5, "key26", "E5", "dzwieki/E5.wav", false),
            PianoKey(GLFW_KEY_6, "key25", "F5", "dzwieki/F5.wav", false),
            PianoKey(GLFW_KEY_7, "key24", "G5", "dzwieki/G5.wav", false)
        };

        // --- SYSTEM AUTOMATYCZNEGO ODTWARZANIA (PIANOLA) ---
        struct NoteEvent {
            float startTime;      // Kiedy klawisz ma się wcisnąć (w sekundach)
            std::string noteName; // Muzyczna nazwa nuty (np. "C4") z pliku txt
            std::string meshName; // Wyliczona nazwa siatki z modelu (np. "key49")
            float duration;       // Jak długo ma być wciśnięty
            bool isPlaying = false;
            bool isFinished = false;
        };

        std::vector<NoteEvent> song;
        std::ifstream file("dzwieki/nuty.txt");
        
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                float start, duration;
                std::string readNote;
                
                // Odczytujemy: CzasStartu, NazwaNuty, CzasTrwania
                if (iss >> start >> readNote >> duration) {
                    NoteEvent note;
                    note.startTime = start;
                    note.noteName = readNote;
                    note.duration = duration;
                    note.meshName = ""; // Domyślnie puste

                    // Szukamy siatki 3D przypisanej do tej nuty
                    for (const auto& key : keys) {
                        if (key.noteName == readNote) {
                            note.meshName = key.meshName;
                            break; // Znaleziono, przerywamy szukanie
                        }
                    }

                    // Zapisujemy nutę tylko wtedy, gdy program rozpoznał jej nazwę
                    if (note.meshName != "") {
                        song.push_back(note);
                    } else {
                        std::cout << "UWAGA: Nierozpoznana nuta '" << readNote << "' w pliku txt!\n";
                    }
                }
            }
            file.close();
            std::cout << "INFO: Zaladowano utwor z pliku nuty.txt! Liczba nut: " << song.size() << "\n";
        } else {
            std::cout << "UWAGA: Nie znaleziono pliku nuty.txt w folderze projektu.\n";
        }

        // Zmienne sterujące trybami gry
        bool isAutoPlaying = false;
        float playbackTime = 0.0f;
        bool pKeyPressed = false; // Blokada dla klawisza 'P'

        // ZABEZPIECZENIE DLA WSL: Wyłączamy spamowanie błędami SFML w konsoli
        sf::err().rdbuf(NULL); 
        bool soundSystemWorking = true;

        // Wczytujemy pliki .wav do pamięci
        for (auto& key : keys) {
            if (key.buffer.loadFromFile(key.soundPath)) {
                key.sound.setBuffer(key.buffer);
            } else {
                soundSystemWorking = false; // Jeśli brakuje pliku lub WSL blokuje audio
            }
        }

        if (!soundSystemWorking) {
            std::cout << "INFO: Uruchomiono bez obslugi dzwieku lub brakuje plikow .wav.\n";
        }


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

            if (firstMouse)
            {
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

            yaw += xoffset;
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
            if (window.isKeyPressed(GLFW_KEY_Z))
            {
                fov -= zoomSpeed;
            }
            // X - Oddalanie (zwiększamy kąt widzenia FOV)
            if (window.isKeyPressed(GLFW_KEY_X))
            {
                fov += zoomSpeed;
            }

            // Zabezpieczenia: nie pozwalamy odwrócić ekranu na lewą stronę (FOV < 1)
            // i nie pozwalamy na zbyt duże zniekształcenie "rybie oko" (FOV > 45)
            if (fov < 1.0f)
                fov = 1.0f;
            if (fov > 45.0f)
                fov = 45.0f;

            // OBLICZAMY NOWĄ MACIERZ PROJEKCJI CO KLATKĘ
            glm::mat4 P = glm::perspective(glm::radians(fov), 1200.0f / 800.0f, 0.1f, 100.0f);

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

            

            // --- PRZEŁĄCZNIK PIANOLI (Klawisz 'P') ---
            if (window.isKeyPressed(GLFW_KEY_P)) {
                if (!pKeyPressed) {
                    pKeyPressed = true;
                    if (!isAutoPlaying) {
                        // START PIANOLI
                        isAutoPlaying = true;
                        playbackTime = 0.0f; // Zerujemy czas
                        // Resetujemy stan wszystkich nut przed każdym odtworzeniem
                        for (auto& note : song) {
                            note.isPlaying = false;
                            note.isFinished = false;
                        }
                        std::cout << "Pianola: START!\n";
                    } else {
                        // RĘCZNE ZATRZYMANIE PIANOLI
                        isAutoPlaying = false;
                        std::cout << "Pianola: STOP!\n";
                    }
                }
            } else {
                pKeyPressed = false;
            }

            // --- ANIMACJA I DŹWIĘK KLAWISZY ---
            std::unordered_map<std::string, glm::mat4> localTransforms;
            float keyPressDepth = -0.5f;

            if (isAutoPlaying) {
                // ==========================================
                // TRYB AUTOMATYCZNY (PIANOLA GRA Z PLIKU)
                // ==========================================
                playbackTime += deltaTime;
                bool allFinished = true; // Sprawdzamy, czy utwór dobiegł końca

                for (auto& note : song) {
                    if (!note.isFinished) {
                        allFinished = false;

                        // 1. Czy nadszedł czas, aby wcisnąć klawisz?
                        if (playbackTime >= note.startTime && !note.isPlaying) {
                            note.isPlaying = true;
                            if (soundSystemWorking) {
                                for (auto& key : keys) {
                                    if (key.meshName == note.meshName) {
                                        key.sound.play(); // Gramy i pozwalamy wybrzmieć do końca (bez ucinania)
                                        break;
                                    }
                                }
                            }
                        }

                        // 2. Opadnięcie klawisza (animacja w dół)
                        if (note.isPlaying) {
                            localTransforms[note.meshName] = glm::translate(glm::mat4(1.0f), glm::vec3(-keyPressDepth, 0.0f, 0.0f));
                        }

                        // 3. Czy nadszedł czas, aby puścić klawisz? (Powrót animacji do góry)
                        if (playbackTime >= note.startTime + note.duration) {
                            note.isPlaying = false;
                            note.isFinished = true;
                            // UWAGA: Nie wywołujemy key.sound.stop(), dzięki czemu dźwięk pięknie wybrzmiewa (efekt pedału)
                        }
                    }
                }

                // Automatyczny powrót do trybu ręcznego, gdy piosenka się skończy
                if (allFinished && song.size() > 0) {
                    isAutoPlaying = false;
                    std::cout << "Pianola: Koniec utworu.\n";
                }

            } else {
                // ==========================================
                // TRYB RĘCZNY (TY GRASZ NA KLAWIATURZE)
                // ==========================================
                for (auto& key : keys) {
                    if (window.isKeyPressed(key.keyCode)) {
                        // Ugięcie klawisza na ekranie
                        localTransforms[key.meshName] = glm::translate(glm::mat4(1.0f), glm::vec3(-keyPressDepth, 0.0f, 0.0f));
                        
                        // Zagraj tylko w momencie pierwszego wciśnięcia
                        if (!key.isPressed) {
                            if (soundSystemWorking) {
                                key.sound.play(); // Gramy i pozwalamy mu naturalnie wybrzmieć
                            }
                            key.isPressed = true;
                        }
                    } else {
                        // Klawisz odskakuje (zdejmujemy blokadę dźwięku)
                        key.isPressed = false; 
                    }
                }
            }
            //Animacja spacji (pedała)
            if (window.isKeyPressed(GLFW_KEY_SPACE))
            {
                glm::mat4 pedalTransform = glm::mat4(1.0f);
                // Pedał zazwyczaj obraca się lekko w dół, więc dodajemy obrót na osi X
                pedalTransform = glm::translate(pedalTransform, glm::vec3(0.0f, -0.5f, 0.0f));
                pedalTransform = glm::rotate(pedalTransform, glm::radians(-5.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                localTransforms["pedal"] = pedalTransform;
            }

            // --- OBSŁUGA OTWIERANIA KLAPY (Klawisz 'O') ---
            if (window.isKeyPressed(GLFW_KEY_O))
            {
                if (!oKeyPressed)
                {
                    isPianoOpen = !isPianoOpen; // Zmieniamy stan na przeciwny
                    oKeyPressed = true;         // Blokujemy do czasu puszczenia klawisza
                }
            }
            else
            {
                oKeyPressed = false; // Klawisz puszczony, zdejmujemy blokadę
            }

            // Płynna zmiana kąta na podstawie stanu
            if (isPianoOpen)
            {
                currentLidAngle += lidSpeed * deltaTime;
                if (currentLidAngle > maxLidAngle)
                    currentLidAngle = maxLidAngle;
            }
            else
            {
                currentLidAngle -= lidSpeed * deltaTime;
                if (currentLidAngle < 0.0f)
                    currentLidAngle = 0.0f;
            }

            // --- ANIMACJA KLAPY (NAPRAWIONY PUNKT OBROTU) ---
            if (currentLidAngle > 0.0f)
            {
                glm::mat4 lidTransform = glm::mat4(1.0f);


                // 1. Definiujemy wektor przesunięcia DO zawiasów (tył klapy).
                glm::vec3 pivotOffset = glm::vec3(0.0f, 76.0f, -120.0f);
                // Operacja 3: Przesuwamy z powrotem na obudowę (translacja dodatnia)
                lidTransform = glm::translate(lidTransform, -pivotOffset);

                // Operacja 2: Obrót (deska obraca się idealnie wokół krawędzi w 0,0,0)
                lidTransform = glm::rotate(lidTransform, glm::radians(currentLidAngle), glm::vec3(1.0f, 0.0f, 0.0f));

                // Operacja 1: Przesuwamy krawędź zawiasu do środka świata (translacja ujemna)
                lidTransform = glm::translate(lidTransform, pivotOffset);

                // Przekazujemy do mapy (nazwa 'pianoTop' z Twojego logu)
                localTransforms["pianoTop"] = lidTransform;
            }

            // Przesłanie macierzy do shadera
            myShader.setMat4("V", V);
            myShader.setVec3("viewPos", cameraPos); // Bardzo ważne dla oświetlenia Blinna-Phonga!
            myShader.setMat4("P", P);               // P wysyłamy jak zwykle

            // Rysowanie modelu
            myModel.Draw(myShader, M, localTransforms);

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