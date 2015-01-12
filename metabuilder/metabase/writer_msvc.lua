--Note there's a bug in MSVC 2010 in which property pages stop working if absolute paths are used when including files
--https://connect.microsoft.com/VisualStudio/feedback/details/635294/using-absolute-path-in-clcompile-item-prevents-property-pages-from-showing
import "writer_common.lua"

g_currentTarget = mbwriter.solution.targets[1]

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

function MSVCGetFileMappingType(filepath)
	local fileType = nil

	if MSVCGetFileMappingTypeHook then
		fileType = MSVCGetFileMappingTypeHook(filepath)
	else
		local ext = mbfilepath.getextension(filepath)
		fileType = g_fileTypeMap[ext]
	end

	if fileType == nil then
		fileType = "None"
	end

	return fileType
end

function MSVCInitFolder(folderList, path_, filename)
	local path = mbfilepath.trimtrailingslash(path_)

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

			if #pathComponents[i] > 0 then
				local newFolderID = mbwriter.msvcgenerateid()

				currentFolder = {
					fullName						= mbwriter.normalisewindowsfilepath(mbwriter.getoutputrelfilepath(currentPath)),
					winNormRelativePath	= currentPath,
					shortName						= pathComponents[i],
					id									= newFolderID,
					parentid						= currentParentID,
					childIDs						= {},
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
			end
		else
			local currentFolder = folderList[currentPath]
			currentParentID = currentFolder.id
			currentParentPath = currentPath
		end
	end
end

function MSVCInitFolders(folderList, groups)
	logprofile("MSVCInitFolders")
	-- Lazily initialise chains of folders based upon the list of files provided
	-- For each folder store:
	--	the short name of the folder
	--	its unique id
	--	the unique id of its parent

	for _, group in ipairs(groups) do
		for i = 1, #group.fileInfo do
			MSVCInitFolder(folderList, group.fileInfo[i].winNormInputRelativeDir, group.fileInfo[i].winNormOutputRelativeFilename)
		end
	end

	logprofile("END MSVCInitFolders")
end

--[[ FILE WRITING ]] --------------------------------------------------------------------------------

function MSVCFileGroupCompare(a, b)
	return a.name < b.name
end

function MSVCBuildFileGroups(currentTarget)
	logprofile("MSVCBuildFileGroups " .. currentTarget.name)

	local fileIncludedInBuild = {}

	for i = 1, #currentTarget.files do
		local f = currentTarget.files[i]
		fileIncludedInBuild[f] = true
	end

	local groupMap = {}

	for i = 1, #currentTarget.allsourcefiles do
		local mbNormalisedPath = currentTarget.allsourcefiles[i]

		local fIncludedInBuild = fileIncludedInBuild[f]
		if fIncludedInBuild == nil then
			fIncludedInBuild = false
		end

		fileType = MSVCGetFileMappingType(mbNormalisedPath)

		local group = groupMap[fileType]
		if group == nil then
			local newGroup = {name=fileType, fileInfo={}}
			groupMap[fileType] = newGroup
			group = newGroup
		end

		local fileInfo = {
			winNormInputRelativeDir	= nil,
			shortName								= nil,
			ext											= nil,
			includedInBuild = fIncludedInBuild,
			winNormOutputRelativeFilename = mbwriter.normalisewindowsfilepath(mbwriter.getoutputrelfilepath(mbNormalisedPath))
		}
		fileInfo.winNormInputRelativeDir, fileInfo.shortName, fileInfo.ext = mbfilepath.decompose(fileInfo.winNormOutputRelativeFilename)

		if g_enableHLSL == false then
			if fileInfo.ext == "hlsl" then
				loginfo("HLSL support enabled")
				g_enableHLSL = true
			end
		end

		group.fileInfo[#group.fileInfo+1] = fileInfo
	end

	local sortedGroups = {}
	for _, group in pairs(groupMap) do
		sortedGroups[#sortedGroups+1] = group
	end
	
	table.sort(sortedGroups, MSVCFileGroupCompare)

	logprofile("END MSVCBuildFileGroups")
	return sortedGroups
end

function MSVCWriteVcxProjGroupItemHeader(file, currentTarget, msvcPlatform, groupName, fileInfo)
	file:write("   <" .. groupName .. " Include=\"" .. fileInfo.winNormOutputRelativeFilename .. "\">\n")
end

function MSVCWriteVcxProjGroupItemFooter(file, currentTarget, msvcPlatform, groupName, fileInfo)
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

function MSVCWriteVcxProjPropertyGroupOptions(file, groupOption)
	if groupOption ~= nil then
		for jOption = 1, #groupOption do
			local keyValue = split(groupOption[jOption], "=")
			local key = keyValue[1]
			local value = keyValue[2]

			file:write("      <" .. key .. ">" .. value .. "</" .. key .. ">\n")
		end
	end
end

function MSVCWriteVcxProjRawXMLBlocks(file, groupOptionRawXml)
	if groupOptionRawXml ~= nil then
		for jOption = 1, #groupOptionRawXml do
			local keyValue = groupOptionRawXml[jOption]
			file:write(keyValue)
		end
	end
end

function MSVCWriteVcxProj(currentTarget, groups)
	logprofile("MSVCWriteVcxProj")

	local projectID = mbwriter.msvcgenerateid()
	mbwriter.msvcregisterprojectid(currentTarget.name, projectID)

	mbwriter.mkdir(mbwriter.global.makeoutputdirabs)

	local msvcPlatform = mbutil.getkvvalue(mbwriter.global.options.msvc, "platform")
	if msvcPlatform == nil then
		mbwriter.fatalerror("unknown platform!")
	end

	local vcxprojName = nil
	if MSVCGetProjSlnOutputDir() then
		vcxprojName = MSVCGetProjSlnOutputDir() .. "\\" .. currentTarget.name .. ".vcxproj";
	else
		vcxprojName = mbwriter.global.makeoutputdirabs .. "\\" .. currentTarget.name .. ".vcxproj";
	end
	vcxprojName = mbwriter.normalisewindowsfilepath(vcxprojName)

	local file = mbfile.open(vcxprojName, "w")
	if (file == nil) then
		mbwriter.fatalerror("Failed to open file " .. vcxprojName .. " for writing")
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
	if MSVCRootHook then
		MSVCRootHook(file)
	end

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

	MSVCWriteVcxProjPropertyGroupOptions(file, mbwriter.global.options.msvcglobals)
	MSVCWriteVcxProjRawXMLBlocks(file, mbwriter.global.options.msvcglobalsrawxml)

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

		MSVCWriteVcxProjPropertyGroupOptions(file, config.options.msvconfiguration)
		MSVCWriteVcxProjRawXMLBlocks(file, config.options.msvconfigurationrawxml)

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
			file:write(mbwriter.getoutputrelfilepath(config.exedirs[jExeDir]) .. ";")
		end
		file:write("$(ExecutablePath)</ExecutablePath>\n")
	    file:write("    <IntDir>" .. mbwriter.normalisewindowsfilepath(mbwriter.global.intdir) .. "\\$(ProjectName)\\$(Configuration)\\</IntDir>\n")
	    file:write("    <OutDir>" .. mbwriter.normalisewindowsfilepath(mbwriter.global.outdir) .. "\\$(ProjectName)\\</OutDir>\n")
	    file:write("    <TargetName>$(ProjectName)_$(Configuration)</TargetName>\n")

		MSVCWriteVcxProjPropertyGroupOptions(file, config.options.msvcpropertygroup)
		MSVCWriteVcxProjRawXMLBlocks(file, config.options.msvcpropertygrouprawxml)

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
			local includeDir = mbwriter.getoutputrelfilepath(config.includedirs[jIncludeDir])
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
			local libDir = mbwriter.getoutputrelfilepath(config.libdirs[jLibDir])
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
		local postbuild = mbutil.getkvvalue(config.options.msvc, "postbuild")
		if postbuild ~= nil then
			file:write("	  <Command>" .. postbuild .. "</Command>\n")
		end
		file:write("    </PostBuildEvent>\n")

		MSVCWriteVcxProjPropertyGroupOptions(file, config.options.msvcitemdef)
		MSVCWriteVcxProjRawXMLBlocks(file, config.options.msvcitemdefrawxml)

		file:write("  </ItemDefinitionGroup>\n")
	end

	local filesNotUsingPchMap = {}
	for i = 1, #currentTarget.nopchfiles do
		local f = currentTarget.nopchfiles[i]
		filesNotUsingPchMap[f] = 1
	end

	--New group writing method which isn't horrid and totally hard-coded
	--TODO - add function pointer approach for intrinsic types.
	for _, group in ipairs(groups) do
		file:write("  <ItemGroup>\n")
		if group.name == "ClCompile" then
			for i = 1, #group.fileInfo do
				MSVCWriteVcxProjGroupItemHeader(file, currentTarget, msvcPlatform, group.name, group.fileInfo[i])
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

				MSVCWriteVcxProjGroupItemFooter(file, currentTarget, msvcPlatform, group.name, group.fileInfo[i])
			end
		elseif group.name == "ClInclude" then
			for i = 1, #group.fileInfo do
				MSVCWriteVcxProjGroupItemHeader(file, currentTarget, msvcPlatform, group.name, group.fileInfo[i])
				MSVCWriteVcxProjGroupItemFooter(file, currentTarget, msvcPlatform, group.name, group.fileInfo[i])
			end
		elseif group.name == "None" then
			for i = 1, #group.fileInfo do
				MSVCWriteVcxProjGroupItemHeader(file, currentTarget, msvcPlatform, group.name, group.fileInfo[i])
				MSVCWriteVcxProjGroupItemFooter(file, currentTarget, msvcPlatform, group.name, group.fileInfo[i])
			end
		else
			if GetCustomGroupRule ~= nil then
				for i = 1, #group.fileInfo do
					local fileInfo = group.fileInfo[i]
					local customRuleWriterFunc = GetCustomGroupRule(group.name, fileInfo.shortName)
					MSVCWriteVcxProjGroupItemHeader(file, currentTarget, msvcPlatform, group.name, fileInfo)
						if customRuleWriterFunc ~= nil then
							customRuleWriterFunc(file, currentTarget, msvcPlatform, fileInfo.winNormOutputRelativeFilename)
						end
					MSVCWriteVcxProjGroupItemFooter(file, currentTarget, msvcPlatform, group.name, fileInfo)
				end
			else
				for i = 1, #group.fileInfo do
					MSVCWriteVcxProjGroupItemHeader(file, currentTarget, msvcPlatform, group.name, group.fileInfo[i])
					MSVCWriteVcxProjGroupItemFooter(file, currentTarget, msvcPlatform, group.name, group.fileInfo[i])
				end
			end
		end

		file:write("  </ItemGroup>\n")
	end

	file:write("  <ItemGroup>\n")
	for i = 1, #currentTarget.depends do
		local dependency = currentTarget.depends[i]
		local path, filename, ext = mbfilepath.decompose(dependency)

		local projectFilename = filename .. ".vcxproj"
		file:write("    <ProjectReference Include=\"" .. projectFilename .. "\">\n")

		local projectID = mbwriter.msvcgetprojectid(filename)
		file:write("      <Project>{" .. projectID .. "}</Project>\n")
		file:write("    </ProjectReference>\n")
	end
	file:write("  </ItemGroup>\n")

	MSVCWriteVcxProjRawXMLBlocks(file, currentTarget.options.msvcitemgrouprawxml)

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
	mbwriter.reportoutputfile(vcxprojName)
	logprofile("END MSVCWriteVcxProj")
end

function MSVCFormatFilterPath(path)
	return mbwriter.normalisewindowsfilepath(mbfilepath.trimtrailingslash(path))
end

function MSVCFilterFolderCompare(a, b)
	return a.winNormRelativePath < b.winNormRelativePath
end

function MSVCWriterVcxProjFilters(currentTarget, groups)
	local vcxProjFiltersFilename = nil
	if MSVCGetProjSlnOutputDir() then
		vcxProjFiltersFilename = MSVCGetProjSlnOutputDir() .. "\\" .. currentTarget.name .. ".vcxproj.filters";
	else
		vcxProjFiltersFilename = mbwriter.global.makeoutputdirabs .. "\\" .. currentTarget.name .. ".vcxproj.filters"
	end
	vcxProjFiltersFilename = mbwriter.normalisewindowsfilepath(vcxProjFiltersFilename)

	local file = mbfile.open(vcxProjFiltersFilename, "w")

	file:write("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
	file:write("<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n")

	local folders = {}
	MSVCInitFolders(folders, groups)

	--Write out filter folders
	file:write("  <ItemGroup>\n")

	local sortedFolderList = {}
	for _, v in pairs(folders) do
		sortedFolderList[#sortedFolderList+1] = v
	end

	table.sort(sortedFolderList, MSVCFilterFolderCompare)

	for _, folder in ipairs(sortedFolderList) do
		if folder.shortName ~= "" then
			local formattedPath = MSVCFormatFilterPath(folder.winNormRelativePath)
			if formattedPath ~= "." then
				file:write("    <Filter Include=\"" .. formattedPath .. "\">\n")
				file:write("      <UniqueIdentifier>{" .. folder.id .. "}</UniqueIdentifier>\n")
				file:write("    </Filter>\n")
			end
		end
	end
	file:write("  </ItemGroup>\n")

	file:write("  <ItemGroup>\n")
	for _, group in ipairs(groups) do
		for i = 1, #group.fileInfo do
			file:write("   <" .. group.name .. " Include=\"" .. group.fileInfo[i].winNormOutputRelativeFilename .. "\">\n")
			file:write("      <Filter>" .. MSVCFormatFilterPath(group.fileInfo[i].winNormInputRelativeDir) .. "</Filter>\n")
			file:write("   </" .. group.name .. ">\n")
		end
	end
	file:write("  </ItemGroup>\n")

	file:write("</Project>\n")
	file:close()
	mbwriter.reportoutputfile(vcxProjFiltersFilename)
end

function WriteSolution(projectList, currentTarget)
	if mbwriter.solution.msvcversion == nil then
		-- TODO ERROR HERE
	end

	local msvcVersion = mbutil.getkvvalue(mbwriter.global.options.msvc, "version")
	local msvcPlatform = mbutil.getkvvalue(mbwriter.global.options.msvc, "platform")
	--if msvcPlatform == nil then
		-- TODO error
	--end

	--TODO remove this hard-coding of MSVC version information
	local msvcFormatVersion = "11.00" -- Default to 2010 format
	if msvcVersion == "2012" then
		msvcFormatVersion = "12.00"
	end

	local slnFilename = nil
	if MSVCGetProjSlnOutputDir() then
		slnFilename = MSVCGetProjSlnOutputDir() .. "\\" .. currentTarget.name .. ".sln";
	else
		slnFilename = mbwriter.global.makeoutputdirabs .. "\\" .. currentTarget.name .. ".sln"
	end
	slnFilename = mbwriter.normalisewindowsfilepath(slnFilename)

	local file = mbfile.open(slnFilename, "w")
	if file == nil then
		mbwriter.fatalerror("Failed to open file for writing " .. slnFilename)
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
	mbwriter.reportoutputfile(slnFilename)
end

--[[ MAIN ]]

local customWriter = mbutil.getkvvalue(mbwriter.global.options.msvc, "customwriter")
if customWriter ~= nil then
	import(customWriter)
end

local customPreGenerateRule = CustomPreGenerateRule
if customPreGenerateRule ~= nil then
	customPreGenerateRule()
end

local groups = MSVCBuildFileGroups(g_currentTarget)
MSVCWriteVcxProj(g_currentTarget, groups)
MSVCWriterVcxProjFilters(g_currentTarget, groups)

--Solutions only required by apps
local projectList = {}
if g_currentTarget.targettype == "app" then
	--Create a list of project GUID and name pairs. We'll need this to form the contents of our solution.

	--Current target.
	local projectID = mbwriter.msvcgetprojectid(g_currentTarget.name)
	projectList[#projectList+1] = {projectID, g_currentTarget.name}

	--Other targets we require.
	for i = 1, #g_currentTarget.depends do
		local dependency = g_currentTarget.depends[i]
		local path, filename, ext = mbfilepath.decompose(dependency)

		local projectID = mbwriter.msvcgetprojectid(filename)
		projectList[#projectList+1] = {projectID, filename}
	end

	WriteSolution(projectList, g_currentTarget)
end

PostGenerateEvent()

if CustomPostGenerateRule ~= nil then
	CustomPostGenerateRule(projectList, g_currentTarget)
end
