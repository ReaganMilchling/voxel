#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "camera.h"

class Input {
public:
	Input(Camera& camera, const unsigned int SCR_WIDTH, const unsigned int SCR_HEIGHT);
	~Input();

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void processInput(GLFWwindow* window);
private:
	Camera& m_Camera;
	float deltaTime;
	float lastFrame;
	float lastX;
	float lastY;
	bool firstMouse;
};