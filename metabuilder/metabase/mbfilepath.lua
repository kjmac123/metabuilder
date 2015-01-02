local module = {}

function module.join(path, filename, dirSep)
	if path == "" then
		return filename
	end

	local length = string.len(path)
	local lastChar = string.sub(path, length, length)
	if lastChar == g_utility_dirSep then
		return path .. filename
	end

	return path .. dirSep .. filename
end

function module.decompose(filepath)
	return string.match(filepath, "(.-)([^\\/]-%.?([^%.\\/]*))$")
end

function module.getdir(filepath)
	local path, filename, ext = module.decompose(filepath)
	return path
end

function module.getshortname(filepath)
	local path, filename, ext = module.decompose(filepath)
	return filename
end

function module.getextension(filepath)
	local path, filename, ext = module.decompose(filepath)
	return ext
end

function module.trimtrailingslash(path)
	local length = string.len(path)
	if length == 0 then
		return path
	end

	local lastChar = string.sub(path, length, length)
	if lastChar == '\\' or lastChar == '/' then
		return string.sub(path, 1, length-1)
	end

	return path
end

function module.trimtrailingdot(path)
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

function module.markedasraw(filepath)
	local firstChar = string.sub(filepath, 1, 1)
	if firstChar == "!" then
		return true
	end
	return false
end

function module.removeextension(filepath)
	local path, filename, ext = module.decompose(filepath)

	return path .. filename
end	

return module
