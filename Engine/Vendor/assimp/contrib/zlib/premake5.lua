project "zlib"
	language "C"
	kind "StaticLib"
	defines { "N_FSEEKO" }
	
	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	files {
		"**.h",
		"**.c"
	}
	
	filter "system:windows"
		defines { "_WINDOWS" }
	
	filter "system:not windows"
		defines { "HAVE_UNISTD_H" }
