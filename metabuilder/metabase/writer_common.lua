package.path = package.path .. ";" .. mbwriter.global.metabasedirabs .. "/?.lua"

--third party modules
inspect = require('inspect')

--internal modules
mbutil = require('mbutil')
mbstring = require('mbstring')
mbfilepath = require('mbfilepath')

if mbwriter.global.verbose then 
	loginfo("mbwriter.global:\n")
	loginfo(inspect(mbwriter.global))
	loginfo("\n")
	loginfo("mbwriter.solution:\n")
	loginfo(inspect(mbwriter.solution))
end

