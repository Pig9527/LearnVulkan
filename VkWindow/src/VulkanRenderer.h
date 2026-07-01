#pragma once
#include <vulkan/vulkan.h>
#include <memory>
#include <optional>
#include <vector>
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

struct SwapChainSupportDetails
{
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> Formats;
  std::vector<VkPresentModeKHR> PresentModes;
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
  void CreateSwapChain();
  void CreateImageView();
  void CreateGraphicPipeline();
  void CreateRenderPass();
  void CreateFrameBuffers();
  void CreateCommandPool();
  void CreateCommandBuffer();
  void CreateSyncObject();

  void DrawFrame();

  void WaitIdle();
  void RecordCommandBuffer(VkCommandBuffer buffer,uint32_t imageIndex);

  VkInstance GetInstance() { return m_vkInstance; }

  static std::shared_ptr<VulkanRenderer> Create(GLFWwindow *window);

private:
  bool isSuitableDevice(VkPhysicalDevice physicalDevice);
  QueueFamilyIndices FindQueueFamily(VkPhysicalDevice physicalDevice);
  bool CheckDeviceExtensSupport(VkPhysicalDevice physicalDevice);
  SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice);
  VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

private:
  GLFWwindow *m_widnow;
  VkInstance m_vkInstance;
  VkDebugUtilsMessengerEXT m_vkDebugMessenger;
  VkPhysicalDevice m_vkPhysicalDevice;
  VkDevice m_vkDevice;
  VkQueue m_vkGraphicsQueue;
  VkQueue m_vkPresentQueue;
  VkSurfaceKHR m_vkSurface;
  VkSwapchainKHR m_vkSwapChain;
  std::vector<VkImage> m_SwapChainImages;
  VkFormat m_SwapChainImageFromat;
  VkExtent2D m_SwapChainImageExtent;
  std::vector<VkImageView> m_SwapChainImageViews;
  VkRenderPass m_vkRenderPass;
  VkPipelineLayout m_vkPipeLineLayout;
  VkPipeline m_vkGraphicPipeLine;
  VkShaderModule m_vkVertexModule;
  VkShaderModule m_vkFragmentModule;

  std::vector<VkFramebuffer> m_SwapChainFrameBuffers;

  VkCommandPool m_vkCommandPool;
  VkCommandBuffer m_vkCommandBuffer;

  VkSemaphore m_ImageAvailableSemaphore;
  VkSemaphore m_RenderFinishedSemaphore;
  VkFence m_inFlightFence;
};
