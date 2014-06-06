package.path = package.path .. ";" .. writer_global.metabasedirabs .. "/?.lua"
inspect = require('inspect')

util = require('utility')

if writer_global.verbose then 
	print("writer_global:\n")
	print(inspect(writer_global))
	print("\n")
	print("writer_solution:\n")
	print(inspect(writer_solution))
end

