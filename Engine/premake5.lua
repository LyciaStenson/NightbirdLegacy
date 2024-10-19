project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")
	
	defines { "GLFW_INCLUDE_NONE" }

	files {
		"Source/**.h",
		"Source/**.cpp",
		
		"Vendor/jolt/**.h",
		--"Vendor/jolt/**.cpp",

		--"Vendor/glfw/**.h",
		--"Vendor/glfw/**.c",
		--"Vendor/glfw/**.cpp",

		"Vendor/glad/**.h",
		--"Vendor/glad/**.c",

		"Vendor/glm/**.h",
		"Vendor/glm/**.hpp",
		--"Vendor/glm/**.cpp",
		"Vendor/glm/**.inl",

		"Vendor/stb/stb_image.h",

		"Vendor/cgltf/cgltf.h
		"Vendor/cgltf/cgltf_write.h

		"Vendor/flecs/**.h",
		--"Vendor/flecs/**.c",
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
