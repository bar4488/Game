#include "KeyboardMgr.h"

#include "../../Game.h"

KeyboardMgr::KeyboardMgr(GLFWwindow* window, bool verbose):
	m_Window(window),
	m_Verbose(verbose)
{
	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, KeyboardMgr::KeyCallback);
}

void KeyboardMgr::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	auto k = static_cast<KeyboardMgr*>(glfwGetWindowUserPointer(window));
	k->UpdateKeyState(key, scancode, action, mods);
}

void KeyboardMgr::BeginUpdate()
{
	m_PressedKeys.clear();
	m_ReleasedKeys.clear();
}

void KeyboardMgr::Update()
{

}

void KeyboardMgr::UpdateKeyState(int key, int scancode, int action, int mods)
{
	if(action == GLFW_PRESS)
	{
		if(m_Verbose)
			std::cout << "new key event! key with id: " << key << " was pressed\n";
		m_PressedKeys.insert(key);
	}
	else if(action == GLFW_RELEASE)
	{
		if(m_Verbose)
			std::cout << "new key event! key with id: " << key << " was released\n";
		m_ReleasedKeys.insert(key);
	}
	key_state e = { key, scancode, action == GLFW_PRESS || action == GLFW_REPEAT, mods };
	m_KeyStates[key] = e;
}

bool KeyboardMgr::IsPressed(int key)
{
	if(m_KeyStates.contains(key))
	{
		return m_KeyStates[key].pressed;
	}
	return false;
}

bool KeyboardMgr::IsReleased(int key)
{
	if(m_KeyStates.contains(key))
	{
		return !m_KeyStates[key].pressed;
	}
	return true;
}

bool KeyboardMgr::IsPressStarted(int key)
{
	return m_PressedKeys.contains(key);
}

bool KeyboardMgr::IsPressFinished(int key)
{
	return m_ReleasedKeys.contains(key);
}
