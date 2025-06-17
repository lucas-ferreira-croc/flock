#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <GLFW/glfw3.h>

class Camera
{
public:
	Camera(glm::vec3 position, glm::vec3 target);

	glm::mat4 getLookAt();
	void calculateLookAt();
	void update(double deltaTime);

	double getXpos() {
		return _xpos;
	}
	double getYpos() {
		return _ypos;
	}

	glm::vec3 getPosition() { return position; }

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);

	//void calculateLocalPosition(Transform transform);
	glm::vec3 getLocalPosition() { return _localPosition; };

	glm::vec3 getTarget() { return _target; };

	bool getDislodgeMouse() { return _dislodgeMouse; }
	void flipDislodgeMouse() { _dislodgeMouse = !_dislodgeMouse; }

private:
	bool keys[1024];
	glm::vec3 _localPosition;

	glm::vec3 position;
	glm::vec3 _target;

	float _speed;
	float _yawSpeed;
	float _camYawAngle;
	float _camPitchAngle;

	glm::mat4 _rotation;

	glm::mat4 _view;


	double _xpos;
	double _ypos;

	double _previousXpos;
	double _previousYpos;

	bool _dislodgeMouse;
};



#endif

