project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	files { "**.h", "**.cpp" }

	defines { "GLFW_INCLUDE_NONE" }

	includedirs { "Source/Public", "Source/Public/Components", "Vendor/glfw/include", "Vendor/glad/include", "Vendor/glm", "Vendor/stb" }

	links { "glfw", "glad" }
	
	include "Vendor/glfw"
	include "Vendor/glad"
