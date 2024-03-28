#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <string>
#include <thread>

#include "engine/shader.h"
#include "engine/camera.h"
#include "world/chunk.h"
#include "engine/texture.h"
#include "engine/camera.h"
#include "world/world.h"
#include "engine/Input.h"
#include "threadpool.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, World* world);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 129.0f, -32.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

Input input(camera, SCR_WIDTH, SCR_HEIGHT);

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Voxel", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSwapInterval(1);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_ALWAYS);
    glEnable(GL_BLEND); //Enable blending.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set blending function.
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);

    // build and compile our shader program
    // ------------------------------------
    Shader textureShader("res/shaders/Texture.shader");
    Shader rgbShader("res/shaders/Basic.shader");

    // load, create, and Bind a texture 
    // -------------------------
    Texture texture1("res/textures/stone.jpg");
    texture1.Bind(1);
    Texture texture2("res/textures/dirt.jpg");
    texture2.Bind(2);
    Texture texture3("res/textures/grass.jpg");
    texture3.Bind(3);
    Texture texture4("res/textures/water.jpg");
    texture4.Bind(4);
    Texture texture5("res/textures/snow.jpg");
    texture5.Bind(5);
    Texture texture6("res/textures/sand.jpg");
    texture5.Bind(6);
    Texture texture7("res/textures/gravel.jpg");
    texture5.Bind(7);

    textureShader.setUniform1i("stone", 1);
    textureShader.setUniform1i("dirt", 2);
    textureShader.setUniform1i("grass", 3);
    textureShader.setUniform1i("water", 4);
    textureShader.setUniform1i("snow", 5);
    textureShader.setUniform1i("sand", 6);
    textureShader.setUniform1i("gravel", 7);
    textureShader.Bind();
    rgbShader.Bind();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    const auto processor_count = std::thread::hardware_concurrency();
    std::cout << processor_count << std::endl;
    
    ThreadPool *tp = new ThreadPool();

    World world(&camera, 6, tp);
    world.generate();
    //std::thread t1(&World::generate, &world);
    //std::cout << std::this_thread::get_id() << std::endl;
    //std::thread t1(&World::generatechunk, &world, 1, 0);

    // time step and frame time calculation variables
    double dt = 0.01;
    double prevTime = 0.0;
    double crntTime = 0.0;
    double timeDiff;
    unsigned int counter = 0, tick = 0; // Keeps track of the amount of frames in timeDiff

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // Updates counter and times
        crntTime = glfwGetTime();
        timeDiff = crntTime - prevTime;
        counter++;

        if (timeDiff >= dt)
        {
            // Creates new title
            std::string FPS = std::to_string((int)((1.0 / timeDiff) * counter));
            std::string ms = std::to_string((timeDiff / counter) * 1000);
            std::string newTitle = FPS + " FPS / " + ms + "ms  -  " 
                + std::to_string(world.getChunkSize()) + ":" + std::to_string(world.getViewableChunkSize())
                + " Collisions:" + std::to_string(camera.collisions)
                + " Fly:" + std::to_string(camera.fly)
                + " Position:" + glm::to_string(camera.Position)
                + " tick:" + std::to_string(tick);
            glfwSetWindowTitle(window, newTitle.c_str());

            // Resets times and counter
            prevTime = crntTime;
            counter = 0;
        }

        // per-frame time logic
        double currentFrame = static_cast<double>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window, &world);
        camera.update(&world);
        world.update();

        // render
        glClearColor(0.0f, 0.6f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        textureShader.setUniform1i("stone", 1);
        textureShader.setUniform1i("dirt", 2);
        textureShader.setUniform1i("grass", 3);
        textureShader.setUniform1i("water", 4);
        textureShader.setUniform1i("snow", 5);
        textureShader.setUniform1i("sand", 6);
        textureShader.setUniform1i("gravel", 7);
        texture1.Bind(1);
        texture2.Bind(2);
        texture3.Bind(3);
        texture4.Bind(4);
        texture5.Bind(5);
        texture6.Bind(6);
        texture7.Bind(7);

        // MVP
        //player.getcamera
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.01f, 300.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 mvp = projection * view * model;
        textureShader.setUniformMatrix4fv("mvp", mvp);

        world.Render(textureShader);
        

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    glfwTerminate(); 
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window, World* world)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime, world);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime, world);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime,  world);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime, world);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UPWARD, deltaTime, world);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWNWARD, deltaTime, world);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(INCSPEED, deltaTime, world);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        GLint mode[2];
        glGetIntegerv(GL_POLYGON_MODE, mode);
        if (mode[1] == GL_LINE && mode[0] == GL_LINE) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        if (camera.collisions) {
            camera.collisions = false;
            camera.fly = true;
        } else {
            camera.collisions = true;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        if (camera.fly) {
            camera.fly = false;
        } else {
            camera.fly = true;
        }
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
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

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
