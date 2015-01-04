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

return module
