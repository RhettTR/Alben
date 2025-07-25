	
-- create repository
	
	Repository["German/ge_army01"] = Repository:new({"German/ge_army01", "German/ge_army01_back"})
	Repository["German/ge_army02"] = Repository:new({"German/ge_army02", "German/ge_army02_back"})
	Repository["German/ge_army03"] = Repository:new({"German/ge_army03", "German/ge_army03_back"})
	Repository["French/fr_army03"] = Repository:new({"French/fr_army03", "French/fr_army03_back"})
	Repository["French/fr_army04"] = Repository:new({"French/fr_army04", "French/fr_army04_back"})
	Repository["French/fr_army05"] = Repository:new({"French/fr_army05", "French/fr_army05_back"})
	Repository["British/bef_army"] = Repository:new({"British/bef_army", "British/bef_army_back"})

	Repository["Markers/action_cp1"] = Repository:new({"Markers/action_cp1"})
	Repository["Markers/action_cp2"] = Repository:new({"Markers/action_cp2"})
	Repository["Markers/action_cp3"] = Repository:new({"Markers/action_cp3"})
	Repository["Markers/action_cp4"] = Repository:new({"Markers/action_cp4"})
	Repository["Markers/action_cp5"] = Repository:new({"Markers/action_cp5"})
	Repository["Markers/action_cp6"] = Repository:new({"Markers/action_cp6"})
	
	Repository["Markers/warstatus_ap"] = Repository:new({"Markers/warstatus_ap"})
	Repository["Markers/warstatus_cp"] = Repository:new({"Markers/warstatus_cp"})
	Repository["Markers/warstatus_combined"] = Repository:new({"Markers/warstatus_combined"})
	
	Repository["Allied/ap_trench1"] = Repository:new({"Allied/ap_trench1", "Allied/ap_trench2"})
	
	
	Repository["Central/card_cp02"] = Repository:new({"Central/card_cp02", "Central/cardback_cp"})
	Repository["Central/card_cp03"] = Repository:new({"Central/card_cp03", "Central/cardback_cp"})
	Repository["Central/card_cp06"] = Repository:new({"Central/card_cp06", "Central/cardback_cp"})
	Repository["Central/card_cp12"] = Repository:new({"Central/card_cp12", "Central/cardback_cp"})
	
	
	
-- set traits for all counters
	
	Repository:addTraits("German/ge_army01", German:new())
	Repository:addTraits("German/ge_army02", German:new())
	Repository:addTraits("German/ge_army03", German:new())
	
	Repository:addTraits("French/fr_army03", French:new())
	Repository:addTraits("French/fr_army04", French:new())
	Repository:addTraits("French/fr_army05", French:new())
	
	Repository:addTraits("British/bef_army", British:new())

	Repository:addTrait("Allied/ap_trench1", "Marker", Marker:new("Allied/ap_trench1", -20, 0, "Allied"))
	
	Repository:addTrait("Markers/warstatus_ap", "WhenMoved", WhenMoved:new())
	Repository:addTrait("Markers/warstatus_cp", "WhenMoved", WhenMoved:new())
	Repository:addTrait("Markers/warstatus_combined", "CantBeMoved", CantBeMoved)

	
		
-- set the content of the repository window


root(0)		
			
	tabs(1)
		tab(2, "British")
			create_class(Repository, "British/bef_army")
				
		tab(2, "German")
			create_class(Repository, "German/ge_army01")
			create_class(Repository, "German/ge_army02")
			create_class(Repository, "German/ge_army03")
			
		tab(2, "French")
			create_class(Repository, "French/fr_army03")
			create_class(Repository, "French/fr_army04")
			create_class(Repository, "French/fr_army05")
			create_class(Repository, "Allied/ap_trench1")
			
		tab(2, "Markers")
			create_class(Repository, "Markers/action_cp1")
			create_class(Repository, "Markers/action_cp2")
			create_class(Repository, "Markers/action_cp3")
			create_class(Repository, "Markers/action_cp4")
			create_class(Repository, "Markers/action_cp5")
			create_class(Repository, "Markers/action_cp6")
			
			create_class(Repository, "Markers/warstatus_ap")
			create_class(Repository, "Markers/warstatus_cp")
			create_class(Repository, "Markers/warstatus_combined")
			
		tab(2, "Cards")
			create_class(Repository, "Central/card_cp02")
			create_class(Repository, "Central/card_cp03")
			create_class(Repository, "Central/card_cp06")
			create_class(Repository, "Central/card_cp12")
	

	
return Repository
