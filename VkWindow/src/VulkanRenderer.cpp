#include "VulkanRenderer.h"
#include "LoadFile.h"
#include <string>
#include <set>
#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <GLFW/glfw3.h>

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#ifdef NDBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

static VkDebugUtilsMessengerCreateInfoEXT s_DebugUtilsMessengerCreateInfo;

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSevertiy,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData)
{
  std::cerr << "Validation layer:" << pCallbackData->pMessage << std::endl;
  return VK_FALSE;
}

VulkanRenderer::VulkanRenderer(GLFWwindow *window)
    : m_widnow(window)
{
  s_DebugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  s_DebugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;

  s_DebugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;

  s_DebugUtilsMessengerCreateInfo.pfnUserCallback = DebugCallback;
  s_DebugUtilsMessengerCreateInfo.pUserData = nullptr;
}

VulkanRenderer::~VulkanRenderer()
{
  for (auto framebuffer : m_SwapChainFrameBuffers)
  {
    vkDestroyFramebuffer(m_vkDevice, framebuffer, nullptr);
  }

  vkDestroyPipeline(m_vkDevice, m_vkGraphicPipeLine, nullptr);
  vkDestroyPipelineLayout(m_vkDevice, m_vkPipeLineLayout, nullptr);
  vkDestroyRenderPass(m_vkDevice, m_vkRenderPass, nullptr);

  for (auto imageView : m_SwapChainImageViews)
  {
    vkDestroyImageView(m_vkDevice, imageView, nullptr);
  }

  vkDestroySwapchainKHR(m_vkDevice, m_vkSwapChain, nullptr);
  vkDestroyDevice(m_vkDevice, nullptr);

  if (enableValidationLayers)
  {
    auto DestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_vkInstance, "vkDestroyDebugUtilsMessengerEXT");
    if (DestroyDebugUtilsMessengerEXT)
    {
      DestroyDebugUtilsMessengerEXT(m_vkInstance, m_vkDebugMessenger, nullptr);
    }
  }
  vkDestroySurfaceKHR(m_vkInstance, m_vkSurface, nullptr);
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

  std::vector<const char *> extensions(glfwExtension, glfwExtension + glfwExtensionsCount);
  if (enableValidationLayers)
  {
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  VkInstanceCreateInfo createinfo{};
  createinfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createinfo.pApplicationInfo = &applicationInfo;
  createinfo.enabledExtensionCount = extensions.size();
  createinfo.ppEnabledExtensionNames = extensions.data();
  if (enableValidationLayers)
  {
    createinfo.enabledLayerCount = validationLayers.size();
    createinfo.ppEnabledLayerNames = validationLayers.data();
    createinfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&s_DebugUtilsMessengerCreateInfo;
  }
  else
  {
    createinfo.enabledLayerCount = 0;
    createinfo.ppEnabledLayerNames = nullptr;
  }
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
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> layers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

  for (const VkLayerProperties &properties : layers)
  {
    std::cout << properties.layerName << std::endl;
  }

  for (const char *layerName : validationLayers)
  {
    for (const VkLayerProperties &properties : layers)
    {
      if (strcmp(layerName, properties.layerName) == 0)
      {
        break;
      }
    }
  }

  auto CreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_vkInstance, "vkCreateDebugUtilsMessengerEXT");
  if (!CreateDebugUtilsMessengerEXT)
  {
    std::cout << " failed to load vkCreateDebugUtilsMessengerEXT" << std::endl;
  }

  VkResult result = VK_SUCCESS;
  result = CreateDebugUtilsMessengerEXT(m_vkInstance, &s_DebugUtilsMessengerCreateInfo, nullptr, &m_vkDebugMessenger);

  if (result != VK_SUCCESS)
  {
    std::cout << "failed to create debug utils" << std::endl;
  }
}

void VulkanRenderer::CreateSurface()
{
  VkResult result = VK_SUCCESS;
  glfwCreateWindowSurface(m_vkInstance, m_widnow, nullptr, &m_vkSurface);
  if (result != VK_SUCCESS)
  {
    std::cout << "falied to create window surface" << std::endl;
  }
}

void VulkanRenderer::CreatePhysicalDevice()
{
  VkResult result = VK_SUCCESS;
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, nullptr);
  if (deviceCount == 0)
  {
    std::cout << "failed to find GPUs with vulkan support" << std::endl;
    return;
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(m_vkInstance, &deviceCount, devices.data());

  for (const VkPhysicalDevice &device : devices)
  {
    if (isSuitableDevice(device))
    {
      m_vkPhysicalDevice = device;
    }
  }

  if (m_vkPhysicalDevice == VK_NULL_HANDLE)
  {
    std::cout << "failed to fin a suitable physical GPU" << std::endl;
  }
}

void VulkanRenderer::CreateDevice()
{
  QueueFamilyIndices indice = FindQueueFamily(m_vkPhysicalDevice);

  float queuePrority = 1.0f;
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {indice.GrpahicFamily.value(), indice.PresentFamily.value()};
  for (uint32_t queuqFamily : uniqueQueueFamilies)
  {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queuqFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePrority;

    queueCreateInfos.push_back(queueCreateInfo);
  }

  VkPhysicalDeviceFeatures deviceFeature{};
  VkDeviceCreateInfo deviceCreateInfo{};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
  deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
  deviceCreateInfo.pEnabledFeatures = &deviceFeature;
  deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
  deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

  VkResult result = vkCreateDevice(m_vkPhysicalDevice, &deviceCreateInfo, nullptr, &m_vkDevice);
  if (result != VK_SUCCESS)
  {
    std::cout << "failed to ceate device" << std::endl;
  }

  vkGetDeviceQueue(m_vkDevice, indice.GrpahicFamily.value(), 0, &m_vkGraphicsQueue);
  vkGetDeviceQueue(m_vkDevice, indice.PresentFamily.value(), 0, &m_vkPresentQueue);
}

void VulkanRenderer::CreateGraphicPipeline()
{
  VkResult result = VK_SUCCESS;

  LoadFile vertexfile("shader/vert.spv");
  vertexfile.Open();
  std::vector<char> vertexBuff(vertexfile.GetFileSize());
  vertexfile.ReadFromFile(vertexBuff);

  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = vertexBuff.size();
  createInfo.pCode = reinterpret_cast<uint32_t *>(vertexBuff.data());

  result = vkCreateShaderModule(m_vkDevice, &createInfo, nullptr, &m_vkVertexModule);

  if (result != VK_SUCCESS)
  {
    std::cout << "failed to create vertex shader" << std::endl;
    return;
  }

  LoadFile fragFile("shader/frag.spv");
  fragFile.Open();
  std::vector<char> fragBuff(fragFile.GetFileSize());
  fragFile.ReadFromFile(fragBuff);

  createInfo.codeSize = fragBuff.size();
  createInfo.pCode = reinterpret_cast<uint32_t *>(fragBuff.data());
  result = vkCreateShaderModule(m_vkDevice, &createInfo, nullptr, &m_vkFragmentModule);

  if (result != VK_SUCCESS)
  {
    std::cout << "failed to crate fragment shader" << std::endl;
    return;
  }
  VkPipelineShaderStageCreateInfo vertexPipelineCreateInfo{};
  vertexPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertexPipelineCreateInfo.module = m_vkVertexModule;
  vertexPipelineCreateInfo.pName = "main";
  vertexPipelineCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;

  VkPipelineShaderStageCreateInfo fragmentPipelineCreateInfo{};
  fragmentPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragmentPipelineCreateInfo.module = m_vkFragmentModule;
  fragmentPipelineCreateInfo.pName = "main";
  fragmentPipelineCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkPipelineShaderStageCreateInfo shaderStage[] = {vertexPipelineCreateInfo, fragmentPipelineCreateInfo};

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 0;
  vertexInputInfo.pVertexBindingDescriptions = nullptr;
  vertexInputInfo.vertexAttributeDescriptionCount = 0;
  vertexInputInfo.pVertexAttributeDescriptions = nullptr;

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkPipelineViewportStateCreateInfo viewportInfo{};
  viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportInfo.viewportCount = 1;
  viewportInfo.scissorCount = 1;

  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  std::vector<VkDynamicState> dynamicStates = {
      VK_DYNAMIC_STATE_VIEWPORT,
      VK_DYNAMIC_STATE_SCISSOR};

  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();

  VkPipelineLayoutCreateInfo pipeLayoutInfo{};
  pipeLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeLayoutInfo.setLayoutCount = 0;
  pipeLayoutInfo.pushConstantRangeCount = 0;
  if (vkCreatePipelineLayout(m_vkDevice, &pipeLayoutInfo, nullptr, &m_vkPipeLineLayout) != VK_SUCCESS)
  {
    std::cout << "failed to ceate pipeline layout" << std::endl;
  }

  VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
  pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineCreateInfo.stageCount = 2;
  pipelineCreateInfo.pStages = shaderStage;
  pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
  pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
  pipelineCreateInfo.pViewportState = &viewportInfo;
  pipelineCreateInfo.pRasterizationState = &rasterizer;
  pipelineCreateInfo.pMultisampleState = &multisampling;
  pipelineCreateInfo.pColorBlendState = &colorBlending;
  pipelineCreateInfo.pDynamicState = &dynamicState;
  pipelineCreateInfo.layout = m_vkPipeLineLayout;
  pipelineCreateInfo.renderPass = m_vkRenderPass;
  pipelineCreateInfo.subpass = 0;
  pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineCreateInfo.basePipelineIndex = -1;
  if (vkCreateGraphicsPipelines(m_vkDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_vkGraphicPipeLine) != VK_SUCCESS)
  {
    std::cout << "failed to create vkPipeline" << std::endl;
  }

  vkDestroyShaderModule(m_vkDevice, m_vkFragmentModule, nullptr);
  vkDestroyShaderModule(m_vkDevice, m_vkVertexModule, nullptr);
}

void VulkanRenderer::CreateRenderPass()
{
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = m_SwapChainImageFromat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef{};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;

  if (vkCreateRenderPass(m_vkDevice, &renderPassInfo, nullptr, &m_vkRenderPass) != VK_SUCCESS)
  {
    std::cout << "failed to create Render Pass" << std::endl;
  }
}

void VulkanRenderer::CreateFrameBuffers()
{
  m_SwapChainFrameBuffers.resize(m_SwapChainImageViews.size());
  for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
  {
    VkImageView attachments[] = {
        m_SwapChainImageViews[i]};
    VkFramebufferCreateInfo frameBufferInfo{};
    frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferInfo.renderPass = m_vkRenderPass;
    frameBufferInfo.attachmentCount = 1;
    frameBufferInfo.pAttachments = attachments;
    frameBufferInfo.width = m_SwapChainImageExtent.width;
    frameBufferInfo.height = m_SwapChainImageExtent.height;
    frameBufferInfo.layers = 1;

    if (vkCreateFramebuffer(m_vkDevice, &frameBufferInfo, nullptr, &m_SwapChainFrameBuffers[i]) != VK_SUCCESS)
    {
      std::cout << "failed to create vkFrame buffer" << std::endl;
    }
  }
}

void VulkanRenderer::CreateSwapChain()
{
  SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(m_vkPhysicalDevice);
  VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.Formats);
  VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.PresentModes);
  VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
  if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
  {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }
  VkSwapchainCreateInfoKHR swapChainCreateInfo{};
  swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapChainCreateInfo.surface = m_vkSurface;
  swapChainCreateInfo.minImageCount = imageCount;
  swapChainCreateInfo.imageFormat = surfaceFormat.format;
  swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
  swapChainCreateInfo.imageExtent = extent;
  swapChainCreateInfo.imageArrayLayers = 1;
  swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  QueueFamilyIndices indices = FindQueueFamily(m_vkPhysicalDevice);
  uint32_t queueFamilyIndices[] = {indices.GrpahicFamily.value(), indices.PresentFamily.value()};

  if (indices.GrpahicFamily != indices.PresentFamily)
  {
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    swapChainCreateInfo.queueFamilyIndexCount = 2;
    swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
  }
  else
  {
    swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapChainCreateInfo.queueFamilyIndexCount = 0;
    swapChainCreateInfo.pQueueFamilyIndices = nullptr;
  }
  swapChainCreateInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapChainCreateInfo.presentMode = presentMode;
  swapChainCreateInfo.clipped = VK_TRUE;
  swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

  VkResult result = vkCreateSwapchainKHR(m_vkDevice, &swapChainCreateInfo, nullptr, &m_vkSwapChain);
  if (result != VK_SUCCESS)
  {
    std::cout << "failed to create swapchain" << std::endl;
  }

  vkGetSwapchainImagesKHR(m_vkDevice, m_vkSwapChain, &imageCount, nullptr);
  m_SwapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(m_vkDevice, m_vkSwapChain, &imageCount, m_SwapChainImages.data());

  m_SwapChainImageFromat = surfaceFormat.format;
  m_SwapChainImageExtent = extent;
}

void VulkanRenderer::CreateImageView()
{
  m_SwapChainImageViews.resize(m_SwapChainImages.size());
  for (size_t i = 0; i < m_SwapChainImages.size(); i++)
  {
    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = m_SwapChainImages[i];
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = m_SwapChainImageFromat;
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_vkDevice, &imageViewCreateInfo, nullptr, &m_SwapChainImageViews[i]) != VK_SUCCESS)
    {
      std::cout << "Failed to create swapchain Image View" << std::endl;
    }
  }
}

void VulkanRenderer::CreateDeviceQueueFamily()
{
}

std::shared_ptr<VulkanRenderer> VulkanRenderer::Create(GLFWwindow *window)
{
  return std::make_shared<VulkanRenderer>(window);
}

bool VulkanRenderer::isSuitableDevice(VkPhysicalDevice physicalDevice)
{
  VkPhysicalDeviceProperties deviceProperties{};
  vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
  std::cout << "physical name :" << deviceProperties.deviceName << std::endl;

  VkPhysicalDeviceFeatures deviceFeature{};
  vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeature);

  QueueFamilyIndices indice = FindQueueFamily(physicalDevice);

  bool extensionsSupported = CheckDeviceExtensSupport(physicalDevice);
  bool swapChainAdequate = false;
  if (extensionsSupported)
  {
    SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice);
    swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
  }
  return indice.isComplete() && extensionsSupported && swapChainAdequate;
}

QueueFamilyIndices VulkanRenderer::FindQueueFamily(VkPhysicalDevice physicalDevice)
{
  QueueFamilyIndices indice{};

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

  int index = 0;
  for (const VkQueueFamilyProperties &properties : queueFamilies)
  {
    if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
    {
      indice.GrpahicFamily = index;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, index, m_vkSurface, &presentSupport);
    if (presentSupport)
    {
      indice.PresentFamily = index;
    }

    index++;
  }
  return indice;
}

bool VulkanRenderer::CheckDeviceExtensSupport(VkPhysicalDevice physcialDevice)
{
  uint32_t extensionCount = 0;
  vkEnumerateDeviceExtensionProperties(physcialDevice, nullptr, &extensionCount, nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(physcialDevice, nullptr, &extensionCount, availableExtensions.data());

  for (const auto &extiension : availableExtensions)
  {
    std::cout << "extensions " << extiension.extensionName << std::endl;
  }

  std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
  for (const auto &extension : availableExtensions)
  {
    requiredExtensions.erase(extension.extensionName);
  }
  return requiredExtensions.empty();
}

SwapChainSupportDetails VulkanRenderer::QuerySwapChainSupport(VkPhysicalDevice physicalDevice)
{
  SwapChainSupportDetails details;
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_vkSurface, &details.capabilities);

  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_vkSurface, &formatCount, nullptr);
  if (formatCount != 0)
  {
    details.Formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_vkSurface, &formatCount, details.Formats.data());
  }

  uint32_t presentModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_vkSurface, &presentModeCount, nullptr);
  if (presentModeCount != 0)
  {
    details.PresentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_vkSurface, &presentModeCount, details.PresentModes.data());
  }
  return details;
}

VkSurfaceFormatKHR VulkanRenderer::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
  for (const auto &availableformat : availableFormats)
  {
    if (availableformat.format == VK_FORMAT_B8G8R8A8_SRGB && availableformat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
    {
      return availableformat;
    }
  }
  return availableFormats[0];
}

VkPresentModeKHR VulkanRenderer::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
  for (const auto &presentMode : availablePresentModes)
  {
    if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
    {
      return presentMode;
    }
  }
  return availablePresentModes[0];
}

VkExtent2D VulkanRenderer::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
  {
    return capabilities.currentExtent;
  }
  else
  {
    int width, height;
    glfwGetFramebufferSize(m_widnow, &width, &height);

    VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.width, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    return actualExtent;
  }
}
