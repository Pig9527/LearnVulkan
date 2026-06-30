#pragma once
#include <string>
#include <vulkan/vulkan.h>
#include <optional>
#include <vector>
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


  struct ShaderCode
  {
    char* Buff;
    int CodeSize;
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
  void CreateGraphicPipeline();

  bool readFile(const std::string& filePaths,ShaderCode& code);
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

  VkShaderModule m_vkVertexShader;
  VkShaderModule m_vkFragmentShader;

};
