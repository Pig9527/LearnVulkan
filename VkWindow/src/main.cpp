#pragma once

//#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include "Window.h"

#include <iostream>
#include <vulkan/vulkan.h>
#include <vector>



std::vector<const char*> RequireLayer = { "VK_LAYER_KHRONOS_validation" };


int main(int argc, char* argv[])
{

  Window::WindowInfo info{ 1280,720,"vk" };
  Window* window = new Window(info);

  window->Run();

  delete window;

  return 0;
}
