package.path = package.path .. ";" .. writer_global.metabasedirabs .. "/?.lua"
local inspect = require('inspect')
local util = require('utility')

if writer_global.verbose then 
	print("writer_global:\n")
	print(inspect(writer_global))
	print("\n")
	print("writer_solution:\n")
	print(inspect(writer_solution))
end


--Map relative to absolute path
g_filePathMap = {}

----------------------------------------------------------------------------------------------------------------------------------------------------------------

function GetFullFilePath(filepath)
	local newfilepath = g_filePathMap[filepath]
	if newfilepath == nil then
		return Util_FileConvertToAbsolute({g_filePathMap}, writer_global.currentmetamakedirabs, filepath)
	end

	return newfilepath
end

--[[
function GetWorkspaceDir(currentTargetName, configName)
	return writer_global.makeoutputdirabs .. "/" .. currentTargetName .. "/" .. configName
end
]]

----------------------------------------------------------------------------------------------------------------------------------------------------------------
--FILE WRITING
----------------------------------------------------------------------------------------------------------------------------------------------------------------

function WriteMakeFile(currentTarget, config)

	local makeDir = writer_global.makeoutputdirabs .. "/" .. currentTarget.name .. "/" .. config.name
	mkdir(makeDir)

	print(inspect(config))

	local makeFilename = makeDir .. "/" .. "build.ninja"
	local file = io.open(makeFilename, "w")

	local intdir = writer_global.intdir .. "/" .. currentTarget.name 
	local outdir = writer_global.outdir  

	file:write("intdir = " .. intdir .. "\n")
	file:write("outdir = " .. outdir .. "\n")
	file:write("builddir = $intdir\n")
	file:write("\n")
	file:write("cflags = -Wall\n")
	file:write("ldflags = \n")
	file:write("\n")
	file:write("rule cxx\n")
	file:write("  depfile = $out.d\n")
	file:write("  deps = gcc\n")
	file:write("  command = g++ -MMD -MF $out.d $cflags -c $in -o $out\n")
	file:write("\n")
	file:write("rule link\n")
	file:write("  command = g++ $ldflags -o $out $in $libs\n")
	file:write("\n")

	local filesToLink = {}	
	for i = 1, #currentTarget.files do
		local f = currentTarget.files[i]
		local ext = Util_FileExtension(f)
		if ext == "c" or ext == "cpp" then
			local shortName = Util_FileShortname(f)
			local fileToBuild = GetFullFilePath(f)
		    local fileToLink = Util_FileReplaceExtension(shortName, ext, "o")
		    file:write("build $intdir/" .. fileToLink .. ": cxx " .. fileToBuild .. "\n")
		    table.insert(filesToLink, fileToLink)
		end
	end
	file:write("\n")

	print(inspect(filesToLink))
	local numFilesToLink = #filesToLink
	file:write("build $outdir/" .. currentTarget.name .. "_" .. config.name .. ": link $\n")
	for i = 1, numFilesToLink do
		local fileToLink = filesToLink[i]
		file:write("  $intdir/" .. fileToLink .. " $\n")
	end
	file:write("\n")
	file:close()

	reportoutputfile(makeFilename)	
end

----------------------------------------------------------------------------------------------------------------------------------------------------------------
--MAIN
----------------------------------------------------------------------------------------------------------------------------------------------------------------

local currentTarget = writer_solution.targets[1]

for i = 1, #currentTarget.configs do
	local config = currentTarget.configs[i]
	WriteMakeFile(currentTarget, config)
end

