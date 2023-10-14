#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <ASSIMP/config.h>

#include <iostream>

#include "../Header Files/stb_image.h"
#include "../Header Files/shader.h"
#include "../Header Files/camera.h"
#include "../Header Files/model.h"


//------------------------------------------------//
//----FORWARD DECLARATIONS AND INITIALISATIONS----//
//------------------------------------------------//


//----FORWARD DECLARATIONS----//
int initialiseOpenGL();
void processInput(GLFWwindow* window);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);


//----SCREEN SETUP----//
constexpr int winX{ 800 };
constexpr int winY{ 800 };
constexpr float aspectRatio{ winX / winY };
GLFWwindow* window;


//----CAMERA SETUP----//
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); //Normalised direction vector, camera points in -z axis
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float mousePosLastFrameX = winX / 2, mousePosLastFrameY = winY / 2; //Initialise to center of screen
bool firstMouse = true; //Used to stop large jump in mouse movement when starting application


//----TIMING----//
float deltaTime = 0.0f; //Time between current frame and last frame
float lastFrame = 0.0f; //Time of last frame


int main() {
	int error = initialiseOpenGL();
	if (error) { return -1; }

	stbi_set_flip_vertically_on_load(true);

	Shader ourShader("C:/Users/shiny/Desktop/Code/OpenGL/LearnOpenGLAdvanced/Source Files/Shaders/vert.glsl",
		             "C:/Users/shiny/Desktop/Code/OpenGL/LearnOpenGLAdvanced/Source Files/Shaders/frag.glsl");

	Model backpackModel("C:/Users/shiny/Desktop/Code/OpenGL/LearnOpenGLAdvanced/Resource Files/Backpack/backpack.obj");


	//------------------------//
	//----MAIN RENDER LOOP----//
	//------------------------//

	while (!glfwWindowShouldClose(window)) {

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Input
		processInput(window);

		//Clear buffer
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		//----GENERATING CAMERA MATRICES----//
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;

		model = glm::mat4(1.0f);
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Fov), aspectRatio, 0.1f, 100.0f);

		ourShader.use();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);
		ourShader.setMat4("model", model);


		//----DRAWING OBJECTS----//

		backpackModel.Draw(ourShader);


		//Check & call events and swap the buffers
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	glfwTerminate();
	return 0;
}



int initialiseOpenGL()
{
	//----------------------------//
	//----INITIAL OPENGL SETUP----//
	//----------------------------//

	//----INITIALISING GLFW AND GLAD, CREATING WINDOW AND VIEWPORT----//
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	window = glfwCreateWindow(winX, winY, "opengl is great", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window, woopsies" << std::endl;
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialise GLAD, woopsies" << std::endl;
		return 1;
	}


	glViewport(0, 0, winX, winY);
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //Hide cursor


	//----SET CALLBACKS----//
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	return 0;
}



void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}



void processInput(GLFWwindow* window) {

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//Movement
	const float moveSpeed = 10.0f;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	//Wireframe toggle
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
}



void mouse_callback(GLFWwindow* window, double xPos, double yPos) {

	if (firstMouse) { //Initially true
		mousePosLastFrameX = xPos;
		mousePosLastFrameY = yPos;
		firstMouse = false;
	}

	//Refreshing variables
	float xOffset = xPos - mousePosLastFrameX;
	float yOffset = mousePosLastFrameY - yPos; //Reversed since y-coordinates range from bottom to top
	mousePosLastFrameX = xPos;
	mousePosLastFrameY = yPos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}



void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
	camera.ProcessMouseScroll(yOffset);
}