project "flecs"
	kind "StaticLib"
	language "C"
	staticruntime "On"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/flecs.c",
		"include/flecs/flecs.h"
	}

	includedirs
	{
		"include/flecs"
	}

	filter { "system:linux", "action:gmake2" }
		buildoptions { "-std=gnu99" }

	filter { }
