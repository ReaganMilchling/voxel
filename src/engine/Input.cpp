
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Input.h"

Input::Input(Camera& camera, const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT)
    : m_Camera(camera), firstMouse(false), deltaTime(0.0f), lastFrame(0.0f)
{
    lastY = SCR_HEIGHT / 2.0f;
    lastX = SCR_WIDTH / 2.0f;
}

Input::~Input()
{

}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void Input::processInput(GLFWwindow* window)
{
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void Input::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
void Input::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    m_Camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void Input::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    m_Camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
