
-- set the content of the repository window

root(0)

	
	-- create repository
	
	Repository["German/12R-51R"] = Repository:new({"German/12R-51R", "German/12R-51R_f"})
	Repository["German/38-94"] = Repository:new({"German/38-94", "German/38-94_f"})
	Repository["French/BRIGADE-3"] = Repository:new({"French/BRIGADE-3", "French/BRIGADE-3_f"})	
	Repository["French/VII-40"] = Repository:new({"French/VII-40", "French/VII-40_f"})
	Repository["Markers/FLAME"] = Repository:new({"Markers/FLAME"})
	Repository["Markers/GAS"] = Repository:new({"Markers/GAS"})
	
	
	-- set traits for all counters
	
	Repository:addTraits("German/12R-51R", German:new())
	Repository:addTraits("German/38-94", German:new())
	
	Repository:addTraits("French/BRIGADE-3", French:new())
	Repository:addTraits("French/VII-40", French:new())
	
	Repository:addTraits("Markers/FLAME", Base:new())
	Repository:addTraits("Markers/GAS", Base:new())		
		
		
			
	tabs(1)
		tab(2, "German")
			create_class(Repository, "German/12R-51R")
			create_class(Repository, "German/38-94")
			
		tab(2, "French")
			create_class(Repository, "French/BRIGADE-3")
			create_class(Repository, "French/VII-40")
			
		tab(2, "Markers")
			create_class(Repository, "Markers/FLAME")
			create_class(Repository, "Markers/GAS")
	
	
	

	
return Repository
