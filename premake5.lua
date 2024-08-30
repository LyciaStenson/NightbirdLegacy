workspace "Nightbird"
	architecture "x86_64"
	startproject "Editor"

	configurations { "Debug", "Release", "Dist" }

	filter { "configurations:Debug"}
		defines { "DEBUG" }
		symbols "On"

	filter { "configurations:Release" }
		defines { "RELEASE" }
		optimize "On"

	filter { "configurations:Dist" }
		defines { "DIST" }
		optimize "On"

	filter { }

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "Engine/Vendor/glfw"
	include "Engine/Vendor/glad"
	include "Engine/Vendor/jolt"
	include "Editor/Vendor/imgui"
group ""

group "Nightbird"
	include "Engine"
	include "Editor"
group ""

group "Projects"
	include "Game"
group ""
