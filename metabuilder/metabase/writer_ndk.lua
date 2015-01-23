import "writer_common.lua"

g_makeOutputDirAbs							= mbwriter.global.makeoutputdirabs
g_makeOutputDirAbsRoot					= nil
g_makeOutputDirAbsTargetConfig	= nil

----------------------------------------------------------------------------------------------------------------------------------------------------------------

function GetWorkspaceDir(currentTargetName, configName)
	return g_makeOutputDirAbs .. "/" .. currentTargetName .. "/" .. configName
end

function GetJNIDir(currentTargetName, configName)
	return GetWorkspaceDir(currentTargetName, configName) .. "/jni"
end

function CreateLinks(currentTarget, config)
	local templateDir = mbutil.getkvvalue(config.options._ndk, "templatedir")
	if templateDir == nil then
		return
		--mbwriter.fatalerror("Template dir not specified")
	end

	--NOTE: templateDir needs to remain relative to working directory, not output
	--files as links are made withing the metabuilder app itself

	local workspaceDir = GetWorkspaceDir(currentTarget.name, config.name);
	mbwriter.mkdir(workspaceDir)

	mbwriter.mklink(templateDir .. "/build.xml",							workspaceDir .. "/build.xml")
	mbwriter.mklink(templateDir .. "/AndroidManifest.xml",		workspaceDir .. "/AndroidManifest.xml")

	if mbwriter.getfiletype(templateDir .. "/assets") ~= "missing" then
		mbwriter.mklink(templateDir .. "/assets",									workspaceDir .. "/assets")
	else
		loginfo("Not creating link for Android " .. templateDir .. "/assets" .. " folder as does not exist")
	end
	mbwriter.mklink(templateDir .. "/res",										workspaceDir .. "/res")
	mbwriter.mklink(templateDir .. "/src",										workspaceDir .. "/src")
end

function NDKSetMakeOutputDir(outputDir)
	--mbwriter.setmakeoutputdirabs(outputDir)
	--print("NDKSetMakeOutputDir " .. outputDir)
end

----------------------------------------------------------------------------------------------------------------------------------------------------------------
--FILE WRITING
----------------------------------------------------------------------------------------------------------------------------------------------------------------

function WriteApplicationMk(currentTarget, config)

	NDKSetMakeOutputDir(g_makeOutputDirAbsTargetConfig)

	local jniDir = GetJNIDir(currentTarget.name, config.name)
	mbwriter.mkdir(jniDir)

	local makeFilename = jniDir .. "/Application.mk"
	local file = mbfile.open(makeFilename, "w")

	file:write("APP_MODULES := " .. currentTarget.name .. " ")
	for i = 1, #currentTarget.depends do
		local dependency = currentTarget.depends[i]
		local path, filename, ext = mbfilepath.decompose(dependency)

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
	mbwriter.reportoutputfile(makeFilename)

	NDKSetMakeOutputDir(g_makeOutputDirAbsRoot)
end

function WriteAndroidMk(currentTarget, config)

	local localLDLibs = {}
	local localSharedLibs = {}
	local localStaticLibs = {}

	if currentTarget.targettype == "app" then
		--Build LD lib,  shared lib and static lib arrays

		for i = 1, #config.libs do
			local f = config.libs[i]
			local path, filename, ext = mbfilepath.decompose(f)
			if string.find(f, "-l") then
				table.insert(localLDLibs, filename)
			elseif ext == "so" then
				table.insert(localSharedLibs, f)
			else
				table.insert(localStaticLibs, f)
			end
		end
	end

	NDKSetMakeOutputDir(g_makeOutputDirAbsTargetConfig)

	local jniDir = GetJNIDir(currentTarget.name, config.name)
	mbwriter.mkdir(jniDir)

	local makeFilename = jniDir .. "/Android.mk"

	local file = mbfile.open(makeFilename, "w")

	for i = 1, #currentTarget.depends do
		local dependency = currentTarget.depends[i]
		local path, filename, ext = mbfilepath.decompose(dependency)

		file:write("SOURCE_ROOT := " .. jniDir .. "\n")
		file:write("include $(SOURCE_ROOT)/../../../" .. filename .. "/" .. config.name .. "/jni/Android.mk\n")
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


	file:write("SOURCE_ROOT := " .. mbwriter.global.currentmetamakedirabs .. "\n")
	file:write("MY_LOCAL_C_INCLUDES := \\\n")
	for i = 1, #config.includedirs do
	    file:write("	$(SOURCE_ROOT)/" .. config.includedirs[i] .. " \\\n")
	end
    file:write("\n")

	-- Add project lib search paths
	file:write("MY_LIB_SEARCH_PATHS:= \\\n")

	local abi = mbutil.getkvvalue(config.options.ndkoptions, "APP_ABI")
	if abi == nil then
		-- TODO error
	end

	f = mbwriter.global.makeoutputdirabs .. "/obj/local/" .. abi
	file:write("	-L" .. f .. " \\\n")
	file:write("\n")

	--Include static libs
	for i = 1, #localStaticLibs do
		local f = localStaticLibs[i]
		local path, filename, ext = mbfilepath.decompose(f)
		filename = string.gsub(filename, "%.a", "")
		filename = mbfilepath.trimrawmarker(filename)

		file:write("include $(CLEAR_VARS)\n")
		file:write("LOCAL_MODULE            := " .. filename .. "\n")

		--file:write("LOCAL_SRC_FILES         := " .. mbwriter.getoutputrelfilepath(f) .. "\n")
		file:write("LOCAL_SRC_FILES         := " .. mbwriter.getabsfilepath(f) .. "\n")
		file:write("include $(PREBUILT_STATIC_LIBRARY)\n\n")
	end

	--Include shared libs
	for i = 1, #localSharedLibs do
		local f = localSharedLibs[i]
		local path, filename, ext = mbfilepath.decompose(f)
		filename = mbfilepath.trimrawmarker(filename)

		file:write("include $(CLEAR_VARS)\n")
		file:write("LOCAL_MODULE            := " .. filename .. "\n")
		--file:write("LOCAL_SRC_FILES         := " .. mbwriter.getoutputrelfilepath(f) .. "\n")
		file:write("LOCAL_SRC_FILES         := " .. mbwriter.getabsfilepath(f) .. "\n")
		file:write("include $(PREBUILT_SHARED_LIBRARY)\n\n")
	end

	file:write("MY_LOCAL_SRC_FILES := \\\n")
	for i = 1, #currentTarget.files do
		local f = currentTarget.files[i]
		local ext = mbfilepath.getextension(f)
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

	if currentTarget.targettype == "app" then

		file:write("LOCAL_LDLIBS :=  $(MY_LIB_SEARCH_PATHS) $(MY_LIBS) \\\n")
		for i = 1, #localLDLibs do
			local f = localLDLibs[i]
			local path, filename, ext = mbfilepath.decompose(f)
			file:write("	" .. filename .. " \\\n")
		end
		file:write("\n")

		file:write("LOCAL_SHARED_LIBRARIES := \\\n")
		for i = 1, #localSharedLibs do
			local f = localSharedLibs[i]
			local path, filename, ext = mbfilepath.decompose(f)
			file:write("	" .. filename .. " \\\n")
		end
		file:write("\n")

		file:write("LOCAL_STATIC_LIBRARIES := \\\n")
		for i = 1, #localStaticLibs do
			local f = localStaticLibs[i]
			local path, filename, ext = mbfilepath.decompose(f)
			filename = string.gsub(filename, "%.a", "")
			filename = mbfilepath.trimrawmarker(filename)

			file:write("	" .. filename .. " \\\n")
		end
		for i = 1, #currentTarget.depends do
			local f = currentTarget.depends[i]
			local path, filename, ext = mbfilepath.decompose(f)
			file:write("	" .. filename .. " \\\n")
		end
		file:write("\n")

		file:write("include $(BUILD_SHARED_LIBRARY)\n")
	elseif currentTarget.targettype == "module" or currentTarget.targettype == "staticlib" then
		file:write("include $(BUILD_STATIC_LIBRARY)\n")
	else
		--TODO - error here
	end

	file:close()
	mbwriter.reportoutputfile(makeFilename)

	NDKSetMakeOutputDir(g_makeOutputDirAbsRoot)
end

function WriteJNI(currentTarget, config)

	--links to template folder required for apps, but not libraries
	if currentTarget.targettype == "app" then
		CreateLinks(currentTarget, config)
	end

	WriteApplicationMk(currentTarget, config)
	WriteAndroidMk(currentTarget, config)
end

----------------------------------------------------------------------------------------------------------------------------------------------------------------
--MAIN
----------------------------------------------------------------------------------------------------------------------------------------------------------------

local currentTarget = mbwriter.solution.targets[1]

for i = 1, #currentTarget.configs do
	local config = currentTarget.configs[i]

	g_makeOutputDirAbsRoot					= mbwriter.global.makeoutputdirabs
	g_makeOutputDirAbsTargetConfig	= GetJNIDir(currentTarget.name, config.name)

	WriteJNI(currentTarget, config)
end
