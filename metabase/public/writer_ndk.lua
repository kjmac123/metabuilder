package.path = package.path .. ";" .. writer_global.metabasedirabs .. "/?.lua"
local inspect = require('inspect')
local util = require('utility')

print(inspect(writer_solution))

g_currentTarget = writer_solution.targets[1]

--Map relative to absolute path
g_filePathMap = {}

function GetFullFilePath(filepath)
	local newfilepath = g_filePathMap[filepath]
	if newfilepath == nil then
		return Util_FileConvertToAbsolute(g_filePathMap, writer_global.currentmetamakedirabs, filepath)
	end

	return newfilepath
end

--[[ FILE WRITING ]] --------------------------------------------------------------------------------

function WriteApplicationMk(configName)
	local config = nil
	for iConfig = 1, #g_currentTarget.configs do
		if  g_currentTarget.configs[iConfig].name == configName then
			config = g_currentTarget.configs[iConfig]
		end
	end

--	if config == nil then
		--TODO ERROR HERE
--		return

	local jniDir = writer_global.makeoutputdirabs .. "/" .. g_currentTarget.name .. "/" .. configName .. "/jni"
	mkdir(jniDir)

	file = io.open(jniDir .. "/Application.mk", "w")

	file:write("APP_MODULES := " .. g_currentTarget.name .. " ")
	for i = 1, #g_currentTarget.depends do
		local dependency = g_currentTarget.depends[i]
		local path, filename, ext = Util_FilePathDecompose(dependency)

		file:write(filename .. " ")
	end
	file:write("\n\n")

	if config.options.ndkoptions ~= nil then
		for jOption = 1, #config.options.ndkoptions do
			local keyValue = split(config.options.ndkoptions[jOption], "=")
			local key = keyValue[1]
			local value = keyValue[2]

			file:write(key .. " := " .. value .. "\n")
		end
	end
	file:close()
end

function WriteAndroidMk(configName)
	local config = nil
	for iConfig = 1, #g_currentTarget.configs do
		if  g_currentTarget.configs[iConfig].name == configName then
			config = g_currentTarget.configs[iConfig]
		end
	end

--	if config == nil then
		--TODO ERROR HERE
--		return

	local jniDir = writer_global.makeoutputdirabs .. "/" .. g_currentTarget.name .. "/" .. configName .. "/jni"
	mkdir(jniDir)
	file = io.open(jniDir .. "/Android.mk", "w")

	for i = 1, #g_currentTarget.depends do
		local dependency = g_currentTarget.depends[i]
		local path, filename, ext = Util_FilePathDecompose(dependency)

		file:write("include ../../../" .. filename .. "/" .. configName .. "/jni/Android.mk\n")
		file:write("include $(CLEAR_VARS)\n\n")
	end

	--file:write("MY_LOCAL_PATH := $(call my-dir)\n")
	file:write("MY_GENERAL_FLAGS := \\\n")

	--Write cflags
	if config.options.cflags ~= nil then
		for jOption = 1, #config.options.cflags do
			local keyValue = split(config.options.cflags[jOption], "=")
			local key = keyValue[1]

			file:write("	" .. key .. " \\\n")			
		end
	end
	--Write defines
	for iDefine = 1, #config.defines do
		local define = config.defines[iDefine]
		file:write("	-D" .. define .. " \\\n")
	end
	file:write("\n")

	file:write("MY_LOCAL_CPPFLAGS 	:= $(MY_GENERAL_FLAGS)\n")
	file:write("MY_LOCAL_CFLAGS 	:= $(MY_GENERAL_FLAGS)\n")

	file:write("SOURCE_ROOT := " .. writer_global.currentmetamakedirabs .. "\n")
	file:write("MY_LOCAL_C_INCLUDES := \\\n")
	for i = 1, #config.includedirs do
	    file:write("	$(SOURCE_ROOT)/" .. config.includedirs[i] .. " \\\n")
	end
    file:write("\n")

	-- Add project lib search paths
	file:write("MY_LIB_SEARCH_PATHS:= \\\n")

	local abi = Util_GetKVValue(config.options.ndkoptions, "APP_ABI")
	if abi == nil then
		-- TODO error
	end

	--[[
	for i = 1, #g_currentTarget.depends do
		local dependency = g_currentTarget.depends[i]
		local path, filename, ext = Util_FilePathDecompose(dependency)
		local f = filename .. ".a"


		--f = writer_global.makeoutputdirabs .. "/" .. filename .. "/" .. configName .. "/obj/local/" .. abi
		f = writer_global.makeoutputdirabs .. "/" .. g_currentTarget.name .. "/" .. configName .. "/obj/local/" .. abi
		
		file:write("	-L" .. f .. " \\\n")
	end
	]]

	f = writer_global.makeoutputdirabs .. "/" .. g_currentTarget.name .. "/" .. configName .. "/obj/local/" .. abi	
	file:write("	-L" .. f .. " \\\n")
	file:write("\n")

    --To make things easier, let's create lists of static and shared libs now
    local sharedLibs = {}
    local staticLibs = {}
	for i = 1, #config.libs do
		local f = config.libs[i]
		local path, filename, ext = Util_FilePathDecompose(f)
		if ext == "so" then
			table.insert(sharedLibs, f)
		else
			table.insert(staticLibs, f)
		end
	end

	--Include shared libs
	for i = 1, #sharedLibs do
		local f = sharedLibs[i]
		local path, filename, ext = Util_FilePathDecompose(f)

		file:write("include $(CLEAR_VARS)\n")
		file:write("LOCAL_MODULE            := " .. filename .. "\n")
		file:write("LOCAL_SRC_FILES         := " .. GetFullFilePath(f) .. "\n")
		file:write("include $(PREBUILT_SHARED_LIBRARY)\n\n")
	end

	file:write("MY_LOCAL_SRC_FILES := \\\n")
	for i = 1, #g_currentTarget.files do
		local f = g_currentTarget.files[i]
		local ext = Util_FileExtension(f)
		if ext == "c" or ext == "cpp" then
		    file:write("	$(SOURCE_ROOT)/" .. f .. "\\\n")
		end
	end
	file:write("\n")

	file:write("include $(CLEAR_VARS)\n")
	file:write("LOCAL_PATH := $(MY_LOCAL_PATH)\n")
	file:write("LOCAL_MODULE := " .. g_currentTarget.name .. "\n")
	file:write("LOCAL_C_INCLUDES := $(MY_LOCAL_C_INCLUDES)\n")
	file:write("LOCAL_CPPFLAGS := $(MY_LOCAL_CPPFLAGS)\n")
	file:write("LOCAL_CFLAGS := $(MY_LOCAL_CFLAGS)\n")
	file:write("LOCAL_SRC_FILES := $(MY_LOCAL_SRC_FILES)\n")
	file:write("\n")

	if g_currentTarget.targetType == "app" then
		
		file:write("LOCAL_LDLIBS :=  $(MY_LIB_SEARCH_PATHS) $(MY_LIBS) -llog -landroid -lEGL -lGLESv2 -lOpenSLES \\\n")

		-- Link with required projects. Must be a better way than this, making better use of the android make system rather than linking 'manually'
		for i = 1, #g_currentTarget.depends do
			local dependency = g_currentTarget.depends[i]
			local path, filename, ext = Util_FilePathDecompose(dependency)

			local abi = Util_GetKVValue(config.options.ndkoptions, "APP_ABI")
			if abi == nil then
				-- TODO error
			end

			libPrefixResult = string.find(filename, "lib")
			if libPrefixResult == nil or libPrefixResult ~= 1 then 
				filename = "lib" .. filename
			end			

			file:write("	-l:" .. writer_global.makeoutputdirabs .. "/" .. g_currentTarget.name .. "/" .. configName .. "/obj/local/" .. abi .. "/" .. filename .. ".a \\\n")
		end

		-- Link with static libs
		for i = 1, #staticLibs do
			local f = staticLibs[i]

			if string.find(f, "-l:") == nil then
				f = "$(SOURCE_ROOT)/" .. f
			else
				--temporarily remove prefix so that we can expand the filepath
				f = string.gsub(f, "-l:", "")
				f = "$(SOURCE_ROOT)/" .. f
				f = "-l:" .. f
			end

			file:write("	" .. f .. " \\\n")
		end

		file:write("\n")


		file:write("LOCAL_SHARED_LIBRARIES := \\\n")
		for i = 1, #sharedLibs do
			local f = sharedLibs[i]
			local path, filename, ext = Util_FilePathDecompose(f)
			file:write("	" .. filename .. " \\\n")
		end
		file:write("\n")

--[[
		file:write("LOCAL_STATIC_LIBRARIES := \\\n")

		-- Link with static libs
		for i = 1, #staticLibs do
			local f = staticLibs[i]
			file:write("	" .. f .. " \\\n")
		end

		file:write("\n")
]]

		file:write("include $(BUILD_SHARED_LIBRARY)\n")
	elseif g_currentTarget.targetType == "staticlib" then
		file:write("include $(BUILD_STATIC_LIBRARY)\n")
	else
		--TODO - error here
	end

	file:close()
end

function WriteJNI(configName)
	WriteApplicationMk(configName)
	WriteAndroidMk(configName)
end

for i = 1, #g_currentTarget.configs do
	--print(inspect(g_currentTarget))
	local config = g_currentTarget.configs[i]
	WriteJNI(config.name)
end

