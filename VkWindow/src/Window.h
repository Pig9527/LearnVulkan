#pragma once
#include <string>
#include <vulkan/vulkan.h>
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



  WindowData m_WindowData;

  VkInstance m_vkInstance;


};