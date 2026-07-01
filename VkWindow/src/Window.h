#pragma once
#include <string>
#include <vulkan/vulkan.h>
#include <optional>
#include <vector>
#include "VulkanRenderer.h"

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

  Window(const WindowInfo& info);
  ~Window();

  void Run();

  GLFWwindow* GetNativeWindow() { return m_WindowData.m_Window; }

private:
  struct WindowData
  {
    GLFWwindow* m_Window;
  };
  std::shared_ptr<VulkanRenderer> m_Renderer;
  WindowData m_WindowData;
};
