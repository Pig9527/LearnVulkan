#include "Window.h"

#include <GLFW/glfw3.h>
#include <functional>
#include <iostream>
#include <vector>
#include <cstring>
static bool s_Runing = true;

const std::vector<const char*> validationLayers = {
  "VK_LAYER_KHRONOS_validation"
};


#ifdef DEBUG

const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;

#endif // DEBUG

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
  void* pUserData)
{
  std::cerr << "validation layer:"<< pCallbackData->pMessage << std::endl;
  return VK_FALSE;
}

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


  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  std::cout << "layer name:" << std::endl;
  for (const VkLayerProperties& properties : availableLayers)
  {
    std::cout <<properties.layerName << std::endl;
  }

  for (const char* layerName : validationLayers)
  {
    bool layerFound = false;

    for (const auto& layerProperties : availableLayers)
    {
      if (strcmp(layerName, layerProperties.layerName) == 0)
      {
        layerFound = true;
        break;
      }
    }
  }

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

  std::vector<const char*> extensions(glfwExtension, glfwExtension + glfwExtensionCount);
  if (enableValidationLayers)
  {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &applicationInfo;
  createInfo.enabledExtensionCount = extensions.size();
  createInfo.ppEnabledExtensionNames = extensions.data();
  if (enableValidationLayers)
  {
    createInfo.enabledLayerCount = validationLayers.size();
    createInfo.ppEnabledLayerNames = validationLayers.data();


    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
      | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = DebugCallback;
    debugCreateInfo.pUserData = nullptr;

    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
  }
  else
  {
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;

  }

  std::cout << "extentions:" << std::endl;
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


  if (!enableValidationLayers)
  {
    return;
  }

  auto CreateDebugUtilMessengerEXTfunc = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_vkInstance, "vkCreateDebugUtilsMessengerEXT");
  if (!CreateDebugUtilMessengerEXTfunc)
  {
    std::cout << "vkCreateDebugUtilsMessengerEXT func not found" << std::endl;
    return;
  }
  result = CreateDebugUtilMessengerEXTfunc(m_vkInstance, &debugCreateInfo, nullptr, &m_DebugMessenger);
  if (result != VK_SUCCESS)
  {
    std::cout << "Create debug utils messenger falied" << std::endl;
  }


}

Window::~Window()
{
  if (enableValidationLayers)
  {
    auto DestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_vkInstance, "vkDestroyDebugUtilsMessengerEXT");
    if (DestroyDebugUtilsMessengerEXT)
    {
      DestroyDebugUtilsMessengerEXT(m_vkInstance, m_DebugMessenger, nullptr);
    }
  }

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
