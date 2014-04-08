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