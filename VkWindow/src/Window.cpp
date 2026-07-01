#include "Window.h"

#include <GLFW/glfw3.h>
#include <functional>
#include <iostream>

#include <set>
#include <cstring>
#include <fstream>

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


  m_Renderer = VulkanRenderer::Create(window);

  m_Renderer->CreateInstance();
  m_Renderer->CreateDebug();
  m_Renderer->CreateSurface();
  m_Renderer->CreatePhysicalDevice();
  m_Renderer->CreateDevice();
  m_Renderer->CreateSwapChain();
  m_Renderer->CreateImageView();
  m_Renderer->CreateRenderPass();
  m_Renderer->CreateGraphicPipeline();
}

Window::~Window()
{
  glfwDestroyWindow(m_WindowData.m_Window);
  glfwTerminate();
}

void Window::Run()
{
  while (s_Runing)
  {
    glfwPollEvents();

  }
}
