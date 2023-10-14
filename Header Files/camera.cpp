#include "camera.h"
#include <iostream>


Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
{
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;

	Front = glm::vec3(0.0f, 0.0f, -1.0f);
	MovementSpeed = MOVEMENT_SPEED;
	MouseSensitivity = SENSITIVITY;
	Fov = FOV;

	updateCameraVectors();
}

//Scalar Constructor
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
{
	Position = glm::vec3(posX, posY, posZ);
	WorldUp = glm::vec3(upX, upY, upZ);
	Yaw = yaw;
	Pitch = pitch;

	Front = glm::vec3(0.0f, 0.0f, -1.0f);
	MovementSpeed = MOVEMENT_SPEED;
	MouseSensitivity = SENSITIVITY;
	Fov = FOV;

	updateCameraVectors();
}


//Returns the view matrix calculated using euler angles and the LookAt matrix
glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Front, Up); //lookAt() automatically normalises direction argument (Position + Front)
}


//Processes keyboard input in the form of camera defined ENUM
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = MovementSpeed * deltaTime;
	if (direction == FORWARD)
		Position += Front * velocity;
	if (direction == BACKWARD)
		Position -= Front * velocity;
	if (direction == LEFT)
		Position -= Right * velocity;
	if (direction == RIGHT)
		Position += Right * velocity;
}


//Processes mouse input
void Camera::ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch)
{
	xOffset *= MouseSensitivity;
	yOffset *= MouseSensitivity;

	Yaw += xOffset;
	Pitch += yOffset;

	//Constraints
	if (constrainPitch) {
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	//Update Front, Right, and Up vectors using the updated euler angles
	updateCameraVectors();
}


//Processes scroll wheel input
void Camera::ProcessMouseScroll(float yOffset)
{
	Fov -= (float)yOffset;
	if (Fov < 1.0f)
		Fov = 1.0f;
	if (Fov > 100.0f)
		Fov = 100.0f;
}

//Calculates the front vector from the camera's updated euler angles
void Camera::updateCameraVectors()
{
	//Calculate the new front vector
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);

	//Calculate the new Right and Up vectors
	Right = glm::normalize(glm::cross(Front, WorldUp));
	Up = glm::normalize(glm::cross(Right, Front));
}