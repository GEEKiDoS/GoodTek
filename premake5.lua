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
	systemversion "latest"
	symbols "On"
	staticruntime "On"
	editandcontinue "Off"
	warnings "Off"
	characterset "ASCII"

	flags {
		"NoIncrementalLink",
		"NoMinimalRebuild",
		"MultiProcessorCompile",
	}

	defines {
		"ZLIB_WINAPI",
	}

	staticruntime "Off"

	configuration "Release"
		optimize "Full"
		buildoptions "/Os"

	configuration "Debug"
		optimize "Debug"

	project "Goodtek"
		targetname "goodtek"

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

		links { 
			"capstone",
			"PolyHook_2",
			"zlibstat.lib",
		}

		configuration "Release"
			linkoptions "/LTCG /SAFESEH:NO"
			syslibdirs {
				"./libs/Release",
			}
		configuration "Debug"
			linkoptions "/SAFESEH:NO"
			syslibdirs {
				"./libs/Debug",
			}
