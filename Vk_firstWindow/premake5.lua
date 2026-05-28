project "Vk_firstWindow"
    kind "consoleApp"
    language "c++"
    cppdialect "c++17"
    staticruntime "on"

    targetdir ("../bin/"..outputdir.."/%{prj.name}")
    objdir ("../bin-int/"..outputdir.."/%{prj.name}")



    files
    {
        "src/*.cpp",
        "src/*.h",

    }

    includedirs
    {
        "D:/Program Files/VulkanSDK/1.3.243.0/Include",
        "../vendor/spdlog/src",
        "../vendor/spdlog/include",
        "../vendor/glfw/include",
    }

    links
    {

        "glfw"
        "D:/Program Files/VulkanSDK/1.3.243.0/Lib/vulkan-1.lib",
    }
    filter "system:windows"
        systemversion "latest"

        defines
        {
            --"GLFW_INCLUDE_VULKAN",
            "_GLFW_WIN32",
            "_CRT_SECURE_NO_WARNINGS",
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
    filter "configurations:Release"
        runtime "Release"
        optimize "on"