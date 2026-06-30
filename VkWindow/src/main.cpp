#pragma once

#define GLFW_INCLUDE_VULKAN

#include "GLFW/glfw3.h"

#include <iostream>
#include <vulkan/vulkan.h>
#include <vector>



std::vector<const char*> RequireLayer = { "VK_LAYER_KHRONOS_validation" };


int main(int argc, char* argv[])
{


  if (!glfwInit())
  {
    return 0;
  }


  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);



  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);


  VkApplicationInfo appInfo = {
    VK_STRUCTURE_TYPE_APPLICATION_INFO,
    nullptr,
    "care",
    VK_MAKE_VERSION(1,0,0),
    "don't care",
    VK_MAKE_VERSION(1,0,0),
    VK_API_VERSION_1_0
  };

  uint32_t glfwExtensionCount = 0;
  const char** glfwExetensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  for (int i=0;i<glfwExtensionCount;i++)
  {
    std::cout << "  " << glfwExetensions[i] << std::endl;
  }

  VkInstanceCreateInfo instanceCreateInfo = {
    VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    nullptr,
    0,
    &appInfo,
    RequireLayer.size(),
    RequireLayer.data(),
    glfwExtensionCount,
    glfwExetensions
  };

  VkInstance Instance;

  VkResult rs =  vkCreateInstance(&instanceCreateInfo, nullptr, &Instance);
  if (rs != VK_SUCCESS)
  {
    std::cout << "failed to create window" << std::endl;
  }

  VkSurfaceKHR Surface;


  GLFWwindow* window = glfwCreateWindow(1280, 720, "LearnVulkan", nullptr, nullptr);

  glfwCreateWindowSurface(Instance, window, nullptr, &Surface);

  while (!glfwWindowShouldClose(window))
  {

    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();


  return 0;
}
