#include "VulkanRenderer.h"
#include "LoadFile.h"
#include <string>
#include <set>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#include <GLFW/glfw3.h>

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

static VkDebugUtilsMessengerCreateInfoEXT s_DebugUtilsMessengerCreateInfo;

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSevertiy,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData)
{
  std::cerr << "Validation layer:" << pCallbackData->pMessage << std::endl;
  return VK_FALSE;
}

VulkanRenderer::VulkanRenderer(GLFWwindow *window)
    : m_widnow(window)
{
  s_DebugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  s_DebugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;

  s_DebugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;

  s_DebugUtilsMessengerCreateInfo.pfnUserCallback = DebugCallback;
  s_DebugUtilsMessengerCreateInfo.pUserData = nullptr;
}

VulkanRenderer::~VulkanRenderer()
{
  vkDestroyDevice(m_vkDevice, nullptr);

  if (enableValidationLayers)
  {
    auto DestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_vkInstance, "vkDestroyDebugUtilsMessengerEXT");
    if (DestroyDebugUtilsMessengerEXT)
    {
      DestroyDebugUtilsMessengerEXT(m_vkInstance, m_vkDebugMessenger, nullptr);
    }
  }
  vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, nullptr);
  vkDestroyInstance(m_vkInstance, nullptr);
}

void VulkanRenderer::CreateInstance()
{
  VkApplicationInfo applicationInfo{};
  applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  applicationInfo.apiVersion = VK_API_VERSION_1_3;
  applicationInfo.pApplicationName = "vulkan";
  applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  applicationInfo.pEngineName = "no engine";
  applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

  uint32_t glfwExtensionsCount = 0;
  const char **glfwExtension = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

  std::vector<const char *> extensions(glfwExtension, glfwExtension + glfwExtensionsCount);
  if (enableValidationLayers)
  {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  VkInstanceCreateInfo createinfo{};
  createinfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createinfo.pApplicationInfo = &applicationInfo;
  createinfo.enabledExtensionCount = extensions.size();
  createinfo.ppEnabledExtensionNames = extensions.data();
  if (enableValidationLayers)
  {
    createinfo.enabledLayerCount = validationLayers.size();
    createinfo.ppEnabledLayerNames = validationLayers.data();
    createinfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&s_DebugUtilsMessengerCreateInfo;
  }
  else
  {
    createinfo.enabledLayerCount = 0;
    createinfo.ppEnabledLayerNames = nullptr;
  }
  for (uint32_t i = 0; i < glfwExtensionsCount; i++)
  {
    std::cout << *(glfwExtension + i) << std::endl;
  }
  VkResult result = VK_SUCCESS;
  result = vkCreateInstance(&createinfo, nullptr, &m_vkInstance);

  if (result != VK_SUCCESS)
  {
    switch (result)
    {
    case VK_ERROR_INCOMPATIBLE_DRIVER:
      std::cout << "incompatible driver" << std::endl;
      break;
    case VK_ERROR_EXTENSION_NOT_PRESENT:
      std::cout << "extension not present" << std::endl;
      break;
    default:
      std::cout << "Unknown error" << std::endl;
      break;
    }
    std::cout << "failed to create vk instance" << std::endl;
  }
}

void VulkanRenderer::CreateDebug()
{
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> layers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

  for (const VkLayerProperties &properties : layers)
  {
    std::cout << properties.layerName << std::endl;
  }

  for (const char *layerName : validationLayers)
  {
    for (const VkLayerProperties &properties : layers)
    {
      if (strcmp(layerName, properties.layerName) == 0)
      {
        break;
      }
    }
  }

  auto CreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_vkInstance, "vkCreateDebugUtilsMessengerEXT");
  if (!CreateDebugUtilsMessengerEXT)
  {
    std::cout << " failed to load vkCreateDebugUtilsMessengerEXT" << std::endl;
  }

  VkResult result = VK_SUCCESS;
  result = CreateDebugUtilsMessengerEXT(m_vkInstance, &s_DebugUtilsMessengerCreateInfo, nullptr, &m_vkDebugMessenger);

  if (result != VK_SUCCESS)
  {
    std::cout << "failed to create debug utils" << std::endl;
  }
}

void VulkanRenderer::CreateSurface()
{
  VkResult result = VK_SUCCESS;
  glfwCreateWindowSurface(m_vkInstance, m_widnow, nullptr, &m_vkSurface);
  if (result != VK_SUCCESS)
  {
    std::cout << "falied to create window surface" << std::endl;
  }
}

void VulkanRenderer::CreatePhysicalDevice()
{
  VkResult result = VK_SUCCESS;
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, nullptr);
  if (deviceCount == 0)
  {
    std::cout << "failed to find GPUs with vulkan support" << std::endl;
    return;
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, devices.data());

  for (const VkPhysicalDevice &device : devices)
  {
    if (isSuitableDevice(device))
    {
      m_vkPhysicalDevice = device;
    }
  }

  if (m_vkPhysicalDevice == VK_NULL_HANDLE)
  {
    std::cout << "failed to fin a suitable physical GPU" << std::endl;
  }
}

void VulkanRenderer::CreateDevice()
{
  QueueFamilyIndices indice = FindQueueFamily(m_vkPhysicalDevice);

  float queuePrority = 1.0f;
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {indice.GrpahicFamily.value(), indice.PresentFamily.value()};
  for (uint32_t queuqFamily : uniqueQueueFamilies)
  {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queuqFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePrority;

    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeature{};
  VkDeviceCreateInfo deviceCreateInfo{};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
  deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
  deviceCreateInfo.pEnabledFeatures = &deviceFeature;
  deviceCreateInfo.enabledExtensionCount = 0;
  VkResult result = vkCreateDevice(m_vkPhysicalDevice, &deviceCreateInfo, nullptr, &m_vkDevice);
  if (result != VK_SUCCESS)
  {
    std::cout << "failed to ceate device" << std::endl;
  }

  vkGetDeviceQueue(m_vkDevice, indice.GrpahicFamily.value(), 0, &m_vkGraphicsQueue);
  vkGetDeviceQueue(m_vkDevice, indice.PresentFamily.value(), 0, &m_vkPresentQueue);
}

void VulkanRenderer::CreateShader()
{
  VkResult result = VK_SUCCESS;

  LoadFile vertexfile("shader/vert.spv");
  vertexfile.Open();
  std::vector<char> vertexBuff(vertexfile.GetFileSize());
  vertexfile.ReadFromFile(vertexBuff);

  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = vertexBuff.size();
  createInfo.pCode = reinterpret_cast<uint32_t *>(vertexBuff.data());

  result = vkCreateShaderModule(m_vkDevice, &createInfo, nullptr, &m_vkVertexModule);

  if (result != VK_SUCCESS)
  {
    std::cout << "failed to create vertex shader" << std::endl;
    return;
  }

  LoadFile fragFile("shader/frag.spv");
  fragFile.Open();
  std::vector<char> fragBuff(fragFile.GetFileSize());
  fragFile.ReadFromFile(fragBuff);

  createInfo.codeSize = fragBuff.size();
  createInfo.pCode = reinterpret_cast<uint32_t *>(fragBuff.data());
  result = vkCreateShaderModule(m_vkDevice, &createInfo, nullptr, &m_vkFragmentModule);

  if (result != VK_SUCCESS)
  {
    std::cout << "failed to crate fragment shader" << std::endl;
    return;
  }
  VkPipelineShaderStageCreateInfo vertexPipelineCreateInfo{};
  vertexPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertexPipelineCreateInfo.module = m_vkVertexModule;
  vertexPipelineCreateInfo.pName = "main";
  vertexPipelineCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

  VkPipelineShaderStageCreateInfo fragmentPipelineCreateInfo{};
  fragmentPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragmentPipelineCreateInfo.module = m_vkFragmentModule;
  fragmentPipelineCreateInfo.pName = "main";
  fragmentPipelineCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkPipelineShaderStageCreateInfo shaderInfo[] = {vertexPipelineCreateInfo, fragmentPipelineCreateInfo};

  vkDestroyShaderModule(m_vkDevice, m_vkFragmentModule, nullptr);
  vkDestroyShaderModule(m_vkDevice, m_vkVertexModule, nullptr);
}

void VulkanRenderer::CreateDeviceQueueFamily()
{
}

std::shared_ptr<VulkanRenderer> VulkanRenderer::Create(GLFWwindow *window)
{
  return std::make_shared<VulkanRenderer>(window);
}

bool VulkanRenderer::isSuitableDevice(VkPhysicalDevice physicalDevice)
{
  VkPhysicalDeviceProperties deviceProperties{};
  vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
  std::cout << "physical name :" << deviceProperties.deviceName << std::endl;

  VkPhysicalDeviceFeatures deviceFeature{};
  vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeature);

  QueueFamilyIndices indice = FindQueueFamily(physicalDevice);

  return indice.isComplete();
}

QueueFamilyIndices VulkanRenderer::FindQueueFamily(VkPhysicalDevice physicalDevice)
{
  QueueFamilyIndices indice{};

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

  int index = 0;
  for (const VkQueueFamilyProperties &properties : queueFamilies)
  {
    if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
    {
      indice.GrpahicFamily = index;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index, m_vkSurface, &presentSupport);
    if (presentSupport)
    {
      indice.PresentFamily = index;
    }

    index++;
  }
  return indice;
}
