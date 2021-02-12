//
// Created by bar44 on 11/2/2020.
//

#include "Game.h"
#include "GL/glew.h"
#include "iostream"
#include "chrono"

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

using namespace std;

void APIENTRY openglCallbackFunction(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam);

static void error_callback(int error, const char* description);

Game::Game(int width, int height) :
	m_Width(width),
	m_Height(height),
	m_TicksPerSecond(60),
	m_Window(SetupGraphics()),
	m_Renderer(width, height),
	m_Configuration{2u,1u,width,height,m_Window}
{
}

void Game::Run() {
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
	glfwSwapInterval(0);

	// Initialize World
	world = new World(&m_Renderer, &m_Configuration);

	// Game Loop //
	using clock = std::chrono::high_resolution_clock;
	auto previous = clock::now();
	std::chrono::nanoseconds lag(0ns);
	std::chrono::nanoseconds timestamp(chrono::milliseconds(1000 / m_TicksPerSecond));
	while (!glfwWindowShouldClose(m_Window)) {
		auto current = clock::now();
		auto deltaTime = current - previous;
		previous = current;
		lag += std::chrono::duration_cast<std::chrono::nanoseconds>(deltaTime);

		while (lag >= timestamp) {
			glfwPollEvents();
			int state = glfwGetKey(m_Window, GLFW_KEY_ESCAPE);
			if (state == GLFW_PRESS) {
				glfwSetWindowShouldClose(m_Window, 1);
			}
			world->Update();
			lag = std::chrono::nanoseconds(0ns);
		}
		world->Draw();
		glfwSwapBuffers(m_Window);
	}
	glfwTerminate();
	std::cout << "done!" << std::endl;
}

GLFWwindow* Game::SetupGraphics() {
	glfwSetErrorCallback(error_callback);

	glewExperimental = GL_TRUE;
	/* Initialize the library */
	if (!glfwInit())
		exit(-1);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow* window = glfwCreateWindow(m_Width, m_Height, "Minecraft", nullptr, nullptr);


	if (!window) {
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		exit(-1);
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// Sets the key mode, so when a key is pressed and released fast,
	// we will still be notified that it has been pressed
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);


	//Initialize GLEW
	GLenum err = glewInit();

	//If GLEW hasn't initialized
	if (err != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		exit(-1);
	}

#if _DEBUG
	if (glDebugMessageCallback) {
		std::cout << "Register OpenGL debug callback " << std::endl;
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(openglCallbackFunction, nullptr);
		GLuint unusedIds = 0;
		glDebugMessageControl(GL_DONT_CARE,
			GL_DONT_CARE,
			GL_DONT_CARE,
			0,
			&unusedIds,
			true);
		std::cout << glGetString(GL_VERSION) << std::endl;
	}
	else
		std::cout << "glDebugMessageCallback not available" << std::endl;
#endif

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);

	return window;
}

void Game::Update() {
}

void Game::Draw() {
}

Game::~Game() = default;

void APIENTRY openglCallbackFunction(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar * message,
	const void* userParam) {

	std::cout << "---------------------opengl-callback-start------------" << std::endl;
	std::cout << "message: " << message << std::endl;
	std::cout << "type: ";
	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		std::cout << "ERROR";
		break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		std::cout << "DEPRECATED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		std::cout << "UNDEFINED_BEHAVIOR";
		break;
	case GL_DEBUG_TYPE_PORTABILITY:
		std::cout << "PORTABILITY";
		break;
	case GL_DEBUG_TYPE_PERFORMANCE:
		std::cout << "PERFORMANCE";
		break;
	case GL_DEBUG_TYPE_OTHER:
	default:
		std::cout << "OTHER";
		break;
	}
	std::cout << std::endl;

	std::cout << "id: " << id << std::endl;
	std::cout << "severity: ";
	switch (severity) {
	case GL_DEBUG_SEVERITY_LOW:
		std::cout << "LOW";
		break;
	case GL_DEBUG_SEVERITY_MEDIUM:
		std::cout << "MEDIUM";
		break;
	case GL_DEBUG_SEVERITY_HIGH:
		std::cout << "HIGH";
		break;
	default:
		std::cout << "UNKNOWN";
		break;
	}
	std::cout << std::endl;
	std::cout << "---------------------opengl-callback-end--------------" << std::endl;
}

//Define an error callback
static void error_callback(int error, const char* description) {
	fputs(description, stderr);
	_fgetchar();
}
