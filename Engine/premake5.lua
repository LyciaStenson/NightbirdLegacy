project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	files { "**.h", "**.cpp" }

	defines { "GLFW_INCLUDE_NONE" }

	includedirs { "Source/Public", "Source/Public/Components", "Source/Public/Systems", "Vendor/glfw/include", "Vendor/glad/include", "Vendor/glm", "Vendor/stb", "Vendor/jolt" }

	links { "glfw", "glad", "jolt" }
