function Util_EscapeMagicLuaChars(str)
	luaMagicChars = {"(", ")",".","%","+","-","*","?","[","]","^","$"}

	strlength = string.len(str)
	
	local buf = {}
	for c in str:gmatch"." do
		for jMagic = 1, #luaMagicChars do
			if c == luaMagicChars[jMagic] then
				table.insert(buf, "%")
			end
		end
		table.insert(buf, c)
	end
	
	result = table.concat(buf)		
	return result;			
end

function Util_UnescapeMagicLuaChars(str)
	luaMagicChars = {"(", ")",".","%","+","-","*","?","[","]","^","$"}

	strlength = string.len(str)
	
	local buf = {}
	for c in str:gmatch"." do
		local foundEscape = false
		for jMagic = 1, #luaMagicChars do
			if c == "%" then
				foundEscape = true
			end
		end
		
		if foundEscape == false then
			table.insert(buf, c)
		end
	end
	
	result = table.concat(buf)		
	return result;			
end

function Util_StringReplace(str, oldStr, newStr)
	local escapedStr = Util_EscapeMagicLuaChars(str)
	local escapedOldStr = Util_EscapeMagicLuaChars(oldStr)
	
	escapedStr = string.gsub(escapedStr, escapedOldStr, newStr)
	
	return Util_UnescapeMagicLuaChars(escapedStr)
end

function Util_FilePathJoin(path, filename)
	if path == "" then
		return filename
	end

	local length = string.len(path)
	local lastChar = string.sub(path, length, length)
	if lastChar == "/" then
		return path .. filename
	end

	return path .. "/" .. filename
end

function Util_FilePathDecompose(filepath)
	return string.match(filepath, "(.-)([^\\/]-%.?([^%.\\/]*))$")
end

function Util_FilePath(filepath)
	local path, filename, ext = Util_FilePathDecompose(filepath)
	return path
end

function Util_FileShortname(filepath)
	local path, filename, ext = Util_FilePathDecompose(filepath)
	return filename
end

function Util_FileExtension(filepath)
	local path, filename, ext = Util_FilePathDecompose(filepath)
	return ext
end

function Util_FileTrimTrailingSlash(path)
	local length = string.len(path)
	if length == 0 then
		return path
	end

	local lastChar = string.sub(path, length, length)
	if lastChar == "/" then
		return string.sub(path, 1, length-1)
	end

	return path
end

function Util_FileTrimTrailingDot(path)
	local length = string.len(path)
	if length == 0 then
		return path
	end

	local lastChar = string.sub(path, length, length)
	if lastChar == "." then
		return string.sub(path, 1, length-1)
	end

	return path
end

function Util_FilePathMarkedAsRaw(filepath)
	local firstChar = string.sub(filepath, 1, 1)
	if firstChar == "!" then
		return true
	end
	return false
end

function Util_FileConvertToAbsolute(filePathMap, baseDir, filepath)
	local length = string.len(filepath)
	if length == 0 then
		return filepath
	end

	--Bail if we've marked this filepath to not be expanded
	if Util_FilePathMarkedAsRaw(filepath) then
		return string.sub(filepath, 2, length)
	end

	--Expand!
	local newfilepath = filePathMap[filepath]
	if newfilepath == nil then
		return baseDir .. "/" .. filepath
	end

	return newfilepath
end

function Util_FileTrimExtension(filepath)
	local path, filename, ext = Util_FilePathDecompose(filepath)

	return path .. filename
end	

function Util_FileNormaliseWindows(filepath)
	return Util_FileTrimTrailingDot(Util_StringReplace(filepath, "/", "\\"))
end

function Util_FileNormaliseUnix(filepath)
	return Util_StringReplace(filepath, "\\", "/")
end

function Util_GetKVValue(keyValueList, key)
	if keyValueList == nil then
		return nil
	end
	
	for i = 1, #keyValueList do
		local keyValue = mbwriter_split(keyValueList[i], "=")
		if keyValue[1] == key then
			local value = keyValue[2]
			return value;
		end
	end

	return nil
end

function Util_FileQuoted(filepath)
	return '"' .. filepath .. '"'
end

function Util_FindLast(str, toFind)
	local toFindEscaped = Util_EscapeMagicLuaChars(toFind)
    local i = str:match(".*" .. toFindEscaped .. "()")
    if i == nil then
		return nil
	end
	
	return i-1
end
