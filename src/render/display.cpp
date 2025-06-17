#include "display.hpp"
#include <iostream>
#include <vector>

Display::Display(unsigned int windowWidth, unsigned int windowHeight)
	: _windowWidth(windowWidth), _windowHeight(windowHeight),
	window(nullptr)
{
}

Display::~Display()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Display::initializeWindow()
{
	if (!glfwInit())
	{
		std::cout << "GLFW Failed to init!" << std::endl;
		glfwTerminate();

		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	window = glfwCreateWindow(_windowWidth, _windowHeight, "Anubis", NULL, NULL);

	if (!window)
	{
		std::cout << "GLFW failed to create window!" << std::endl;
		glfwTerminate();
		return;
	}

	glfwGetFramebufferSize(window, &_bufferWidth, &_bufferHeight);

	glfwMakeContextCurrent(window);
	//glfwSwapInterval(1);

	createCallbacks();


	if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
	{
		std::cerr << "failed initializing glad\n";
		glfwDestroyWindow(window);
		glfwTerminate();
		return;
	}

	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, _bufferWidth, _bufferHeight);
	glfwSetWindowUserPointer(window, this);

	glfwSwapInterval(1);
}

void Display::swapBuffers()
{
	glfwPollEvents();
	glfwSwapBuffers(window);
}

void Display::clearColor(int r, int g, int b, int a)
{
	glClearColor(r, g, b, a);
}

void Display::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

bool Display::shouldClose()
{
	return glfwWindowShouldClose(window);
}

void Display::createCallbacks()
{
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
}

void Display::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	Display* display = static_cast<Display*>(glfwGetWindowUserPointer(window));
	glViewport(0, 0, width, height);

	display->_windowWidth = width;
	display->_windowHeight = height;
}


