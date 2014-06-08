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


	file:write("cflags = $\n")
	if config.options.cflags ~= nil then
		for i = 1, #config.options.cflags do
			file:write("  " .. config.options.cflags[i] .. " $\n")
		end
	end

	for i = 1, #config.defines do
		file:write("  -D" .. config.defines[i] .. " $\n")
	end

	for i = 1, #config.includedirs do
		local includeDir = GetFullFilePath(config.includedirs[i])
		print(includeDir)
		file:write("  -I" .. includeDir .. " $\n")
	end
	file:write("\n")

	file:write("ldflags = $\n")
	if config.options.ldflags ~= nil then
		for i = 1, #config.options.ldflags do
			file:write(config.options.ldflags[i] .. " $\n")
		end
	end

	for i = 1, #config.libdirs do
		local libDir = GetFullFilePath(config.libdirs[i])
		file:write("  -L" .. libDir .. " $\n")
	end
	file:write("\n")

	file:write("arflags = $\n")
	if config.options.arflags ~= nil then
		for i = 1, #config.options.arflags do
			file:write("  " .. config.options.arflags[i] .. " $\n")
		end
	end	
	file:write("\n")

	if currentTarget.targetType == "app" then
		file:write("\n")
		file:write("rule cxx\n")
		file:write("  depfile = $out.d\n")
		file:write("  deps = gcc\n")
		file:write("  command = gcc -MMD -MF $out.d $cflags -c $in -o $out\n")
		file:write("\n")
		file:write("rule ar\n")
		--c = create archive
		--r = Insert the files member... into archive (with replacement). This operation differs from q in that any previously existing members are deleted if their names match those being added.
		--s = Write an object-file index into the archive, or update an existing one, even if no other change is made to the archive. 
		--T = Make the specified archive a thin archive.  If it already exists and is a regular archive, the existing members must be present in the same directory as archive.
		file:write("  command = ar $arflags $out $in\n")
		file:write("\n")
		file:write("rule link\n")
		file:write("  command = g++ $ldflags -o $out $in $libs\n")
	end
	file:write("\n")

	--print(inspect(filesToLink))

	for i = 1, #currentTarget.depends do
		local dependency = currentTarget.depends[i]
		local path, filename, ext = Util_FilePathDecompose(dependency)
		local dependsDir = writer_global.makeoutputdirabs .. "/" .. filename .. "/" .. config.name
		local f = dependsDir .. "/build.ninja"

		file:write("subninja " .. f .. "\n")
		file:write("\n")
	end

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

	--Link or create archive
	local numFilesToLink = #filesToLink
	if currentTarget.targetType == "app" then
		file:write("build $outdir/" .. currentTarget.name .. "_" .. config.name .. ": link $\n")
	elseif currentTarget.targetType == "staticlib" then
		file:write("build $outdir/" .. currentTarget.name .. "_" .. config.name .. ".a: ar $\n")
	else
		--TODO - error here
	end

	for i = 1, numFilesToLink do
		local fileToLink = filesToLink[i]
		file:write("  $intdir/" .. fileToLink .. " $\n")
	end

	if currentTarget.targetType == "app" then
		-- Link with required projects
		for i = 1, #currentTarget.depends do
			local dependency = currentTarget.depends[i]
			local path, filename, ext = Util_FilePathDecompose(dependency)
			local f = filename .. "_" .. config.name .. ".a"
			file:write("  $outdir/" .. f .. " $\n")
		end		
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
