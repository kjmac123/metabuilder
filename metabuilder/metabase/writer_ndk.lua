import "writer_common.lua"

--Map relative to absolute path
g_filePathMap = {}

----------------------------------------------------------------------------------------------------------------------------------------------------------------

function GetFullFilePath(filepath)
	local newfilepath = g_filePathMap[filepath]
	if newfilepath == nil then
		return Util_FileConvertToAbsolute({g_filePathMap}, writer_global.currentmetamakedirabs, filepath)
	end

	return newfilepath
end

function GetWorkspaceDir(currentTargetName, configName)
	return writer_global.makeoutputdirabs .. "/" .. currentTargetName .. "/" .. configName
end

function GetJNIDir(currentTargetName, configName)
	return GetWorkspaceDir(currentTargetName, configName) .. "/jni"
end

function CreateLinks(currentTarget, config)
	local templateDir = Util_GetKVValue(config.options._ndk, "templatedir")
	if templateDir == nil then
		--TODO proper error here
		print("Template dir not specified")
		os.exit(1)
	end
	templateDir = GetFullFilePath(templateDir)

	local workspaceDir = GetWorkspaceDir(currentTarget.name, config.name);
	mkdir(workspaceDir)

	--print(templateDir)
	--print(workspaceDir)
	mklink(templateDir .. "/build.xml",									workspaceDir .. "/build.xml")
	mklink(templateDir .. "/AndroidManifest.xml",						workspaceDir .. "/AndroidManifest.xml")
	mklink(templateDir .. "/assets",									workspaceDir .. "/assets")
	mklink(templateDir .. "/res",										workspaceDir .. "/res")
	mklink(templateDir .. "/src",										workspaceDir .. "/src")
end

----------------------------------------------------------------------------------------------------------------------------------------------------------------
--FILE WRITING
----------------------------------------------------------------------------------------------------------------------------------------------------------------

function WriteApplicationMk(currentTarget, config)

--	if config == nil then
		--TODO ERROR HERE
--		return

	local jniDir = writer_global.makeoutputdirabs .. "/" .. currentTarget.name .. "/" .. config.name .. "/jni"
	mkdir(jniDir)

	local makeFilename = jniDir .. "/Application.mk"
	local file = io.open(makeFilename, "w")

	file:write("APP_MODULES := " .. currentTarget.name .. " ")
	for i = 1, #currentTarget.depends do
		local dependency = currentTarget.depends[i]
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
	reportoutputfile(makeFilename)
end

function WriteAndroidMk(currentTarget, config)

--	if config == nil then
		--TODO ERROR HERE
--		return

	local jniDir = writer_global.makeoutputdirabs .. "/" .. currentTarget.name .. "/" .. config.name .. "/jni"
	mkdir(jniDir)

	local makeFilename = jniDir .. "/Android.mk"
	local file = io.open(makeFilename, "w")

	for i = 1, #currentTarget.depends do
		local dependency = currentTarget.depends[i]
		local path, filename, ext = Util_FilePathDecompose(dependency)

		file:write("SOURCE_ROOT := " .. jniDir .. "\n")
		file:write("include $(SOURCE_ROOT)/../../../" .. filename .. "/" .. config.name .. "/jni/Android.mk\n")
		--file:write("include ../../../" .. filename .. "/" .. config.name .. "/jni/Android.mk\n")
--		local includeFile = jniDir .. "/../../../" .. filename .. "/" .. config.name .. "/jni/Android.mk"
--		file:write("include " .. includeFile .. "\n")

		file:write("include $(CLEAR_VARS)\n\n")
	end

	--file:write("MY_LOCAL_PATH := $(call my-dir)\n")
	file:write("MY_GENERAL_FLAGS := \\\n")

	--Write defines
	for iDefine = 1, #config.defines do
		tmp = string.gsub(config.defines[iDefine], "\"", "\\\"")
		file:write("	-D" .. tmp .. " \\\n")
	end
	file:write("\n")

	file:write("MY_LOCAL_CPPFLAGS 	:= $(MY_GENERAL_FLAGS)\n")
	file:write("MY_LOCAL_CFLAGS 	:= $(MY_GENERAL_FLAGS)\n")

	-- Add Neon Support for armeabi-v7a
	file:write("ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)\n") 
    file:write("LOCAL_ARM_NEON  := true\n")
   	file:write("endif # TARGET_ARCH_ABI == armeabi-v7a\n")


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

	f = writer_global.makeoutputdirabs .. "/" .. currentTarget.name .. "/" .. config.name .. "/obj/local/" .. abi	
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

	--Include static libs
	for i = 1, #staticLibs do
		local f = staticLibs[i]
		local path, filename, ext = Util_FilePathDecompose(f)
		filename = string.gsub(filename, "%.a", "")

		file:write("include $(CLEAR_VARS)\n")
		file:write("LOCAL_MODULE            := " .. filename .. "\n")
		file:write("LOCAL_SRC_FILES         := " .. GetFullFilePath(f) .. "\n")
		file:write("include $(PREBUILT_STATIC_LIBRARY)\n\n")
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
	for i = 1, #currentTarget.files do
		local f = currentTarget.files[i]
		local ext = Util_FileExtension(f)
		if ext == "c" or ext == "cpp" then
		    file:write("	$(SOURCE_ROOT)/" .. f .. "\\\n")
		end
	end
	file:write("\n")

	file:write("include $(CLEAR_VARS)\n")
	file:write("LOCAL_PATH := $(MY_LOCAL_PATH)\n")
	file:write("LOCAL_MODULE := " .. currentTarget.name .. "\n")
	file:write("LOCAL_C_INCLUDES := $(MY_LOCAL_C_INCLUDES)\n")
	file:write("LOCAL_CPPFLAGS := $(MY_LOCAL_CPPFLAGS)\n")
	file:write("LOCAL_CFLAGS := $(MY_LOCAL_CFLAGS)\n")
	file:write("LOCAL_SRC_FILES := $(MY_LOCAL_SRC_FILES)\n")
	file:write("\n")

	if currentTarget.targetType == "app" then
		
		file:write("LOCAL_LDLIBS :=  $(MY_LIB_SEARCH_PATHS) $(MY_LIBS) -llog -landroid -lEGL -lGLESv2 -lOpenSLES \\\n")

		-- Link with required projects. Must be a better way than this, making better use of the android make system rather than linking 'manually'
		--[[
		for i = 1, #currentTarget.depends do
			local dependency = currentTarget.depends[i]
			local path, filename, ext = Util_FilePathDecompose(dependency)

			local abi = Util_GetKVValue(config.options.ndkoptions, "APP_ABI")
			if abi == nil then
				-- TODO error
			end

			libPrefixResult = string.find(filename, "lib")
			if libPrefixResult == nil or libPrefixResult ~= 1 then 
				filename = "lib" .. filename
			end			

			file:write("	-l:" .. writer_global.makeoutputdirabs .. "/" .. currentTarget.name .. "/" .. config.name .. "/obj/local/" .. abi .. "/" .. filename .. ".a \\\n")
		end
]]
--[[
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
]]
		file:write("\n")


		file:write("LOCAL_SHARED_LIBRARIES := \\\n")
		for i = 1, #sharedLibs do
			local f = sharedLibs[i]
			local path, filename, ext = Util_FilePathDecompose(f)
			file:write("	" .. filename .. " \\\n")
		end
		file:write("\n")

		file:write("LOCAL_STATIC_LIBRARIES := ")

		local libList = {}
		

		for i = 1, #staticLibs do
			local f = staticLibs[i]
			local path, filename, ext = Util_FilePathDecompose(f)

--[[
			if string.find(f, "-l:") == nil then
				f = "$(SOURCE_ROOT)/" .. f
			else
				--temporarily remove prefix so that we can expand the filepath
				f = string.gsub(f, "-l:", "")
				f = "$(SOURCE_ROOT)/" .. f
				f = "-l:" .. f
			end
]]

			filename = string.gsub(filename, "%.a", "")
			table.insert(libList, filename)
		end

		for i = 1, #currentTarget.depends do
			local dependency = currentTarget.depends[i]
			local path, filename, ext = Util_FilePathDecompose(dependency)
			table.insert(libList, filename)
		end
		

		for i = 1, #libList do
			local lib = libList[i]
			file:write(lib .. " ")
		end
		file:write("\n")		

		file:write("include $(BUILD_SHARED_LIBRARY)\n")
	elseif currentTarget.targetType == "staticlib" then
		file:write("include $(BUILD_STATIC_LIBRARY)\n")
	else
		--TODO - error here
	end

	file:close()
	reportoutputfile(makeFilename)
end

function WriteJNI(currentTarget, config)
	--links to template folder required for apps, but not libraries
	if currentTarget.targetType == "app" then
		CreateLinks(currentTarget, config)
	end

	WriteApplicationMk(currentTarget, config)
	WriteAndroidMk(currentTarget, config)
end

----------------------------------------------------------------------------------------------------------------------------------------------------------------
--MAIN
----------------------------------------------------------------------------------------------------------------------------------------------------------------

local currentTarget = writer_solution.targets[1]

for i = 1, #currentTarget.configs do
	local config = currentTarget.configs[i]
	WriteJNI(currentTarget, config)
end

