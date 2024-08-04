project "Engine"
	kind "ConsoleApp"
	language "C++"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	files { "**.h", "**.cpp" }

	defines { "GLFW_INCLUDE_NONE" }

	includedirs { "Source/Public", "%{wks.location}/Vendor/glfw/include", "%{wks.location}/Vendor/glad/include", "%{wks.location}/Vendor/glm", "%{wks.location}/Vendor/stb" }

	links { "glfw", "glad", "GL", "X11" }
