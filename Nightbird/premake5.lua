project "Nightbird"
	kind "ConsoleApp"
	language "C++"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	files { "**.h", "**.cpp" }

	defines { "GLFW_INCLUDE_NONE" }

	includedirs { "%{wks.location}/vendor/glfw/include", "%{wks.location}/vendor/glad/include" }

	links { "glfw", "glad", "GL", "X11" }
