import "writer_common.lua"

g_mainGroupID = "BE854ED918CA1112008EAFCD"

g_projectObjectID = mbwriter.xcodegenerateid()
g_infoPlistBuildFileID = mbwriter.xcodegenerateid()

g_buildPhaseFrameworkID = mbwriter.xcodegenerateid()

g_PBXBuildFileIDMap = {}
g_PBXFileRefIDMap = {}

g_PBXContainerItemProxyIDMap	= {} 
g_PBXReferenceProxyIDMap 		= {} 
g_PBXTargetDependencyIDMap		= {} 
g_PBXTargetProxy				= {} 
g_ProductGroupIDs				= {}

g_currentTarget = mbwriter.solution.targets[1]

--used to refer to the product we're building here externally
g_externalProductID = mbwriter.xcodegenerateid()
g_pbxNativeTargetID = mbwriter.xcodegenerateid()

--used for overriding filepaths, e.g. for frameworks
g_filePathMap = {}

g_lastKnownFileTypeMap = {}

g_sourceTreeMap = {}

--folder information
g_sourceFolders = {}
g_resourceFolders = {}

g_fileListType = {} -- Source, Resource or Framework

g_PBXProjectConfigIDs = {}
g_PBXNativeTargetConfigIDs = {}


function GetLastKnownFileType(filepath)
	local ext = Util_FileExtension(filepath)
	local lastKnownType = g_lastKnownFileTypeMap[ext]
	if lastKnownType == nil then
		local filetype = mbwriter.getfiletype(filepath)
		if filetype == "dir" then
			return "folder"
		end

		return "text"
	end

	return lastKnownType
end

function GetSourceTree(filepath)
	local sourceTree = g_sourceTreeMap[filepath]
	if sourceTree == nil then
		return "\"<group>\""
	end

	return sourceTree
end

function GetFullFilePath(filepath)
	return Util_GetFullFilePath(filepath, mbwriter.global.currentmetamakedirabs, mbwriter.global.makeoutputdirabs, "/", g_filePathMap)
end

function GetFileListType(filepath)
	local filelisttype = g_fileListType[filepath]
	if filelisttype == nil then
		return "Unknown"
	end

	return filelisttype
end

function InitFolder(folderList, path, filename)
	local pathComponents = { "" }
	
	if (path ~= "") then 
		pathComponents = split(path, "/")
		table.insert(pathComponents, 1, "")
	end

	local fullProjectRelativeFilePath = Util_FilePathJoin(path, filename, mbwriter.global.targetDirSep)

	local currentPath = ""
	local currentParentID = g_mainGroupID
	local currentParentPath = nil

	local nComponents = #pathComponents
	local currentFolderID = g_mainGroupID

	for i = 1, #pathComponents do
		if i == 1 then
			currentPath = pathComponents[i]
		elseif i == 2 then
			currentPath = pathComponents[i]
		else			
			currentPath = currentPath .. "/" .. pathComponents[i]
		end


		local currentFolder = folderList[currentPath]
		if currentFolder == nil then
			--we've not encountered this path before

			local newFolderID = mbwriter.xcodegenerateid()

			currentFolder = {
				shortName = pathComponents[i],
				id = newFolderID,
				parentid = currentParentID,
				childIDs = {}
			}

			-- Add as a child of our parent
			if currentParentPath ~= nil then
				parentFolder = folderList[currentParentPath]
				table.insert(parentFolder.childIDs, newFolderID)
			end

			--print("created " .. currentPath .. " parent id " .. currentParentID)

			--update our parent for the next folder
			currentParentID = newFolderID
			currentParentPath = currentPath

			folderList[currentPath] = currentFolder
		else
			local currentFolder = folderList[currentPath]
			currentParentID = currentFolder.id
			currentParentPath = currentPath
		end


	end

	--Add child file within current folder
	do
		local currentFolder = folderList[path]
		if currentFolder == nil then	
			print("Failed to lookup folder:\n")

			for k, v in pairs(folderList) do 
				print(k .. " " .. inspect(v))
			end
		end

		local fileID = g_PBXFileRefIDMap[fullProjectRelativeFilePath]
		table.insert(currentFolder.childIDs, fileID)
	end
end

function InitFolders(folderList, fileList)
	-- Lazily initialise chains of folders based upon the list of files provided
	-- For each folder store:
	--	the short name of the folder
	--	its unique id
	--	the unique id of its parent
	--	the filenames of the files within this folder

	for i = 1, #fileList do
		local f = fileList[i]
		local path, filename, ext = Util_FilePathDecompose(f)

		--remove trailing slash
		local path = string.sub(path, 1, -2)

		InitFolder(folderList, path, filename)
	end

	--print(inspect(folderList))
end

function WritePBXGroup()
	file:write("/* Begin PBXGroup section */\n")

	file:write("		" .. g_mainGroupID .. " = {\n")
	file:write("			isa = PBXGroup;\n")
	file:write("			children = (\n")
	file:write("				BE854EEF18CA1112008EAFCD /* Source */,\n")
	file:write("				BE854EE418CA1337008EAFCD /* Resources */,\n")
	file:write("				BE854EE418CA1112008EAFCD /* Frameworks */,\n")
	file:write("				BE854EE318CA1112008EAFCD /* Products */,\n")

	--dependency projects written at root level
	for i = 1, #g_currentTarget.depends do
		local dependency = g_currentTarget.depends[i]
		file:write("				" .. g_PBXFileRefIDMap[dependency] .. " /* " .. dependency .. "*/,\n")
	end	

	file:write("			);\n")
	file:write("			sourceTree = \"<group>\";\n")
	file:write("		};\n")

	file:write("		BE854EE318CA1112008EAFCD /* Products */ = {\n")
	file:write("			isa = PBXGroup;\n")
	file:write("			children = (\n")
	file:write("				" .. g_externalProductID .. " /* " .. g_currentTarget.name .. " */,\n")
	file:write("			);\n")
	file:write("			name = Products;\n")
	file:write("			sourceTree = \"<group>\";\n")
	file:write("		};\n")

	--dependencies (hidden group)
	for i = 1, #g_currentTarget.depends do
		local dependency = g_currentTarget.depends[i]
		file:write("		" .. g_ProductGroupIDs[dependency] .." /* Products */ = {\n")
		file:write("			isa = PBXGroup;\n")
		file:write("			children = (\n")
		file:write("				" .. g_PBXReferenceProxyIDMap[dependency] .. " /* " .. dependency .. " */,\n")
		file:write("			);\n")
		file:write("			name = Products;\n")
		file:write("			sourceTree = \"<group>\";\n")
		file:write("		};\n")	
	end

	--frameworks
	file:write("		BE854EE418CA1112008EAFCD /* Frameworks */ = {\n")
	file:write("			isa = PBXGroup;\n")
	file:write("			children = (\n")
	for i = 1, #g_currentTarget.frameworks do
		local f = g_currentTarget.frameworks[i]
		file:write("				" .. g_PBXFileRefIDMap[f] 	.. " /* " .. g_currentTarget.frameworks[i] .. " */,\n")
	end	
	file:write("			);\n")
	file:write("			name = Frameworks;\n")
	file:write("			sourceTree = \"<group>\";\n")
	file:write("		};\n")

	for k, v in pairs(g_sourceFolders) do 
		local folder = v

		--special handling for root folder
		if folder.shortName == "" then
			file:write("		BE854EEF18CA1112008EAFCD /* Source */ = {\n")
			file:write("			isa = PBXGroup;\n")
			file:write("			children = (\n")
			for i = 1, #folder.childIDs do
				local f = folder.childIDs[i]
				file:write("				" .. f .. ",\n")
			end
			file:write("			);\n")
			file:write("			name = \"Source\";\n")
			file:write("			sourceTree = \"<group>\";\n")
			file:write("		};	\n")			
		else
			file:write("		" .. folder.id .. " /* " .. folder.shortName .. " */ = {\n")
			file:write("			isa = PBXGroup;\n")
			file:write("			children = (\n")
			for i = 1, #folder.childIDs do
				local f = folder.childIDs[i]
				file:write("				" .. f .. ",\n")
			end
			file:write("			);\n")		
			file:write("			name = \"" .. folder.shortName .. "\";\n")
			file:write("			sourceTree = \"<group>\";\n")
			file:write("		};\n")
		end
	end

	for k, v in pairs(g_resourceFolders) do 
		local folder = v

		--special handling for root folder
		if folder.shortName == "" then		
			file:write("		BE854EE418CA1337008EAFCD /* Resources */ = {\n")
			file:write("			isa = PBXGroup;\n")
			file:write("			children = (\n")
	
			for i = 1, #folder.childIDs do
				local f = folder.childIDs[i]
				file:write("				" .. f .. ",\n")
			end
			file:write("			);\n")
			file:write("			name = Resources;\n")
			file:write("			sourceTree = \"<group>\";\n")
			file:write("		};\n")
		else
			file:write("		" .. folder.id .. " /* " .. folder.shortName .. " */ = {\n")
			file:write("			isa = PBXGroup;\n")
			file:write("			children = (\n")
			for i = 1, #folder.childIDs do
				local f = folder.childIDs[i]
				file:write("				" .. f .. ",\n")
			end
			file:write("			);\n")		
			file:write("			name = \"" .. folder.shortName .. "\";\n")
			file:write("			sourceTree = \"<group>\";\n")
			file:write("		};\n")
		end
	end	

	file:write("/* End PBXGroup section */\n")
end

--[[ PRE-PROCESSING ]] --------------------------------------------------------------------------------

-- Create file type map
	g_lastKnownFileTypeMap["m"]			= "sourcecode.c.objc"
	g_lastKnownFileTypeMap["mm"]		= "sourcecode.cpp.objcpp"
	g_lastKnownFileTypeMap["c"]			= "sourcecode.c.c"
	g_lastKnownFileTypeMap["cpp"]		= "sourcecode.cpp.cpp"
	g_lastKnownFileTypeMap["h"]			= "sourcecode.c.h"
	g_lastKnownFileTypeMap["framework"]	= "wrapper.framework"
	g_lastKnownFileTypeMap["app"]		= "wrapper.application"
	g_lastKnownFileTypeMap["storyboard"]	= "file.storyboard"
	g_lastKnownFileTypeMap["vsh"]		= "sourcecode.glsl"
	g_lastKnownFileTypeMap["psh"]		= "sourcecode.glsl"
	g_lastKnownFileTypeMap["fsh"]		= "sourcecode.glsl"
	g_lastKnownFileTypeMap["glsl"]		= "sourcecode.glsl"
	g_lastKnownFileTypeMap["xcassets"]	= "folder.assetcatalog"
	g_lastKnownFileTypeMap["xib"]		= "file.xib"
	g_lastKnownFileTypeMap["png"]		= "image.png"
	g_lastKnownFileTypeMap["zip"]		= "archive.zip"
	g_lastKnownFileTypeMap["xml"]		= "text.xml"
	g_lastKnownFileTypeMap["plist"]		= "text.plist.xml"
	g_lastKnownFileTypeMap["a"]			= "archive.ar"
	g_lastKnownFileTypeMap["xcodeproj"]	= "wrapper.pb-project"

	--'folder' is also a valid last known type

function BuildListPerSDK(stringList)
	local stringListsPerSDK = {}
	local sdkNames = {}

	for i = 1, #stringList do
		local str = stringList[i]

		local sdk
		local stringWithoutSDK

		local sdkStartIndex = string.find(str, "%[sdk=")
		if sdkStartIndex then
			sdkEndIndex = string.find(str, "]")
			sdk = string.sub(str, sdkStartIndex, sdkEndIndex)
			stringWithoutSDK = string.sub(str, 1, sdkStartIndex-1)
			--print("Found SDK specific string " .. stringWithoutSDK .. " SDK " .. sdk)
		else
			sdk = ""
			stringWithoutSDK = str
		end

		--Find table to insert data for this SDK into
		local stringListForSDK = stringListsPerSDK[sdk]
		if stringListForSDK == nil then
			--Create new destination table if none exists
			stringListForSDK = {}
			stringListsPerSDK[sdk] = stringListForSDK
			table.insert(sdkNames, sdk)
		end
		table.insert(stringListForSDK, stringWithoutSDK)
	end

	--Duplicate common strings per SDK
	local commonStrings = stringListsPerSDK[""]
--	print(inspect(commonStrings))
	if commonStrings ~= nil then 
		for i = 1, #sdkNames do
			local sdk = sdkNames[i]
			local sdkStringList = stringListsPerSDK[sdk]

			if sdk ~= "" then
--				print(sdk)
				for j = 1, #commonStrings do
					local commonString = commonStrings[j]
					local sdkCommonString = commonString
					table.insert(sdkStringList, sdkCommonString)
				end
			end
		end
	end

--	print(inspect(stringListsPerSDK))
	return stringListsPerSDK, sdkNames
end


function WriteXCBuildConfigurations()
	file:write("/* Begin XCBuildConfiguration section */\n")

	for i = 1, #g_currentTarget.configs do
		--print(inspect(g_currentTarget))
		local config = g_currentTarget.configs[i]
		local configID = g_PBXProjectConfigIDs[config.name]

		file:write("		" .. configID .. " /* " .. config.name .. " */ = {\n")
		file:write("			isa = XCBuildConfiguration;\n")
		file:write("			buildSettings = {\n")

		-- Add custom compiler options
		if config.options.compiler ~= nil then 
			for j = 1, #config.options.compiler do
				local compilerOption = config.options.compiler[j]
				file:write("				" .. compilerOption .. ";\n")
			end
		end

		-- App specific compiler options
		if g_currentTarget.targettype == "app" then
			if config.options.compiler_app ~= nil then 
				for j = 1, #config.options.compiler_app do
					local compilerOption = config.options.compiler_app[j]
					file:write("				" .. compilerOption .. ";\n")
				end
			end
		-- Static lib specific compiler options
		elseif g_currentTarget.targettype == "module" or g_currentTarget.targettype == "staticlib" then 
			if config.options.compiler_staticlib ~= nil then 
				for j = 1, #config.options.compiler_staticlib do
					local compilerOption = config.options.compiler_staticlib[j]
					file:write("				" .. compilerOption .. ";\n")
				end
			end
		end

		-- Add defines
		file:write("				GCC_PREPROCESSOR_DEFINITIONS = (\n")
		for j = 1, #config.defines do
			tmp = string.gsub(config.defines[j], "\"", "\\\\\\\"")
			file:write("					\"" .. tmp .. "\",\n")
		end
		file:write("					\"$(inherited)\",\n")
		file:write("				);\n")

		file:write("				HEADER_SEARCH_PATHS = (\n")
		file:write("					\"$(inherited)\",\n")
		file:write("					\"/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include\",\n")
		for j = 1, #config.includedirs do
			file:write("					\"" .. GetFullFilePath(config.includedirs[j]) .. "\",\n")
		end
		file:write("				);\n")


		file:write("				FRAMEWORK_SEARCH_PATHS = (\n")
		file:write("					\"$(inherited)\",\n")

		for i = 1, #g_currentTarget.frameworks do
			local f = g_currentTarget.frameworks[i]
	
			local dirCharIndex = string.find(f, "/")
			if dirCharIndex ~= nil then
				local frameworkPath = Util_FilePath(f)
				file:write("					\"" .. GetFullFilePath(frameworkPath) .. "\",\n")
			end
		end
		file:write("				);\n\n")

		local stringListsPerSDK, sdkNames = BuildListPerSDK(config.libdirs)
		for iSDK = 1, #sdkNames do
			local sdk =  sdkNames[iSDK]
			local strings = stringListsPerSDK[sdk]
			if sdk == "" then 
				file:write("				LIBRARY_SEARCH_PATHS = (\n")
			else
				file:write("				\"LIBRARY_SEARCH_PATHS" .. sdk .. "\" = (\n")
			end
			for jString = 1, #strings do
				file:write("					\"" .. GetFullFilePath(strings[jString]) .. "\",\n")
			end
			file:write("				);\n\n")
		end

		file:write("			};\n")
		file:write("			name = " .. config.name .. ";\n")
		file:write("		};\n")
	end

	for i = 1, #g_currentTarget.configs do
		local config = g_currentTarget.configs[i]	
		local configID = g_PBXNativeTargetConfigIDs[config.name]

		file:write("		" .. configID .. " /* " .. config.name .. " */ = {\n")
		file:write("			isa = XCBuildConfiguration;\n")
		file:write("			buildSettings = {\n")
		file:write("				ASSETCATALOG_COMPILER_APPICON_NAME = AppIcon;\n")
		file:write("				ASSETCATALOG_COMPILER_LAUNCHIMAGE_NAME = LaunchImage;\n")

		-- Prefix PCH support
--		if g_currentTarget.pch ~= nil and g_currentTarget.pch ~= "" then
--			print(g_currentTarget.name .. " PCH " .. g_currentTarget.pch)
--			file:write("				GCC_PRECOMPILE_PREFIX_HEADER = YES;\n")
--			file:write("				GCC_PREFIX_HEADER = \"" .. GetFullFilePath(g_currentTarget.pch) .. "\";\n")
--		else
			file:write("				GCC_PRECOMPILE_PREFIX_HEADER = NO;\n")
--		end

		do
			local infoplist = Util_GetKVValue(config.options._xcode, "infoplist")
			if infoplist ~= nil then 
				file:write("				INFOPLIST_FILE = \"" .. GetFullFilePath(infoplist) .. "\";\n")
			end
		end

		local stringListsPerSDK, sdkNames = BuildListPerSDK(config.libs)
		for iSDK = 1, #sdkNames do
			local sdk =  sdkNames[iSDK]
			local strings = stringListsPerSDK[sdk]
			if sdk == "" then 
				file:write("				OTHER_LDFLAGS = (\n")
			else
				file:write("				\"OTHER_LDFLAGS" .. sdk .. "\" = (\n")
			end
			for jString = 1, #strings do
				file:write("					\"" .. strings[jString] .. "\",\n")
			end
			file:write("				);\n\n")
		end

		file:write("				PRODUCT_NAME = \"$(TARGET_NAME)\";\n")
		if g_currentTarget.targettype == "app" then
			if g_currentTarget.targetsubsystem == "console" then 
			--	print("console")
			else
			--	print("app")
				file:write("				WRAPPER_EXTENSION = app;\n")
			end
		elseif g_currentTarget.targettype == "module" or g_currentTarget.targettype == "staticlib" then
			file:write("				SKIP_INSTALL = YES;\n")
		end
		file:write("			};\n")
		file:write("			name = " .. config.name .. ";\n")
		file:write("		};\n")		
	end

	file:write("/* End XCBuildConfiguration section */\n\n")
end

function WriteXCConfigurationList()
	file:write("/* Begin XCConfigurationList section */\n")

	file:write("		BE854EDD18CA1112008EAFCD /* Build configuration list for PBXProject " .. g_currentTarget.name .. " */ = {\n")
	file:write("			isa = XCConfigurationList;\n")
	file:write("			buildConfigurations = (\n")	
	for i = 1, #g_currentTarget.configs do
		local config = g_currentTarget.configs[i]	
		local configID = g_PBXProjectConfigIDs[config.name]
		file:write("				" .. configID .. " /* " .. config.name .. " */,\n")
	end
	file:write("			);\n")
	file:write("			defaultConfigurationIsVisible = 0;\n")
	file:write("			defaultConfigurationName = Release;\n")
	file:write("		};\n")
	file:write("		BE854F1F18CA1112008EAFCD /* Build configuration list for PBXNativeTarget " .. g_currentTarget.name .." */ = {\n")
	file:write("			isa = XCConfigurationList;\n")
	file:write("			buildConfigurations = (\n")
	for i = 1, #g_currentTarget.configs do
		local config = g_currentTarget.configs[i]	
		local configID = g_PBXNativeTargetConfigIDs[config.name]
		file:write("				" .. configID .. " /* " .. config.name .. " */,\n")
	end
	file:write("			);\n")
	file:write("			defaultConfigurationIsVisible = 0;\n")
	file:write("		};\n")
	file:write("/* End XCConfigurationList section */\n\n")
end

--[[Assign PBX files IDs ]] --------------------------------------------------------------------------------

-- Create IDs for frameworks
for i = 1, #g_currentTarget.frameworks do
	local f = g_currentTarget.frameworks[i]
	
	g_PBXBuildFileIDMap[f]		= mbwriter.xcodegenerateid()

	local dirCharIndex = string.find(f, "/")
	if dirCharIndex == nil then
		-- TODO, support non system frameworks
		g_filePathMap[f] = "System/Library/Frameworks/" .. f
		g_sourceTreeMap[f] = "SDKROOT"
	end
end

-- CREATE IDs for static libs
for i = 1, #g_currentTarget.depends do
	local f = g_currentTarget.depends[i]	
	g_PBXBuildFileIDMap[f]		= mbwriter.xcodegenerateid()
end

-- Create IDs for resources
for i = 1, #g_currentTarget.resources do
	local f = g_currentTarget.resources[i]	
	g_PBXBuildFileIDMap[f]		= mbwriter.xcodegenerateid()
end

-- Create REF IDs for ALL files
for i = 1, #g_currentTarget.allfiles do
	local f = g_currentTarget.allfiles[i]	
	g_PBXFileRefIDMap[f]		= mbwriter.xcodegenerateid()
end

-- Create BUILD IDs for files we're going to build
for i = 1, #g_currentTarget.files do
	local f = g_currentTarget.files[i]	
	g_PBXBuildFileIDMap[f]		= mbwriter.xcodegenerateid()
end

g_currentTargetFileExtension = ""
if g_currentTarget.targettype == "app" then
	if g_currentTarget.targetsubsystem == "console" then
	else
		g_currentTargetFileExtension = ".app"
	end
else
	g_currentTargetFileExtension = ".a"
end

g_currentTargetFilenameWithExt = g_currentTarget.name .. g_currentTargetFileExtension

g_PBXFileRefIDMap[g_currentTargetFilenameWithExt] = mbwriter.xcodegenerateid()

-- Create file list type lookup table (Source, Resource or Framework)
for i = 1, #g_currentTarget.files do
	local f = g_currentTarget.files[i]
	g_fileListType[f] = "Source"
end

for i = 1, #g_currentTarget.resources do
	local f = g_currentTarget.resources[i]
	g_fileListType[f] = "Resource"
end

for i = 1, #g_currentTarget.frameworks do
	local f = g_currentTarget.frameworks[i]
	g_fileListType[f] = "Framework"
end

-- Assign IDs for configs
for i = 1, #g_currentTarget.configs do
	local config = g_currentTarget.configs[i]
	g_PBXProjectConfigIDs[config.name] = mbwriter.xcodegenerateid()
	g_PBXNativeTargetConfigIDs[config.name] = mbwriter.xcodegenerateid()
end

-- Assign IDs for dependencies
for i = 1, #g_currentTarget.depends do
	local dependency = g_currentTarget.depends[i]
	
	local path, filename, ext = Util_FilePathDecompose(dependency)

	--store relative filepath
	g_PBXFileRefIDMap[dependency]				= mbwriter.xcodegenerateid()
	g_PBXBuildFileIDMap[dependency]				= mbwriter.xcodegenerateid()
	g_PBXReferenceProxyIDMap[dependency] 		= mbwriter.xcodegenerateid()
	g_PBXTargetDependencyIDMap[dependency]		= mbwriter.xcodegenerateid()
	g_PBXContainerItemProxyIDMap[dependency]	= mbwriter.xcodegenerateid()
	g_PBXReferenceProxyIDMap[dependency]		= mbwriter.xcodegenerateid()
	g_PBXTargetProxy[dependency]				= mbwriter.xcodegenerateid()
	g_ProductGroupIDs[dependency]				= mbwriter.xcodegenerateid()

	--Xcode projects written adjacent to each other
	local dependencyPhysicalFilePath = filename .. ".xcodeproj"

	g_filePathMap[dependency] = dependencyPhysicalFilePath
	g_filePathMap[dependencyPhysicalFilePath] = dependencyPhysicalFilePath
end

--[[ CREATE FOLDER TREE ]] --------------------------------------------------------------------------------

	-- Init Source folders
	do
		local sourceFileList = {}
		for i = 1, #g_currentTarget.allfiles do
			local f = g_currentTarget.allfiles[i]
			if GetFileListType(f) == "Source" or GetFileListType(f) == "Unknown" then
				table.insert(sourceFileList, f)
			end
		end
		InitFolders(g_sourceFolders, sourceFileList)
	end

	do
		-- Init Resource folders
		local resourceFileList = {}
		for i = 1, #g_currentTarget.resources do
			local f = g_currentTarget.resources[i]
			if GetFileListType(f) == "Resource" then
				table.insert(resourceFileList, f)
			end
		end
		InitFolders(g_resourceFolders, resourceFileList)
	end

--[[ FILE WRITING ]] --------------------------------------------------------------------------------

g_projectoutputfile = mbwriter.global.makeoutputdirabs .. "/" .. mbwriter.solution.name .. ".xcodeproj"

--print("Creating project dir " .. g_projectoutputfile)
mbwriter.mkdir(g_projectoutputfile)

local pbxprojFilename = g_projectoutputfile .. "/project.pbxproj"

--print("Writing pbxproj " .. pbxprojFilename)
file = io.open(pbxprojFilename, "w")
if file == nil then
	--TODO - error
end

file:write(
"// !$*UTF8*$!					\
{						\
	archiveVersion = 1;			\
	classes = {				\
	};					\
	objectVersion = 46;			\
	objects = {				\
						\
")

file:write("/* Begin PBXBuildFile section */\n")

for i = 1, #g_currentTarget.frameworks do
	local f = g_currentTarget.frameworks[i]
	file:write("		" .. g_PBXBuildFileIDMap[f] 	.. " /* " .. g_currentTarget.frameworks[i] .. " */ = {isa = PBXBuildFile; fileRef = " .. g_PBXFileRefIDMap[f] .. "; };\n")
end

for i = 1, #g_currentTarget.resources do
	local f = g_currentTarget.resources[i]
	file:write("		" .. g_PBXBuildFileIDMap[f] 	.. " /* " .. g_currentTarget.resources[i] .. " */ = {isa = PBXBuildFile; fileRef = " .. g_PBXFileRefIDMap[f] .. "; };\n")
end

for i = 1, #g_currentTarget.files do
	local f = g_currentTarget.files[i]
	file:write("		" .. g_PBXBuildFileIDMap[f] 	.. " /* " .. g_currentTarget.files[i] .. " */ = {isa = PBXBuildFile; fileRef = " .. g_PBXFileRefIDMap[f] .. "; };\n")
end

-- Link with required projects
for i = 1, #g_currentTarget.depends do
	local dependency = g_currentTarget.depends[i]
	local path, filename, ext = Util_FilePathDecompose(dependency)
	local f = filename .. ".a"
	file:write("		" .. g_PBXBuildFileIDMap[dependency] .. " /* " .. f .. " */ = {isa = PBXBuildFile; fileRef = " .. g_PBXReferenceProxyIDMap[dependency] .. "; };\n")
end

file:write("/* End PBXBuildFile section */\n\n")

file:write("/* Begin PBXContainerItemProxy section */\n")

for i = 1, #g_currentTarget.depends do
	local dependency = g_currentTarget.depends[i]
	local path, filename, ext = Util_FilePathDecompose(dependency)
	local f = filename .. ".xcodeproj"

	file:write("		" .. g_PBXContainerItemProxyIDMap[dependency] .. " /* PBXContainerItemProxy */ = {\n")
	file:write("			isa = PBXContainerItemProxy;\n")
	file:write("			containerPortal = " .. g_PBXFileRefIDMap[dependency]  .. " /* " .. f .. " */;\n")
	file:write("			proxyType = 2;\n")
	file:write("			remoteGlobalIDString = " .. mbwriter.xcodegetpbxfilereference_external(filename) .. ";\n")
	file:write("			remoteInfo = " .. filename .. ";\n")
	file:write("		};\n")
	file:write("		" .. g_PBXTargetProxy[dependency] .. " /* PBXContainerItemProxy */ = {\n")
	file:write("			isa = PBXContainerItemProxy;\n")
	file:write("			containerPortal = " .. g_PBXFileRefIDMap[dependency]  .. " /* " .. f .. " */;\n")
	file:write("			proxyType = 1;\n")
	file:write("			remoteGlobalIDString = " .. mbwriter.xcodegetpbxnativetarget_external(filename) .. ";\n")
	file:write("			remoteInfo = " .. filename .. ";\n")
	file:write("		};\n")
end

file:write("/* End PBXContainerItemProxy section */\n\n")

--PBXFileReference
file:write("/* Begin PBXFileReference section */\n")

g_productType = nil
if g_currentTarget.targettype == "app" then
	if g_currentTarget.targetsubsystem == "console" then
		g_productType = "wrapper.tool"
	else
		g_productType = "wrapper.application"
	end
elseif g_currentTarget.targettype == "module" or g_currentTarget.targettype == "staticlib" then
	g_productType = "archive.ar"
end

mbwriter.xcoderegisterpbxfilereference_external(g_currentTarget.name, g_externalProductID)
file:write("		" .. g_externalProductID .. " /* " .. g_currentTargetFilenameWithExt .. " */ = {isa = PBXFileReference; explicitFileType = " .. g_productType .. "; includeInIndex = 0; path = \"" .. g_currentTargetFilenameWithExt .. "\"; sourceTree = BUILT_PRODUCTS_DIR; };\n")

--All files, regardless of whether they're for this platform or not.
for i = 1, #g_currentTarget.allfiles do
	local f = g_currentTarget.allfiles[i]
	file:write("		" .. g_PBXFileRefIDMap[f] 	.. " /* " .. f .. " */ = {isa = PBXFileReference; fileEncoding = 4; lastKnownFileType = " .. GetLastKnownFileType(f) .. "; name = \"" .. Util_FileShortname(f) .. "\"; path = \"" .. GetFullFilePath(f) .. "\"; sourceTree = " .. GetSourceTree(f) .. "; };\n")
end

for i = 1, #g_currentTarget.depends do
	local dependency = g_currentTarget.depends[i]
	local dependencyXcodeproj = dependency .. ".xcodeproj"

	file:write("		" .. g_PBXFileRefIDMap[dependency] 	.. " /* " .. dependency .. " */ = {isa = PBXFileReference; fileEncoding = 4; lastKnownFileType = " .. GetLastKnownFileType(dependencyXcodeproj) .. "; name = \"" .. Util_FileShortname(dependency) .. ".xcodeproj\"; path = \"" .. GetFullFilePath(dependency) .. "\"; sourceTree = " .. GetSourceTree(dependency) .. "; };\n")
end

file:write("/* End PBXFileReference section */\n\n")

--The phase responsible on linking with frameworks. Known as 'Link Binary With Libraries' in the UI.
file:write("/* Begin PBXFrameworksBuildPhase section */\n")
file:write("		BE854EDF18CA1112008EAFCD /* Frameworks */ = {\n")
file:write("			isa = PBXFrameworksBuildPhase;\n")
file:write("			buildActionMask = 2147483647;\n")
file:write("			files = (\n")

for i = 1, #g_currentTarget.depends do
	local f = g_currentTarget.depends[i]
	file:write("				" ..  g_PBXBuildFileIDMap[f] .. " /* " .. f .. " in Frameworks */,\n")
end

for i = 1, #g_currentTarget.frameworks do
	local f = g_currentTarget.frameworks[i]
	file:write("				" ..  g_PBXBuildFileIDMap[f] .. " /* " .. f .. " in Frameworks */,\n")
end
file:write("			);\n")
file:write("			runOnlyForDeploymentPostprocessing = 0;\n")
file:write("		};\n")
file:write("/* End PBXFrameworksBuildPhase section */\n\n")

WritePBXGroup()

file:write("/* Begin PBXNativeTarget section */\n")

mbwriter.xcoderegisterpbxnativetarget_external(g_currentTarget.name, g_pbxNativeTargetID)
file:write("		" .. g_pbxNativeTargetID .. " /* " .. g_currentTarget.name .. " */ = {\n")
file:write("			isa = PBXNativeTarget;\n")
file:write("			buildConfigurationList = BE854F1F18CA1112008EAFCD /* Build configuration list for PBXNativeTarget \"" .. g_currentTarget.name .. "\" */;\n")
file:write("			buildPhases = (\n")
file:write("				BE854EDE18CA1112008EAFCD /* Sources */,\n")
file:write("				BE854EDF18CA1112008EAFCD /* Frameworks */,\n")
file:write("				BE854EE018CA1112008EAFCD /* Resources */,\n")
file:write("			);\n")
file:write("			buildRules = (\n")
file:write("			);\n")
file:write("			dependencies = (\n")

for i = 1, #g_currentTarget.depends do
	local dependency = g_currentTarget.depends[i]
	file:write("				" .. g_PBXTargetDependencyIDMap[dependency] .. " /* " .. dependency .. " */,\n")
end

file:write("			);\n")
file:write("			name = " .. g_currentTarget.name .. ";\n")
file:write("			productName = " .. g_currentTarget.name .. ";\n")
file:write("			productReference = " .. g_externalProductID .. " /* " .. g_currentTarget.name .. " */;\n")

if g_currentTarget.targettype == "app" then
	if g_currentTarget.targetsubsystem == "console" then
		file:write("			productType = \"com.apple.product-type.tool\";\n")
	else	
		file:write("			productType = \"com.apple.product-type.application\";\n")
	end
elseif g_currentTarget.targettype == "module" or g_currentTarget.targettype == "staticlib" then
	file:write("			productType = \"com.apple.product-type.library.static\";\n")
end
file:write("		};\n")
file:write("/* End PBXNativeTarget section */\n")

file:write("/* Begin PBXProject section */\n")
file:write("		" .. g_projectObjectID .. " /* Project object */ = {\n")
file:write("			isa = PBXProject;\n")
file:write("			attributes = {\n")
file:write("				LastUpgradeCheck = 0500;\n")
file:write("				ORGANIZATIONNAME = metabuild;\n")
file:write("			};\n")
file:write("			buildConfigurationList = BE854EDD18CA1112008EAFCD /* Build configuration list for PBXProject \"" .. g_currentTarget.name .. "\" */;\n")
file:write("			compatibilityVersion = \"Xcode 3.2\";\n")
file:write("			developmentRegion = English;\n")
file:write("			hasScannedForEncodings = 0;\n")
file:write("			knownRegions = (\n")
file:write("				en,\n")
file:write("				Base,\n")
file:write("			);\n")
file:write("			mainGroup = " .. g_mainGroupID .. ";\n")
file:write("			productRefGroup = BE854EE318CA1112008EAFCD /* Products */;\n")
file:write("			projectDirPath = \"\";\n")
file:write("			projectRoot = \"\";\n")
file:write("			projectReferences = (\n")
for i = 1, #g_currentTarget.depends do
	local dependency = g_currentTarget.depends[i]
	file:write("				{\n")
	file:write("					ProductGroup = " .. g_ProductGroupIDs[dependency] .. " /* Products */;\n")
	file:write("					ProjectRef = " .. g_PBXFileRefIDMap[dependency] .. " /* " .. dependency .. " */;\n")
	file:write("				},\n")
end
file:write("			);\n")
file:write("			targets = (\n")
file:write("				" .. g_pbxNativeTargetID .. " /* " .. g_currentTarget.name .. " */,\n")
file:write("			);\n")
file:write("		};\n")
file:write("/* End PBXProject section */\n\n")

file:write("/* Begin PBXReferenceProxy section */\n")

for i = 1, #g_currentTarget.depends do
	local dependency = g_currentTarget.depends[i]
	local path, filename, ext = Util_FilePathDecompose(dependency)

	file:write("		" .. g_PBXReferenceProxyIDMap[dependency] .. " /* " .. dependency .. ".a */ = {\n")
	file:write("			isa = PBXReferenceProxy;\n")
	file:write("			fileType = archive.ar;\n")
--	file:write("			name = " .. filename .. ".a;\n")
	file:write("			path = lib" .. filename .. ".a;\n")
	file:write("			remoteRef = " .. g_PBXContainerItemProxyIDMap[dependency] .. " /* PBXContainerItemProxy */;\n")
	file:write("			sourceTree = BUILT_PRODUCTS_DIR;\n")
	file:write("		};\n")
end

file:write("/* End PBXReferenceProxy section */\n")

file:write("/* Begin PBXResourcesBuildPhase section */\n")
file:write("		BE854EE018CA1112008EAFCD /* Resources */ = {\n")
file:write("			isa = PBXResourcesBuildPhase;\n")
file:write("			buildActionMask = 2147483647;\n")
file:write("			files = (\n")
for i = 1, #g_currentTarget.resources do
	local f = g_currentTarget.resources[i]
	file:write("				" ..  g_PBXBuildFileIDMap[f] .. " /* " .. f .. " in Resources */,\n")
end
--file:write("				BE854F0918CA1112008EAFCD /* Images.xcassets in Resources */,\n")
file:write("			);\n")
file:write("			runOnlyForDeploymentPostprocessing = 0;\n")
file:write("		};\n")
file:write("/* End PBXResourcesBuildPhase section */\n")


file:write("/* Begin PBXSourcesBuildPhase section */\n")
file:write("		BE854EDE18CA1112008EAFCD /* Sources */ = {\n")
file:write("			isa = PBXSourcesBuildPhase;\n")
file:write("			buildActionMask = 2147483647;\n")
file:write("			files = (\n")
for i = 1, #g_currentTarget.files do
	local f = g_currentTarget.files[i]
	local ext = Util_FileExtension(f)
	
	if ext == "m" or ext == "mm" or ext == "c" or ext == "cpp" then
		file:write("				" .. g_PBXBuildFileIDMap[f] 	.. " /* " .. g_currentTarget.files[i] .. " */,\n")
	end
end
file:write("			);\n")
file:write("			runOnlyForDeploymentPostprocessing = 0;\n")
file:write("		};\n")
file:write("/* End PBXSourcesBuildPhase section */\n")

file:write("/* Begin PBXTargetDependency section */\n")

for i = 1, #g_currentTarget.depends do
	local dependency = g_currentTarget.depends[i]
	local shortname = Util_FileShortname(dependency)

	file:write("		" .. g_PBXTargetDependencyIDMap[dependency] .. " /* PBXTargetDependency */ = {\n")
	file:write("			isa = PBXTargetDependency;\n")
	file:write("			name = " .. shortname .. ";\n")
	file:write("			targetProxy = " ..g_PBXTargetProxy[dependency] .. " /* PBXContainerItemProxy */;\n")
	file:write("		};\n")
end

file:write("/* End PBXTargetDependency section */\n")

--PBXVariantGroup is used by Xcode to represent localizations.
--[kenm] TODO - replace hard-coding with final version
file:write("/* Begin PBXVariantGroup section */\n")
file:write("		BE854EF218CA1112008EAFCD /* InfoPlist.strings */ = {\n")
file:write("			isa = PBXVariantGroup;\n")
file:write("			children = (\n")
file:write("				BE854EF318CA1112008EAFCD /* en */,\n")
file:write("			);\n")
file:write("			name = InfoPlist.strings;\n")
file:write("			sourceTree = \"<group>\";\n")
file:write("		};\n")
file:write("		BE854EFB18CA1112008EAFCD /* Main_iPhone.storyboard */ = {\n")
file:write("			isa = PBXVariantGroup;\n")
file:write("			children = (\n")
file:write("				BE854EFC18CA1112008EAFCD /* Base */,\n")
file:write("			);\n")
file:write("			name = Main_iPhone.storyboard;\n")
file:write("			sourceTree = \"<group>\";\n")
file:write("		};\n")
file:write("		BE854EFE18CA1112008EAFCD /* Main_iPad.storyboard */ = {\n")
file:write("			isa = PBXVariantGroup;\n")
file:write("			children = (\n")
file:write("				BE854EFF18CA1112008EAFCD /* Base */,\n")
file:write("			);\n")
file:write("			name = Main_iPad.storyboard;\n")
file:write("			sourceTree = \"<group>\";\n")
file:write("		};\n")
file:write("		BE854F1818CA1112008EAFCD /* InfoPlist.strings */ = {\n")
file:write("			isa = PBXVariantGroup;\n")
file:write("			children = (\n")
file:write("				BE854F1918CA1112008EAFCD /* en */,\n")
file:write("			);\n")
file:write("			name = InfoPlist.strings;\n")
file:write("			sourceTree = \"<group>\";\n")
file:write("		};\n")
file:write("/* End PBXVariantGroup section */\n\n")

WriteXCBuildConfigurations()
WriteXCConfigurationList()

file:write("	};\n")
file:write("	rootObject = " .. g_projectObjectID .. " /* Project object */;\n")
file:write("}\n")

file:close()
mbwriter.reportoutputfile(pbxprojFilename)
