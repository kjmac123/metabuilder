package.path = package.path .. ";" .. mbwriter.global.metabasedirabs .. "/?.lua"
inspect = require('inspect')

util = require('utility')

if mbwriter.global.verbose then 
	loginfo("mbwriter.global:\n")
	loginfo(inspect(mbwriter.global))
	loginfo("\n")
	loginfo("mbwriter.solution:\n")
	loginfo(inspect(mbwriter.solution))
end

