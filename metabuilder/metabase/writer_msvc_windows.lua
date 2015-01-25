local g_windowsFileTypeMap = {}
	g_windowsFileTypeMap["c"]		= "ClCompile"
	g_windowsFileTypeMap["cpp"]		= "ClCompile"
	g_windowsFileTypeMap["h"]		= "ClInclude"
	g_windowsFileTypeMap["hpp"]		= "ClInclude"
	g_windowsFileTypeMap["inl"]		= "ClInclude"
	g_windowsFileTypeMap["hlsl"]	= "CompilerShader"
	g_windowsFileTypeMap["vsh"]		= "CompilerShader"
	g_windowsFileTypeMap["psh"]		= "CompilerShader"
	g_windowsFileTypeMap["rc"]		= "ResourceCompile"

function MSVCGetFileMappingTypeHook(filepath)
	local ext = mbfilepath.getextension(filepath)
	return g_windowsFileTypeMap[ext]
end

function WriteCustomRuleShader(file, target, msvcPlatform, inputfilename, shaderType)
	local entryPoint = nil
	
	for iConfig = 1, #target.configs do
		local config = target.configs[iConfig]

		local profile = nil
		if shaderType == "Vertex" then
			profile = "vs_3_0"
		elseif shaderType == "Pixel" then
			profile = "ps_3_0"
		end
		file:write("      <TargetProfile Condition=\"'$(Configuration)|$(Platform)'=='" .. config.name .. "|" .. msvcPlatform .. "'\">" .. profile .. "</TargetProfile>\n")
	end
end

function WriteCustomRuleFragmentShader(file, target, msvcPlatform, inputfilename)
	WriteCustomRuleShader(file, target, msvcPlatform, inputfilename, "Pixel")
end

function WriteCustomRuleVertexShader(file, target, msvcPlatform, inputfilename)
	WriteCustomRuleShader(file, target, msvcPlatform, inputfilename, "Vertex")
end

function PostGenerateEvent()
	--Copy helper files
	if g_enableHLSL then
		mbwriter.copyfile(mbwriter.global.metabasedirabs .. "/msvc/hlsl.lc",		mbwriter.global.makeoutputdirabs .. "/hlsl.lc")
		mbwriter.copyfile(mbwriter.global.metabasedirabs .. "/msvc/hlsl.props",		mbwriter.global.makeoutputdirabs .. "/hlsl.props")
		mbwriter.copyfile(mbwriter.global.metabasedirabs .. "/msvc/hlsl.targets",	mbwriter.global.makeoutputdirabs .. "/hlsl.targets")
		mbwriter.copyfile(mbwriter.global.metabasedirabs .. "/msvc/hlsl.xml",		mbwriter.global.makeoutputdirabs .. "/hlsl.xml")
	end
end

function GetCustomGroupRule(groupName, inputfilename)
	if string.find(inputfilename, "Ps.hlsl") ~= nil then
		return WriteCustomRuleFragmentShader
	end

	if string.find(inputfilename, "Vs.hlsl") ~= nil then
		return WriteCustomRuleVertexShader
	end

	return nil	
end

function MSVCCustomPropertySheets(file)
end

function MSVCCustomGlobal(file)
end