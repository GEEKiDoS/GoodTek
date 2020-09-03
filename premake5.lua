workspace "Goodtek"
	location "./build"
	objdir "%{wks.location}/obj"
	targetdir "%{wks.location}/bin/%{cfg.platform}/%{cfg.buildcfg}"

	architecture "x86"
	platforms "x86"

	configurations {
		"Debug",
		"Release",
	}

	buildoptions "/std:c++latest"
	linkoptions "/LTCG"
	systemversion "latest"
	symbols "On"
	staticruntime "On"
	editandcontinue "Off"
	warnings "Extra"
	characterset "ASCII"

	flags {
		"NoIncrementalLink",
		"NoMinimalRebuild",
		"MultiProcessorCompile",
	}

	staticruntime "Off"

	configuration "Release"
		optimize "Full"
		buildoptions "/Os"

	configuration "Debug"
		optimize "Debug"

	project "Goodtek"
		targetname "gdtek"

		language "C++"
		kind "SharedLib"

		files {
			"./src/Goodtek/headers/**.hpp",
			"./src/Goodtek/sources/**.cpp",
		}

		includedirs {
			"./src/Goodtek/headers",
			"./includes",
			"%{prj.location}/src",
		}

		syslibdirs {
			"./libs",
		}

		links { 
			"capstone",
			"PolyHook_2",
		}
