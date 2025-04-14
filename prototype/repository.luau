	
-- create repository
	
	Repository["German/12R-51R"] = Repository:new({"German/12R-51R", "German/12R-51R_f"})
	Repository["German/38-94"] = Repository:new({"German/38-94", "German/38-94_f"})
	Repository["German/Artillery/5-77-2"] = Repository:new({"German/Artillery/5-77-2"})
	Repository["German/Artillery/5-150H"] = Repository:new({"German/Artillery/5-150H", "German/Artillery/5-150H_f"})
	Repository["German/Artillery/5-420H-6"] = Repository:new({"German/Artillery/5-420H-6", "German/Artillery/5-420H-6_f"})
	Repository["German/Targets/GermanTarget1"] = Repository:new({"German/Targets/GermanTarget1"})
	Repository["German/Targets/GermanTarget2"] = Repository:new({"German/Targets/GermanTarget2"})
	Repository["French/BRIGADE-3"] = Repository:new({"French/BRIGADE-3", "French/BRIGADE-3_f"})	
	Repository["French/VII-40"] = Repository:new({"French/VII-40", "French/VII-40_f"})
	Repository["Markers/FLAME"] = Repository:new({"Markers/FLAME"})
	Repository["Markers/GAS"] = Repository:new({"Markers/GAS"})
	
	
-- set traits for all counters
	
	Repository:addTraits("German/12R-51R", German:new())
	Repository:addTraits("German/38-94", German:new())
	Repository:addTraits("German/Artillery/5-77-2", German:new())
	Repository:addTraits("German/Artillery/5-150H", German:new())
	Repository:addTraits("German/Artillery/5-420H-6", German:new())
	Repository:addTraits("German/Targets/GermanTarget1", Base:new())
	Repository:addTraits("German/Targets/GermanTarget2", Base:new())
	
	Repository:addTraits("French/BRIGADE-3", French:new())
	Repository:addTraits("French/VII-40", French:new())
	
	Repository:addTraits("Markers/FLAME", Base:new())
	Repository:addTraits("Markers/GAS", Base:new())	
	
	Repository:addTrait("Markers/GAS", "AreaOfEffect", AreaOfEffect:new(0.5, "red", 0.2))
	Repository:addTrait("German/12R-51R", "Clone", Clone:new())	
	Repository:addTrait("French/BRIGADE-3", "Select", Select:new())
	Repository:addTrait("French/VII-40", "DoesNotStack", DoesNotStack)
	--Repository:addTrait("German/38-94", "Visibility", Visibility:new(0.3))
	Repository:addTrait("German/Targets/GermanTarget1", "OnlyOneInstance", OnlyOneInstance)
	Repository:addTrait("German/Targets/GermanTarget2", "OnlyOneInstance", OnlyOneInstance)			 	

	
		
-- set the content of the repository window

root(0)		
			
	tabs(1)
		tab(2, "German")
			listbox(3)
				listitem(4, "Units")
					create_class(Repository, "German/12R-51R")
					create_class(Repository, "German/38-94")
				listitem(4, "Artillery")
					create_class(Repository, "German/Artillery/5-77-2")
					create_class(Repository, "German/Artillery/5-150H")
					create_class(Repository, "German/Artillery/5-420H-6")
				listitem(4, "Targets")
					create_class(Repository, "German/Targets/GermanTarget1")
					create_class(Repository, "German/Targets/GermanTarget2")
			
		tab(2, "French")
			create_class(Repository, "French/BRIGADE-3")
			create_class(Repository, "French/VII-40")
			
		tab(2, "Markers")
			create_class(Repository, "Markers/FLAME")
			create_class(Repository, "Markers/GAS")
	
	
	

	
return Repository
