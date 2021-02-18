#pragma once

#include <unordered_map>
#include <unordered_set>
#include "gl/glew.h"
#include <GLFW/glfw3.h>


class Game;

struct key_state
{
	int key;
	int scancode;
	bool pressed;
	int mods;
};

class KeyboardMgr
{
public:

	// key pressed/released callback (called by the glfw framework)
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	explicit KeyboardMgr(GLFWwindow* window, bool verbose = true);

	// called by the engine each frame before polling events;
	void BeginUpdate();
	// called by the engine each frame after polling events;
	void Update();

	bool IsPressed(int key);

	bool IsReleased(int key);

	bool IsPressStarted(int key);

	bool IsPressFinished(int key);

	void SetOnPressCallback();

	void SetOnReleaseCallback();

private:
	void UpdateKeyState(int key, int scancode, int action, int mods);
	// every tick they are cleared and then updated with the new keys pressed for the current tick;
	GLFWwindow* m_Window;
	bool m_Verbose;
	std::unordered_map<int, key_state> m_KeyStates;
	std::unordered_set<int> m_PressedKeys;
	std::unordered_set<int> m_ReleasedKeys;
};
