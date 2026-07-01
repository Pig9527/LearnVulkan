#pragma once
#include <vulkan/vulkan.h>
#include <memory>
#include <optional>
struct GLFWwindow;

struct QueueFamilyIndices
{
  std::optional<uint32_t> GrpahicFamily;
  std::optional<uint32_t> PresentFamily;

  bool isComplete()
  {
    return GrpahicFamily.has_value() && PresentFamily.has_value();
  }
};

class VulkanRenderer
{
public:
  VulkanRenderer(GLFWwindow *window);
  ~VulkanRenderer();

  void CreateInstance();
  void CreateDebug();
  void CreateSurface();
  void CreatePhysicalDevice();
  void CreateDevice();
  void CreateDeviceQueueFamily();
  void CreateShader();

  VkInstance GetInstance() { return m_vkInstance; }

  static std::shared_ptr<VulkanRenderer> Create(GLFWwindow *window);

private:
  bool isSuitableDevice(VkPhysicalDevice physicalDevice);
  QueueFamilyIndices FindQueueFamily(VkPhysicalDevice physicalDevice);
private:
  GLFWwindow *m_widnow;
  VkInstance m_vkInstance;
  VkDebugUtilsMessengerEXT m_vkDebugMessenger;
  VkPhysicalDevice m_vkPhysicalDevice;
  VkDevice m_vkDevice;
  VkQueue m_vkGraphicsQueue;
  VkQueue m_vkPresentQueue;
  VkSurfaceKHR m_vkSurface;
  VkShaderModule m_vkVertexModule;
  VkShaderModule m_vkFragmentModule;
};
