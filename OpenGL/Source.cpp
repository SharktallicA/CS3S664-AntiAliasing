#include "Includes.h"
#include "Object.h"
#include "Renderer.h"
#include "Physics.h"
#include "Utility.h"
#include "Scene.h"
#include <vector>

#pragma region OGL function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
#pragma endregion

#pragma region Scene configuration
//								 width	height 	near-plane	far-plane
Camera_settings camera_settings{ 800, 800, 0.1, 100.0 };
Camera* camera = new Camera(camera_settings, glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = camera_settings.screenWidth / 2.0f;
float lastY = camera_settings.screenHeight / 2.0f;
Timer timer;
#pragma endregion

Scene* scene;

int main()
{
	Utility::setConTitle("15005070");

	//Request user input for AA setting
	cout << "1) No anti-aliasing\n";
	cout << "2) FBO Supersampling anti-aliasing (SSAA)\n";
	cout << "3) FBO SSAA + Multi-sampling anti-aliasing (MSAA)\n";
	AntiAliasingOption aaOpt = AntiAliasingOption(Utility::getInt("Enter anti-aliasing option: ", 1, 3) - 1);

	int aaSampleRate = 1;
	if (aaOpt != None)
		aaSampleRate = Utility::getInt("Enter sample rate (s | 2): ", 2, 128);

	#pragma region OGL configuration
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	//Provide sample rate
	glfwWindowHint(GLFW_SAMPLES, aaSampleRate);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(camera_settings.screenWidth, camera_settings.screenHeight, "Real-Time Rendering: Tutorial 3", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Set the callback functions
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	#pragma endregion

	//Rendering settings
	glfwSwapInterval(1); // glfw enable swap interval to match screen v-sync
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE); //Enables face culling
	glFrontFace(GL_CCW);//Specifies which winding order if front facing
	glEnable(GL_FRAMEBUFFER_SRGB);

	//Enable MSAA if requested
	if (aaOpt == MSAA) glEnable(GL_MULTISAMPLE);

	scene = new Scene(aaOpt, aaSampleRate);

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		timer.tick();

		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		scene->Render(camera, timer.getDeltaTimeMiliseconds());

		string title = "FPS: " + std::to_string(timer.averageFPS()) + " SPF: " + std::to_string(timer.currentSPF());
		glfwSetWindowTitle(window, title.c_str());

		// glfw: swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

#pragma region OGL functions
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
	timer.updateDeltaTime();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		scene->sceneCam->processKeyboard(FORWARD, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		scene->sceneCam->processKeyboard(BACKWARD, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		scene->sceneCam->processKeyboard(LEFT, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		scene->sceneCam->processKeyboard(RIGHT, timer.getDeltaTimeSeconds());
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	glViewport(0, 0, width, height);
	camera->updateScreenSize(width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		scene->sceneCam->processMouseMovement(xoffset, yoffset);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	scene->sceneCam->processMouseScroll(yoffset);
}
#pragma endregion