project "Engine"
	kind "StaticLib"
	language "C++"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	files { "**.h", "**.cpp" }

	defines { "GLFW_INCLUDE_NONE" }

	includedirs { "Source/Public", "Vendor/glfw/include", "Vendor/glad/include", "Vendor/glm", "Vendor/stb" }

	links { "glfw", "glad", "GL", "X11" }
	
	include "Vendor/glfw"
	include "Vendor/glad"
