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
function WriteSourceToObjRule(file, sourceFileExt)
	local compilerFlags = "$(CPPFLAGS)"
	if sourceFileExt == "c" then
		compilerFlags = compilerFlags .. " $(CFLAGS)"
	elseif sourceFileExt == "cpp" then
		compilerFlags = compilerFlags .. " $(CXXFLAGS)"
	else
		mbwriter_fatalerror("unsupported source file type")
	end

	file:write("%.o : %." .. sourceFileExt .. " %.d \n")
	file:write("	$(CC) " .. compilerFlags .. " -o '$@' '$<'; \\\n")
end

function WriteMakeFileCommonVars(file, currentTarget)
	file:write("BUILD := " .. currentTarget.configs[1].name .. "\n\n")
	file:write("CC := g++\n")

	for i = 1, #currentTarget.configs do
		local config = currentTarget.configs[i]

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
		if config.options.defines ~= nil then
			for i = 1, #config.options.defines do
				file:write("  -D" .. config.options.defines[i] .. " \\\n")
			end
		end
		file:write("\n")
		file:write("CPPFLAGS." .. config.name .. " += ${DEFINES." .. config.name .. "}\n")

		--Out include paths are shared between languages, so store in CPPFLAGS
		file:write("INCLUDES." .. config.name .. " := \\\n")
		for i = 1, #config.includedirs do
			local includeDir = GetFullFilePath(config.includedirs[i])
			file:write("  -I" .. includeDir .. " \\\n")
		end
		file:write("\n")
		file:write("CPPFLAGS." .. config.name .. " += ${INCLUDES." .. config.name .. "}\n")
	end

	file:write("CPPFLAGS	:= ${CPPFLAGS.${BUILD}}\n")
	file:write("CFLAGS		:= ${CFLAGS.${BUILD}}\n")
	file:write("CXXFLAGS	:= ${CXXFLAGS.${BUILD}}\n")
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
	file:write("LDFLAGS := ${LDFLAGS.${BUILD}} ${LDLIBS.${BUILD}}\n")
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

	file:write("ARFLAGS := ${ARFLAGS.${BUILD}}\n")
	
	file:write("CFLAGS += -c\n")
	file:write("CXXFLAGS += -c\n")
end

function WriteMakeFileAppTarget(file, currentTarget)
	file:write(currentTarget.name .. " : $(OBJ) \n")
	file:write("	$(LD) $(LDFLAGS) $(MODULEOBJ) '$<' -o '$@' ;\n")
end

function WriteMakeFileModuleTarget(file, currentTarget)
	file:write(currentTarget.name .. " : $(OBJ) \n")
	file:write("	$(LD) $(LDFLAGS) -r $(MODULEOBJ) '$<' -o '$@' ;\n")
end

function WriteMakeFileStaticLibTarget(file, currentTarget)
	local compileTargetName = currentTarget.name .. "_compile"
	file:write(compileTargetName .. " : $(OBJ) \n\n")
	file:write(currentTarget.name .. " : " .. compileTargetName .. " \n")
	file:write("	$(AR) $(ARFLAGS) " .. currentTarget.name .. ".a ${OBJ}\n")
end

function WriteMakeFile(currentTarget)

	local makeDir = writer_global.makeoutputdirabs .. "/" .. currentTarget.name
	mkdir(makeDir)

	print(inspect(config))

	local makeFilename = makeDir .. "/" .. "Makefile"
	local file = io.open(makeFilename, "w")

	local intdir = writer_global.intdir .. "/" .. currentTarget.name 
	local outdir = writer_global.outdir  

	--file:write("intdir = " .. intdir .. "\n")
	--file:write("outdir = " .. outdir .. "\n")
	--file:write("builddir = $intdir\n")
		
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

    local srcFiles = {}
	for i = 1, #currentTarget.files do
		local f = currentTarget.files[i]
		local path, filename, ext = Util_FilePathDecompose(f)
		if ext == "c" or ext == "cpp" then
			local filenameAbs = GetFullFilePath(f)
			table.insert(srcFiles, filenameAbs)
		end
	end
	file:write("\n")
	file:write("SRC := \\\n")
	for i = 1, #srcFiles do
		file:write("	" .. srcFiles[i] .. " \\\n")
	end
	
	file:write("\n")
	file:write("OBJ := $(SRC:.cpp=.o)\n")
	file:write("OBJ += $(SRC:.c=.o)\n")
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
	
	if #currentTarget.depends > 0 then
		file:write("makemodules : \n")

		for i = 1, #currentTarget.depends do
			local dependency = currentTarget.depends[i]
			local path, filename, ext = Util_FilePathDecompose(dependency)

			file:write("	$(MAKE) -C " .. path .. " BUILD='$(BUILD)'\\\n\n")
			file:write("MODULEOBJ += " .. dependency .. "\n")
		end

		file:write("all : makemodules \\\n")
	else
		file:write("all : \\\n")
	end
	file:write("    " .. currentTarget.name .. "\\\n")
	file:write("\n\n")

	WriteSourceToObjRule(file, "cpp");
	file:write("\n")
	WriteSourceToObjRule(file, "c");	
	file:write("\n")

	file:write("%.d: ;\n")
	file:write("-include $(SRC:%.cpp=%.d)\n")
	
	file:write("\n")
	file:close()

	reportoutputfile(makeFilename)	
end

----------------------------------------------------------------------------------------------------------------------------------------------------------------
--MAIN
----------------------------------------------------------------------------------------------------------------------------------------------------------------

local currentTarget = writer_solution.targets[1]

WriteMakeFile(currentTarget)

