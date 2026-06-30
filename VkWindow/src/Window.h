#pragma once
#include <string>
#include <vulkan/vulkan.h>
#include <optional>
struct GLFWwindow;

class Window
{
public:
  struct WindowInfo
  {
  int Width;
  int Height;
  std::string Tile;
  };

  struct QueueFamilyIndices {
    std::optional<uint32_t> GraphicsFamily;
    std::optional<uint32_t> PresentFamily;
    bool isComplete()
    {
      return GraphicsFamily.has_value() && PresentFamily.has_value();
    }
  };


  Window(const WindowInfo& info);

  ~Window();

  void Run();

  GLFWwindow* GetNativeWindow() { return m_WindowData.m_Window; }

private:
  struct WindowData
  {
  GLFWwindow* m_Window;
  };



  WindowData m_WindowData;

  VkInstance m_vkInstance;

  VkDebugUtilsMessengerEXT m_DebugMessenger;

  VkPhysicalDevice m_vkPhysicalDevice;

  QueueFamilyIndices m_QueueFamilyIndices;

  VkDevice m_vkLogicDevice;

  VkQueue m_vkGraphicsQueue;
  VkQueue m_vkPresentQueue;

  VkSurfaceKHR m_vkSurface;



};
