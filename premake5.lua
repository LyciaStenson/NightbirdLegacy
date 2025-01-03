workspace "Nightbird"
	architecture "x86_64"
	startproject "Editor"

	configurations { "debug", "release", "dist" }

	filter { "configurations:debug"}
		defines { "DEBUG" }
		symbols "On"
		runtime "Debug"

	filter { "configurations:release" }
		defines { "RELEASE" }
		optimize "On"
		runtime "Release"

	filter { "configurations:dist" }
		defines { "DIST" }
		optimize "On"
		runtime "Release"

	filter { }

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "Engine/Vendor/glfw"
	include "Engine/Vendor/glad"
	include "Engine/Vendor/fastgltf"
	include "Engine/Vendor/flecs"
	include "Engine/Vendor/jolt"
	include "Editor/Vendor/imgui"
group ""

group "Nightbird"
	include "Engine"
	include "Editor"
group ""

group "Projects"
	include "Game"
	include "Spin"
	include "Neurons"
	include "Diorama"
group ""
