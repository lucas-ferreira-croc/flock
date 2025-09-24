#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "render/shader.hpp"

class Display
{
public:
	Display(unsigned int windowWidth, unsigned int windowHeight);
	~Display();

	void initializeWindow();

	void swapBuffers();

	bool isRunning() { return _isRunning; };

	void clearColor(float r, float g, float b, float a);
	void clear();

	bool shouldClose();

	GLFWwindow* getWindow() { return window; };

	int getBufferWidth() { return _bufferWidth; };
	int getBufferHeight() { return _bufferHeight; };

	void renderFramebuffer();
	unsigned int framebuffer;
	private:
	GLFWwindow* window;
	
	unsigned int _windowWidth;
	unsigned int _windowHeight;
	
	int _bufferWidth;
	int _bufferHeight;
	
	bool _isRunning;
	
	// Framebuffer
	std::shared_ptr<Shader> _framebufferShader;
	unsigned int rbo;
	unsigned int textureColorbuffer;
	unsigned int quadVAO;
	unsigned int quadVBO;

private:

	void createCallbacks();

	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};



#endif

