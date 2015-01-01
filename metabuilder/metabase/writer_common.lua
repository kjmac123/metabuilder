package.path = package.path .. ";" .. mbwriter_global.metabasedirabs .. "/?.lua"
inspect = require('inspect')

util = require('utility')

if mbwriter_global.verbose then 
	loginfo("mbwriter_global:\n")
	loginfo(inspect(mbwriter_global))
	loginfo("\n")
	loginfo("mbwriter_solution:\n")
	loginfo(inspect(mbwriter_solution))
end

