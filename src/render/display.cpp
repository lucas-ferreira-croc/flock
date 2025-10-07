#include "display.hpp"
#include "log/logger.hpp"
#include <iostream>
#include <vector>
#include "texture/texture.hpp"

Display::Display(unsigned int windowWidth, unsigned int windowHeight)
	: _windowWidth(windowWidth), _windowHeight(windowHeight),
	window(nullptr)
{
}

Display::~Display()
{
	glDeleteFramebuffers(1, &framebuffer); 
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Display::initializeWindow()
{
	if (!glfwInit())
	{
		Logger::err("GLFW Failed to init!");
		glfwTerminate();

		return;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	
	window = glfwCreateWindow(_windowWidth, _windowHeight, "flock", NULL, NULL);

	if (!window)
	{
		Logger::err("GLFW failed to create window!");
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
	glEnable(GL_MULTISAMPLE);  
	//glEnable(GL_STENCIL_TEST);
	
	glViewport(0, 0, _bufferWidth, _bufferHeight);
	glfwSetWindowUserPointer(window, this);
	
	glfwSwapInterval(1);
	
	glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    // create a color attachment texture
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _bufferWidth, _bufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    // create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _bufferWidth, _bufferHeight); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		Logger::log("additional framebuffer creation completed succesfully");
	}
	else{
		Logger::err("error creating additional framebuffer");
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
  
	std::string vsFilename = "C:\\dev\\shader\\flock\\assets\\shaders\\v_framebuffer.glsl";
	std::string  fsFilename = "C:\\dev\\shader\\flock\\assets\\shaders\\f_framebuffer.glsl";

    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	_framebufferShader = std::make_shared<Shader>();
	_framebufferShader->createFromFile(vsFilename, fsFilename);

	// depth for shadows
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	_depthbufferShader = std::make_shared<Shader>();
	
	vsFilename = "C:\\dev\\shader\\flock\\assets\\shaders\\v_depthMap.glsl";
 	fsFilename = "C:\\dev\\shader\\flock\\assets\\shaders\\f_depthMap.glsl";
	_depthbufferShader->createFromFile(vsFilename, fsFilename);
	

	vsFilename = "C:\\dev\\shader\\flock\\assets\\shaders\\v_debug_depth.glsl";
 	fsFilename = "C:\\dev\\shader\\flock\\assets\\shaders\\f_debug_depth.glsl";
	depthDebugShader = std::make_shared<Shader>();
	depthDebugShader->createFromFile(vsFilename, fsFilename);
	_depthbufferShader->bind();
	_depthbufferShader->setInt("depthMap", 0);
}

void Display::swapBuffers()
{
	glfwPollEvents();
	glfwSwapBuffers(window);
}

void Display::clearColor(float r, float g, float b, float a)
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


void Display::renderFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessary actually, since we won't be able to see behind the quad anyways)
    glClear(GL_COLOR_BUFFER_BIT);
 

	_framebufferShader->bind();
    _framebufferShader->setInt("screenTexture", 0);
    glBindVertexArray(quadVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Display::renderDepthBuffer()
{
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);

}

void Display::renderDepthBufferDebug()
{
	_depthbufferShader->bind();
    _depthbufferShader->setInt("shadowMap", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
}