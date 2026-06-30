#include "Window.h"

#include <GLFW/glfw3.h>
#include <functional>
#include <iostream>

#include <set>
#include <cstring>
#include <fstream>

static bool s_Runing = true;

const std::vector<const char*> validationLayers = {
  "VK_LAYER_KHRONOS_validation"
};
const std::vector<const char*> deviceExtensions = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
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
  applicationInfo.apiVersion = VK_API_VERSION_1_3;
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


  result = glfwCreateWindowSurface(m_vkInstance, m_WindowData.m_Window, nullptr, &m_vkSurface);
  if (result != VK_SUCCESS)
  {
    std::cout << "failed to create window surface" << std::endl;
    return;
  }


  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, nullptr);

  if (deviceCount == 0)
  {
    std::cout << "failed to find GPUs with vulkan support" << std::endl;
    return;
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);

  vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, devices.data());

  for (const VkPhysicalDevice& device :devices)
  {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    std::cout << deviceProperties.deviceName << std::endl;

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const VkQueueFamilyProperties& queueFamily : queueFamilies)
    {
      QueueFamilyIndices indices{};
      if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
      {
        indices.GraphicsFamily = i;
      }

      VkBool32 presentSupport = false;
      vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_vkSurface, &presentSupport);

      if (presentSupport)
      {
        indices.PresentFamily = i;
      }

      if (indices.isComplete())
      {
        m_QueueFamilyIndices = indices;
        m_vkPhysicalDevice = device;
        break;
      }

      i++;
    }
  }
  if (m_vkPhysicalDevice == VK_NULL_HANDLE)
  {
    std::cout << " failed to find a suitable GPU" << std::endl;
  }


  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = { m_QueueFamilyIndices.GraphicsFamily.value(),m_QueueFamilyIndices.PresentFamily.value() };
  float queuePriority = 1.0f;
  for (uint32_t queuefamily : uniqueQueueFamilies)
  {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queuefamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeatures{};
  VkDeviceCreateInfo deviceCreateInfo{};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
  deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
  deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
  deviceCreateInfo.enabledExtensionCount = 0;

  result = vkCreateDevice(m_vkPhysicalDevice, &deviceCreateInfo, nullptr, &m_vkLogicDevice);
  if (result != VK_SUCCESS)
  {
    std::cout << "failed to create logic device" << std::endl;
  }

  vkGetDeviceQueue(m_vkLogicDevice, m_QueueFamilyIndices.GraphicsFamily.value(), 0, &m_vkGraphicsQueue);
  vkGetDeviceQueue(m_vkLogicDevice, m_QueueFamilyIndices.PresentFamily.value(), 0, &m_vkPresentQueue);

  CreateGraphicPipeline();
}

Window::~Window()
{
  vkDestroyDevice(m_vkLogicDevice,nullptr);

  if (enableValidationLayers)
  {
    auto DestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_vkInstance, "vkDestroyDebugUtilsMessengerEXT");
    if (DestroyDebugUtilsMessengerEXT)
    {
      DestroyDebugUtilsMessengerEXT(m_vkInstance, m_DebugMessenger, nullptr);
    }
  }

  vkDestroySurfaceKHR(m_vkInstance,m_vkSurface,nullptr);
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

void Window::CreateGraphicPipeline()
{

  ShaderCode vertexCode{};
  ShaderCode fragmentCode{};
  bool resultV = readFile("shader/vert.spv", vertexCode);
  bool resultF = readFile("shader/frag.spv", fragmentCode);
  if (!resultV || !resultF)
  {
    return;
  }
  std::cout << "vertex size :" << vertexCode.CodeSize << std::endl;
  std::cout << "fragmnet size :" << fragmentCode.CodeSize << std::endl;

  VkShaderModuleCreateInfo shaderModuleInfo{};
  shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shaderModuleInfo.codeSize = vertexCode.CodeSize;
  shaderModuleInfo.pCode = reinterpret_cast<const uint32_t*>(vertexCode.Buff);

  VkResult result = vkCreateShaderModule(m_vkLogicDevice, &shaderModuleInfo, nullptr, &m_vkVertexShader);
  if (result != VK_SUCCESS)
  {
    std::cout << "failed to create vertex shader" << std::endl;
  }

  shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shaderModuleInfo.codeSize = fragmentCode.CodeSize;
  shaderModuleInfo.pCode = reinterpret_cast<uint32_t*>(fragmentCode.Buff);
  result = vkCreateShaderModule(m_vkLogicDevice, &shaderModuleInfo, nullptr, &m_vkFragmentShader);

  if (result != VK_SUCCESS)
  {
    std::cout << "failed to create Fragment shader" << std::endl;
  }


  VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = m_vkVertexShader;
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragmentShaderStageInfo{};
  fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragmentShaderStageInfo.module = m_vkFragmentShader;
  fragmentShaderStageInfo.pName = "main";


  VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo ,fragmentShaderStageInfo };

  vkDestroyShaderModule(m_vkLogicDevice, m_vkFragmentShader, nullptr);
  vkDestroyShaderModule(m_vkLogicDevice, m_vkVertexShader, nullptr);

  delete vertexCode.Buff;
  delete fragmentCode.Buff;
}

bool Window::readFile(const std::string& filePath, ShaderCode& code)
{

  std::ifstream file(filePath, std::ios::ate | std::ios::binary);
  if (!file.is_open())
  {
    return false;
  }
  size_t fileSize = file.tellg();

  code.Buff = new char[fileSize];
  code.CodeSize = fileSize;

  file.seekg(0);
  file.read(code.Buff, fileSize);

  file.close();
  return true;
}
