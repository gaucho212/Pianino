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

#include <cstdlib> 

#include <optional>

// Globalna definicja struktury do automatycznego grania
struct NoteEvent
{
    float startTime;      // Kiedy klawisz ma się wcisnąć 
    std::string noteName; // Muzyczna nazwa nuty z pliku txt
    std::string meshName; // Wyliczona nazwa siatki z modelu 
    float duration;       // Jak długo ma być wciśnięty
    bool isPlaying = false;
    bool isFinished = false;
};

int main()
{
    try
    {
        //Menu konsoli
        int song_choise = 0;
        std::vector<NoteEvent> song;

        std::cout << "Podaj numer muzyki:\n";
        std::cout << "1. Oda do radosci - Beethoven\n";
        std::cout << "2. Walc a-moll - Chopin\n";
        std::cout << "3. Preludium C-dur - Bach\n";
        std::cout << "4. Zagraj wlasny plik MIDI (.mid)\n";
        std::cout << "5. Załadowany plik nuty.txt\n";
        std::cout << "Twoj wybor: ";
        std::cin >> song_choise;

        std::string filePath = "dzwieki/nuty.txt";

        if (song_choise == 4)
        {
            std::string midiName;
            std::cout << "Podaj nazwe pliku MIDI (musi byc w folderze projektu, np. piosenka.mid): ";
            std::cin >> midiName;

            // Budujemy komendę do terminala 
            std::string command = "python konwerter.py " + midiName + " dzwieki/nuty.txt";
            std::cout << "Konwertuje plik " << midiName << "...\n";

            // Skrypt w pythonie
            int result = std::system(command.c_str());

            if (result == 0)
            {
                std::cout << "Konwersja udana! Ladowanie utworu do Pianoli...\n";
                filePath = "dzwieki/nuty.txt";
            }
            else
            {
                std::cout << "BLAD: Nie udalo sie skonwertowac pliku.\n";
                filePath = "";
            }
        }
        else if (song_choise >= 1 && song_choise <= 5 && song_choise != 4)
        {
            switch (song_choise)
            {
            case 1:
                filePath = "dzwieki/Oda_do_radosci.txt";
                break;
            case 2:
                filePath = "dzwieki/Walc_a-moll.txt";
                break;
            case 3:
                filePath = "dzwieki/Preludium_C-dur.txt";
                break;
            case 5:
                filePath = "dzwieki/nuty.txt";
                break;
            }
        }
        else
        {
            std::cout << "UWAGA: Podano zly numer. Tryb bez utworu.\n";
            filePath = "";
        }


        // Inicjalizacja okna
        Window window(1200, 800, "Projekt Pianino 3D - Final");

        // Ładowanie shaderów z plików
        Shader myShader("shader.vert", "shader.frag");

        // Ładowanie modelu 3D
        Model myModel("Tekstury_objekty/Piano.obj");

        //zmienna "tex" używa slotu numer 0
        myShader.use();

        // Zmienne kamery
        glm::vec3 cameraPos = glm::vec3(3.0f, 1.0f, 0.0f);     // Gdzie stoimy
        glm::vec3 cameraFront = glm::vec3(0.0f, -0.3f, -1.0f); // Gdzie patrzymy 
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);      // Gdzie jest "sufit"

        float cameraSpeed = 3.5f; // Szybkość poruszania się

        // Zmienne myszy i katow eulera do poruszania sie
        window.disableCursor(); // Ukrywamy i blokujemy kursor

        float yaw = -180.0f;  
        float pitch = -20.0f;

        double lastX = 600.0, lastY = 400.0; // Środek okna 
        bool firstMouse = true;              // Flaga, żeby kamera nie skoczyła przy pierwszym ruchu

        // Zmienne obrotu
        float angleX = 0.0f;
        float angleY = 0.0f;
        float rotationSpeed = 0.5f;
        float lastFrameTime = glfwGetTime();

        // Zmienna przechowująca nasz Field of View 
        float fov = 45.0f;

        // Zmienne do animacji otwierania klapy fortepianu
        bool isPianoOpen = false;     
        float currentLidAngle = 0.0f; // Aktualny kąt otwarcia
        float maxLidAngle = 55.0f;    // Maksymalny kąt otwarcia w stopniach
        float lidSpeed = 80.0f;       // Prędkość otwierania 
        bool oKeyPressed = false;     

        // Obliczanie fps
        float time_second = 0.0f;
        int frames = 0;

        // Konfiguracja dzwiekow i klawiszy 
        struct PianoKey
        {
            int keyCode;                    // Przycisk na klawiaturze 
            std::string meshName;           // Nazwa siatki z logów Assimpa
            std::string noteName;           // Nazwa muzyczna nuty 
            std::string soundPath;          // Ścieżka do pliku audio
            bool isPressed;                 
            sf::SoundBuffer buffer;         // Pamięć RAM dla dźwięku
            std::optional<sf::Sound> sound; // Odtwarzacz SFML

            float pitch; //Zmiana tonacji 

            PianoKey(int k, std::string m, std::string n, std::string s, bool p, float pt = 1.0f)
                : keyCode(k), meshName(m), noteName(n), soundPath(s), isPressed(p), pitch(pt) {}
        };

        // Tworzenie listy klawiszy z nazwami nut
        std::vector<PianoKey> keys = {
            
            
            PianoKey(GLFW_KEY_UNKNOWN, "key58", "A0", "dzwieki/A0v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key94", "A0s", "dzwieki/A0v1.wav", false, 1.05946f), 
            PianoKey(GLFW_KEY_UNKNOWN, "key57", "B0", "dzwieki/A0v1.wav", false, 1.12246f),  

            
            
            PianoKey(GLFW_KEY_UNKNOWN, "key56", "C1", "dzwieki/C1v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key93", "C1s", "dzwieki/C1v1.wav", false, 1.05946f), 
            PianoKey(GLFW_KEY_UNKNOWN, "key55", "D1", "dzwieki/C1v1.wav", false, 1.12246f),  
            PianoKey(GLFW_KEY_UNKNOWN, "key92", "D1s", "dzwieki/D#1v1.wav", false, 1.0f),    
            PianoKey(GLFW_KEY_UNKNOWN, "key54", "E1", "dzwieki/D#1v1.wav", false, 1.05946f), 
            PianoKey(GLFW_KEY_UNKNOWN, "key53", "F1", "dzwieki/F#1v1.wav", false, 0.94387f), 
            PianoKey(GLFW_KEY_UNKNOWN, "key91", "F1s", "dzwieki/F#1v1.wav", false, 1.0f),    
            PianoKey(GLFW_KEY_UNKNOWN, "key52", "G1", "dzwieki/F#1v1.wav", false, 1.05946f), 
            PianoKey(GLFW_KEY_UNKNOWN, "key90", "G1s", "dzwieki/A1v1.wav", false, 0.94387f), 
            PianoKey(GLFW_KEY_UNKNOWN, "key51", "A1", "dzwieki/A1v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key89", "A1s", "dzwieki/A1v1.wav", false, 1.05946f), 
            PianoKey(GLFW_KEY_UNKNOWN, "key50", "B1", "dzwieki/A1v1.wav", false, 1.12246f),  

            
            
            PianoKey(GLFW_KEY_UNKNOWN, "key49", "C2", "dzwieki/C2v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key88", "C2s", "dzwieki/C2v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_UNKNOWN, "key48", "D2", "dzwieki/C2v1.wav", false, 1.12246f),
            PianoKey(GLFW_KEY_UNKNOWN, "key87", "D2s", "dzwieki/D#2v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key47", "E2", "dzwieki/D#2v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_UNKNOWN, "key46", "F2", "dzwieki/F#2v1.wav", false, 0.94387f),
            PianoKey(GLFW_KEY_UNKNOWN, "key86", "F2s", "dzwieki/F#2v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key45", "G2", "dzwieki/F#2v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_UNKNOWN, "key85", "G2s", "dzwieki/A2v1.wav", false, 0.94387f),
            PianoKey(GLFW_KEY_UNKNOWN, "key44", "A2", "dzwieki/A2v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key84", "A2s", "dzwieki/A2v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_UNKNOWN, "key43", "B2", "dzwieki/A2v1.wav", false, 1.12246f),

            
            
            PianoKey(GLFW_KEY_UNKNOWN, "key42", "C3", "dzwieki/C3v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key83", "C3s", "dzwieki/C3v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_UNKNOWN, "key41", "D3", "dzwieki/C3v1.wav", false, 1.12246f),
            PianoKey(GLFW_KEY_UNKNOWN, "key82", "D3s", "dzwieki/D#3v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key40", "E3", "dzwieki/D#3v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_UNKNOWN, "key39", "F3", "dzwieki/F#3v1.wav", false, 0.94387f),
            PianoKey(GLFW_KEY_UNKNOWN, "key81", "F3s", "dzwieki/F#3v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key38", "G3", "dzwieki/F#3v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_UNKNOWN, "key80", "G3s", "dzwieki/A3v1.wav", false, 0.94387f),
            PianoKey(GLFW_KEY_UNKNOWN, "key37", "A3", "dzwieki/A3v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key79", "A3s", "dzwieki/A3v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_UNKNOWN, "key36", "B3", "dzwieki/A3v1.wav", false, 1.12246f),

            
            
            PianoKey(GLFW_KEY_UNKNOWN, "key35", "C4", "dzwieki/C4v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key78", "C4s", "dzwieki/C4v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_UNKNOWN, "key34", "D4", "dzwieki/C4v1.wav", false, 1.12246f),
            PianoKey(GLFW_KEY_UNKNOWN, "key77", "D4s", "dzwieki/D#4v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key33", "E4", "dzwieki/D#4v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_UNKNOWN, "key32", "F4", "dzwieki/F#4v1.wav", false, 0.94387f),
            PianoKey(GLFW_KEY_UNKNOWN, "key76", "F4s", "dzwieki/F#4v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key31", "G4", "dzwieki/F#4v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_UNKNOWN, "key75", "G4s", "dzwieki/A4v1.wav", false, 0.94387f),
            PianoKey(GLFW_KEY_1, "key30", "A4", "dzwieki/A4v1.wav", false, 1.0f), // Klawisz 1
            PianoKey(GLFW_KEY_UNKNOWN, "key74", "A4s", "dzwieki/A4v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_2, "key29", "B4", "dzwieki/A4v1.wav", false, 1.12246f), // Klawisz 2

            
            
            PianoKey(GLFW_KEY_3, "key28", "C5", "dzwieki/C5v1.wav", false, 1.0f), // Klawisz 3
            PianoKey(GLFW_KEY_UNKNOWN, "key73", "C5s", "dzwieki/C5v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_4, "key27", "D5", "dzwieki/C5v1.wav", false, 1.12246f), // Klawisz 4
            PianoKey(GLFW_KEY_UNKNOWN, "key72", "D5s", "dzwieki/D#5v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_5, "key26", "E5", "dzwieki/D#5v1.wav", false, 1.05946f), // Klawisz 5
            PianoKey(GLFW_KEY_6, "key25", "F5", "dzwieki/F#5v1.wav", false, 0.94387f), // Klawisz 6
            PianoKey(GLFW_KEY_UNKNOWN, "key71", "F5s", "dzwieki/F#5v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_7, "key24", "G5", "dzwieki/F#5v1.wav", false, 1.05946f), // Klawisz 7
            PianoKey(GLFW_KEY_UNKNOWN, "key70", "G5s", "dzwieki/A5v1.wav", false, 0.94387f),
            PianoKey(GLFW_KEY_8, "key23", "A5", "dzwieki/A5v1.wav", false, 1.0f), // Klawisz 8
            PianoKey(GLFW_KEY_UNKNOWN, "key69", "A5s", "dzwieki/A5v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_9, "key22", "B5", "dzwieki/A5v1.wav", false, 1.12246f), // Klawisz 9

            
            
            PianoKey(GLFW_KEY_UNKNOWN, "key21", "C6", "dzwieki/C6v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key68", "C6s", "dzwieki/C6v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_UNKNOWN, "key20", "D6", "dzwieki/C6v1.wav", false, 1.12246f),
            PianoKey(GLFW_KEY_UNKNOWN, "key67", "D6s", "dzwieki/D#6v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key19", "E6", "dzwieki/D#6v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_UNKNOWN, "key18", "F6", "dzwieki/F#6v1.wav", false, 0.94387f),
            PianoKey(GLFW_KEY_UNKNOWN, "key66", "F6s", "dzwieki/F#6v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key17", "G6", "dzwieki/F#6v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_UNKNOWN, "key65", "G6s", "dzwieki/A6v1.wav", false, 0.94387f),
            PianoKey(GLFW_KEY_UNKNOWN, "key16", "A6", "dzwieki/A6v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key64", "A6s", "dzwieki/A6v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_UNKNOWN, "key15", "B6", "dzwieki/A6v1.wav", false, 1.12246f),

            
            
            PianoKey(GLFW_KEY_UNKNOWN, "key14", "C7", "dzwieki/C7v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key63", "C7s", "dzwieki/C7v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_UNKNOWN, "key13", "D7", "dzwieki/C7v1.wav", false, 1.12246f),
            PianoKey(GLFW_KEY_UNKNOWN, "key62", "D7s", "dzwieki/D#7v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key12", "E7", "dzwieki/D#7v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_UNKNOWN, "key11", "F7", "dzwieki/F#7v1.wav", false, 0.94387f),
            PianoKey(GLFW_KEY_UNKNOWN, "key61", "F7s", "dzwieki/F#7v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key10", "G7", "dzwieki/F#7v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_UNKNOWN, "key60", "G7s", "dzwieki/A7v1.wav", false, 0.94387f),
            PianoKey(GLFW_KEY_UNKNOWN, "key9", "A7", "dzwieki/A7v1.wav", false, 1.0f),
            PianoKey(GLFW_KEY_UNKNOWN, "key59", "A7s", "dzwieki/A7v1.wav", false, 1.05946f),
            PianoKey(GLFW_KEY_UNKNOWN, "key8", "B7", "dzwieki/A7v1.wav", false, 1.12246f),

            
            
            PianoKey(GLFW_KEY_UNKNOWN, "key7", "C8", "dzwieki/C8v1.wav", false, 1.0f)};

        std::ifstream file(filePath);

        if (file.is_open())
        {
            std::string line;
            while (std::getline(file, line))
            {
                std::istringstream iss(line);
                float start, duration;
                std::string readNote;

                // CzasStartu, NazwaNuty, CzasTrwania
                if (iss >> start >> readNote >> duration)
                {
                    NoteEvent note;
                    note.startTime = start;
                    note.noteName = readNote;
                    note.duration = duration;
                    note.meshName = ""; // Domyślnie puste

                    // Szukamy siatki przypisanej do tej nuty
                    for (const auto &key : keys)
                    {
                        if (key.noteName == readNote)
                        {
                            note.meshName = key.meshName;
                            break;
                        }
                    }

                    // Zapisujemy nutę 
                    if (note.meshName != "")
                    {
                        song.push_back(note);
                    }
                    else
                    {
                        std::cout << "UWAGA: Nierozpoznana nuta '" << readNote << "' w pliku txt!\n";
                    }
                }
            }
            file.close();
            std::cout << "INFO: Zaladowano utwor Liczba nut: " << song.size() << "\n";
        }
        else
        {
            std::cout << "UWAGA: Nie znaleziono pliku utworu w folderze projektu.\n";
        }

        // Zmienne sterujące trybami gry
        bool isAutoPlaying = false;
        float playbackTime = 0.0f;
        bool pKeyPressed = false; 

        // Wyłączamy spamowanie błędami SFML w konsoli
        sf::err().rdbuf(NULL);
        bool soundSystemWorking = true;

        // Wczytujemy pliki .wav do pamięci
        for (auto &key : keys)
        {
            if (key.buffer.loadFromFile(key.soundPath))
            {
                key.sound.emplace(key.buffer);  //Tworzymy dźwięk i dajemy mu bufor
                key.sound->setPitch(key.pitch); 
            }
            else
            {
                soundSystemWorking = false;
            }
        }

        if (!soundSystemWorking)
        {
            std::cout << "INFO: Uruchomiono bez obslugi dzwieku lub brakuje plikow .wav.\n";
        }

        lastFrameTime = glfwGetTime();

        //Tablica na nuty

        //Definiujemy wierzchołki dla płaskiego prostokąta 
        float quadVertices[] = {
            // Pozycje X,Y,Z      // Normalne           // Tekstury U,V
            -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // Lewa góra
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // Lewy dół
            0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // Prawy dół
            0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,  // Prawy dół
            0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,   // Prawa góra
            -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f   // Lewa góra
        };

        GLuint quadVAO, quadVBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        // Konfiguracja atrybutów zgodna z klasą Mesh 
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
        glBindVertexArray(0);

        //Dwie tekstury o wielkości 1x1 piksela (Biała i Niebieska)        
        GLuint whiteTexture, noteTexture;

        glGenTextures(1, &whiteTexture);
        glBindTexture(GL_TEXTURE_2D, whiteTexture);
        unsigned char whitePixel[] = {240, 240, 240, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);

        glGenTextures(1, &noteTexture);
        glBindTexture(GL_TEXTURE_2D, noteTexture);
        unsigned char bluePixel[] = {50, 150, 255, 255}; 
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, bluePixel);

        std::unordered_map<std::string, glm::mat4> localTransforms;
        localTransforms.reserve(100); // Rezerwujemy pamięć na zapas dla 88 klawiszy

        // zmienna trybu siatki
        bool isWireframeMode = false;
        bool rKeyPressed = false;

        while (!window.shouldClose())
        {
            // Obsługa klawiszy
            window.processInput();

            // Obliczanie czasu
            float currentFrameTime = glfwGetTime();
            float deltaTime = currentFrameTime - lastFrameTime;
            time_second += deltaTime;
            frames++;

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

            // Rozgladanie sie
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
            float yoffset = lastY - mouseY; 
            lastX = mouseX;
            lastY = mouseY;

            float sensitivity = 0.005f; // Czułość myszki 
            xoffset *= sensitivity;
            yoffset *= sensitivity;

            yaw += xoffset;
            pitch += yoffset;

            // Zabezpieczenie przed patrzeniem nienaturalnie
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;

            glm::vec3 front;
            //Przeliczenie kątów Eulera na wektor kierunkowy 
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

            // Obliczamy płaski wektor patrzenia
            glm::vec3 cameraFrontFlat = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
            float currentSpeed = cameraSpeed * deltaTime;

            // Chodzenie do przodu i do tyłu 
            if (window.isKeyPressed(GLFW_KEY_W))
                cameraPos += currentSpeed * cameraFrontFlat;
            if (window.isKeyPressed(GLFW_KEY_S))
                cameraPos -= currentSpeed * cameraFrontFlat
            if (window.isKeyPressed(GLFW_KEY_A))
                cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * currentSpeed;
            if (window.isKeyPressed(GLFW_KEY_D))
                cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * currentSpeed;

            float zoomSpeed = 30.0f * deltaTime; // Szybkość zoomowania

            // Przybliżanie
            if (window.isKeyPressed(GLFW_KEY_Z))
            {
                fov -= zoomSpeed;
            }
            // Oddalanie
            if (window.isKeyPressed(GLFW_KEY_X))
            {
                fov += zoomSpeed;
            }

            //Zabezpieczenia przed zjawiskiem rybiego oka
            if (fov < 1.0f)
                fov = 1.0f;
            if (fov > 45.0f)
                fov = 45.0f;

            // Macierz korelacji
            glm::mat4 P = glm::perspective(glm::radians(fov), 1200.0f / 800.0f, 0.1f, 100.0f);

            // Czyszczenie tła 
            glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Aktywacja shadera
            myShader.use();

            // pozycję kamery
            // Macierz widoku
            glm::mat4 V = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

            

            // Switalo 1 kierunkowe chlodne
            // Wektor kierunku - świeci z góry, z prawej, lekko z tyłu
            myShader.setVec3("dirLightDir", glm::vec3(-0.2f, -1.0f, -0.3f));
            myShader.setVec3("dirLightColor", glm::vec3(0.4f, 0.4f, 0.5f)); // Chłodny błękit/szarość

            // Swiatlo 2 punktowe lampka
            // Przesuwamy lampkę nad klawisze 
            myShader.setVec3("pointLightPos", glm::vec3(0.0f, 3.0f, 4.0f));
            // Lekko przyciemniamy 
            myShader.setVec3("pointLightColor", glm::vec3(0.8f, 0.6f, 0.3f));

            // Tworzenie macierzy Modelu 
            glm::mat4 M = glm::mat4(1.0f);
            M = glm::translate(M, glm::vec3(0.0f, -1.0f, 0.0f));
            M = glm::rotate(M, angleX, glm::vec3(0.0f, 1.0f, 0.0f));
            M = glm::rotate(M, angleY, glm::vec3(1.0f, 0.0f, 0.0f));
            M = glm::rotate(M, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            M = glm::scale(M, glm::vec3(0.01f)); 

            // Przelacznik pianoli
            if (window.isKeyPressed(GLFW_KEY_P))
            {
                if (!pKeyPressed)
                {
                    pKeyPressed = true;
                    if (!isAutoPlaying)
                    {
                        
                        isAutoPlaying = true;
                        playbackTime = 0.0f; // Zerujemy czas
                        // Resetujemy stan wszystkich nut przed każdym odtworzeniem
                        for (auto &note : song)
                        {
                            note.isPlaying = false;
                            note.isFinished = false;
                        }
                        std::cout << "Pianola: START!\n";
                    }
                    else
                    {
                        // Reczne zatrzymanie
                        isAutoPlaying = false;
                        std::cout << "Pianola: STOP!\n";
                    }
                }
            }
            else
            {
                pKeyPressed = false;
            }

            // Animacji i dzwiek klawiszy
            localTransforms.clear();
            float keyPressDepth = -0.5f;

            if (isAutoPlaying)
            {
                // ==========================================
                // TRYB AUTOMATYCZNY (PIANOLA GRA Z PLIKU)
                
                plaTryb recznywór dobiegł końca

                for (auto &note : song)
                {
                    if (!note.isFinished)
                    {
                        allFinished = false;

                        // 1. Czy nadszedł czas, aby wcisnąć klawisz?
                        if (playbackTime >= note.startTime && !note.isPlaying)
                        {
                            note.isPlaying = true;
                            if (soundSystemWorking)
                            {
                                for (auto &key : keys)
                                {
                                    if (key.meshName == note.meshName)
                                    {
                                        if (key.sound)
                                            key.sound->play();
                                        break;
                                    }
                                }
                            }
                        }

                        // 2. Opadnięcie klawisza (animacja w dół)
                        if (note.isPlaying)
                        {
                            localTransforms[note.meshName] = glm::translate(glm::mat4(1.0f), glm::vec3(-keyPressDepth, 0.0f, 0.0f));
                        }

                        // 3. Czy nadszedł czas, aby puścić klawisz? (Powrót animacji do góry)
                        if (playbackTime >= note.startTime + note.duration)
                        {
                            note.isPlaying = false;
                            note.isFinished = true;
                            // UWAGA: Nie wywołujemy key.sound.stop(), dzięki czemu dźwięk pięknie wybrzmiewa (efekt pedału)
                        }
                    }
                }

                // Automatyczny powrót do trybu ręcznego, gdy piosenka się skończy
                if (allFinished && song.size() > 0)
                {
                    isAutoPlaying = false;
                    std::cout << "Pianola: Koniec utworu.\n";
                }
            }
            else
            {
                
                // Tryb reczny
            
                for (auto &key : keys)
                {
                    if (window.isKeyPressed(key.keyCode))
                    {
                        // Ugięcie klawisza na ekranie
                        localTransforms[key.meshName] = glm::translate(glm::mat4(1.0f), glm::vec3(-keyPressDepth, 0.0f, 0.0f));

                        // Klawisz gra w momencie 1 klikniecia
                        if (!key.isPressed)
                        {
                            if (soundSystemWorking && key.sound)
                            {
                                key.sound->play();
                            }
                            key.isPressed = true;
                        }
                    }
                    else
                    {
                        // Klawisz odskakuje 
                        key.isPressed = false;
                    }
                }
            }
            // Animacja spacji pedała
            if (window.isKeyPressed(GLFW_KEY_SPACE))
            {
                glm::mat4 pedalTransform = glm::mat4(1.0f);
                pedalTransform = glm::translate(pedalTransform, glm::vec3(0.0f, -0.5f, 0.0f));
                pedalTransform = glm::rotate(pedalTransform, glm::radians(-5.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                localTransforms["pedal"] = pedalTransform;
            }

            // Otwieranie klapy
            if (window.isKeyPressed(GLFW_KEY_O))
            {
                if (!oKeyPressed)
                {
                    isPianoOpen = !isPianoOpen; 
                    oKeyPressed = true;         
                }
            }
            else
            {
                oKeyPressed = false; 
            }
            // Tryb roboczy siatka wire
            if (window.isKeyPressed(GLFW_KEY_R))
            {
                if (!rKeyPressed)
                {
                    isWireframeMode = !isWireframeMode; 
                    rKeyPressed = true;                 
                }
            }
            else
            {
                rKeyPressed = false;
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

            // Animacja klapy
            if (currentLidAngle > 0.0f)
            {
                glm::mat4 lidTransform = glm::mat4(1.0f);

                // Definiujemy wektor przesunięcia do zawiasów 
                glm::vec3 pivotOffset = glm::vec3(0.0f, 76.0f, -120.0f);
                // Przesuwamy z powrotem na obudowę
                lidTransform = glm::translate(lidTransform, -pivotOffset);

                // Obrót 
                lidTransform = glm::rotate(lidTransform, glm::radians(currentLidAngle), glm::vec3(1.0f, 0.0f, 0.0f));

                // Przesuwamy krawędź zawiasu do środka świata 
                lidTransform = glm::translate(lidTransform, pivotOffset);

                // Przekazujemy do mapy nazwe z logow
                localTransforms["pianoTop"] = lidTransform;
            }

            // Przesłanie macierzy do shadera
            myShader.setMat4("V", V);
            myShader.setVec3("viewPos", cameraPos); 
            myShader.setMat4("P", P);               

            //Rysowanie fortepianu
            
            // Tryb roboczy
            if (isWireframeMode) 
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }

            // Rysowanie modelu
            myModel.Draw(myShader, M, localTransforms);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

           //Rysowanie kartki papieru

            // Definiujemy bazę dla położenia i obrotu kartki.
            glm::mat4 paperBaseM = glm::mat4(1.0f);
            paperBaseM = glm::translate(paperBaseM, glm::vec3(-2.0f, 1.5f, 0.0f)); // Twoja znaleziona pozycja
            paperBaseM = glm::rotate(paperBaseM, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Twój obrót o 90 stopni

            //Rysujemy białą tablicę
            glm::mat4 boardM = paperBaseM;
            boardM = glm::scale(boardM, glm::vec3(5.0f, 2.0f, 1.0f)); 

            myShader.setMat4("M", boardM);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, whiteTexture); 

            glBindVertexArray(quadVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            
            
            
            // Wizualizacja granych nut

            glBindTexture(GL_TEXTURE_2D, noteTexture); // Zmieniamy kolor na błękitny

            // Obsługa wizualizacji dla automatycznego odtwarzania 
            if (isAutoPlaying)
            {
                for (auto &note : song)
                {
                    if (note.isPlaying)
                    {
                        // Szukamy indeksu klawisza (0 - 87)
                        int keyIdx = -1;
                        for (size_t i = 0; i < keys.size(); i++)
                        {
                            if (keys[i].meshName == note.meshName)
                            {
                                keyIdx = i;
                                break;
                            }
                        }

                        if (keyIdx != -1)
                        {
                            float percent = (float)keyIdx / (keys.size() - 1);
                            
                            
                            float localX = -2.5f + (percent * 5.0f);

                            glm::mat4 noteM = paperBaseM; // Nuta dziedziczy pozycję i obrot od kartki
                            
                             
                            // Przesuwamy troche do przodu aby uniknac problemu z-figtingu
                            noteM = glm::translate(noteM, glm::vec3(localX, 0.0f, 0.01f)); 
                            noteM = glm::scale(noteM, glm::vec3(0.04f, 2.0f, 1.0f)); 

                            myShader.setMat4("M", noteM);
                            glDrawArrays(GL_TRIANGLES, 0, 6);
                        }
                    }
                }
            }
            //Obsługa wizualizacji dla ręcznego grania na klawiaturze
            else
            {
                for (size_t i = 0; i < keys.size(); i++)
                {
                    if (keys[i].isPressed)
                    {
                        float percent = (float)i / (keys.size() - 1);
                        float localX = -2.5f + (percent * 5.0f);

                        glm::mat4 noteM = paperBaseM; // Dziedziczymy obrót kartki
                        
                        noteM = glm::translate(noteM, glm::vec3(localX, 0.0f, 0.01f));
                        noteM = glm::scale(noteM, glm::vec3(0.04f, 2.0f, 1.0f));

                        myShader.setMat4("M", noteM);
                        glDrawArrays(GL_TRIANGLES, 0, 6);
                    }
                }
            }

            glBindVertexArray(0); // Odpinamy VAO na koniec klatki

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