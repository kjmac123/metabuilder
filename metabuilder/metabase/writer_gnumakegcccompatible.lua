import "writer_common.lua"

g_firstTargetWritten = false

--Count the number of times a base name appears so that we can avoid filename clashes
g_sourceFileBaseNameCounts = {}

g_varBUILDCONFIG		= ""
g_varINTDIR					= ""
g_varOUTDIR					= ""
g_varMODULEOBJ			= ""
g_varCFLAGS					= ""
g_varCPPFLAGS				= ""
g_varCXXFLAGS				= ""
g_varINCLUDES				= ""
g_varLIBS						= ""
g_varLDLIBS					= ""
g_varLIBDIRS				= ""
g_varLDFLAGS				= ""
g_varARFLAGS				= ""
g_varCC							= ""
g_varCXX						= ""
g_varAR							= ""
g_varLD							= ""
g_varSRC						= ""
g_varOBJ						= ""
g_varDEFINES				= ""

----------------------------------------------------------------------------------------------------------------------------------------------------------------

function InitVars(currentTarget)
	g_varBUILDCONFIG	= "BUILDCONFIG"
	g_varINTDIR				= currentTarget.name .. "_INTDIR"
	g_varOUTDIR				= currentTarget.name .. "_OUTDIR"
	g_varMODULEOBJ		= currentTarget.name .. "_MODULEOBJ"
	g_varCFLAGS 			= currentTarget.name .. "_CFLAGS"
	g_varCPPFLAGS			= currentTarget.name .. "_CPPFLAGS"
	g_varCXXFLAGS			= currentTarget.name .. "_CXXFLAGS"
	g_varINCLUDES			= currentTarget.name .. "_INCLUDES"
	g_varLIBS					= currentTarget.name .. "_LIBS"
	g_varLDLIBS				= currentTarget.name .. "_LDLIBS"
	g_varLIBDIRS			= currentTarget.name .. "_LIBDIRS"
	g_varLDFLAGS			= currentTarget.name .. "_LDFLAGS"
	g_varARFLAGS			= currentTarget.name .. "_ARFLAGS"
	g_varCC						= currentTarget.name .. "_CC"
	g_varCXX					= currentTarget.name .. "_CXX"
	g_varAR						= currentTarget.name .. "_AR"
	g_varLD						= currentTarget.name .. "_LD"
	g_varSRC					= currentTarget.name .. "_SRC"
	g_varOBJ					= currentTarget.name .. "_OBJ"
	g_varDEFINES			= currentTarget.name .. "_DEFINES"
end

function GetDollarVar(var)
	return "$(" .. var .. ")"
end

function GetDollarVarIntDir()
	return GetDollarVar(g_varINTDIR .. ".$(BUILDCONFIG)")
end

function GetDollarVarOutDir()
	return GetDollarVar(g_varOUTDIR .. ".$(BUILDCONFIG)")
end

----------------------------------------------------------------------------------------------------------------------------------------------------------------
--FILE WRITING
----------------------------------------------------------------------------------------------------------------------------------------------------------------

function WriteSourceToObjRule(file, buildFile)

	local compiler = ""
	local compilerFlags = GetDollarVar(g_varCPPFLAGS)
	if buildFile.ext == "c" then
		compiler = GetDollarVar(g_varCC)
		compilerFlags = compilerFlags .. " " .. GetDollarVar(g_varCFLAGS)
	elseif buildFile.ext == "cpp" then
		compiler = GetDollarVar(g_varCXX)
		compilerFlags = compilerFlags .. " " .. GetDollarVar(g_varCXXFLAGS)
	else
		mbwriter.fatalerror("unsupported source file type")
	end
	basename = mbstring.replace(buildFile.objFile, ".o", "")

	file:write(buildFile.objFile .. " : " .. buildFile.srcFile .. " " .. basename .. ".d \n")
	file:write("	@echo " .. compiler .. " " .. buildFile.srcFile .. "\n")
	file:write("	@" .. compiler .. " " .. compilerFlags .. " -o '$@' '$<'; \n")
end

function WriteMakeFileGlobalVars(file, currentTarget)
	file:write("BUILDCONFIG := " .. currentTarget.configs[#currentTarget.configs].name .. "\n\n")
end

function WriteMakeFileCommonVars(file, currentTarget)
		if currentTarget.options.cc == nil then
		mbwriter.fatalerror("No C compiler set")
	end

	file:write(g_varCC .. 			" := " .. currentTarget.options.cc[1] .. "\n")
	if currentTarget.options.cxx == nil then
		mbwriter.fatalerror("No C++ compiler set")
	end
	file:write(g_varCXX ..			" := " .. currentTarget.options.cxx[1] .. "\n")
	file:write("\n")

	for i = 1, #currentTarget.configs do
		local config = currentTarget.configs[i]

		file:write(g_varINTDIR .. "." .. config.name .. " := " .. config.targetintdir .. "\n")
		file:write(g_varOUTDIR .. "." .. config.name .. " := " .. config.targetoutdir .. "\n")
		file:write("\n")

		--CPPFLAGS is common to C and C++
		file:write(g_varCPPFLAGS .. "." .. config.name .. " := \\\n")
		if config.options.cppflags ~= nil then
			for i = 1, #config.options.cppflags do
				file:write("  " .. config.options.cppflags[i] .. " \\\n")
			end
		end
		file:write("\n")

		--CFLAGS is for C only
		file:write(g_varCFLAGS .. "." .. config.name .. " := \\\n")
		if config.options.cflags ~= nil then
			for i = 1, #config.options.cflags do
				file:write("  " .. config.options.cflags[i] .. " \\\n")
			end
		end
		file:write("\n")

		--CXXFLAGS is for C++ only
		file:write(g_varCXXFLAGS .. "." .. config.name .. " := \\\n")
		if config.options.cxxflags ~= nil then
			for i = 1, #config.options.cxxflags do
				file:write("  " .. config.options.cxxflags[i] .. " \\\n")
			end
		end
		file:write("\n")

		--Preprocessor macros will live in CPPFLAGS
		file:write(g_varDEFINES .. "." .. config.name .. " := \\\n")
		if config.defines ~= nil then
			for i = 1, #config.defines do
				file:write("  -D" .. config.defines[i] .. " \\\n")
			end
		end
		file:write("\n")
		file:write(g_varCPPFLAGS .. "." .. config.name .. " += $(" .. g_varDEFINES .. "." .. config.name .. ")\n")

		--Out include paths are shared between languages, so store in CPPFLAGS
		file:write(g_varINCLUDES .. "." .. config.name .. " := \\\n")
		for i = 1, #config.includedirs do
			local includeDir = mbwriter.getoutputrelfilepath(config.includedirs[i])
			file:write("  -I" .. mbstring.quoted(includeDir) .. " \\\n")
		end
		file:write("\n")
		file:write(g_varCPPFLAGS .. "." .. config.name .. " += $(" .. g_varINCLUDES .. "." .. config.name .. ")\n")
	end

	file:write(g_varCPPFLAGS .. "	:= -MMD -MP $(" .. g_varCPPFLAGS .. "." .. GetDollarVar(g_varBUILDCONFIG) .. ")\n")
	file:write(g_varCFLAGS ..	"	:= $(" .. g_varCFLAGS .. "." .. GetDollarVar(g_varBUILDCONFIG) .. ")\n")
	file:write(g_varCXXFLAGS .. "	:= $(" .. g_varCXXFLAGS .. "." .. GetDollarVar(g_varBUILDCONFIG) .. ")\n")
end

function WriteMakeFileAppVars(file, currentTarget)
	file:write(g_varCFLAGS .. 	" += -c\n")
	file:write(g_varCXXFLAGS ..	" += -c\n")
	if currentTarget.options.ld == nil then
		mbwriter.fatalerror("No linker set")
	end
	file:write(g_varLD .. 		" := " .. currentTarget.options.ld[1] .. "\n")
	file:write("\n")

	for i = 1, #currentTarget.configs do
		local config = currentTarget.configs[i]

		--Linker flags
		file:write(g_varLDFLAGS .. "." .. config.name .. " := \\\n")
		if config.options.ldflags ~= nil then
			for i = 1, #config.options.ldflags do
				file:write(config.options.ldflags[i] .. " \\\n")
			end
		end
		file:write("\n")

		--Library directories. Will be stored in LDFLAGS
		file:write(g_varLIBDIRS .. "." .. config.name .. " := \\\n")
		if config.libdirs ~= nil then
			for i = 1, #config.libdirs do
				local libDir = mbwriter.getoutputrelfilepath(config.libdirs[i])
				file:write("  -L" .. libDir .. " \\\n")
			end
		end
		file:write("\n")
		file:write(g_varLDFLAGS .. "." .. config.name .. " += $(" .. g_varLIBDIRS .. "." .. config.name .. ")\n")

		--Libraries
		file:write(g_varLIBS .. "." .. config.name .. " := \\\n")
		if config.libs ~= nil then
			for i = 1, #config.libs do
				local lib = config.libs[i]
				--file:write("  -l:" .. lib .. " \\\n")
				file:write(" " .. lib .. " \\\n")
			end
		end
		file:write("\n")
		file:write(g_varLDLIBS .. "." .. config.name .. " += $(" .. g_varLIBS .. "." .. config.name .. ")\n")

		file:write("\n")
	end
	file:write(g_varLDFLAGS .. " := $(" .. g_varLDFLAGS .. "." .. GetDollarVar(g_varBUILDCONFIG) .. ") $(" .. g_varLDLIBS .. "." .. GetDollarVar(g_varBUILDCONFIG) .. ")\n")
end

function WriteMakeFileModuleVars(var1, var2)
	file = var1
	currentTarget = var2

	if currentTarget.options.ld == nil then
		mbwriter.fatalerror("No linker set")
	end
	file:write(g_varLD .. 		" := " .. currentTarget.options.ld[1] .. "\n")
	file:write(g_varCFLAGS ..	" += -c\n")
	file:write(g_varCXXFLAGS ..	" += -c\n")
	file:write(g_varLDFLAGS ..	" := \n")
end

function WriteMakeFileStaticLibVars(file, currentTarget)
	if currentTarget.options.ar == nil then
		mbwriter.fatalerror("No archive tool specified")
	end
	file:write(g_varAR .. " := " .. currentTarget.options.ar[1] .. "\n")
	file:write("\n")

	for i = 1, #currentTarget.configs do
		local config = currentTarget.configs[i]

		file:write(g_varARFLAGS .. "." .. config.name .. " := \\\n")
		if config.options.arflags ~= nil then
			for i = 1, #config.options.arflags do
				file:write("  " .. config.options.arflags[i] .. " \\\n")
			end
		end
		file:write("\n")
	end

	file:write(g_varARFLAGS .. " := $(" .. g_varARFLAGS .. "." .. GetDollarVar(g_varBUILDCONFIG) .. ")\n")

	file:write(g_varCFLAGS .. " += -c\n")
	file:write(g_varCXXFLAGS .. " += -c\n")
end

function GetTargetOutputFilename(currentTarget)
	local ext = ""
	if currentTarget.targettype == "app" then
		if (currentTarget.options.appfileextension ~= nil) then
			ext = "." .. currentTarget.options.appfileextension[1]
		end
	elseif currentTarget.targettype == "staticlib" then
		ext = ".a"
	else
	end

	local targetName = GetDollarVarOutDir() .. "/" .. currentTarget.name
	if (ext ~= nil) then
		targetName = targetName .. ext
	end

	return targetName
end

function GetPreLinkFileName(currentTarget, config)
	local targetFileName = GetDollarVarIntDir() .. "/__" .. currentTarget.name .. "__prelink__"
	return targetFileName
end

function WriteCompileRule(file, currentTarget)
	local targetFileName = GetPreLinkFileName(currentTarget)

	file:write(targetFileName .. " : " .. GetDollarVar(g_varOBJ) .. " ")

	for i = 1, #currentTarget.depends do
		local dependency = currentTarget.depends[i]
		local path, filename, ext = mbfilepath.decompose(dependency)
		local submakeLinkTargetAbs = mbwriter.gettarget(filename)

		file:write(submakeLinkTargetAbs .. " ")
	end
	file:write("\n")
	file:write("	@echo " .. "ld" .. " Creating prelink obj " .. targetFileName .. "\n")
	file:write("	@" .. "ld" .. " " .. " -r " .. " " .. GetDollarVar(g_varOBJ) .. " -o '$@' ;\n")
	file:write("\n")

	return targetFileName
end

function WriteMakeFileAppTarget(file, currentTarget)
	local targetFileName = GetTargetOutputFilename(currentTarget)
	local targetPreLinkFileName = WriteCompileRule(file, currentTarget)

	file:write(targetFileName .. " : " .. targetPreLinkFileName  .. "\n")
	file:write("	@echo " .. GetDollarVar(g_varLD) .. " Linking " .. targetFileName .. "\n")
	file:write("	@" .. GetDollarVar(g_varLD) .. " " .. GetDollarVar(g_varMODULEOBJ) .. " " .. GetDollarVar(g_varOBJ) .. " " .. GetDollarVar(g_varLDFLAGS) .. " -o '$@' ;\n")
end

function WriteMakeFileModuleTarget(file, currentTarget)
	local targetFileName = GetTargetOutputFilename(currentTarget)
	local targetPreLinkFileName = WriteCompileRule(file, currentTarget)

	file:write(targetFileName .. " : " .. targetPreLinkFileName  .. "\n")
	file:write("	@echo " .." ld" .. " Creating module obj " .. targetFileName .. "\n")
	file:write("	@" .. "ld" .. " " .. " -r " .. GetDollarVar(g_varMODULEOBJ) .. " " .. GetDollarVar(g_varOBJ) .. " -o '$@' ;\n")
	mbwriter.registertarget(currentTarget.name, targetFileName)
end

function WriteMakeFileStaticLibTarget(file, currentTarget)
	local targetFileName = GetTargetOutputFilename(currentTarget)
	local targetPreLinkFileName = WriteCompileRule(file, currentTarget)

	file:write(targetFileName .. " : " .. targetPreLinkFileName  .. "\n")
	file:write("	@echo " .. GetDollarVar(g_varAR) .. " Creating static lib " .. targetFileName .. "\n")
	file:write("	@" .. GetDollarVar(g_varAR) .. " " .. GetDollarVar(g_varARFLAGS) .. " " .. targetFileName .. " " .. GetDollarVar(g_varOBJ) .. "\n")
	mbwriter.registertarget(currentTarget.name, targetFileName)
end

function WriteMakeFile(currentTarget)

	local makeDir = mbfilepath.join(mbwriter.global.makeoutputdirabs, "", mbwriter.global.targetDirSep)
	mbwriter.mkdir(makeDir)

	local makeFilename = ""
	if (mbwriter.global.ismainmakefile) then
		makeFilename = mbfilepath.join(makeDir, "Makefile", mbwriter.global.targetDirSep)
	else
		makeFilename = mbfilepath.join(makeDir, currentTarget.name .. ".mk", mbwriter.global.targetDirSep)
	end

	makeFilename = mbwriter.normalisetargetfilepath(makeFilename)
	local file = mbfile.open(makeFilename, "w")
	if file == nil then
		mbwriter.fatalerror("failed to open file " .. makeFilename)
	end

	InitVars(currentTarget)

		--write out content we only require once per makefile
	if (g_firstTargetWritten == false) then
		g_firstTargetWritten = true

		--write out content we only require at the start of the main makefile
		if mbwriter.global.ismainmakefile then
			WriteMakeFileGlobalVars(file, currentTarget)
		end
	end

	file:write("\n")
	if mbwriter.global.ismainmakefile then
		file:write("default : all \n")
	end

	file:write(g_varMODULEOBJ .. " := \n")
	--include submakefiles
	for i = 1, #currentTarget.depends do
		local dependency = currentTarget.depends[i]
		local path, filename, ext = mbfilepath.decompose(dependency)

		local submakeLinkTarget = mbwriter.gettarget(filename)
		local submakefile = filename .. ".mk"

		file:write("include " .. submakefile .. "\n")
		file:write(g_varMODULEOBJ .. " += " .. submakeLinkTarget .. "\n")
	end
	file:write("\n")

	WriteMakeFileCommonVars(file, currentTarget)

	--Write out variables for current target type
	if currentTarget.targettype == "app" then
		WriteMakeFileAppVars(file, currentTarget)
	elseif currentTarget.targettype == "staticlib" then
		WriteMakeFileStaticLibVars(file, currentTarget)
	elseif currentTarget.targettype == "module" then
		WriteMakeFileModuleVars(file, currentTarget)
	else
		mbwriter.fatalerror("unsupported target type")
	end

	--Generate list of object files and their source files
	local buildFiles = {}
	for i = 1, #currentTarget.files do
		local f = currentTarget.files[i]
		local path, filename, ext = mbfilepath.decompose(f)
		if ext == "c" or ext == "cpp" then
			local filenameAbs = mbwriter.getoutputrelfilepath(f)

			local fileBaseNameCount = g_sourceFileBaseNameCounts[filename]
			if (fileBaseNameCount == nil) then
				g_sourceFileBaseNameCounts[filename] = 0
				fileBaseNameCount = 0
			end

			local obj = mbstring.replace(filename, ".cpp", "")
			obj = mbstring.replace(obj, ".c", "")
			obj = GetDollarVarIntDir() .. "/" .. obj
			local objWithCount = obj
			if (fileBaseNameCount > 0) then
				obj = obj .. "__" .. fileBaseNameCount
			end
			obj = obj .. ".o"

			buildFiles[#buildFiles+1] = {objFile=obj, srcFile=filenameAbs, ext=ext}
			g_sourceFileBaseNameCounts[filename] = fileBaseNameCount + 1
		end
	end
	file:write("\n")
	file:write(g_varSRC .. " := \\\n")

	for i = 1, #buildFiles do
		file:write("	" .. buildFiles[i].srcFile .. " \\\n")
	end
	file:write("\n")

	file:write(g_varOBJ .. " := \\\n")
	for i = 1, #buildFiles do
		file:write("	" .. buildFiles[i].objFile .. " \\\n")
	end

	file:write("\n")

	for i = 1, #buildFiles do
		WriteSourceToObjRule(file, buildFiles[i]);
	end

	file:write("\n")

	--Write out target
	if currentTarget.targettype == "app" then
		WriteMakeFileAppTarget(file, currentTarget)
	elseif currentTarget.targettype == "staticlib" then
		WriteMakeFileStaticLibTarget(file, currentTarget)
	elseif currentTarget.targettype == "module" then
		WriteMakeFileModuleTarget(file, currentTarget)
	else
		mbwriter.fatalerror("unsupported target type")
	end

	file:write("\n")

	file:write("\n")
	file:write(GetDollarVar(g_varOBJ) .. " : | " .. GetDollarVarIntDir() .. "\n\n")
	file:write(GetDollarVarIntDir() .. ":\n")
	file:write("	mkdir -p " .. GetDollarVarIntDir().. "\n")
	file:write("	mkdir -p " .. GetDollarVarOutDir() .. "\n")
	file:write("\n")

	--write 'all' target for current target
	file:write(".PHONY: all_" .. currentTarget.name .. "\n")
	file:write("all_" .. currentTarget.name .. " : ")
	file:write(GetTargetOutputFilename(currentTarget) .. " ")
	file:write("\n")
	file:write("\n")

	--write 'clean' target for current target
	file:write(".PHONY: clean_" .. currentTarget.name .. "\n")
	file:write("clean_" .. currentTarget.name .. " : ")
	for i = 1, #currentTarget.depends do
		local dependency = currentTarget.depends[i]
		local path, filename, ext = mbfilepath.decompose(dependency)
		file:write("clean_" .. filename .. " ")
	end
	file:write("\n")
	file:write("	@echo Cleaning " .. GetDollarVarIntDir() .. "\n")
	file:write("	@rm -f \"" .. GetDollarVarIntDir() .. "\"/*\n")
	file:write("	@if [ -d \"" .. GetDollarVarIntDir() .. "\" ]; then rmdir \"" .. GetDollarVarIntDir() .. "\";fi\n")
	file:write("	@echo Cleaning " .. GetDollarVarOutDir() .. "\n")
	file:write("	@rm -f \"" .. GetDollarVarOutDir() .. "\"/*\n")
	file:write("	@if [ -d \"" .. GetDollarVarOutDir() .. "\" ]; then rmdir \"" .. GetDollarVarOutDir() .. "\";fi\n")
	file:write("\n")

	if (mbwriter.global.ismainmakefile) then
		--write 'all' target for main makefile
		file:write(".PHONY: all\n")
		file:write("all : " .. "all_" .. currentTarget.name .. "\n")
		file:write("\n")

		file:write("\n")
		--write 'clean' target for main makefile
		file:write(".PHONY: clean\n")
		file:write("clean : clean_" .. currentTarget.name .. "\n")
	end

	file:write("\n")

	--Avoid 'no rule to make target' errors
	file:write("%.d: ;\n")

	for i = 1, #buildFiles do
		local objFile = buildFiles[i].objFile
		local depFile = mbstring.replace(objFile, ".o", ".d")
		file:write("-include " .. depFile .. "\n")
	end

	file:write("\n")
	file:close()

	mbwriter.reportoutputfile(makeFilename)
end

----------------------------------------------------------------------------------------------------------------------------------------------------------------
--MAIN
----------------------------------------------------------------------------------------------------------------------------------------------------------------

local currentTarget = mbwriter.solution.targets[1]
WriteMakeFile(currentTarget)
