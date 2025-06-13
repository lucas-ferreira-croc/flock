#include "camera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <xutility>
#include <iostream>

Camera::Camera(glm::vec3 position, glm::vec3 target)
	: position(position), _target(target), _speed(5.0f), _yawSpeed(10.0f), _camYawAngle(0.0f), _camPitchAngle(0.0f),
	_previousXpos(0.0f), _previousYpos(0.0), _xpos(0.0f), _ypos(0.0f)
{
	_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(_camYawAngle), glm::vec3(0.0f, 1.0f, 0.0f));

	std::fill(std::begin(keys), std::end(keys), false);
	calculateLookAt();

    _dislodgeMouse = true;
}

void Camera::calculateLookAt()
{

	//_rotation = glm::rotate(glm::mat4(1.0f), deg_to_radians(_camYawAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat q = glm::angleAxis(glm::radians(_camYawAngle), glm::vec3(0.0f, 1.0f, 0.0f));

	//_rotation = glm::rotate(_rotation, deg_to_radians(camPitchAngle), glm::vec3(1.0f, 0.0f, 0.0f));
	q *= glm::angleAxis(glm::radians(_camPitchAngle), glm::vec3(1.0f, 0.0f, 0.0f));

	_rotation = glm::toMat4(q);
	_target = glm::normalize(glm::vec3(_rotation * glm::vec4(0.0f, 0.0f, -1.0f, 0.0)));
	glm::vec3 cameraRight = glm::cross(_target, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec3 cameraUp = glm::cross(cameraRight, _target);

	_view = glm::lookAt(position, position + _target, cameraUp);
}

void Camera::update(double deltaTime)
{
    bool cam_moved = false;

    // Vetores de direção relativos à câmera
    glm::vec3 right = glm::normalize(glm::cross(_target, glm::vec3(0.0f, 1.0f, 0.0f))); // Direção direita
    glm::vec3 forward = glm::normalize(glm::vec3(_target.x, 0.0f, _target.z)); // Direção para frente (ignorar Y)


    if (keys[GLFW_KEY_A])
    {
        position -= right * (_speed * (float)deltaTime);
        cam_moved = true;
    }
    if (keys[GLFW_KEY_D])
    {
        position += right * (_speed * (float)deltaTime);
        cam_moved = true;
    }
    if (keys[GLFW_KEY_W])
    {
        position += forward * (_speed * (float)deltaTime);
        cam_moved = true;
    }
    if (keys[GLFW_KEY_S])
    {
        position -= forward * (_speed * (float)deltaTime);
        cam_moved = true;
    }
    if (keys[GLFW_KEY_Q])
    {
        position.y += _speed * (float)deltaTime;
        cam_moved = true;
    }
    if (keys[GLFW_KEY_E])
    {
        position.y -= _speed * (float)deltaTime;
        cam_moved = true;
    }

    if (keys[GLFW_KEY_LEFT])
    {
        _camYawAngle += _yawSpeed * deltaTime;
        cam_moved = true;
    }
    if (keys[GLFW_KEY_RIGHT])
    {
        _camYawAngle -= _yawSpeed * deltaTime;
        cam_moved = true;
    }


    if(!_dislodgeMouse)
    {
        if (_previousXpos == 0.0 && _previousYpos == 0.0)
        {
            _previousXpos = _xpos;
            _previousYpos = _ypos;
        }

        float sensitivity = 0.1f;
        double x_displacement = _xpos - _previousXpos;
        if (abs(x_displacement) > 0.0)
        {
            _camYawAngle -= x_displacement * sensitivity;
            _previousXpos = _xpos;
            cam_moved = true;
        }

        double y_displacement = _ypos - _previousYpos;
        if (abs(y_displacement) > 0.0)
        {
            _camPitchAngle -= y_displacement * sensitivity;
            _camPitchAngle = glm::clamp(_camPitchAngle, -89.0f, 89.0f);
            _previousYpos = _ypos;
            cam_moved = true;
        }
    }

    if (cam_moved)
    {
        calculateLookAt();
    }
}


void Camera::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Camera* this_camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			this_camera->keys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			this_camera->keys[key] = false;

		}
	}
}

void Camera::cursorPositionCallback(GLFWwindow* window, double _xpos, double _ypos)
{
	Camera* this_camera = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	this_camera->_xpos = _xpos;
    this_camera->_ypos = _ypos;
}

// void Camera::calculateLocalPosition(Transform transform)
// {
//     _LocalPosition = transform.world_position_to_local_position(position);
// }

glm::mat4 Camera::getLookAt()
{
	return _view;
}

