project "flecs"
	kind "StaticLib"
	language "C"
	staticruntime "On"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	files
	{
		"flecs.c",
		"flecs.h"
	}

	includedirs
	{
		""
	}

	filter { "system:linux", "action:gmake2" }
		buildoptions { "-std=gnu99" }

	filter { }
