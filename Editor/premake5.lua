project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	local outBinDir = "%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}"

	targetdir (outBinDir)
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	files { "**.h", "**.cpp" }

	defines { "GLFW_INCLUDE_NONE" }

	includedirs {
		"Source/Public",
		"%{wks.location}/Engine/Source/Public",
		"%{wks.location}/Engine/Source/Public/Components",
		"%{wks.location}/Engine/Source/Public/Systems",
		"%{wks.location}/Engine/Vendor/glfw/include",
		"%{wks.location}/Engine/Vendor/glad/include",
		"%{wks.location}/Engine/Vendor/flecs/include",
		"%{wks.location}/Engine/Vendor/glm",
		"%{wks.location}/Engine/Vendor/stb",
		"%{wks.location}/Engine/Vendor/cgltf",
		"%{wks.location}/Editor/Vendor/imgui"
	}

	links { "Engine", "glfw", "glad", "flecs", "ImGui" }

	filter { "configurations:debug" }
		debugdir (outBinDir)
	filter { }
