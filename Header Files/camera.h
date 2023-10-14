#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

#include <vector>


//Possible camera movement options
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};


//Default camera values
const float YAW = -90.0f; //Rotated 90 degrees to ensure camera looks along +z axis
const float PITCH = 0.0f;
const float MOVEMENT_SPEED = 10.0f;
const float SENSITIVITY = 0.1f;
const float FOV = 90.0f;


//Abstract camera classes that processes inputs and calculates corresponding euler angles, vectors, and matrices for use with OpenGL
class Camera
{
public:
	//----ATTRIBUTES----//
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	
	float Yaw;
	float Pitch;
	
	float MovementSpeed;
	float MouseSensitivity;
	float Fov;

	
	//----CONSTRUCTORS----//
	
	//Vector Constructor
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
	//Scalar Constructor
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);


	//----METHODS----//

	//Returns the view matrix calculated using euler angles and the LookAt matrix
	glm::mat4 GetViewMatrix();
	
	//Processes keyboard input in the form of camera defined ENUM
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);

	
	//Processes mouse input
	void ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true);


	//Processes scroll wheel input
	void ProcessMouseScroll(float yOffset);


private:
	//Calculates the front vector from the camera's updated euler angles
	void updateCameraVectors();
};


#endif