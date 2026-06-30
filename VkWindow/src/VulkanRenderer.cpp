#include "VulkanRenderer.h"
#include <string>
#include <set>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>
#include <GLFW/glfw3.h>

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSevertiy,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData)
{
    std::cerr << "Validation layer:" << pCallbackData->pMessage << std::endl;
}

VulkanRenderer::VulkanRenderer()
{
}

VulkanRenderer::~VulkanRenderer()
{
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

    VkInstanceCreateInfo createinfo{};
    createinfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createinfo.pApplicationInfo = &applicationInfo;
    createinfo.enabledExtensionCount = glfwExtensionsCount;
    createinfo.ppEnabledExtensionNames = glfwExtension;
    createinfo.enabledLayerCount = 0;
    createinfo.ppEnabledLayerNames = nullptr;

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
}

std::shared_ptr<VulkanRenderer> VulkanRenderer::Create()
{
    return std::make_shared<VulkanRenderer>();
}
