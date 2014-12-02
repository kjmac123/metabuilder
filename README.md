Metabuilder
===========

A Cross-platform meta project/makefile generator.

Author: Ken Macleod:
Email: kjmac123@gmail.com
Twitter: @PanzerGnome

What is Metabuilder?
Metabuilder is a system for generating project files or makefiles for multiple development environments using a single ‘meta’ makefile. Metabuilder’s makefiles are written in Lua with (I hope) an emphasis on simplicity and ease of maintenance.

The metabuilder system consists of a binary that provides scaffolding for a set of Lua scripts (called the Metabase) that deal with the task of writing out the project files and makefiles that are supported. As a result it is straightforward to extend metabuilder to support additional platforms without any need to recompile code. Just add the required new files into the Metabase.

The version of Metabuilder on Github is able to generate the following project types:
MSVC 2010, 2012, 2013, 2015 for Windows x86 and x64
XCode for OSX and iOS. Versions 5 and 6 of Xcode have been tested.
Android NDK. Currently supported under OSX only. No examples available just yet.

Metabuilder has been used successfully in commercial projects and behind closed doors it has been extended to support the generation of projects for a number of games consoles. This was achieved purely by adding new Metabase files. 

Why use a meta-make solution such as Metabuilder?
When working on large projects targeting several platforms the task of maintaining separate makefiles/MSVC/XCode projects for each platform becomes an extreme burden. It’s difficult to keep projects in sync, whether when adding new files or when modifying configuration options such as preprocessor macros.

Wouldn’t it be better if we could maintain one set of makefiles and generate projects for whatever platforms we require in a single click? Metabuilder offers such a solution.

Aren’t there already similar projects out there?
The concept of a meta-make approach is not new. CMake and Premake (which also uses Lua) are two examples. Certain ‘make’ alternatives can also themselves generate IDE projects, for example JamPlus.

I didn’t feel that the existing project generation/make solutions I’d encountered offered the ease of use and flexibility I required. Especially when considering the need to support platforms not open to the public.

Metabuilder workflow
To begin with you’ll need to generate a project for your IDE. You would typically do this with a batch file/shell script in the root of your project that is used to invoke Metabuilder. The Metabuilder examples are set up this way. 

When building for MSVC the example batch files generate the project and then launch and load the project into MSVC for you. If you wish to add/remove files from your project you’ll need to update the relevant metabuilder lua file, close MSVC and re-run the generation batch file.

Note that by convention the name of the main metabuilder make file is metabuilder.lua. Metabuilder lua files can of course include other lua files in order to better manage large projects.

To work with XCode on the Mac a shell script is first used to invoke metabuilder and generate the XCode project file. Currently you’ll need to then navigate to and open the generated file, which can be found within the metatmp folder in your project’s root.

Once open in XCode if you need to add/remove files you’ll again need to re-run the initial shell script. However unlike MSVC you don’t need to close XCode as the project changes will be automatically detected and refreshed.

Example Metabuilder make file
From metabuilder/examples/helloworld/metabuilder.lua

makesetup ""
	--subdir for intermediate files
	intdir "int"
	--subdir for final output files
	outdir "out"	
makesetup_end()

solution "helloworld"

	--Currently metabuilder supports only a single target per solution
	target "helloworld"
		--this is an application (as opposed to a staticlib for example)
		target_type "app"

		--Additional makefiles for libraries we need to build and link against
		depends("hellolib",	"../hellolib/metabuilder.lua")

		includedirs 
		{ 
			"../hellolib",
		}

		--This is the list of files to build.
		--Note that for each source file added metabuilder will automatically
		--add any .h or .inl files found with a matching name
		files
		{
			"main.cpp"
		}

		--A current metabuilder quirk is that any build configurations you
		--wish to have must be at least stubbed out like this in all your
		--make files.
		--This is because there's no concept of a mapping table to use if
		--one of your libraries has a 'missing' build configuration.
		config "Debug"
		config_end()

		config "Release"
		config_end()

		config "Profile"
		config_end()

		config "Master"
		config_end()
		
 	target_end()

solution_end()

Example projects
Please see the metabuilder/examples folder for example projects. In the future documentation for the examples will be added into each folder.

Currently the examples are rather simplistic, more involved ones will be added.

The Metabase
If you wish to modify/examine the Metabase Lua files they can be found in metabuilder/metabase

And finally
This document, and metabuilder itself, remain a work in progress. I’ll try and tidy up and document things as soon as I’m able.
Documentation for the metabuilder syntax is obviously needed but will take a little time to write up.
The code for the metabuilder binary is _not_ a shining example of the ultimate in C++ excellence. It was written quickly in order to urgently fill a very real need. With things calming down a little now I should be able to revisit and tidy things up.
That said very few changes to the C++ code have been necessary after metabuilder started being used ‘for real’ so things can’t be that bad!
