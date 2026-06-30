#pragma once
#include <vulkan/vulkan.h>
#include <memory>

class VulkanRenderer
{
public:
    VulkanRenderer();
    ~VulkanRenderer();

    void CreateInstance();
    void CreateDebug();

    VkInstance GetInstance(){return m_vkInstance;}

    static std::shared_ptr<VulkanRenderer> Create();
private:
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