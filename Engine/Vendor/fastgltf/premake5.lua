project "fastgltf"
	kind "StaticLib"
	language "C++"
    cppdialect "C++17"
	staticruntime "On"

	targetdir ("%{wks.location}/out/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/out/obj/" .. outputdir .. "/%{prj.name}")

	files
	{
		"**.hpp",
		"**.cpp"
	}

	includedirs
	{
		"include"
	}

	filter { "system:linux", "action:gmake2" }
		buildoptions { "-std=gnu99" }

	filter { }
