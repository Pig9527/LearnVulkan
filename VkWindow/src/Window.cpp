#include "Window.h"

#include <GLFW/glfw3.h>
#include <functional>
#include <iostream>

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


  VkApplicationInfo applicationInfo{};
  applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  applicationInfo.apiVersion = VK_VERSION_1_3;
  applicationInfo.pApplicationName = "vulkan";
  applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  applicationInfo.pEngineName = "no engine";
  applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

  uint32_t glfwExtensionCount;
  const char** glfwExtension;
  glfwExtension = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &applicationInfo;
  createInfo.enabledExtensionCount = glfwExtensionCount;
  createInfo.ppEnabledExtensionNames = glfwExtension;
  createInfo.enabledLayerCount = 0;
  createInfo.ppEnabledLayerNames = nullptr;

  for (uint32_t i = 0; i < glfwExtensionCount; i++)
  {
    std::cout << *(glfwExtension + i) << std::endl;
  }

  VkResult result = vkCreateInstance(&createInfo, nullptr, &m_vkInstance);
  if (result != VK_SUCCESS)
  {
    switch (result)
    {
    case VK_ERROR_INCOMPATIBLE_DRIVER:
      std::cout << "no incompatible driver" << std::endl;
      break;
    case VK_ERROR_EXTENSION_NOT_PRESENT:
      std::cout << " vulkan could not support extension" << std::endl;
      break;
    default:
      std::cout << "unkown error" << std::endl;
      break;
    }
  }

}

Window::~Window()
{
  vkDestroyInstance(m_vkInstance,nullptr);

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
