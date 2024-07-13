workspace "Nightbird"
	architecture "x64"

	configurations { "Debug", "Release" }

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Editor"
	location "Editor"
	kind "ConsoleApp"
	language "C++"

	targetdir ("out/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("out/obj/" .. outputDir .. "/%{prj.name}")

	files { "**.h", "**.cpp" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
