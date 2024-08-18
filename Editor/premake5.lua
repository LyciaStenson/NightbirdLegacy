project "Editor"
	kind "ConsoleApp"
	language "C++"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	files { "**.h", "**.cpp" }

	defines { "GLFW_INCLUDE_NONE" }

	includedirs { "Source/Public", "%{wks.location}/Engine/Source/Public", "%{wks.location}/Engine/Vendor/glfw/include", "%{wks.location}/Engine/Vendor/glad/include", "%{wks.location}/Engine/Vendor/glm", "%{wks.location}/Engine/Vendor/stb", "%{wks.location}/Editor/Vendor/imgui" }

	links { "Engine", "GLFW", "glad", "ImGui" }

	include "Vendor/imgui"
