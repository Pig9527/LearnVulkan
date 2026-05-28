workspace "LearnVulkan"
    architecture "x64"
    startproject "Vk_firstWindow"

    configurations
    {
        "Debug",
        "Release"
    }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "vendor/glfw"



include "VK_firstWindow"


