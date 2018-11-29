
#include <GLAD/glad.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <iostream>
#include "Shader.h"
#include "stb_image.h"
#include "Camera.h"
#include "Model.h"
using namespace std;
using namespace glm;

#pragma region Global Variables
//Window Settings
void framebuffer_size_callback(GLFWwindow* window, int width, int height); //GLFW will automatically call this when window is resized and paramaters will be filled
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int LoadTexture(string path);
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 1080;
typedef Shader::LightSettings lightsettings;

//Camera 
Camera camera(vec3(0.0f, 0.0f, 3.0f));
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;

//Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//Lighting
vec3 lightPos(1.2f, 1.0f, 2.0f);
#pragma endregion

int main()
{
#pragma region Window and GLAD initialization
	//====Initialize glfw====
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Setting the GLFW version to 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Telling glfw to use core which alots a smaller subset of OpenGL features

	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Barry's Engine", NULL, NULL); //Window object creation
	if (window == NULL) //Check if the window was created correctly
	{
		cout << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}

	//====Create Viewport====
	//Call back if window is resized
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //Tells GLFW we want to call this function on every window resize
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//====Initialize glad====
	//important because glad is used to manage function pointers for opengl
	//it needs to be initialized before using any of the opengl functions
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) //Loading the correct OpenGL function pointer location to GLAD (specific to os)
	{
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

#pragma endregion


#pragma region Render Setup
	//====Setting up rendering stuff====
	//Configure global opengl states
	glEnable(GL_DEPTH_TEST);

	//Build and compile shader
	Shader lightingShader("VertexShader.vert", "FragmentShader.frag");
	Shader lampShader("LampShader.vert", "LampShader.frag");


	float testSquareVerts[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(1.0f,  2.0f, 0.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
	};

	Model ourModel("resources/Hiker_2.1.obj");

	//Setup Cube VAO and VBO
	unsigned int cubeVAO, VBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(testSquareVerts), testSquareVerts, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);


	unsigned int lampVAO;
	glGenVertexArrays(1, &lampVAO);
	glBindVertexArray(lampVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

#pragma endregion

#pragma region Load Texture
	unsigned int diffuse = LoadTexture("container.png");	
	unsigned int specular = LoadTexture("container_specular.png");

#pragma endregion

#pragma region Game Loop
	lightingShader.StartPipelineProgram();
	lightingShader.setInt("material.diffuse", 0); // or with shader class
	lightingShader.setInt("material.specular", 1);
	//====Game loop====
	while (!glfwWindowShouldClose(window)) //Check if the window is supposed to close
	{
		//per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//Input commands
		processInput(window);
    
		//Rendering commands
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Bind textures
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuse);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specular);

		//View/Projection/world transform
		mat4 projection = perspective(glm::radians(camera.Zoom), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
		mat4 view = camera.GetViewMatrix();
		mat4 model = mat4(1.0f);
		lightingShader.StartPipelineProgram(projection, view, model);
		lightingShader.setVec3("viewPos", camera.Position);
		lightingShader.setFloat("material.shininess", 32.0f);

		//####Lighting shader######
		#pragma region Lighting shader
		lightsettings settings; // = { "pointLights[0]", pointLightPositions[0], vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
								//	vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f };
		// directional light
		lightingShader.setVec3("dirLight.direction", vec3(-0.2f, -1.0f, -0.3f));
		lightingShader.setVec3("dirLight.ambient", vec3(0.05f, 0.05f, 0.05f));
		lightingShader.setVec3("dirLight.diffuse", vec3(0.4f, 0.4f, 0.4f));
		lightingShader.setVec3("dirLight.specular", vec3(0.5f, 0.5f, 0.5f));
		// point light 1
		settings = { "pointLights[0]", pointLightPositions[0], vec3(0.5f, 0.5f, 0.5f),
					vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.09f, 0.032f };
		lightingShader.setPointLight(settings);

		// point light 2
		settings.name = "pointLights[1]";
		settings.position = pointLightPositions[1];
		lightingShader.setPointLight(settings);

		// point light 3
		settings.name = "pointLights[2]";
		settings.position = pointLightPositions[2];
		lightingShader.setPointLight(settings);

		// point light 4
		settings.name = "pointLights[3]";
		settings.position = pointLightPositions[3];
		lightingShader.setPointLight(settings);
		// spotLight
		lightingShader.setPointLight(settings);
		lightingShader.setVec3("spotLight.position", camera.Position);
		lightingShader.setVec3("spotLight.direction", camera.Front);
		lightingShader.setVec3("spotLight.ambient", vec3(0.0f, 0.0f, 0.0f));
		lightingShader.setVec3("spotLight.diffuse", vec3(1.0f, 1.0f, 1.0f));
		lightingShader.setVec3("spotLight.specular", vec3(1.0f, 1.0f, 1.0f));
		lightingShader.setFloat("spotLight.constant", 1.0f);
		lightingShader.setFloat("spotLight.linear", 0.09);
		lightingShader.setFloat("spotLight.quadratic", 0.032);
		lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
		#pragma endregion

		//Render the test cube
		glBindVertexArray(cubeVAO);
		model = scale(model, glm::vec3(1.2f, 1.4f, 1.2f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		model = mat4(1.0f);
		model = scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
		lightingShader.setMat4("model", model);
		ourModel.Draw(lightingShader);
		

		//Also draw the lamp
		model = mat4(1.0f);
		model = translate(model, lightPos);
		model = scale(model, vec3(0.2f));
		lampShader.StartPipelineProgram(projection, view, model);

		glBindVertexArray(lampVAO);
		for (unsigned int i = 0; i < 4; i++)
		{
			switch (i)
			{
			case 0:
				lampShader.setVec3("color", vec3(1.0, 0.0, 0.0));
				break;
			case 1:
				lampShader.setVec3("color", vec3(0.0, 1.0, 0.0));
				break;
			case 2:
				lampShader.setVec3("color", vec3(0.0, 0.0, 1.0));
				break;
			case 3:
				lampShader.setVec3("color", vec3(1.0, 1.0, 0.0));
				break;
			}
			model = mat4(1.0f);
			model = translate(model, pointLightPositions[i]);
			model = scale(model, vec3(0.2f));
			lampShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//Check and call events | Buffer swapping
		glfwSwapBuffers(window);
		glfwPollEvents(); //Checks for events triggerd (Ex: keyboard or mouse input)
	}

	glDeleteVertexArrays(1, &lampVAO);
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &VBO);
	//After exiting the main loop we need to clean/delet all resources
	glfwTerminate();
#pragma endregion
	return 0;
}

#pragma region Helper/Utility Functions


/*
* Input method
*/
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		camera.Position = vec3(0.0f, 0.0f, 3.0f);
}

unsigned int LoadTexture(string path)
{
	//Loading in Texture
	unsigned int textureID;
	// texture 1
	// ---------
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// load image, create texture and generate mipmaps
	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
											// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// set the texture wrapping parameters & filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //Filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	return textureID;
}
#pragma endregion

#pragma region Callback Functions
/*
* Frame Call back method
*/
void framebuffer_size_callback(GLFWwindow* window, int width, int height) //GLFW will automatically call this when window is resized and paramaters will be filled
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

//struct LightSettings
//{
//	vec3 position;
//	vec3 ambient;
//	vec3 diffuse;
//	vec3 specular;
//	float constant;
//	float linear;
//	float quadratic;
//};


#pragma endregion
