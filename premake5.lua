workspace "noose"
	architecture "x64"
	startproject "noose"

	configurations
	{
		"Debug",
		"Release"
	}
	
	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "noose"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp",
		"vendor/nativefiledialog/src/**.h",
		"vendor/nativefiledialog/src/**.c",
		"vendor/nativefiledialog/src/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
		-- "SFML_STATIC"
	}

	includedirs
	{
		"src",
		"vendor/SFML/include",
		"vendor/nativefiledialog/src"
	}

	libdirs
	{
		"vendor/SFML/lib"
	}

	-- links
	-- {
	-- 	"opengl32",
	-- 	"freetype",
	-- 	"winmm"
	-- }

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"NOOSE_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "NOOSE_DEBUG"
		runtime "Debug"
		symbols "on"
		links
		{
			"sfml-graphics-d",
			"sfml-window-d",
			"sfml-system-d"
		}

	filter "configurations:Release"
		defines "NOOSE_RELEASE"
		runtime "Release"
		optimize "on"
		links
		{
			"sfml-graphics",
			"sfml-window",
			"sfml-system"
		}