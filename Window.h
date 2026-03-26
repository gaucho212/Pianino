#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <string>

class Window
{
private:
    GLFWwindow *windowHandle = nullptr;
    int width;
    int height;

public:
    // Konstruktor: Inicjalizuje GLFW, tworzy okno i ładuje GLEW
    Window(int w, int h, const std::string &title) : width(w), height(h)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Nie udało się zainicjalizować GLFW!");
        }

        // Konfiguracja kontekstu OpenGL (Core Profile, np. wersja 3.3)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        // Tworzenie okna
        windowHandle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!windowHandle)
        {
            glfwTerminate(); // Sprzątamy, jeśli okno się nie stworzyło
            throw std::runtime_error("Nie udało się utworzyć okna GLFW!");
        }

        // Przypisanie kontekstu OpenGL do obecnego wątku
        glfwMakeContextCurrent(windowHandle);

        // Inicjalizacja GLEW
        glewExperimental = GL_TRUE; // Wymagane w Core Profile
        if (glewInit() != GLEW_OK)
        {
            throw std::runtime_error("Nie udało się zainicjalizować GLEW!");
        }

        // Definiujemy obszar renderowania
        glViewport(0, 0, width, height);

        // Rejestrujemy callback do zmiany rozmiaru okna
        glfwSetFramebufferSizeCallback(windowHandle, [](GLFWwindow *, int w, int h)
                                       { glViewport(0, 0, w, h); });

        // Włączamy testowanie głębi
        glEnable(GL_DEPTH_TEST);
    }

    // Destruktor: Automatyczne sprzątanie (RAII)
    ~Window()
    {
        if (windowHandle)
        {
            glfwDestroyWindow(windowHandle);
        }
        glfwTerminate();
        std::cout << "Zasoby GLFW zostaly poprawnie zwolnione.\n";
    }

    // Zablokowanie kopiowania okna
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    // Metody pomocnicze do pętli głównej

    bool isKeyPressed(int key) const {
        return glfwGetKey(windowHandle, key) == GLFW_PRESS;
    }
    
    bool shouldClose() const
    {
        return glfwWindowShouldClose(windowHandle);
    }

    void swapBuffers()
    {
        glfwSwapBuffers(windowHandle);
    }

    void pollEvents()
    {
        glfwPollEvents();
    }

    // Usuwanie okna po ESC
    void processInput()
    {
        if (glfwGetKey(windowHandle, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(windowHandle, true);
        }
    }
};