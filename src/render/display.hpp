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
	void renderDepthBuffer();
	void renderDepthBufferDebug();
	unsigned int framebuffer;
	
	void initializeUI();
	void renderUI();
	void destroyUI();

	// depthbuffer
	std::shared_ptr<Shader> _depthbufferShader;
	std::shared_ptr<Shader> depthDebugShader;
	unsigned int depthMapFBO;
	unsigned int depthMap;
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

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

