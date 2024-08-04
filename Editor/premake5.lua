project "Editor"
	kind "ConsoleApp"
	language "C++"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	files { "**.h", "**.cpp" }

	defines { "GLFW_INCLUDE_NONE" }

	includedirs { "Source/Public", "%{wks.location}/Engine/Source/Public", "%{wks.location}/Vendor/glfw/include", "%{wks.location}/Vendor/glad/include", "%{wks.location}/Vendor/glm", "%{wks.location}/Vendor/stb", "%{wks.location}/Vendor/imgui" }

	links { "Engine", "GLFW", "glad", "ImGui", "GL", "X11" }