package.path = package.path .. ";" .. writer_global.metabasedirabs .. "/?.lua"
local inspect = require('inspect')
local util = require('utility')

if writer_global.verbose then 
	print("writer_global:\n")
	print(inspect(writer_global))
	print("\n")
	print("writer_solution:\n")
	print(inspect(writer_solution))
end

g_firstTargetWritten = false

g_useRelativePaths = true

--Count the number of times a base name appears so that we can avoid filename clashes
g_sourceFileBaseNameCounts = {}

--Map relative to absolute path
g_filePathMap = {}

g_intdir = ""
g_outdir = ""

g_varBUILDCONFIG = ""
g_varINTDIR = ""
g_varOUTDIR = ""
g_varMODULEOBJ = ""
g_varCFLAGS = ""
g_varCPPFLAGS = ""
g_varCXXFLAGS = ""
g_varINCLUDES = ""
g_varLIBS = ""
g_varLDLIBS = ""
g_varLIBDIRS = ""
g_varLDFLAGS = ""
g_varARFLAGS = ""
g_varCC = ""
g_varCXX = ""
g_varAR = ""
g_varLD = ""
g_varSRC = ""
g_varOBJ = ""
g_varDEFINES = ""

----------------------------------------------------------------------------------------------------------------------------------------------------------------

function GetLongestCommonSequenceLengthFromStart(str1, str2)
	local str1Table = {}
	
	for c in str1:gmatch"." do
		table.insert(str1Table, c)
	end

	
	local count = 1
	for c in str2:gmatch"." do
		if (str1Table[count] ~= c) then
			break
		end
		
		count = count + 1
	end
	
	return count
end

function GetNumDirLevels(dir)
	local dirCount = 0

	for c in dir:gmatch"." do
		if c == "/" then
			dirCount = dirCount + 1
		end
	end
	
	return dirCount
end

function BuildPathBack(nLevels)
	local result = ""
	for i = 1, nLevels do
		result = result .. "../"
	end
	
	return result
end

function GetFullFilePath(filepath)
	local result = ""
	
	if (g_useRelativePaths == true) then
		local normalisedFilepathAbs = Util_FileNormaliseUnix(Util_FileConvertToAbsolute({g_filePathMap}, writer_global.currentmetamakedirabs, filepath))
		
		local normalisedMakeOutputDirAbs = Util_FileNormaliseUnix(writer_global.makeoutputdirabs)

		--baseDir is the common path fragment shared by the makefile output directory and 'filepath'
		local baseDir = nil
		do
			local commonSubSequenceLength = GetLongestCommonSequenceLengthFromStart(normalisedFilepathAbs, normalisedMakeOutputDirAbs)
			local commonSubSequence = normalisedFilepathAbs:sub(1, commonSubSequenceLength)
			
			--Look for last dir sep character in order to ignore a partial path or file match
			local lastDirSep = Util_FindLast(commonSubSequence, "/")
			if (lastDirSep > 0) then
				--Take sequence up to last dir sep as our base dir
				baseDir = commonSubSequence:sub(1, lastDirSep)
			end
		end
		
		local pathFromBaseToOutDir = Util_StringReplace(normalisedMakeOutputDirAbs, baseDir, "")
		local nDirLevels = GetNumDirLevels("/" .. pathFromBaseToOutDir)
		--Path back from make output dir to base dir
		local pathBack = BuildPathBack(nDirLevels)
		
		local filepathBaseRelative = Util_StringReplace(normalisedFilepathAbs, baseDir, "")
		result = pathBack .. filepathBaseRelative		
	else
		result = Util_FileNormaliseUnix(Util_FileConvertToAbsolute({g_filePathMap}, writer_global.currentmetamakedirabs, filepath))
	end
	
	return result
end


function InitVars(currentTarget)
	g_varBUILDCONFIG	= "BUILDCONFIG"
	g_varINTDIR			= currentTarget.name .. "_INTDIR"
	g_varOUTDIR			= currentTarget.name .. "_OUTDIR"
	g_varMODULEOBJ 		= currentTarget.name .. "_MODULEOBJ"
	g_varCFLAGS 		= currentTarget.name .. "_CFLAGS"
	g_varCPPFLAGS		= currentTarget.name .. "_CPPFLAGS"
	g_varCXXFLAGS		= currentTarget.name .. "_CXXFLAGS"
	g_varINCLUDES		= currentTarget.name .. "_INCLUDES"
	g_varLIBS			= currentTarget.name .. "_LIBS"
	g_varLDLIBS			= currentTarget.name .. "_LDLIBS"
	g_varLIBDIRS		= currentTarget.name .. "_LIBDIRS"
	g_varLDFLAGS		= currentTarget.name .. "_LDFLAGS"
	g_varARFLAGS		= currentTarget.name .. "_ARFLAGS"
	g_varCC				= currentTarget.name .. "_CC"
	g_varCXX			= currentTarget.name .. "_CXX"
	g_varAR				= currentTarget.name .. "_AR"
	g_varLD				= currentTarget.name .. "_LD"
	g_varSRC			= currentTarget.name .. "_SRC"
	g_varOBJ			= currentTarget.name .. "_OBJ"
	g_varDEFINES		= currentTarget.name .. "_DEFINES"
end

function GetDollarVar(var)
	return "$(" .. var .. ")"
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
		mbwriter_fatalerror("unsupported source file type")
	end
	basename = Util_StringReplace(buildFile.objFile, ".o", "")

	file:write(buildFile.objFile .. " : " .. buildFile.srcFile .. " " .. basename .. ".d \n")
	file:write("	@echo " .. compiler .. " " .. buildFile.srcFile .. "\n")
	file:write("	@" .. compiler .. " " .. compilerFlags .. " -o '$@' '$<'; \n")
end

function WriteMakeFileGlobalVars(file, currentTarget)
	file:write("BUILDCONFIG := " .. currentTarget.configs[1].name .. "\n\n")
end

function WriteMakeFileCommonVars(file, currentTarget)
	file:write(g_varINTDIR .. 		" := " .. g_intdir .. "\n")
	file:write(g_varOUTDIR .. 		" := " .. g_outdir .. "\n")
	file:write("\n")
	if currentTarget.options.cc == nil then
		mbwriter_fatalerror("No C compiler set")
	end
	
	--print(inspect(currentTarget))
	
	file:write(g_varCC .. 			" := " .. currentTarget.options.cc[1] .. "\n")
	if currentTarget.options.cxx == nil then
		mbwriter_fatalerror("No C++ compiler set")
	end	
	file:write(g_varCXX ..			" := " .. currentTarget.options.cxx[1] .. "\n")

	for i = 1, #currentTarget.configs do
		local config = currentTarget.configs[i]
		--print(inspect(config))

		--CPPFLAGS is commong to C and C++
		file:write(g_varCPPFLAGS .. "." .. config.name .. " := \\\n")
		if config.options.cppflags ~= nil then
			for i = 1, #config.options.cppflags do
				file:write("  " .. config.options.cppflags[i] .. " \\\n")
			end
		end
		file:write("\n")
		file:write(g_varCPPFLAGS .. "." .. config.name .. " := $(" .. g_varCPPFLAGS .. "." .. config.name .. ")\n")

		--CFLAGS is for C only
		file:write(g_varCFLAGS .. "." .. config.name .. " := \\\n")
		if config.options.cflags ~= nil then
			for i = 1, #config.options.cflags do
				file:write("  " .. config.options.cflags[i] .. " \\\n")
			end
		end
		file:write("\n")
		file:write(g_varCFLAGS .. "." .. config.name .. " := $(" .. g_varCFLAGS .. "." .. config.name .. ")\n")

		--CXXFLAGS is for C++ only
		file:write(g_varCXXFLAGS .. "." .. config.name .. " := \\\n")
		if config.options.cxxflags ~= nil then
			for i = 1, #config.options.cxxflags do
				file:write("  " .. config.options.cxxflags[i] .. " \\\n")
			end
		end
		file:write("\n")
		file:write(g_varCXXFLAGS .. "." .. config.name .. " := $(" .. g_varCXXFLAGS .. "." .. config.name .. ")\n")

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
			local includeDir = GetFullFilePath(config.includedirs[i])
			file:write("  -I" .. Util_FileQuoted(includeDir) .. " \\\n")
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
		mbwriter_fatalerror("No linker set")
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
		file:write(g_varLDFLAGS .. "." .. config.name .. " += $(" .. g_varLDFLAGS .. "." .. config.name .. ")\n")

		--Library directories. Will be stored in LDFLAGS
		file:write(g_varLIBDIRS .. "." .. config.name .. " := \\\n")
		if config.options.libdirs ~= nil then
			for i = 1, #config.libdirs do
				local libDir = GetFullFilePath(config.libdirs[i])
				file:write("  -L" .. libDir .. " \\\n")
			end
		end
		file:write("\n")
		file:write(g_varLDFLAGS .. "." .. config.name .. " += $(" .. g_varLIBDIRS .. "." .. config.name .. ")\n")
		
		--Libraries
		file:write(g_varLIBS .. "." .. config.name .. " := \\\n")
		if config.options.libs ~= nil then
			for i = 1, #currentTarget.libs do
				local lib = GetFullFilePath(config.libs[i])
				file:write("  " .. lib .. " \\\n")
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
		mbwriter_fatalerror("No linker set")
	end
	file:write(g_varLD .. 		" := " .. currentTarget.options.ld[1] .. "\n")
	file:write(g_varCFLAGS ..	" += -c\n")
	file:write(g_varCXXFLAGS ..	" += -c\n")
	file:write(g_varLDFLAGS ..	" := \n")
end

function WriteMakeFileStaticLibVars(file, currentTarget)
	if currentTarget.options.ar == nil then
		mbwriter_fatalerror("No archive tool specified")
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
	--print(inspect(currentTarget))
	if currentTarget.targetType == "app" then
		if (currentTarget.options.appfileextension ~= nil) then
			ext = "." .. currentTarget.options.appfileextension[1]
		end
	elseif currentTarget.targetType == "staticlib" then
		ext = ".a"
	else
	end

	local targetName = GetDollarVar(g_varOUTDIR) .. "/" .. currentTarget.name
	if (ext ~= nil) then
		targetName = targetName .. ext
	end

	return targetName	
end

function GetPreLinkFileName(currentTarget)
	local targetFileName = GetDollarVar(g_varINTDIR) .. "/__" .. currentTarget.name .. "__prelink__"
	return targetFileName
end

function WriteCompileRule(file, currentTarget)
	local targetFileName = GetPreLinkFileName(currentTarget)

	file:write(targetFileName .. " : " .. GetDollarVar(g_varOBJ) .. " ")

	for i = 1, #currentTarget.depends do
		local dependency = currentTarget.depends[i]
		local path, filename, ext = Util_FilePathDecompose(dependency)
		local submakeLinkTargetAbs = mbwriter_gettarget(filename)
		
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
	file:write("	@" .. GetDollarVar(g_varLD) .. " " .. GetDollarVar(g_varLDFLAGS) .. " " .. GetDollarVar(g_varMODULEOBJ) .. " " .. GetDollarVar(g_varOBJ) .. " -o '$@' ;\n")
end

function WriteMakeFileModuleTarget(file, currentTarget)
	local targetFileName = GetTargetOutputFilename(currentTarget)
	local targetPreLinkFileName = WriteCompileRule(file, currentTarget)

	file:write(targetFileName .. " : " .. targetPreLinkFileName  .. "\n")
	file:write("	@echo " .." ld" .. " Creating module obj " .. targetFileName .. "\n")
	file:write("	@" .. "ld" .. " " .. " -r " .. GetDollarVar(g_varMODULEOBJ) .. " " .. GetDollarVar(g_varOBJ) .. " -o '$@' ;\n")
	mbwriter_registertarget(currentTarget.name, targetFileName)
end

function WriteMakeFileStaticLibTarget(file, currentTarget)
	local targetFileName = GetTargetOutputFilename(currentTarget)
	local targetPreLinkFileName = WriteCompileRule(file, currentTarget)

	file:write(targetFileName .. " : " .. targetPreLinkFileName  .. "\n")
	file:write("	@echo " .. GetDollarVar(g_varAR) .. " Creating static lib " .. targetFileName .. "\n")
	file:write("	@" .. GetDollarVar(g_varAR) .. " " .. GetDollarVar(g_varARFLAGS) .. " " .. targetFileName .. " " .. GetDollarVar(g_varOBJ) .. "\n")
	mbwriter_registertarget(currentTarget.name, targetFileName)
end

function WriteMakeFile(currentTarget)

	local makeDir = Util_FilePathJoin(writer_global.makeoutputdirabs, "")
	mkdir(makeDir)
	
	local makeFilename = ""
	if (writer_global.ismainmakefile) then
		makeFilename = Util_FilePathJoin(makeDir, "Makefile")
	else
		makeFilename = Util_FilePathJoin(makeDir, currentTarget.name .. ".mk")
	end
	
	local file = io.open(makeFilename, "w")
	
	InitVars(currentTarget)	

	if (g_useRelativePaths == true) then
		g_intdir = writer_global.intdir
		g_outdir = writer_global.outdir
	else
		g_intdir = Util_FilePathJoin(writer_global.makeoutputdirabs, writer_global.intdir)
		g_outdir = Util_FilePathJoin(writer_global.makeoutputdirabs, writer_global.outdir)
	end
	g_intdir = Util_FilePathJoin(g_intdir, currentTarget.name .. "/" .. GetDollarVar(g_varBUILDCONFIG))
	g_outdir = Util_FilePathJoin(g_outdir, currentTarget.name .. "/" .. GetDollarVar(g_varBUILDCONFIG))
	
	g_intdir = GetFullFilePath(g_intdir)
	g_outdir = GetFullFilePath(g_outdir)
	
	--print(inspect(writer_global))
	--print(inspect(currentTarget))

	--write out content we only require once per makefile
	if (g_firstTargetWritten == false) then
		g_firstTargetWritten = true

		--write out content we only require at the start of the main makefile
		if (writer_global.ismainmakefile) then
			WriteMakeFileGlobalVars(file, currentTarget)
		end
	end
	
	file:write("\n")
	if (writer_global.ismainmakefile) then
		file:write("default : all \n")
	end
		
	file:write(g_varMODULEOBJ .. " := \n")
	--include submakefiles
	for i = 1, #currentTarget.depends do
		local dependency = currentTarget.depends[i]
		local path, filename, ext = Util_FilePathDecompose(dependency)

		local submakeLinkTarget = mbwriter_gettarget(filename)
		local submakefile = filename .. ".mk"
		if (g_useRelativePaths == true) then
			submakefile = filename .. ".mk"
		else
			submakefile = writer_global.makeoutputdirabs .. "/" .. filename .. ".mk"
		end
		
		file:write("include " .. submakefile .. "\n")
		file:write(g_varMODULEOBJ .. " += " .. submakeLinkTarget .. "\n")
	end
	file:write("\n")
			
	WriteMakeFileCommonVars(file, currentTarget)

	--Write out variables for current target type
	if currentTarget.targetType == "app" then
		WriteMakeFileAppVars(file, currentTarget)
	elseif currentTarget.targetType == "staticlib" then
		WriteMakeFileStaticLibVars(file, currentTarget)
	elseif currentTarget.targetType == "module" then
		WriteMakeFileModuleVars(file, currentTarget)
	else
		mbwriter_fatalerror("unsupported target type")
	end
		
    local buildFiles = {}
	for i = 1, #currentTarget.files do
		local f = currentTarget.files[i]
		local path, filename, ext = Util_FilePathDecompose(f)
		if ext == "c" or ext == "cpp" then
			local filenameAbs = GetFullFilePath(f)
						
			local fileBaseNameCount = g_sourceFileBaseNameCounts[filename]
			if (fileBaseNameCount == nil) then
				g_sourceFileBaseNameCounts[filename] = 0
				fileBaseNameCount = 0				
			end
			
			local obj = Util_StringReplace(filename, ".cpp", "")
			obj = Util_StringReplace(obj, ".c", "")
			obj = GetDollarVar(g_varINTDIR) .. "/" .. obj
			local objWithCount = obj
			if (fileBaseNameCount > 0) then
				obj = obj .. "__" .. fileBaseNameCount
			end
			obj = obj .. ".o"

			table.insert(buildFiles, {objFile=obj, srcFile=filenameAbs, ext=ext})
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
	if currentTarget.targetType == "app" then
		WriteMakeFileAppTarget(file, currentTarget)
	elseif currentTarget.targetType == "staticlib" then
		WriteMakeFileStaticLibTarget(file, currentTarget)
	elseif currentTarget.targetType == "module" then
		WriteMakeFileModuleTarget(file, currentTarget)
	else
		mbwriter_fatalerror("unsupported target type")
	end

	file:write("\n")
	
	--print(inspect(currentTarget))
	
	file:write("\n")
	file:write(GetDollarVar(g_varOBJ) .. " : | " .. GetDollarVar(g_varINTDIR) .. "\n\n")
	file:write(GetDollarVar(g_varINTDIR) .. ":\n")
	file:write("	mkdir -p " .. GetDollarVar(g_varINTDIR) .. "\n")
	file:write("	mkdir -p " .. GetDollarVar(g_varOUTDIR) .. "\n")
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
		local path, filename, ext = Util_FilePathDecompose(dependency)
		file:write("clean_" .. filename .. " ")
	end
	file:write("\n")
	file:write("	@echo Cleaning " .. GetDollarVar(g_varINTDIR) .. "\n")
	file:write("	@rm -f \"" .. GetDollarVar(g_varINTDIR) .. "\"/*\n")
	file:write("	@if [ -d \"" .. GetDollarVar(g_varINTDIR) .. "\" ]; then rmdir \"" .. GetDollarVar(g_varINTDIR) .. "\";fi\n")
	file:write("	@echo Cleaning " .. GetDollarVar(g_varOUTDIR) .. "\n")
	file:write("	@rm -f \"" .. GetDollarVar(g_varOUTDIR) .. "\"/*\n")
	file:write("	@if [ -d \"" .. GetDollarVar(g_varOUTDIR) .. "\" ]; then rmdir \"" .. GetDollarVar(g_varOUTDIR) .. "\";fi\n")
	file:write("\n")
	
	if (writer_global.ismainmakefile) then
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
		local depFile = Util_StringReplace(objFile, ".o", ".d")
		file:write("-include " .. depFile .. "\n")
	end	
	
	file:write("\n")
	file:close()

	reportoutputfile(makeFilename)	
end

----------------------------------------------------------------------------------------------------------------------------------------------------------------
--MAIN
----------------------------------------------------------------------------------------------------------------------------------------------------------------

local currentTarget = writer_solution.targets[1]
WriteMakeFile(currentTarget)
