package.path = package.path .. ";" .. mbwriter_global.metabasedirabs .. "/?.lua"
inspect = require('inspect')

util = require('utility')

if mbwriter_global.verbose then 
	print("mbwriter_global:\n")
	print(inspect(mbwriter_global))
	print("\n")
	print("mbwriter_solution:\n")
	print(inspect(mbwriter_solution))
end

