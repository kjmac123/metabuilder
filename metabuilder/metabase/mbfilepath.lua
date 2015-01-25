local module = {}

function module.join(a, b, dirSep)
	if a == "" then
		return b
	end
	if b == "" then
		return a
	end

	module.trimtrailingslash(a)
	module.trimleadingslash(b)

	return a .. dirSep .. b
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

function module.trimtrailingslash(filepath)
	local length = string.len(filepath)
	if length == 0 then
		return filepath
	end

	local lastChar = string.sub(filepath, length, length)
	if lastChar == '\\' or lastChar == '/' then
		return string.sub(filepath, 1, length-1)
	end

	return filepath
end

function module.trimleadingslash(filepath)
	local length = string.len(filepath)
	if length == 0 then
		return filepath
	end

	local firstChar = string.sub(filepath, 1, 1)
	if firstChar == '\\' or firstChar == '/' then
		return string.sub(filepath, 2, length)
	end

	return filepath
end

function module.trimrawmarker(filepath)
	local length = string.len(filepath)
	if length == 0 then
		return filepath
	end

	local firstChar = string.sub(filepath, 1, 1)
	if firstChar == '!' then
		return string.sub(filepath, 2, length)
	end

	return filepath
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

function module.containsdirsep(filepath)
	return string.find(filepath, "/") or string.find(filepath, "\\")
end

return module
