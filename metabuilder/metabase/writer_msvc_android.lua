mbwriter.setmakeoutputdirabs(mbwriter.global.makeoutputdirbaseabs)

local g_simpleFileTypeMap = {}
g_simpleFileTypeMap["c"]					= "ClCompile"
g_simpleFileTypeMap["cpp"]					= "ClCompile"
g_simpleFileTypeMap["h"]					= "ClInclude"
g_simpleFileTypeMap["hpp"]					= "ClInclude"
g_simpleFileTypeMap["inl"]					= "ClInclude"
g_simpleFileTypeMap["java"]					= "JCompile"

local g_androidBuildMagicFiles = {}
g_androidBuildMagicFiles["AndroidManifest.xml"]		= "AndroidBuild"
g_androidBuildMagicFiles["res/values/strings.xml"]	= "AndroidBuild"

function MSVCGetTargetFilenameHook(currentTarget, filename)
	return "lib" .. filename
end

function MSVCRootHook(file)
	file:write("  <PropertyGroup Label=\"NsightTegraProject\">\n")
    file:write("    <NsightTegraProjectRevisionNumber>9</NsightTegraProjectRevisionNumber>\n")
	file:write("  </PropertyGroup>\n")
end

function MSVCGetFileMappingTypeHook(filepath)
	local ext = mbfilepath.getextension(filepath)
	local fileType = g_simpleFileTypeMap[ext]

	if fileType == nil then
		fileType = g_androidBuildMagicFiles[filepath]
	end

	if fileType == nil then
		return "None"
	end

	return fileType
end

function PostGenerateEvent()
end

function MSVCCustomPropertySheets(file)
end

function MSVCCustomGlobal(file)

end
