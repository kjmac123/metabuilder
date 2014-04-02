--Note there's a bug in MSVC 2010 in which property pages stop working if absolute paths are used when including files
--https://connect.microsoft.com/VisualStudio/feedback/details/635294/using-absolute-path-in-clcompile-item-prevents-property-pages-from-showing

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

g_currentTarget = writer_solution.targets[1]

--Map relative to absolute path
g_filePathMap = {}

--[[ PRE-PROCESSING ]] --------------------------------------------------------------------------------

-- Create file type map
g_fileTypeMap = {}
g_fileTypeMap["c"]			= "ClCompile"
g_fileTypeMap["cpp"]		= "ClCompile"
g_fileTypeMap["h"]			= "ClInclude"
g_fileTypeMap["hpp"]		= "ClInclude"
g_fileTypeMap["inl"]		= "ClInclude"
g_fileTypeMap["hlsl"]		= "CompileShaderGroup"
g_fileTypeMap["vsh"]		= "CompileShaderGroup"
g_fileTypeMap["psh"]		= "CompileShaderGroup"

function GetFullFilePath(filepath)
	return Util_FileNormaliseWindows(Util_FileConvertToAbsolute(g_filePathMap, writer_global.currentmetamakedirabs, filepath))
end

function GetFileType(filepath)
	local ext = Util_FileExtension(filepath)
	local lastKnownType = g_fileTypeMap[ext]
	if lastKnownType == nil then
		return "None"
	end

	return lastKnownType
end


function InitFolder(folderList, path, filename)
	local pathComponents = { "" }
	
	if (path ~= "") then 
		pathComponents = split(path, "/")
		table.insert(pathComponents, 1, "")
	end

	local fullProjectRelativeFilePath = Util_FilePathJoin(path,filename)

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

			local newFolderID = msvcgenerateid()

			currentFolder = {
				fullName = GetFullFilePath(currentPath),
				relativePath = currentPath,
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
end

function InitFolders(folderList, fileList)
	-- Lazily initialise chains of folders based upon the list of files provided
	-- For each folder store:
	--	the short name of the folder
	--	its unique id
	--	the unique id of its parent

	for i = 1, #fileList do
		local f = fileList[i]
		--print(f)
		local path, filename, ext = Util_FilePathDecompose(f)

		--remove trailing slash
		local path = string.sub(path, 1, -2)

		--print(path .. " from file " .. filename)
		
		InitFolder(folderList, path, filename)
	end

	--print(inspect(folderList))
end

--[[ FILE WRITING ]] --------------------------------------------------------------------------------

function BuildFileGroups(currentTarget, ClCompileGroup, ClIncludeGroup, NoneGroup, CompileShaderGroup)
	local fileIncludedInBuild = {}

	for i = 1, #currentTarget.files do
		local f = currentTarget.files[i]
		fileIncludedInBuild[f] = true
	end

	for i = 1, #currentTarget.allsourcefiles do
		local f = currentTarget.allsourcefiles[i]
		local fIncludedInBuild = fileIncludedInBuild[f]
		if fIncludedInBuild == nil then
			fIncludedInBuild = false
		end

		fileType = GetFileType(f)

		--For each group store included files and whether they need to be excluded from the build
		if fileType == "ClCompile" then
			table.insert(ClCompileGroup, {f, fIncludedInBuild})
		elseif fileType == "ClInclude" then
			table.insert(ClIncludeGroup, {f, fIncludedInBuild})
		elseif fileType == "CompileShaderGroup" then
			table.insert(CompileShaderGroup, {f, fIncludedInBuild})
		else
			table.insert(NoneGroup, {f, fIncludedInBuild})
		end
	end	
end

function WriteVcxProj(currentTarget, ClCompileGroup, ClIncludeGroup, NoneGroup, CompileShaderGroup)
	local projectID = msvcgenerateid()
	msvcregisterprojectid(currentTarget.name, projectID)

	mkdir(writer_global.makeoutputdirabs)

	local msvcPlatform = Util_GetKVValue(writer_global.options.msvc, "platform")
	if msvcPlatform == nil then
		-- TODO error
		os.exit(1)
	end
	
	local vcxprojName = writer_global.makeoutputdirabs .. "\\" .. currentTarget.name .. ".vcxproj";
	vcxprojName = Util_FileNormaliseWindows(vcxprojName)
	print("Writing " .. vcxprojName .. "\n")
	local file = io.open(vcxprojName, "w")

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
	if msvcPlatform == "Win32" then
		file:write("    <Keyword>Win32Proj</Keyword>\n")
	end
	file:write("    <RootNamespace>metabuilder</RootNamespace>\n")
	file:write("  </PropertyGroup>\n")
	file:write("  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />\n")
	for iConfig = 1, #currentTarget.configs do
		local config = currentTarget.configs[iConfig]
		file:write("  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='" .. config.name .. "|" .. msvcPlatform .. "'\" Label=\"Configuration\">\n")
		if currentTarget.targetType == "app" then
			file:write("    <ConfigurationType>Application</ConfigurationType>\n")
		elseif currentTarget.targetType == "staticlib" then 		
			file:write("    <ConfigurationType>StaticLibrary</ConfigurationType>\n")
		end
		--file:write("    <UseDebugLibraries>true</UseDebugLibraries>\n")

		if config.options.msvcgeneral ~= nil then
			for jOption = 1, #config.options.msvcgeneral do
				local keyValue = split(config.options.msvcgeneral[jOption], "=")
				local key = keyValue[1]
				local value = keyValue[2]

				file:write("      <" .. key .. ">" .. value .. "</" .. key .. ">\n")
			end
		end
		
		if config.options.msvcconfigrawxml ~= nil then
			for jOption = 1, #config.options.msvcconfigrawxml do
				local keyValue = config.options.msvcconfigrawxml[jOption]
				file:write(keyValue)
			end
		end
		
		file:write("\n")
		
		file:write("  </PropertyGroup>\n")
	end
	file:write("  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />\n")
	file:write("  <ImportGroup Label=\"ExtensionSettings\">\n")
	if #CompileShaderGroup > 0 then
	    file:write("    <Import Project=\"hlsl.props\" />\n")
	end
	file:write("  </ImportGroup>\n")

	for iConfig = 1, #currentTarget.configs do
		local config = currentTarget.configs[iConfig]

		file:write("  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='" .. config.name .. "|" .. msvcPlatform .. "'\">\n")
		file:write("    <LinkIncremental>false</LinkIncremental>\n") --incremental linking is only ever a source of pain (and corrupt files)
	    file:write("    <ExecutablePath>")
		for jExeDir = 1, #config.exedirs do
			file:write(GetFullFilePath(config.exedirs[jExeDir]) .. ";")
		end
		file:write("$(ExecutablePath)</ExecutablePath>\n")
	    file:write("    <IntDir>" .. Util_FileNormaliseWindows(writer_global.makeoutputdirabs .."\\" .. writer_global.intdir) .. "\\$(ProjectName)\\$(Configuration)\\</IntDir>\n")
	    file:write("    <OutDir>" .. Util_FileNormaliseWindows(writer_global.makeoutputdirabs .."\\" .. writer_global.outdir) .. "\\$(ProjectName)\\</OutDir>\n")
	    file:write("    <TargetName>$(ProjectName)_$(Configuration)</TargetName>\n")
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
			local includeDir = GetFullFilePath(config.includedirs[jIncludeDir])
			file:write(includeDir .. ";")
		end
		file:write("</AdditionalIncludeDirectories>\n")

		file:write("    </ClCompile>\n")
		file:write("    <Link>\n")

		--Lib directories
		file:write("      <AdditionalLibraryDirectories>")
		for jLibDir = 1, #config.libdirs do
			local libDir = GetFullFilePath(config.libdirs[jLibDir])
			file:write(libDir .. ";")
		end
		file:write("</AdditionalLibraryDirectories>\n")

		--Add hardwired options
		--TODO: The below is not ideal, linkage only possible on final app target.
		if currentTarget.targetType == "app" then
	 	    file:write("      <AdditionalDependencies>")
			for jLib = 1, #config.libs do
				local lib = config.libs[jLib]
				file:write(lib .. ";")
			end
	 	    file:write("%(AdditionalDependencies)</AdditionalDependencies>\n")
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

		if #CompileShaderGroup > 0 then		
			file:write("    <CompilerShader/>\n")
--			file:write("      <AdditionalOptions>/LD %(AdditionalOptions)</AdditionalOptions>\n")
--			file:write("    </CompilerShader>\n")	
		end
		
		--Post build event
		file:write("    <PostBuildEvent>\n")
		local postbuild = Util_GetKVValue(config.options.msvc, "postbuild")
		if postbuild ~= nil then
			file:write("	  <Command>" .. postbuild .. "</Command>\n")
		end
		file:write("    </PostBuildEvent>\n")
		
		file:write("  </ItemDefinitionGroup>\n")
	end

	--ClCompile group
	file:write("  <ItemGroup>\n")
	for i = 1, #ClCompileGroup do
		local f = ClCompileGroup[i][1]
		local fIncludedInBuild = ClCompileGroup[i][2]

		file:write("    <ClCompile Include=\"" .. GetFullFilePath(f) .. "\">\n")
		
		for iConfig = 1, #currentTarget.configs do
			local config = currentTarget.configs[iConfig]
			file:write("      <CompileAs Condition=\"'$(Configuration)|$(Platform)'=='" .. config.name .. "|" .. msvcPlatform .. "'\">Default</CompileAs>\n")
		end
		
		--If we're using a pch and this is the pch file
		if pchSourceFile ~= nil and f == pchSourceFile then 
			--For each config ensure we create the pch
			for iConfig = 1, #currentTarget.configs do
				local config = currentTarget.configs[iConfig]
				file:write("      <PrecompiledHeader Condition=\"'$(Configuration)|$(Platform)'=='" .. config.name .. "|" .. msvcPlatform .. "'\">Create</PrecompiledHeader>\n")
			end
		else
			for iConfig = 1, #currentTarget.configs do
				local config = currentTarget.configs[iConfig]
				if fIncludedInBuild == false then 
					file:write("      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='" .. config.name .. "|" .. msvcPlatform .. "'\">true</ExcludedFromBuild>\n")
				end
			end
		end
		file:write("    </ClCompile>\n")
	end	
	file:write("  </ItemGroup>\n")

	--ClInclude group
	file:write("  <ItemGroup>\n")
	for i = 1, #ClIncludeGroup do
		local f = ClIncludeGroup[i][1]
		local fIncludedInBuild = ClIncludeGroup[i][2]

		file:write("    <ClInclude Include=\"" .. GetFullFilePath(f) .. "\">\n")
		for iConfig = 1, #currentTarget.configs do
			local config = currentTarget.configs[iConfig]
			if fIncludedInBuild == false then 
				file:write("      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='" .. config.name .. "|" .. msvcPlatform .. "'\">true</ExcludedFromBuild>\n")
			end
		end		
		file:write("    </ClInclude>\n")
	end	
	file:write("  </ItemGroup>\n")

	--None group
	file:write("  <ItemGroup>\n")
	for i = 1, #NoneGroup do
		local f = NoneGroup[i][1]
		local fIncludedInBuild = NoneGroup[i][2]

		file:write("    <None Include=\"" .. GetFullFilePath(f) .. "\">\n")
		for iConfig = 1, #currentTarget.configs do
			local config = currentTarget.configs[iConfig]
			if fIncludedInBuild == false then 
				file:write("      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='" .. config.name .. "|" .. msvcPlatform .. "'\">true</ExcludedFromBuild>\n")
			end
		end
		file:write("    </None>\n")		
	end	
	file:write("  </ItemGroup>\n")	

	--Shaders
	file:write("  <ItemGroup>\n")
	for i = 1, #CompileShaderGroup do
		local f = CompileShaderGroup[i][1]
		local fIncludedInBuild = CompileShaderGroup[i][2]

		file:write("    <CompilerShader Include=\"" .. GetFullFilePath(f) .. "\">\n")
		for iConfig = 1, #currentTarget.configs do
			local config = currentTarget.configs[iConfig]
			if fIncludedInBuild == false then 
				file:write("      <ExcludedFromBuild Condition=\"'$(Configuration)|$(Platform)'=='" .. config.name .. "|" .. msvcPlatform .. "'\">true</ExcludedFromBuild>\n")
			else
				--TODO: something more robust than the below. Sensible file extensions sound good.
				local profile = "vs_3_0"
				if string.find(f, "Ps.hlsl") ~= nil then
					profile = "ps_3_0"
				end
				file:write("      <TargetProfile Condition=\"'$(Configuration)|$(Platform)'=='" .. config.name .. "|" .. msvcPlatform .. "'\">" .. profile .. "</TargetProfile>\n")
			end
		end
		file:write("    </CompilerShader>\n")		
	end	
	file:write("  </ItemGroup>\n")	


--[[
	if pchFile ~= nil then
		local path, filename, ext = Util_FilePathDecompose(currentTarget.pch)
		file:write("  <ItemGroup>\n")
		file:write("    <ClCompile Include=\"" .. path .. "/" .. filename .. ".cpp\">\n")
		for iConfig = 1, #currentTarget.configs do
			local config = currentTarget.configs[iConfig]
			file:write("      <PrecompiledHeader Condition=\"'$(Configuration)|$(Platform)'=='" .. config.name .. "|" .. msvcPlatform .. "'\">Create</PrecompiledHeader>\n")
		end
		file:write("    </ClCompile>\n")
		file:write("  </ItemGroup>\n")

		file:write("  <ItemGroup>\n")
		file:write("    <ClInclude Include=\"" .. path .. "/" .. filename .. ".h\">\n")
		file:write("    </ClInclude>\n")
		file:write("  </ItemGroup>\n")
	end
]]

	file:write("  <ItemGroup>\n")
	for i = 1, #currentTarget.depends do
		local dependency = currentTarget.depends[i]
		local path, filename, ext = Util_FilePathDecompose(dependency)

		local projectFilename = writer_global.makeoutputdirabs .. "/" .. filename .. ".vcxproj"
		file:write("    <ProjectReference Include=\"" .. projectFilename .. "\">\n")

		local projectID = msvcgetprojectid(filename)
		file:write("      <Project>{" .. projectID .. "}</Project>\n")
		file:write("    </ProjectReference>\n")
	end
	file:write("  </ItemGroup>\n")

	file:write("  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />\n")

	file:write("  <ImportGroup Label=\"ExtensionTargets\">\n")
	if #CompileShaderGroup > 0 then
	    file:write("    <Import Project=\"hlsl.targets\" />\n")
	end
	file:write("  </ImportGroup>\n")

	file:write("</Project>\n")

	file:close()
	reportoutputfile(vcxprojName)
end

function FormatFilterPath(path)
	while string.find(path, "..\\") == 1 do
		path = string.sub(path, 4, length)
	end
	
	return path
end

function WriterVcxProjFilters(currentTarget, ClCompileGroup, ClIncludeGroup, NoneGroup, CompileShaderGroup)
	local vcxProjFiltersFilename = writer_global.makeoutputdirabs .. "/" .. currentTarget.name .. ".vcxproj.filters"
	local file = io.open(vcxProjFiltersFilename, "w")

	file:write("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
	file:write("<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n")

	local folders = {}
	InitFolders(folders, currentTarget.allsourcefiles)

	--Write out filter folders
	file:write("  <ItemGroup>\n")
	for k, v in pairs(folders) do 
		local folder = v
		if folder.shortName ~= "" then 
			local formattedPath = FormatFilterPath(Util_FileNormaliseWindows(folder.relativePath))
			if formattedPath ~= "." then
				file:write("    <Filter Include=\"" .. formattedPath .. "\">\n")
				file:write("      <UniqueIdentifier>{" .. folder.id .. "}</UniqueIdentifier>\n")
				file:write("    </Filter>\n")
			end
		end
	end
	file:write("  </ItemGroup>\n")

	--Write out files to be built
	file:write("  <ItemGroup>\n")
	for iFile = 1, #ClCompileGroup do
		filename = ClCompileGroup[iFile][1]

		file:write("    <ClCompile Include=\"" .. Util_FileNormaliseWindows(GetFullFilePath(filename)) .. "\">\n")
		local path = Util_FilePath(filename)
		path = Util_FileTrimTrailingSlash(path)
		path = Util_FileNormaliseWindows(path)
		file:write("      <Filter>" .. FormatFilterPath(path) .. "</Filter>\n")
		file:write("    </ClCompile>\n")		
	end
	file:write("  </ItemGroup>\n")

	--Write out header files
	file:write("  <ItemGroup>\n")
	for iFile = 1, #ClIncludeGroup do
		filename = ClIncludeGroup[iFile][1]

		file:write("    <ClInclude Include=\"" .. Util_FileNormaliseWindows(GetFullFilePath(filename)) .. "\">\n")
		local path = Util_FilePath(filename)
		path = Util_FileTrimTrailingSlash(path)
		path = Util_FileNormaliseWindows(path)
		file:write("      <Filter>" .. FormatFilterPath(path) .. "</Filter>\n")
		file:write("    </ClInclude>\n")		
	end
	file:write("  </ItemGroup>\n")

	--Write out none build files
	file:write("  <ItemGroup>\n")
	for iFile = 1, #NoneGroup do
		filename = NoneGroup[iFile][1]

		file:write("    <None Include=\"" .. Util_FileNormaliseWindows(GetFullFilePath(filename)) .. "\">\n")
		local path = Util_FilePath(filename)
		path = Util_FileTrimTrailingSlash(path)
		path = Util_FileNormaliseWindows(path)
		file:write("      <Filter>" .. FormatFilterPath(path) .. "</Filter>\n")
		file:write("    </None>\n")		
	end
	file:write("  </ItemGroup>\n")

	--Write out shaders
	file:write("  <ItemGroup>\n")
	for iFile = 1, #CompileShaderGroup do
		filename = CompileShaderGroup[iFile][1]

		file:write("    <CompilerShader Include=\"" .. Util_FileNormaliseWindows(GetFullFilePath(filename)) .. "\">\n")
		local path = Util_FilePath(filename)
		path = Util_FileTrimTrailingSlash(path)
		path = Util_FileNormaliseWindows(path)
		file:write("      <Filter>" .. FormatFilterPath(path) .. "</Filter>\n")
		file:write("    </CompilerShader>\n")		
	end
	file:write("  </ItemGroup>\n")	

	file:write("</Project>\n")

	file:close()
	reportoutputfile(vcxProjFiltersFilename)
end

function WriteSolution(currentTarget)
	if writer_solution.msvcversion == nil then
		-- TODO ERROR HERE
	end

	local msvcVersion = Util_GetKVValue(writer_global.options.msvc, "version")
	local msvcPlatform = Util_GetKVValue(writer_global.options.msvc, "platform")
	--if msvcPlatform == nil then
		-- TODO error
	--end

	local msvcFormatVersion = "11.00" -- Default to 2010 format

	--print(inspect(writer_global.options))
	if msvcVersion == "2012" then
		msvcFormatVersion = "12.00"
	end

	local slnFilename = writer_global.makeoutputdirabs .. "/" .. currentTarget.name .. ".sln"
	local file = io.open(slnFilename, "w")

	file:write("Microsoft Visual Studio Solution File, Format Version " .. msvcFormatVersion .. "\n")
	file:write("# MetaBuilder " .. msvcVersion .. "\n")

	--Create a list of project GUID and name pairs. We'll need this to form the contents of our solution.
	projectList = {}

	--Current target.
	do
		local projectID = msvcgetprojectid(currentTarget.name)
		table.insert(projectList, {projectID, currentTarget.name})
	end

	--Other targets we require.
	for i = 1, #currentTarget.depends do
		local dependency = currentTarget.depends[i]
		local path, filename, ext = Util_FilePathDecompose(dependency)

		local projectID = msvcgetprojectid(filename)	
		table.insert(projectList, {projectID, filename})
	end

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
		end
	end
	file:write("	EndGlobalSection\n")
	file:write("	GlobalSection(SolutionProperties) = preSolution\n")
	file:write("		HideSolutionNode = FALSE\n")
	file:write("	EndGlobalSection\n")
	file:write("EndGlobal\n")

	file:close()
	reportoutputfile(slnFilename)
end

--[[ MAIN ]]

local ClCompileGroup = {}
local ClIncludeGroup = {}
local NoneGroup = {}
local CompileShaderGroup = {}

BuildFileGroups(g_currentTarget, ClCompileGroup, ClIncludeGroup, NoneGroup, CompileShaderGroup)
local enableHLSL = false
if #CompileShaderGroup > 0 then
	enableHLSL = true
end

WriteVcxProj(g_currentTarget, ClCompileGroup, ClIncludeGroup, NoneGroup, CompileShaderGroup)
WriterVcxProjFilters(g_currentTarget, ClCompileGroup, ClIncludeGroup, NoneGroup, CompileShaderGroup)

--Solutions only required by apps
if g_currentTarget.targetType == "app" then
	WriteSolution(g_currentTarget)
end

	--Copy helper files
	if enableHLSL then
		copyfile(writer_global.metabasedirabs .. "/msvc/hlsl.lc",		writer_global.makeoutputdirabs .. "/hlsl.lc")
		copyfile(writer_global.metabasedirabs .. "/msvc/hlsl.props",		writer_global.makeoutputdirabs .. "/hlsl.props")
		copyfile(writer_global.metabasedirabs .. "/msvc/hlsl.targets",	writer_global.makeoutputdirabs .. "/hlsl.targets")
		copyfile(writer_global.metabasedirabs .. "/msvc/hlsl.xml",		writer_global.makeoutputdirabs .. "/hlsl.xml")
	end
