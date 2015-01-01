--Note there's a bug in MSVC 2010 in which property pages stop working if absolute paths are used when including files
--https://connect.microsoft.com/VisualStudio/feedback/details/635294/using-absolute-path-in-clcompile-item-prevents-property-pages-from-showing
logprofile("STARTUP")

import "writer_common.lua"

g_currentTarget = mbwriter_solution.targets[1]

g_enableHLSL = false

--[[ PRE-PROCESSING ]] --------------------------------------------------------------------------------

-- Create file type map
g_fileTypeMap = {}
g_fileTypeMap["c"]			= "ClCompile"
g_fileTypeMap["cpp"]		= "ClCompile"
g_fileTypeMap["h"]			= "ClInclude"
g_fileTypeMap["hpp"]		= "ClInclude"
g_fileTypeMap["inl"]		= "ClInclude"
g_fileTypeMap["rc"]			= "ResourceCompile"

function GetFileType(filepath)
	local ext = Util_FileExtension(filepath)
	local lastKnownType = g_fileTypeMap[ext]
	if lastKnownType == nil then
		return "None"
	end

	return lastKnownType
end


function InitFolder(folderList, path_, filename)
	local path = Util_FileTrimTrailingSlash(path_)

	local pathComponents = { "" }
	
	if (path ~= "") then 
		pathComponents = split(path, "\\")
		table.insert(pathComponents, 1, "")
	end

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
			currentPath = currentPath .. "\\" .. pathComponents[i]
		end


		local currentFolder = folderList[currentPath]
		if currentFolder == nil then
			--we've not encountered this path before
			
			if #pathComponents[i] > 0 and pathComponents[i] ~= ".." then
			
				local newFolderID = mbwriter_msvcgenerateid()

				currentFolder = {
					fullName = mbwriter_getoutputrelfilepath(currentPath),
					relativePath = currentPath,
					shortName = pathComponents[i],
					id = newFolderID,
					parentid = currentParentID,
					childIDs = {}
				}

				-- Add as a child of our parent
				if currentParentPath ~= nil then
					parentFolder = folderList[currentParentPath]
					parentFolder.childIDs[#parentFolder.childIDs] = newFolderID
				end

				--update our parent for the next folder
				currentParentID = newFolderID
				currentParentPath = currentPath

				folderList[currentPath] = currentFolder
				local formattedPath = FormatFilterPath(currentFolder.relativePath)
			end
		else
			local currentFolder = folderList[currentPath]
			currentParentID = currentFolder.id
			currentParentPath = currentPath
		end
	end
end

function InitFolders(folderList, groupMap)
	logprofile("InitFolders")
	-- Lazily initialise chains of folders based upon the list of files provided
	-- For each folder store:
	--	the short name of the folder
	--	its unique id
	--	the unique id of its parent
	
	for groupName, group in pairs(groupMap) do 
		for i = 1, #group.fileInfo do
			InitFolder(folderList, group.fileInfo[i].inputRelativeDir, group.fileInfo[i].outputRelativeFilename)
		end
	end

	logprofile("END InitFolders")
end

--[[ FILE WRITING ]] --------------------------------------------------------------------------------

function BuildFileGroups(currentTarget)
	logprofile("BuildFileGroups " .. currentTarget.name)

	local fileIncludedInBuild = {}

	for i = 1, #currentTarget.files do
		local f = currentTarget.files[i]
		fileIncludedInBuild[f] = true
	end
	
	local groupMap = {}

	for i = 1, #currentTarget.allsourcefiles do
		local f = currentTarget.allsourcefiles[i]
		local fIncludedInBuild = fileIncludedInBuild[f]
		if fIncludedInBuild == nil then
			fIncludedInBuild = false
		end

		fileType = GetFileType(f)
 
		local group = groupMap[fileType]
		if group == nil then
			local newGroup = {fileType=fileType, fileInfo={}}
			groupMap[fileType] = newGroup
			group = newGroup
		end
		
		local fileInfo = {
			inputRelativeDir = nil,
			shortName = nil,
			ext = nil,
			includedInBuild = fIncludedInBuild,
			outputRelativeFilename = mbwriter_getoutputrelfilepath(f)
		}
		fileInfo.inputRelativeDir, fileInfo.shortName, fileInfo.ext = Util_FilePathDecompose(f)
		
		group.fileInfo[#group.fileInfo+1] = fileInfo
	end	
	
	logprofile("END BuildFileGroups")
	return groupMap
end

function WriteVcxProjGroupItemHeader(file, currentTarget, msvcPlatform, groupName, fileInfo)
	file:write("   <" .. groupName .. " Include=\"" .. fileInfo.outputRelativeFilename .. "\">\n")
end

function WriteVcxProjGroupItemFooter(file, currentTarget, msvcPlatform, groupName, fileInfo)
	for iConfig = 1, #currentTarget.configs do
		local config = currentTarget.configs[iConfig]
		local excludedFromBuildStr = nil
		if fileInfo.includedInBuild then
			excludedFromBuildStr = "false"
		else
			excludedFromBuildStr = "true"
		end
		file:write("    <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='" .. config.name .. "|" .. msvcPlatform .. "'\">" .. excludedFromBuildStr .. "</ExcludedFromBuild>\n")
	end
	file:write("   </" .. groupName .. ">\n")
end

function WriteVcxProjPropertyGroupOptions(file, groupOption)
	if groupOption ~= nil then
		for jOption = 1, #groupOption do
			local keyValue = split(groupOption[jOption], "=")
			local key = keyValue[1]
			local value = keyValue[2]

			file:write("      <" .. key .. ">" .. value .. "</" .. key .. ">\n")
		end
	end
end

function WriteVcxProjRawXMLBlocks(file, groupOptionRawXml)
	if groupOptionRawXml ~= nil then
		for jOption = 1, #groupOptionRawXml do
			local keyValue = groupOptionRawXml[jOption]
			file:write(keyValue)
		end
	end
end

function WriteVcxProj(currentTarget, groupMap)	
	logprofile("WriteVcxProj")
	
	local projectID = mbwriter_msvcgenerateid()
	mbwriter_msvcregisterprojectid(currentTarget.name, projectID)
	
	mbwriter_mkdir(mbwriter_global.makeoutputdirabs)

	local msvcPlatform = Util_GetKVValue(mbwriter_global.options.msvc, "platform")
	if msvcPlatform == nil then
		-- TODO error
		print("unknown platform!")
		os.exit(1)
	end
	
	local vcxprojName = mbwriter_global.makeoutputdirabs .. "\\" .. currentTarget.name .. ".vcxproj";
	vcxprojName = mbwriter_normalisetargetfilepath(vcxprojName)
	local file = io.open(vcxprojName, "w")
	if (file == nil) then 
		mbwriter_fatalerror("Failed to open file " .. vcxprojName .. " for writing")
	end

	local pchFileBaseName = nil
	local pchSourceFile = nil
	local pchHeaderFile = nil

	if currentTarget.pch ~= nil and currentTarget.pch ~= "" then
		pchFileBaseName = currentTarget.pch
		pchSourceFile = pchFileBaseName .. ".cpp"
		pchHeaderFile = pchFileBaseName .. ".h"
	end

	file:write("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
	file:write("<Project DefaultTargets=\"Build\" ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n")
	file:write("  <ItemGroup Label=\"ProjectConfigurations\">\n")

	for iConfig = 1, #currentTarget.configs do
		local config = currentTarget.configs[iConfig]
		file:write("    <ProjectConfiguration Include=\"" .. config.name .. "|" .. msvcPlatform .. "\">\n")
		file:write("      <Configuration>" .. config.name .. "</Configuration>\n")
		file:write("      <Platform>" .. msvcPlatform .. "</Platform>\n")
		file:write("    </ProjectConfiguration>\n")
	end
	file:write("  </ItemGroup>\n")

	file:write("  <PropertyGroup Label=\"Globals\">\n")
	file:write("    <ProjectGuid>{" .. projectID .. "}</ProjectGuid>\n")
				
	file:write("    <RootNamespace>metabuilder</RootNamespace>\n")
	
	WriteVcxProjPropertyGroupOptions(file, mbwriter_global.options.msvcglobals)
	WriteVcxProjRawXMLBlocks(file, mbwriter_global.options.msvcglobalsrawxml)

	file:write("    <ProjectName>" .. g_currentTarget.name .. "</ProjectName>\n")
	file:write("  </PropertyGroup>\n")
	MSVCCustomPropertySheets(file)
	file:write("  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />\n")
	for iConfig = 1, #currentTarget.configs do
		local config = currentTarget.configs[iConfig]
		file:write("  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='" .. config.name .. "|" .. msvcPlatform .. "'\" Label=\"Configuration\">\n")
		if currentTarget.targettype == "app" then
			file:write("    <ConfigurationType>Application</ConfigurationType>\n")
		elseif currentTarget.targettype == "staticlib" or currentTarget.targettype == "module" then 		
			file:write("    <ConfigurationType>StaticLibrary</ConfigurationType>\n")
		end

		WriteVcxProjPropertyGroupOptions(file, config.options.msvconfiguration)
		WriteVcxProjRawXMLBlocks(file, config.options.msvconfigurationrawxml)
				
		--print(inspect(config.options))
		file:write("  </PropertyGroup>\n")
	end
	file:write("  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />\n")
	file:write("  <ImportGroup Label=\"ExtensionSettings\">\n")
	if g_enableHLSL == true then
	    file:write("    <Import Project=\"hlsl.props\" />\n")
	end
	file:write("  </ImportGroup>\n")

	for iConfig = 1, #currentTarget.configs do
		local config = currentTarget.configs[iConfig]

		file:write("  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='" .. config.name .. "|" .. msvcPlatform .. "'\">\n")
		file:write("    <LinkIncremental>false</LinkIncremental>\n") --incremental linking is only ever a source of pain (and corrupt files)
	    file:write("    <ExecutablePath>")
		for jExeDir = 1, #config.exedirs do
			file:write(mbwriter_getoutputrelfilepath(config.exedirs[jExeDir]) .. ";")
		end
		file:write("$(ExecutablePath)</ExecutablePath>\n")
	    file:write("    <IntDir>" .. mbwriter_normalisetargetfilepath(mbwriter_global.intdir) .. "\\$(ProjectName)\\$(Configuration)\\</IntDir>\n")
	    file:write("    <OutDir>" .. mbwriter_normalisetargetfilepath(mbwriter_global.outdir) .. "\\$(ProjectName)\\</OutDir>\n")
	    file:write("    <TargetName>$(ProjectName)_$(Configuration)</TargetName>\n")
		
		WriteVcxProjPropertyGroupOptions(file, config.options.msvcpropertygroup)
		WriteVcxProjRawXMLBlocks(file, config.options.msvcpropertygrouprawxml)
		
		file:write("  </PropertyGroup>\n")

		file:write("  <ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='" .. config.name .. "|" .. msvcPlatform .. "'\">\n")
		file:write("    <Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />\n")
		file:write("  </ImportGroup>\n")

		file:write("  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='" .. config.name .. "|" .. msvcPlatform .. "'\">\n")
		file:write("    <ClCompile>\n")
		
 		if pchSourceFile ~= nil then 		
			file:write("      <PrecompiledHeader>Use</PrecompiledHeader>\n")
			file:write("      <PrecompiledHeaderFile>" .. pchHeaderFile .. "</PrecompiledHeaderFile>\n")
		end
		
		if config.options.msvccompile ~= nil then
			for jOption = 1, #config.options.msvccompile do
				local keyValue = split(config.options.msvccompile[jOption], "=")
				local key = keyValue[1]
				local value = keyValue[2]

				file:write("      <" .. key .. ">" .. value .. "</" .. key .. ">\n")
			end
		end

		file:write("      <PreprocessorDefinitions>")
		for jDefine = 1, #config.defines do
			file:write(config.defines[jDefine] .. ";")
		end
		file:write("%(PreprocessorDefinitions)</PreprocessorDefinitions>\n")

		file:write("      <AdditionalIncludeDirectories>")
		for jIncludeDir = 1, #config.includedirs do
			local includeDir = mbwriter_getoutputrelfilepath(config.includedirs[jIncludeDir])
			file:write(includeDir .. ";")
		end
		file:write("</AdditionalIncludeDirectories>\n")

		file:write("    </ClCompile>\n")
		file:write("    <Link>\n")
		
		if currentTarget.targetsubsystem == "console" then
			file:write("    <SubSystem>Console</SubSystem>")
		end

		--Lib directories
		file:write("      <AdditionalLibraryDirectories>")
		for jLibDir = 1, #config.libdirs do
			local libDir = mbwriter_getoutputrelfilepath(config.libdirs[jLibDir])
			file:write(libDir .. ";")
		end
		file:write("</AdditionalLibraryDirectories>\n")

		--Add hardwired options
		--TODO: The below is not ideal, linkage only possible on final app target.
		if currentTarget.targettype == "app" then
	 	    file:write("      <AdditionalDependencies>")
			for jLib = 1, #config.libs do
				local lib = config.libs[jLib]
				file:write(lib .. ";")
			end
	 	    file:write("</AdditionalDependencies>\n")
 		end

		--Add custom options
		if config.options.msvclink ~= nil then
			for jOption = 1, #config.options.msvclink do
				local keyValue = split(config.options.msvclink[jOption], "=")
				local key = keyValue[1]
				local value = keyValue[2]

				file:write("      <" .. key .. ">" .. value .. "</" .. key .. ">\n")
			end
		end
		file:write("    </Link>\n")
		
		if config.options.msvcrawxml ~= nil then
			for jOption = 1, #config.options.msvcrawxml do
				local keyValue = config.options.msvcrawxml[jOption]
				file:write(keyValue)
			end
		end
		
		file:write("\n")

		if g_enableHLSL == true then		
			file:write("    <CompilerShader/>\n")	
		end
		
		--Post build event
		file:write("    <PostBuildEvent>\n")
		local postbuild = Util_GetKVValue(config.options.msvc, "postbuild")
		if postbuild ~= nil then
			file:write("	  <Command>" .. postbuild .. "</Command>\n")
		end
		file:write("    </PostBuildEvent>\n")
		
		WriteVcxProjPropertyGroupOptions(file, config.options.msvcitemdef)
		WriteVcxProjRawXMLBlocks(file, config.options.msvcitemdefrawxml)		
		
		file:write("  </ItemDefinitionGroup>\n")
	end

	local filesNotUsingPchMap = {}
	for i = 1, #currentTarget.nopchfiles do
		local f = currentTarget.nopchfiles[i]
		filesNotUsingPchMap[f] = 1
	end

	--New group writing method which isn't horrid and totally hard-coded
	--TODO - add function pointer approach for intrinsic types.
	for groupName, group in pairs(groupMap) do 
		--print(groupName)
		file:write("  <ItemGroup>\n")
		if groupName == "ClCompile" then
			for i = 1, #group.fileInfo do
				WriteVcxProjGroupItemHeader(file, currentTarget, msvcPlatform, groupName, group.fileInfo[i])
				for iConfig = 1, #currentTarget.configs do
					local config = currentTarget.configs[iConfig]
					file:write("      <CompileAs Condition=\"'$(Configuration)|$(Platform)'=='" .. config.name .. "|" .. msvcPlatform .. "'\">Default</CompileAs>\n")
				end
				--If we're using a pch and this is the pch file
				if pchSourceFile ~= nil then
					if group.fileInfo[i].shortName == pchSourceFile then 
						--For each config ensure we create the pch
						for iConfig = 1, #currentTarget.configs do
							local config = currentTarget.configs[iConfig]
							file:write("      <PrecompiledHeader Condition=\"'$(Configuration)|$(Platform)'=='" .. config.name .. "|" .. msvcPlatform .. "'\">Create</PrecompiledHeader>\n")
						end
					elseif filesNotUsingPchMap[f] ~= nil then
						--This file, for whatever reason, is not using the pch
						for iConfig = 1, #currentTarget.configs do
							local config = currentTarget.configs[iConfig]
							file:write("      <PrecompiledHeader Condition=\"'$(Configuration)|$(Platform)'=='" .. config.name .. "|" .. msvcPlatform .. "'\">NotUsing</PrecompiledHeader>\n")
						end
					end
				end

				WriteVcxProjGroupItemFooter(file, currentTarget, msvcPlatform, groupName, group.fileInfo[i])
			end
		elseif groupName == "ClInclude" then
			for i = 1, #group.fileInfo do
				WriteVcxProjGroupItemHeader(file, currentTarget, msvcPlatform, groupName, group.fileInfo[i])
				WriteVcxProjGroupItemFooter(file, currentTarget, msvcPlatform, groupName, group.fileInfo[i])
			end
		elseif groupName == "None" then
			for i = 1, #group.fileInfo do
				WriteVcxProjGroupItemHeader(file, currentTarget, msvcPlatform, groupName, group.fileInfo[i])
				WriteVcxProjGroupItemFooter(file, currentTarget, msvcPlatform, groupName, group.fileInfo[i])
			end
		else
			if GetCustomGroupRule ~= nil then
				for i = 1, #group.fileInfo do
					local fileInfo = group.fileInfo[i]
					local customRuleWriterFunc = GetCustomGroupRule(groupName, fileInfo.shortName)
					WriteVcxProjGroupItemHeader(file, currentTarget, msvcPlatform, groupName, fileInfo)
						if customRuleWriterFunc ~= nil then
							customRuleWriterFunc(file, currentTarget, msvcPlatform, fileInfo.outputRelativeFilename)
						end
					WriteVcxProjGroupItemFooter(file, currentTarget, msvcPlatform, groupName, fileInfo)
				end
			else
				for i = 1, #group.fileInfo do
					WriteVcxProjGroupItemHeader(file, currentTarget, msvcPlatform, groupName, group.fileInfo[i])
					WriteVcxProjGroupItemFooter(file, currentTarget, msvcPlatform, groupName, group.fileInfo[i])
				end
			end
		end
		
		file:write("  </ItemGroup>\n")	
	end

	file:write("  <ItemGroup>\n")
	for i = 1, #currentTarget.depends do
		local dependency = currentTarget.depends[i]
		local path, filename, ext = Util_FilePathDecompose(dependency)

		local projectFilename = filename .. ".vcxproj"
		file:write("    <ProjectReference Include=\"" .. projectFilename .. "\">\n")

		local projectID = mbwriter_msvcgetprojectid(filename)
		file:write("      <Project>{" .. projectID .. "}</Project>\n")
		file:write("    </ProjectReference>\n")
	end
	file:write("  </ItemGroup>\n")
	
	--print(inspect(currentTarget.options))
	WriteVcxProjRawXMLBlocks(file, currentTarget.options.msvcitemgrouprawxml)

	file:write("\n")
	
	file:write("  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />\n")

	file:write("  <ImportGroup Label=\"ExtensionTargets\">\n")
	if g_enableHLSL == true then
	    file:write("    <Import Project=\"hlsl.targets\" />\n")
	end
	file:write("  </ImportGroup>\n")
	MSVCCustomGlobal(file)
	file:write("</Project>\n")

	file:close()
	mbwriter_reportoutputfile(vcxprojName)
	logprofile("END WriteVcxProj")
end

function FormatFilterPath(path)
--[[
	while string.find(path, "%.%.\\") == 1 do
		path = string.sub(path, 4, length)
	end
]]
	path = Util_FileTrimTrailingSlash(path)
	return mbwriter_normalisetargetfilepath(path)
end

function WriterVcxProjFilters(currentTarget, groupMap)
	local vcxProjFiltersFilename = mbwriter_normalisetargetfilepath(mbwriter_global.makeoutputdirabs .. "\\" .. currentTarget.name .. ".vcxproj.filters")
	local file = io.open(vcxProjFiltersFilename, "w")

	file:write("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
	file:write("<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n")
	
	local folders = {}
	InitFolders(folders, groupMap)

	--Write out filter folders
	file:write("  <ItemGroup>\n")
	for k, v in pairs(folders) do 
		local folder = v
		--print(inspect(folder))
		if folder.shortName ~= "" then 
			local formattedPath = FormatFilterPath(folder.relativePath)
			if formattedPath ~= "." then
				file:write("    <Filter Include=\"" .. formattedPath .. "\">\n")
				file:write("      <UniqueIdentifier>{" .. folder.id .. "}</UniqueIdentifier>\n")
				file:write("    </Filter>\n")
			end
		end
	end
	file:write("  </ItemGroup>\n")

	file:write("  <ItemGroup>\n")	
	for groupName, group in pairs(groupMap) do 
		for i = 1, #group.fileInfo do
			file:write("   <" .. groupName .. " Include=\"" .. group.fileInfo[i].outputRelativeFilename .. "\">\n")
			file:write("      <Filter>" .. FormatFilterPath(group.fileInfo[i].inputRelativeDir) .. "</Filter>\n")
			file:write("   </" .. groupName .. ">\n")				
		end
	end
	file:write("  </ItemGroup>\n")

	file:write("</Project>\n")
	file:close()
	mbwriter_reportoutputfile(vcxProjFiltersFilename)
end

function WriteSolution(projectList, currentTarget)
	if mbwriter_solution.msvcversion == nil then
		-- TODO ERROR HERE
	end

	local msvcVersion = Util_GetKVValue(mbwriter_global.options.msvc, "version")
	local msvcPlatform = Util_GetKVValue(mbwriter_global.options.msvc, "platform")
	--if msvcPlatform == nil then
		-- TODO error
	--end

	local msvcFormatVersion = "11.00" -- Default to 2010 format

	if msvcVersion == "2012" then
		msvcFormatVersion = "12.00"
	end

	local slnFilename = mbwriter_normalisetargetfilepath(mbwriter_global.makeoutputdirabs .. "\\" .. currentTarget.name .. ".sln")
	local file = io.open(slnFilename, "w")
	if file == nil then
		print("Failed to open file for writing " .. slnFilename)
	end

	file:write("Microsoft Visual Studio Solution File, Format Version " .. msvcFormatVersion .. "\n")
	file:write("# MetaBuilder " .. msvcVersion .. "\n")

	--Write out solution ref for targets we must link against
	do
		local projectID = projectList[1][1]
		local projectName = projectList[1][2]
		file:write("Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"" .. projectName .. "\", \"" .. projectName .. ".vcxproj\", \"{" .. projectID .. "}\"\n")
		file:write("	ProjectSection(ProjectDependencies) = postProject\n")
		for i = 2, #projectList do
			local projectID = projectList[i][1]
			local projectName = projectList[i][2]		
			file:write("		{" .. projectID .. "} = {" .. projectID .. "}\n")
			file:write("	EndProjectSection\n")
		end
		file:write("EndProject\n")
	end		

	for i = 2, #projectList do
		local projectID = projectList[i][1]
		local projectName = projectList[i][2]
		file:write("Project(\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\") = \"" .. projectName .. "\", \"" .. projectName .. ".vcxproj\", \"{" .. projectID .. "}\"\n")
		file:write("EndProject\n")
	end		

	file:write("Global\n")
	file:write("	GlobalSection(SolutionConfigurationPlatforms) = preSolution\n")
	for iConfig = 1, #currentTarget.configs do
		local config = currentTarget.configs[iConfig]
		file:write("		" .. config.name .. "|" .. msvcPlatform .. " = " .. config.name .. "|" .. msvcPlatform .. "\n")
	end
	file:write("	EndGlobalSection\n")
	file:write("	GlobalSection(ProjectConfigurationPlatforms) = postSolution\n")
	
	for iProject = 1, #projectList do
		local projectID = projectList[iProject][1]
		local projectName = projectList[iProject][2]

		for jConfig = 1, #currentTarget.configs do
			local config = currentTarget.configs[jConfig]
			file:write("		{" .. projectID .. "}." .. config.name .. "|" .. msvcPlatform .. ".ActiveCfg = " .. config.name .. "|" .. msvcPlatform .. "\n")
			file:write("		{" .. projectID .. "}." .. config.name .. "|" .. msvcPlatform .. ".Build.0 = " .. config.name .. "|" .. msvcPlatform .. "\n")
			if CustomSolutionGlobalSectionRule ~= nil then
				CustomSolutionGlobalSectionRule(file, projectID, config, msvcPlatform)
			end
		end
	end
	file:write("	EndGlobalSection\n")
	file:write("	GlobalSection(SolutionProperties) = preSolution\n")
	file:write("		HideSolutionNode = FALSE\n")
	file:write("	EndGlobalSection\n")
	file:write("EndGlobal\n")

	file:close()
	mbwriter_reportoutputfile(slnFilename)
end

--[[ MAIN ]]

local customWriter = Util_GetKVValue(mbwriter_global.options.msvc, "customwriter")
if customWriter ~= nil then
	--print("Importing custom writer " .. customWriter)
	import(customWriter)
end

local customPreGenerateRule = CustomPreGenerateRule
if customPreGenerateRule ~= nil then
	print("Applying custom pre generate rule")
	customPreGenerateRule()
end

if GetFileTypeMap ~= nil then
	g_fileTypeMap = GetFileTypeMap()
end

local groupMap = BuildFileGroups(g_currentTarget)
--print(inspect(g_fileTypeMap))

--TODO move out this hack for Windows builds
if g_fileTypeMap["hlsl"] == "CompilerShader" then
	--print("HLSL support enabled")
	g_enableHLSL = true
end

WriteVcxProj(g_currentTarget, groupMap)
WriterVcxProjFilters(g_currentTarget, groupMap)

--Solutions only required by apps
local projectList = {}
if g_currentTarget.targettype == "app" then
	--Create a list of project GUID and name pairs. We'll need this to form the contents of our solution.
	
	--Current target.
	local projectID = mbwriter_msvcgetprojectid(g_currentTarget.name)
	projectList[#projectList+1] = {projectID, g_currentTarget.name}
	
	--Other targets we require.
	for i = 1, #g_currentTarget.depends do
		local dependency = g_currentTarget.depends[i]
		local path, filename, ext = Util_FilePathDecompose(dependency)

		local projectID = mbwriter_msvcgetprojectid(filename)	
		projectList[#projectList+1] = {projectID, filename}
	end

	WriteSolution(projectList, g_currentTarget)
end

PostGenerateEvent()

if CustomPostGenerateRule ~= nil then
	print("Applying custom post generate rule")
	CustomPostGenerateRule(projectList, g_currentTarget)
end

logprofile("SHUTDOWN")