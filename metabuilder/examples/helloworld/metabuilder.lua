makesetup ""
	intdir "int"
	outdir "out"	
makesetup_end()

solution "helloworld"

	target "helloworld"
		target_type "app"

		files
		{
			"main.cpp"
		}

		config "Debug"
			defines 
			{ 
			}
		config_end()

		config "Release"
			defines 
			{ 
			}
		config_end()

		config "Master"
			defines 
			{ 
				--Add custom defines here
			}
		config_end()
		
 	target_end()

 solution_end()
