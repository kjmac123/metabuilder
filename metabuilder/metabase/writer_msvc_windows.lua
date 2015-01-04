function GetFileTypeMap()
	local fileTypeMap = {}
	fileTypeMap["c"]		= "ClCompile"
	fileTypeMap["cpp"]		= "ClCompile"
	fileTypeMap["h"]		= "ClInclude"
	fileTypeMap["hpp"]		= "ClInclude"
	fileTypeMap["inl"]		= "ClInclude"
	fileTypeMap["hlsl"]		= "CompilerShader"
	fileTypeMap["vsh"]		= "CompilerShader"
	fileTypeMap["psh"]		= "CompilerShader"
	fileTypeMap["rc"]		= "ResourceCompile"

	return fileTypeMap;
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

	--print("no custom rule")
	return nil	
end

function MSVCCustomPropertySheets(file)
end

function MSVCCustomGlobal(file)
end