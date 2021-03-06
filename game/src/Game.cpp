//
// Created by bar44 on 11/2/2020.
//

#include "Game.h"
#include "GL/glew.h"
#include "iostream"
#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>


#ifdef _WIN32
#include <Windows.h>
#undef DrawText
#else
#include <unistd.h>
#endif

#include "utils/timer.h"

using namespace std;

Game::Game(int width, int height) :
	m_Width(width),
	m_Height(height),
	m_Fullscreen(false),
	m_TicksPerSecond(60),
	m_Window(SetupGraphics()),
	m_Renderer(width, height),
	m_KeyboardMgr(m_Window, true),
	t_pressed(false),
	full_ratio(true),
	viewport_ratio(1.0f),
	m_Configuration{8u, 0u, width, height, &m_KeyboardMgr, m_Window}
{
}

void Game::Run() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glfwSwapInterval(0);

	// Initialize World
	const auto handle = timer::start();
	m_Renderer.LoadFont("arial", "res/fonts/arial.ttf", 48);
	world = new World(&m_Renderer, &m_Configuration, &m_KeyboardMgr);

	// Game Loop //
	using ns = std::chrono::nanoseconds;
	const ns timestamp(chrono::milliseconds(1000 / m_TicksPerSecond));
	const auto fps_handle = timer::start();
	auto lag = timestamp;
	auto seconds_lag = timestamp;
	int current_fps = 60;
	while (!glfwWindowShouldClose(m_Window)) {
		const auto d_time = timer::lap(fps_handle);
		lag += d_time;
		seconds_lag += d_time;

		if(seconds_lag.count() > 1000000000)
		{
			current_fps = 1000000000 / timer::average(fps_handle).count();
			timer::reset(fps_handle);
			seconds_lag = 0ns;
		}
		while (lag >= timestamp) {
			ProcessEvents();
			if (m_KeyboardMgr.IsPressStarted(GLFW_KEY_F)) {
				if (m_Fullscreen) {
					glfwSetWindowMonitor(m_Window, nullptr, 0,0,m_Width, m_Height, 60);
					m_Fullscreen = false;
				}
				else {
					glfwSetWindowMonitor(m_Window, glfwGetPrimaryMonitor(), 0,0,m_Width, m_Height, 60);
					m_Fullscreen = true;
				}
			}
			if (m_KeyboardMgr.IsPressStarted(GLFW_KEY_T)) {
				full_ratio = !full_ratio;
			}
			if(full_ratio)
			{
				if(viewport_ratio <1.0f)
				{
					viewport_ratio += 0.01f;
				}
			}
			if(!full_ratio)
			{
				if(viewport_ratio > 0.8f)
				{
					viewport_ratio -= 0.01f;
				}
			}
			world->Update();
			lag -= timestamp;
		}
		glViewport(0, 0, m_Width * viewport_ratio, m_Height * viewport_ratio);
		world->Draw();
		glViewport(0, 0, m_Width, m_Height);
		if(viewport_ratio < 1.0f)
		{
			m_Renderer.DrawText("arial", "FPS: " + std::to_string(current_fps), 1.0f, glm::vec2(m_Width * viewport_ratio, 10), glm::vec3(1.0f,1.0f,1.0f));
			m_Renderer.DrawText("arial", "Ms. per frame: " + std::to_string(current_fps == 0 ? 0 : 1000 / current_fps), 1.0f, glm::vec2(m_Width * viewport_ratio, 60), glm::vec3(1.0f,1.0f,1.0f));
			m_Renderer.DrawText("arial", "L. Chunks: " + std::to_string(world->m_ChunkMgr.GetChunkCount()), 1.0f, glm::vec2(m_Width * viewport_ratio, 110), glm::vec3(1.0f,1.0f,1.0f));
			m_Renderer.DrawText("arial", "R. Chunks: " + std::to_string(world->m_ChunkMgr.GetRenderedChunksCount()), 1.0f, glm::vec2(m_Width * viewport_ratio, 160), glm::vec3(1.0f,1.0f,1.0f));
			std::stringstream ss;
			ss << "X: " << std::fixed << std::setprecision(1) << world->m_Player.m_Position.x;
			ss << " Y: " << std::fixed << std::setprecision(1) << world->m_Player.m_Position.y;
			ss << " Z: " << std::fixed << std::setprecision(1) << world->m_Player.m_Position.z;
			m_Renderer.DrawText("arial", ss.str(), 0.6f, glm::vec2(m_Width * viewport_ratio, 210), glm::vec3(1.0f,1.0f,1.0f));
			if(world->m_Player.m_IsPointing)
			{
				std::stringstream ss;
				ss << "X: " << std::fixed << std::setprecision(1) << world->m_Player.m_PointedBlock.x;
				ss << " Y: " << std::fixed << std::setprecision(1) << world->m_Player.m_PointedBlock.y;
				ss << " Z: " << std::fixed << std::setprecision(1) << world->m_Player.m_PointedBlock.z;
				m_Renderer.DrawText("arial", ss.str() , 0.6f, glm::vec2(m_Width * viewport_ratio, 260), glm::vec3(1.0f,1.0f,1.0f));
			}
		}
		glfwSwapBuffers(m_Window);
	}
	glfwTerminate();
}


void Game::Update() {
}

void Game::Draw() {
}

void Game::ProcessEvents()
{
	m_KeyboardMgr.BeginUpdate();
	glfwPollEvents();
	m_KeyboardMgr.Update();
	const auto escape_state = glfwGetKey(m_Window, GLFW_KEY_ESCAPE);
	if (escape_state == GLFW_PRESS) {
		glfwSetWindowShouldClose(m_Window, 1);
	}

}

Game::~Game() = default;

void APIENTRY openglCallbackFunction(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam);

static void error_callback(int error, const char* description);


GLFWwindow* Game::SetupGraphics() {
	glfwSetErrorCallback(error_callback);

	glewExperimental = GL_TRUE;
	/* Initialize the library */
	if (!glfwInit())
		exit(-1);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_SAMPLES, 4);
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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_MULTISAMPLE);


	glEnable(GL_CULL_FACE);

	return window;
}

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
