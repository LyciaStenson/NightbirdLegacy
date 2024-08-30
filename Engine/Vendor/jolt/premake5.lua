project "Jolt"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	includedirs {
		""
	}

	files {
		"Jolt/**.h",
		"Jolt/**.cpp"
	}
