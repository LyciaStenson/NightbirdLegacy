project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	defines { "GLFW_INCLUDE_NONE" }

	files {
		"Source/**.h",
		"Source/**.cpp"
	}

	includedirs {
		"Source/Public",
		"Source/Public/Components",
		"Source/Public/Systems",
		"Vendor/glm",
		"Vendor/stb",
		"Vendor/cgltf",
		"Vendor/glfw/include",
		"Vendor/glad/include",
		"Vendor/flecs/include",
		"Vendor/jolt",
	}

	links { "glfw", "glad", "flecs", "jolt" }
