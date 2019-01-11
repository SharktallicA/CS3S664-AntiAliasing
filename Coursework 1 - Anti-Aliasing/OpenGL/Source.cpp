#include "Includes.h"
#include "Object.h"
#include "Renderer.h"
#include "Physics.h"
#include "Utility.h"
#include "SSAAScene.h"
#include <vector>

#pragma region OGL function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
#pragma endregion

#pragma region Main configuration
//								 width	height 	near-plane	far-plane
Camera_settings camera_settings{ 800, 800, 0.1, 100.0 };
Camera* camera = nullptr;
int width = 800;
int height = 600;
float lastX = width / 2.0f;
float lastY = height / 2.0f;
Timer timer;
bool flipScene = false;
#pragma endregion

SSAAScene* ssaaScene;

int main()
{
	Utility::setConTitle("CS3S664: Real-Time Rendering Techniques Coursework 1\n");

	//Print solution information
	cout << "CS3S664: Real-Time Rendering Techniques Coursework 1\n";
	cout << "Khalid Ali (15005070)\n";
	cout << "Press \"space\" to toggle between FBO SSAA scene and non-FBO MSAA scene\n\n";

	//Construct camera after requesting scene width/height from user
	int width = Utility::getInt("Enter scene width: ", 800, 1600);
	int height = Utility::getInt("Enter scene height: ", 600, 900);
	lastX = width / 2.0f;
	lastY = height / 2.0f;
	camera = new Camera(width, height, 0.1, 100.0, glm::vec3(0.0f, 0.0f, 10.0f));

	//Subsample size for AA
	int aaSamples = 1; //1 as fallback means the FBO colour texture will be rendered at 'normal size'
	aaSamples = Utility::getInt("Enter samples (s | 2, otherwise 1 to turn off AA): ", 1, 16);
	while (aaSamples % 2 != 0 && aaSamples != 1)
		aaSamples = Utility::getInt("ERROR: value must be divisible by 2.\nTry again: ", 2, 16);

	#pragma region OGL configuration
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	//Provide AA subsample size
	glfwWindowHint(GLFW_SAMPLES, aaSamples);

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(width, height, "Real-Time Rendering: Tutorial 3", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Set the callback functions
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
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
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE); //Enables face culling
	glFrontFace(GL_CCW);//Specifies which winding order if front facing
	glEnable(GL_FRAMEBUFFER_SRGB);

	//Enable MSAA
	glEnable(GL_MULTISAMPLE);

	ssaaScene = new SSAAScene(camera, width, height, aaSamples);

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);
		timer.tick();

		glClearColor(0, 0, 0, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		if (!flipScene)
			ssaaScene->Render(timer.getDeltaTimeMiliseconds());

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
		camera->processKeyboard(FORWARD, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->processKeyboard(BACKWARD, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->processKeyboard(LEFT, timer.getDeltaTimeSeconds());
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->processKeyboard(RIGHT, timer.getDeltaTimeSeconds());
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	glViewport(0, 0, width, height);
	camera->updateScreenSize(width, height);
}

// glfw: whenever a key is pressed, this callback is called
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		flipScene = !flipScene;
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
		camera->processMouseMovement(xoffset, yoffset);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera->processMouseScroll(yoffset);
}
#pragma endregion