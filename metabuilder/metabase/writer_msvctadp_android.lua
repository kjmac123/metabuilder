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

function MSVCRootConfigHook(file, config)
	file:write("  <AntBuild>\n")
	
	local proguardEnabled = mbutil.getkvvalue(config.options._android, "ProguardEnabled")
	if proguardEnabled == nil then
		proguardEnabled = "false"
	end
	
	file:write("    <ProGuardConfigLocation>$(ProjectDir)\\proguard-project.txt</ProGuardConfigLocation>\n")
	file:write("    <EnableProGuard>" .. proguardEnabled .. "</EnableProGuard>\n")
	
	local debuggable = mbutil.getkvvalue(config.options._android, "Debuggable")
	if debuggable == nil then
		debuggable = "false"
	end
	file:write("    <Debuggable>" .. debuggable .. "</Debuggable>\n")
	
	if config.options.antbuild ~= nil then
		for jOption = 1, #config.options.antbuild do
			local keyValue = split(config.options.antbuild[jOption], "=")
			local key = keyValue[1]
			local value = keyValue[2]

			file:write("      <" .. key .. ">" .. value .. "</" .. key .. ">\n")
		end
	end
	
	file:write("  </AntBuild>\n")
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

function MSVCGetLibLinkerInputArgHook(config, lib)
	local length = string.len(lib)
	if string.find(lib, "-l") then
		return string.sub(lib, 3, length)
	end

	if mbfilepath.containsdirsep(lib) then
		return mbwriter.getoutputrelfilepath(lib)
	end

	return lib
end

function PostGenerateEvent()
end

function MSVCCustomPropertySheets(file)
end

function MSVCCustomGlobal(file)

end
