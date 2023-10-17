#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <ASSIMP/config.h>

#include <iostream>
#include <format>

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


	//----SHADERS AND MODELS----//
	Shader lightingShader("./Source Files/Shaders/Object/vert.glsl",
						  "./Source Files/Shaders/Object/frag.glsl");

	Shader lightSourceShader("./Source Files/Shaders/Light Source/vert.glsl",
							 "./Source Files/Shaders/Light Source/frag.glsl");

	Model boxModel("./Resource Files/Box/box.obj");


	//-----------------------//
	//----OBJECT SETTINGS----//
	//-----------------------//

	const int numPointLights{ 4 };
	glm::vec3 pointLightPositions[numPointLights] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};
	
	glm::vec3 pointLightColours[numPointLights] = {
		glm::vec3(0.925f, 0.690f, 0.909f),
		glm::vec3(0.925f, 0.690f, 0.909f),
		glm::vec3(0.925f, 0.690f, 0.909f),
		glm::vec3(0.925f, 0.690f, 0.909f),
	};


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


		//----SETTING UNIFORMS----//

		lightingShader.use();
		lightingShader.setMat4("model", model);
		lightingShader.setMat4("view", view);
		lightingShader.setMat4("projection", projection);
		lightingShader.setVec3("viewPos", camera.Position);

		//--Material--//
		lightingShader.setFloat("material.shininess", 76.8f);


		//--Lights--//
		//Directional lights
		lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("dirLight.diffuse", 0.3f, 0.3f, 0.3f);
		lightingShader.setVec3("dirLight.specular", 0.7f, 0.7f, 0.7f);

		//Point lights
		for (int i = 0; i < numPointLights; i++)
		{
			lightingShader.setVec3(std::format("pointLights[{}].position", i), pointLightPositions[i]);
			lightingShader.setFloat(std::format("pointLights[{}].constant", i), 1.0f);
			lightingShader.setFloat(std::format("pointLights[{}].linear", i), 0.09f);
			lightingShader.setFloat(std::format("pointLights[{}].quadratic", i), 0.032f);
			lightingShader.setVec3(std::format("pointLights[{}].ambient", i), pointLightColours[i] * 0.1f);
			lightingShader.setVec3(std::format("pointLights[{}].diffuse", i), pointLightColours[i]);
			lightingShader.setVec3(std::format("pointLights[{}].specular", i), pointLightColours[i]);
		}

		//Spotlight
		//lightingShader.setVec3("spotlight.position", camera.Position);
		//lightingShader.setVec3("spotlight.direction", camera.Front);
		//lightingShader.setFloat("spotlight.innerCutoffAngle", glm::cos(glm::radians(10.0f)));
		//lightingShader.setFloat("spotlight.outerCutoffAngle", glm::cos(glm::radians(12.5f)));
		//lightingShader.setFloat("spotlight.constant", 1.0f);
		//lightingShader.setFloat("spotlight.linear", 0.09f);
		//lightingShader.setFloat("spotlight.quadratic", 0.032f);
		//lightingShader.setVec3("spotlight.ambient", 0.0f, 0.0f, 0.0f);
		//lightingShader.setVec3("spotlight.diffuse", 1.0f, 1.0f, 1.0f);
		//lightingShader.setVec3("spotlight.specular", 1.0f, 1.0f, 1.0f);


		//----DRAWING OBJECTS----//

		boxModel.Draw(lightingShader);


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