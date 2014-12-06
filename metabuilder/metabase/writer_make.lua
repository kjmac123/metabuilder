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


--Map relative to absolute path
g_filePathMap = {}

g_intdir = ""
g_outdir = ""

----------------------------------------------------------------------------------------------------------------------------------------------------------------

function GetFullFilePath(filepath)
	local newfilepath = g_filePathMap[filepath]
	if newfilepath == nil then
		return Util_FileNormaliseUnix(Util_FileConvertToAbsolute({g_filePathMap}, writer_global.currentmetamakedirabs, filepath))
	end

	return Util_FileNormaliseUnix(newfilepath)
end

----------------------------------------------------------------------------------------------------------------------------------------------------------------
--FILE WRITING
----------------------------------------------------------------------------------------------------------------------------------------------------------------

function WriteSourceToObjRule(file, buildFile)
	
	local compiler = ""
	local compilerFlags = "$(CPPFLAGS)"
	if buildFile.ext == "c" then
		compiler = "$(CC)"
		compilerFlags = compilerFlags .. " $(CFLAGS)"
	elseif buildFile.ext == "cpp" then
		compiler = "$(CXX)"
		compilerFlags = compilerFlags .. " $(CXXFLAGS)"
	else
		mbwriter_fatalerror("unsupported source file type")
	end
	basename = Util_StringReplace(buildFile.objFile, ".o", "")

	file:write(buildFile.objFile .. " : " .. buildFile.srcFile .. " " .. basename .. ".d \n")
	file:write("	" .. compiler .. " " .. compilerFlags .. " -o '$@' '$<'; \n")
	file:write("\n")
end


function WriteMakeFileCommonVars(file, currentTarget)
	file:write("BUILDCONFIG := " .. currentTarget.configs[1].name .. "\n\n")
	file:write("INTDIR := " .. g_intdir .. "\n")
	file:write("OUTDIR := " .. g_outdir .. "\n")
	file:write("\n")
	file:write("CC := gcc\n")
	file:write("CXX := g++\n")

	for i = 1, #currentTarget.configs do
		local config = currentTarget.configs[i]
		--print(inspect(config))

		--CPPFLAGS is commong to C and C++
		file:write("CPPFLAGS." .. config.name .. " := \\\n")
		if config.options.cppflags ~= nil then
			for i = 1, #config.options.cppflags do
				file:write("  " .. config.options.cppflags[i] .. " \\\n")
			end
		end
		file:write("\n")
		file:write("CPPFLAGS." .. config.name .. " := ${CPPFLAGS." .. config.name .. "}\n")

		--CFLAGS is for C only
		file:write("CFLAGS." .. config.name .. " := \\\n")
		if config.options.cflags ~= nil then
			for i = 1, #config.options.cflags do
				file:write("  " .. config.options.cflags[i] .. " \\\n")
			end
		end
		file:write("\n")
		file:write("CFLAGS." .. config.name .. " := ${CFLAGS." .. config.name .. "}\n")

		--CXXFLAGS is for C++ only
		file:write("CLAGS." .. config.name .. " := \\\n")
		if config.options.cflags ~= nil then
			for i = 1, #config.options.cflags do
				file:write("  " .. config.options.cflags[i] .. " \\\n")
			end
		end
		file:write("\n")
		file:write("CFLAGS." .. config.name .. " := ${CFLAGS." .. config.name .. "}\n")

		--Preprocessor macros will live in CPPFLAGS
		file:write("DEFINES." .. config.name .. " := \\\n")
		if config.defines ~= nil then
			for i = 1, #config.defines do
				file:write("  -D" .. config.defines[i] .. " \\\n")
			end
		end
		file:write("\n")
		file:write("CPPFLAGS." .. config.name .. " += ${DEFINES." .. config.name .. "}\n")

		--Out include paths are shared between languages, so store in CPPFLAGS
		file:write("INCLUDES." .. config.name .. " := \\\n")
		for i = 1, #config.includedirs do
			local includeDir = GetFullFilePath(config.includedirs[i])
			file:write("  -I" .. Util_FileQuoted(includeDir) .. " \\\n")
		end
		file:write("\n")
		file:write("CPPFLAGS." .. config.name .. " += ${INCLUDES." .. config.name .. "}\n")
	end

	file:write("CPPFLAGS	:= -MMD -MP ${CPPFLAGS.${BUILDCONFIG}}\n")
	file:write("CFLAGS		:= ${CFLAGS.${BUILDCONFIG}}\n")
	file:write("CXXFLAGS	:= ${CXXFLAGS.${BUILDCONFIG}}\n")
end

function WriteMakeFileAppVars(file, currentTarget)
	file:write("CFLAGS += -c\n")
	file:write("CXXFLAGS += -c\n")
	file:write("LD := g++\n")
	file:write("\n")

	for i = 1, #currentTarget.configs do
		local config = currentTarget.configs[i]

		--Linker flags
		file:write("LDFLAGS." .. config.name .. " := \\\n")
		if config.options.ldflags ~= nil then
			for i = 1, #config.options.ldflags do
				file:write(config.options.ldflags[i] .. " \\\n")
			end
		end
		file:write("\n")
		file:write("LDFLAGS." .. config.name .. " += ${LDFLAGS." .. config.name .. "}\n")

		--Library directories. Will be stored in LDFLAGS
		file:write("LIBDIRS." .. config.name .. " := \\\n")
		if config.options.libdirs ~= nil then
			for i = 1, #config.libdirs do
				local libDir = GetFullFilePath(config.libdirs[i])
				file:write("  -L" .. libDir .. " \\\n")
			end
		end
		file:write("\n")
		file:write("LDFLAGS." .. config.name .. " += ${LIBDIRS." .. config.name .. "}\n")
		
		--Libraries
		file:write("LIBS." .. config.name .. " := \\\n")
		if config.options.libs ~= nil then
			for i = 1, #currentTarget.libs do
				local lib = GetFullFilePath(config.libs[i])
				file:write("  " .. lib .. " \\\n")
			end
		end
		file:write("\n")
		file:write("LDLIBS." .. config.name .. " += ${LIBS." .. config.name .. "}\n")

		file:write("\n")
	end
	file:write("LDFLAGS := ${LDFLAGS.${BUILDCONFIG}} ${LDLIBS.${BUILDCONFIG}}\n")
end

function WriteMakeFileModuleVars(file, currrentTarget)
	file:write("LD := ld\n")
	file:write("CFLAGS += -c\n")
	file:write("CXXFLAGS += -c\n")
	file:write("LDFLAGS := \n")
end

function WriteMakeFileStaticLibVars(file, currentTarget)
	file:write("AR := ar\n")
	file:write("\n")

	for i = 1, #currentTarget.configs do
		local config = currentTarget.configs[i]

		file:write("ARFLAGS." .. config.name .. " := \\\n")
		if config.options.arflags ~= nil then
			for i = 1, #config.options.arflags do
				file:write("  " .. config.options.arflags[i] .. " \\\n")
			end
		end	
		file:write("\n")
	end

	file:write("ARFLAGS := ${ARFLAGS.${BUILDCONFIG}}\n")
	
	file:write("CFLAGS += -c\n")
	file:write("CXXFLAGS += -c\n")
end

function WriteMakeFileAppTarget(file, currentTarget)
	file:write("$(OUTDIR)/" .. currentTarget.name .. " : $(OBJ) \n")
	file:write("	$(LD) $(LDFLAGS) $(MODULEOBJ) $(OBJ) -o '$@' ;\n")
end

function WriteMakeFileModuleTarget(file, currentTarget)
	file:write("$(OUTDIR)/" .. currentTarget.name .. " : $(OBJ) \n")
	file:write("	$(LD) $(LDFLAGS) -r $(MODULEOBJ) $(OBJ) -o '$@' ;\n")
	mbwriter_registertarget(currentTarget.name, Util_FilePathJoin(g_outdir, currentTarget.name))
end

function WriteMakeFileStaticLibTarget(file, currentTarget)
	local compileTargetName = "$(OUTDIR)/" .. currentTarget.name .. "_compile"
	file:write(compileTargetName .. " : $(OBJ) \n\n")
	file:write("$(OUTDIR)/" .. currentTarget.name .. " : " .. compileTargetName .. " \n")
	file:write("	$(AR) $(ARFLAGS) $(OUTDIR)/" .. currentTarget.name .. ".a ${OBJ}\n")
	mbwriter_registertarget(currentTarget.name, Util_FilePathJoin(g_outdir, currentTarget.name) .. ".a")
end

function WriteMakeFile(currentTarget)

	local makeDir = Util_FilePathJoin(writer_global.makeoutputdirabs, currentTarget.name)
	mkdir(makeDir)

	local makeFilename = Util_FilePathJoin(makeDir, "Makefile")
	local file = io.open(makeFilename, "w")

	g_intdir = Util_FilePathJoin(writer_global.makeoutputdirabs, writer_global.intdir)
	g_outdir = Util_FilePathJoin(writer_global.makeoutputdirabs, writer_global.outdir)
	g_intdir = Util_FilePathJoin(g_intdir, currentTarget.name .. "_$(BUILDCONFIG)")
	g_outdir = Util_FilePathJoin(g_outdir, currentTarget.name .. "_$(BUILDCONFIG)")
		
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
	
	file:write("default : all \n\n")
	
    local buildFiles = {}
	for i = 1, #currentTarget.files do
		local f = currentTarget.files[i]
		local path, filename, ext = Util_FilePathDecompose(f)
		if ext == "c" or ext == "cpp" then
			local filenameAbs = GetFullFilePath(f)
			--local filenameAbsEscaped = escape(filenameAbs
			
			local obj = Util_StringReplace(filename, ".cpp", ".o")
			obj  = Util_StringReplace(obj, ".c", ".o")
			
			obj = "$(INTDIR)/" .. obj
			
			table.insert(buildFiles, {objFile=obj, srcFile=filenameAbs, ext=ext})
		end
	end
	file:write("\n")
	file:write("SRC := \\\n")
	for i = 1, #buildFiles do
		file:write("	" .. buildFiles[i].srcFile .. " \\\n")
	end
	file:write("\n")
	
	file:write("OBJ := \\\n")
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

	file:write("MODULEOBJ := \n")
		
	--print(inspect(currentTarget))
	if #currentTarget.depends > 0 then
		file:write("makemodules : \n")

		for i = 1, #currentTarget.depends do
			local dependency = currentTarget.depends[i]
			local path, filename, ext = Util_FilePathDecompose(dependency)

			local moduleLocation = mbwriter_gettarget(filename)

			file:write("	$(MAKE) -C " .. writer_global.makeoutputdirabs .. "/" .. filename .. " all BUILDCONFIG='$(BUILDCONFIG)' \n\n")
			file:write("MODULEOBJ += " .. moduleLocation .. "\n")
		end

		file:write("all : makemodules \\\n")
	else
		file:write("all : ")
	end
	file:write("    " .. "$(OUTDIR)/" .. currentTarget.name .. " \\\n")
	file:write("\n\n")
	file:write("$(OBJ) : | $(INTDIR)\n\n")

	file:write("$(INTDIR):\n")
	file:write("	mkdir -p $(INTDIR)\n")
	file:write("	mkdir -p $(OUTDIR)\n")
	file:write("\n")
	
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
