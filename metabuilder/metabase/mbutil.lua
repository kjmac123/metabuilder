local module = {}

function module.getkvvalue(keyValueList, key)
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

function module.kvhaskey(keyValue, key)
	local keyValue = split(keyValue, "=")
	if keyValue[1] == key then
		return true
	end

	return false
end

function module.kvgetvalue(keyValue, value)
	return split(keyValue, "=")[2]
end

function module.kvsetvalue(keyValue, value)
	return split(keyValue, "=")[1] .. "=" .. value
end

return module
