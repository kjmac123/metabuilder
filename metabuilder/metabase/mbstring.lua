local module = {}

function module.escapemagic(str)
	luaMagicChars = {"(", ")",".","%","+","-","*","?","[","]","^","$"}

	strlength = string.len(str)
	
	local buf = {}
	for c in str:gmatch"." do
		for jMagic = 1, #luaMagicChars do
			if c == luaMagicChars[jMagic] then
				buf[#buf+1] = "%"
			end
		end
		buf[#buf+1] = c
	end
	
	result = table.concat(buf)		
	return result;			
end

function module.unescapemagic(str)
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
			buf[#buf+1] = c
		end
	end
	
	result = table.concat(buf)		
	return result;			
end

function module.replace(str, oldStr, newStr)
	local escapedStr = module.escapemagic(str)
	local escapedOldStr = module.escapemagic(oldStr)
	
	escapedStr = string.gsub(escapedStr, escapedOldStr, newStr)
	
	return module.unescapemagic(escapedStr)
end

function module.quoted(filepath)
	return '"' .. filepath .. '"'
end

function module.findlast(str, toFind)
	local toFindEscaped = module.escapemagic(toFind)
    local i = str:match(".*" .. toFindEscaped .. "()")
    if i == nil then
		return nil
	end
	
	return i-1
end

return module
