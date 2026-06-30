#include "Window.h"

#include <GLFW/glfw3.h>
#include <functional>

static bool s_Runing = true;

static void gflwCloseFunc(GLFWwindow* window)
{
  s_Runing = false;
}

Window::Window(const WindowInfo& info)
{
  if (!glfwInit())
  {
	return;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  m_WindowData.m_Window = glfwCreateWindow(info.Width, info.Height, info.Tile.c_str(), nullptr, nullptr);

  GLFWwindow* window = m_WindowData.m_Window;

  if (!window)
  {
	return;
  }

  glfwSetWindowUserPointer(window, (void*)&m_WindowData);

  glfwSetWindowCloseCallback(window, gflwCloseFunc);
}

Window::~Window()
{
  
}

void Window::Run()
{
  while (s_Runing)
  {
	glfwPollEvents();

  }
}
