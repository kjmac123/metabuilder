g_Util_GetFullFilePathResults = createtable(0,100000)
g_Util_FileNormaliseResults = createtable(0,100000)

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

function Util_FileConvertToAbsolute(baseDir, filepath, dirSep, filepathMap)
	
	local length = string.len(filepath)
	if length == 0 then
		return filepath
	end

	--Bail if we've marked this filepath to not be expanded
	if Util_FilePathMarkedAsRaw(filepath) then
		return string.sub(filepath, 2, length)
	end

	--Expand!
	local newfilepath = filepathMap[filepath]
	if newfilepath == nil then
		return baseDir .. dirSep .. filepath
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

function Util_FileNormalise(filepath, dirsep)
	local result = g_Util_FileNormaliseResults[filepath]
	if result ~= nil then
		return result
	end

	if dirsep == "\\" then
		result = Util_FileNormaliseWindows(filepath)
	else
		result = Util_FileNormaliseUnix(filepath)
	end

	g_Util_FileNormaliseResults[filepath] = result
	return result
end

----------------------------------------------------------------------------------------------------------------------------------------------------------------

function Util_GetNumDirLevels(dir, dirSep)
	local dirCount = 0

	for c in dir:gmatch"." do
		if c == dirSep then
			dirCount = dirCount + 1
		end
	end
	
	return dirCount
end

function Util_BuildPathBack(nLevels, dirSep)
	local result = ""
	for i = 1, nLevels do
		result = result .. ".." .. dirSep
	end
	
	return result
end

function Util_GetLongestCommonSequenceLengthFromStart(str1, str2)
	local str1Table = {}
	
	for c in str1:gmatch"." do
		table.insert(str1Table, c)
	end

	
	local count = 0
	for c in str2:gmatch"." do
		count = count + 1
		if (str1Table[count] ~= c) then
			break
		end		
	end
	
	if count > 0 then
		count = count + 1
	end
	return count
end

----------------------------------------------------------------------------------------------------------------------------------------------------------------

function Util_GetFullFilePath(filepathUnnormalised, oldBaseDir, newBaseDir, dirSep, filepathMap)
	local result = g_Util_GetFullFilePathResults[filepathUnnormalised]
	if result ~= nil then
		return result
	end

	filepath = Util_FileNormalise(filepathUnnormalised, dirSep)
	oldBaseDir = Util_FileNormalise(oldBaseDir, dirSep)
	newBaseDir = Util_FileNormalise(newBaseDir, dirSep)

	--If filepath has been flagged to escape any kind of modification
	if Util_FilePathMarkedAsRaw(filepath) then
		result = filepath
	else	
		local useRelativePaths = true
	
		result = ""
		if useRelativePaths == true then
			local normalisedFilepathAbs = Util_FileConvertToAbsolute(oldBaseDir, filepath, dirSep, filepathMap)
			local normalisedMakeOutputDirAbs = newBaseDir

			--baseDir is the common path fragment shared by the makefile output directory and 'filepath'
			local baseDir = nil
			do
				local commonSubSequenceLength = Util_GetLongestCommonSequenceLengthFromStart(normalisedFilepathAbs, normalisedMakeOutputDirAbs)
				local commonSubSequence = normalisedFilepathAbs:sub(1, commonSubSequenceLength)
			
				--Look for last dir sep character in order to ignore a partial path or file match
				local lastDirSep = Util_FindLast(commonSubSequence, dirSep)
				if (lastDirSep ~= nil) then
					--Take sequence up to last dir sep as our base dir
					baseDir = commonSubSequence:sub(1, lastDirSep)
				end
			end
		
			if (baseDir ~= nil) then
				local pathFromBaseToOutDir = Util_StringReplace(normalisedMakeOutputDirAbs, baseDir, "")
				local nDirLevels = Util_GetNumDirLevels(dirSep .. pathFromBaseToOutDir, dirSep)
				--Path back from make output dir to base dir
				local pathBack = Util_BuildPathBack(nDirLevels, dirSep)
			
				local filepathBaseRelative = Util_StringReplace(normalisedFilepathAbs, baseDir, "")
				result = pathBack .. filepathBaseRelative
			else
				result = normalisedFilepathAbs
			end
		else
			result = Util_FileConvertToAbsolute(newBaseDir, filepath, dirSep, filepathMap)
		end

		--print("[" .. oldBaseDir .. "] [" .. newBaseDir .. "] " .. result)
		end

	g_Util_GetFullFilePathResults[filepathUnnormalised] = result
	--loginfo("added " .. filepathUnnormalised .. " -> " .. result)
	return result
end

function Util_GetKVValue(keyValueList, key)
	if keyValueList == nil then
		return nil
	end
	
	for i = 1, #keyValueList do
		local keyValue = split(keyValueList[i], "=")
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
