#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <glad/gl.h>
#include <GLFW/glfw3.h>

class Display
{
public:
	Display(unsigned int windowWidth, unsigned int windowHeight);
	~Display();

	void initializeWindow();

	void swapBuffers();

	bool isRunning() { return _isRunning; };

	void clearColor(int r, int g, int b, int a);
	void clear();

	bool shouldClose();

	GLFWwindow* getWindow() { return window; };

	int getBufferWidth() { return _bufferWidth; };
	int getBufferHeight() { return _bufferHeight; };
private:
	GLFWwindow* window;

	unsigned int _windowWidth;
	unsigned int _windowHeight;

	int _bufferWidth;
	int _bufferHeight;

	bool _isRunning;

private:

	void createCallbacks();

	static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};



#endif

