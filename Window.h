/**
 * @file Window.h
 * @brief Klasa opakowująca funkcjonalności okna i wejścia biblioteki GLFW.
 */

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include <string>

/**
 * @brief Klasa zarządzająca cyklem życia okna aplikacji oraz obsługą urządzeń wejścia.
 * * Automatyzuje proces tworzenia okna, inicjalizacji kontekstu OpenGL i biblioteki GLEW,
 * a także zapewnia interfejsy do przechwytywania klawiatury i myszy.
 */
class Window
{
private:
    GLFWwindow *windowHandle = nullptr; ///< Uchwyt (wskaźnik) do właściwego okna GLFW.
    int width;                          ///< Szerokość okna w pikselach.
    int height;                         ///< Wysokość okna w pikselach.

public:
    /**
     * @brief Konstruktor tworzący okno aplikacji.
     * @param w Szerokość okna.
     * @param h Wysokość okna.
     * @param title Tytuł wyświetlany na pasku okna.
     * @throw std::runtime_error Wyrzucany w przypadku niepowodzenia inicjalizacji GLFW lub GLEW.
     */
    Window(int w, int h, const std::string &title) : width(w), height(h)
    {
        if (!glfwInit())
        {
            throw std::runtime_error("Nie udało się zainicjalizować GLFW!");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        windowHandle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!windowHandle)
        {
            glfwTerminate();
            throw std::runtime_error("Nie udało się utworzyć okna GLFW!");
        }

        glfwMakeContextCurrent(windowHandle);

        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK)
        {
            throw std::runtime_error("Nie udało się zainicjalizować GLEW!");
        }

        glViewport(0, 0, width, height);

        glfwSetFramebufferSizeCallback(windowHandle, [](GLFWwindow *, int w, int h)
                                       { glViewport(0, 0, w, h); });

        glEnable(GL_DEPTH_TEST);
    }

    /**
     * @brief Destruktor zwalniający zasoby systemowe okna.
     */
    ~Window()
    {
        if (windowHandle)
        {
            glfwDestroyWindow(windowHandle);
        }
        glfwTerminate();
        std::cout << "Zasoby GLFW zostaly poprawnie zwolnione.\n";
    }

    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;

    /**
     * @brief Sprawdza, czy podany klawisz na klawiaturze jest aktualnie wciśnięty.
     * @param key Kod klawisza (np. GLFW_KEY_W).
     * @return True jeśli wciśnięty, False w przeciwnym razie.
     */
    bool isKeyPressed(int key) const {
        return glfwGetKey(windowHandle, key) == GLFW_PRESS;
    }

    /**
     * @brief Dynamicznie zmienia tytuł na pasku okna (np. do wyświetlania liczby FPS).
     * @param title Nowy tytuł.
     */
    void setTitle(const std::string& title) {
        glfwSetWindowTitle(windowHandle, title.c_str());
    }

    /**
     * @brief Sprawdza, czy użytkownik wydał polecenie zamknięcia okna (np. kliknął krzyżyk).
     * @return True jeśli aplikacja powinna się zakończyć.
     */
    bool shouldClose() const
    {
        return glfwWindowShouldClose(windowHandle);
    }

    /**
     * @brief Zamienia bufory ekranu (Double Buffering), wyświetlając wyrenderowaną klatkę.
     */
    void swapBuffers()
    {
        glfwSwapBuffers(windowHandle);
    }

    /**
     * @brief Przetwarza zdarzenia systemowe (np. ruchy myszy, kliknięcia).
     */
    void pollEvents()
    {
        glfwPollEvents();
    }

    /**
     * @brief Automatyczna obsługa wyjścia z programu po naciśnięciu klawisza ESCAPE.
     */
    void processInput()
    {
        if (glfwGetKey(windowHandle, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(windowHandle, true);
        }
    }

    /**
     * @brief Ukrywa kursor myszy i przypina go do środka okna (typowe dla kamer w grach FPS).
     */
    void disableCursor() {
        glfwSetInputMode(windowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    /**
     * @brief Pobiera aktualną pozycję kursora myszy względem okna aplikacji.
     * @param xpos Referencja, do której wpisana zostanie współrzędna X.
     * @param ypos Referencja, do której wpisana zostanie współrzędna Y.
     */
    void getCursorPos(double& xpos, double& ypos) const {
        glfwGetCursorPos(windowHandle, &xpos, &ypos);
    }
};