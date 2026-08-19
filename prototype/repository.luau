
-- load the resource ids (keys)

ResourceKeys = {}

function setResourceKey(id)
	table.insert(ResourceKeys, id)
end

get_resource()


-- set number of counter columns in repository

set_row_length(4)



-- repository assignment for regular expressions

local function assign(prefix, side)

	local result = {}
	local used = {}
	
	
	for k, v in ipairs(ResourceKeys) do
	
		local key = string.match(v, prefix)
		local n = table.find(used, v)
		
		
	
		-- resource key must match pattern and not already have been used
		if key ~= nil and n == nil then
		
			local ids = {}	
			
			-- replace meta characters   ^$()%.[]*+-?
			local pattern = string.gsub(key, '%-', '%%-')
					
			for k1, v1 in ipairs(ResourceKeys) do
				local str = string.match(v1, pattern)
				if str ~= nil then
					table.insert(ids, v1)
					table.insert(used, v1)				
				end
			end
			
			
			table.insert(result, ids)
	
		end
		
	end
	
		
	if #result == 0 then
		error(1, prefix)
	end 	
	
	for k, v in ipairs(result) do
		--print('Repository["' .. v[1] .. '"] = ' .. dump(v))
		Repository[v[1]] = Repository:new(v, side)
	end
	
	
	for k, v in ipairs(used) do
		-- NOTE: a key in 'used' is not necessaraly a key in 'ResourceKeys'
		local n = table.find(ResourceKeys, v)
		if n ~= nil then
			table.remove(ResourceKeys, n)
		end 	
	end
	
	
end


local function assignTraits(pattern, key, trait)

	for k, v in pairs(Repository) do
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' then
			local match = string.match(k, pattern)
			if match ~= nil then
				Repository[k][key] = trait:new(k)			
			end
		end
	end
	
end


local function assignClass(pattern)

	local result = {}
	
	for k, v in pairs(Repository) do
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' then
			local key = string.match(k, pattern)
			if key ~= nil then
				table.insert(result, Repository[key])
			end
		end
	end

	table.sort(result, function(a, b)	     
						   return a.Image.images[1] < b.Image.images[1]
					   end)	
			   
	for k, v in ipairs(result) do
		create_class(Repository, v.Image.images[1])
	end
	
	
end


local function assignName(pattern)

	

end




	
-- create repository	

	
	Repository["action_ap1"] = Repository:new({"action_ap1"})
	Repository["action_ap2"] = Repository:new({"action_ap2"})
	Repository["action_ap3"] = Repository:new({"action_ap3"})
	Repository["action_ap4"] = Repository:new({"action_ap4"})
	Repository["action_ap5"] = Repository:new({"action_ap5"})
	Repository["action_ap6"] = Repository:new({"action_ap6"})

	Repository["action_cp1"] = Repository:new({"action_cp1"})
	Repository["action_cp2"] = Repository:new({"action_cp2"})
	Repository["action_cp3"] = Repository:new({"action_cp3"})
	Repository["action_cp4"] = Repository:new({"action_cp4"})
	Repository["action_cp5"] = Repository:new({"action_cp5"})
	Repository["action_cp6"] = Repository:new({"action_cp6"})
	
	
	Repository["warstatus_ap"] = Repository:new({"warstatus_ap"})
	Repository["warstatus_cp"] = Repository:new({"warstatus_cp"})
	Repository["warstatus_combined"] = Repository:new({"warstatus_combined"})
	Repository["bid_ap"] = Repository:new({"bid_ap", "bid_cp"})
	Repository["bid_cp"] = Repository:new({"bid_cp", "bid_ap"})
	Repository["vp"] = Repository:new({"vp"})
	

	Repository["mandoffensive_cp"] = Repository:new({"mandoffensive_cp", "mandoffensive_cp_done"})
	Repository["mandoffensive_ap"] = Repository:new({"mandoffensive_ap", "mandoffensive_ap_done"})
	Repository["turn"] = Repository:new({"turn"})
	Repository["us_entry"] = Repository:new({"us_entry", "us_entry_14points"})
	Repository["russian_capit"] = Repository:new({"russian_capit"})
	
	
	assign('^ge_army%w+', 'Central')
	assign('^ge_corps%w*', 'Central')
	assign('^ah_army%w+', 'Central')
	assign('^ah_corps%w*', 'Central')
	assign('^sn_corps', 'Central')
	assign('^yld_army', 'Central')
	assign('^aoi_army', 'Central')
	assign('^tu_corps', 'Central')
	Repository["cp_ne_sr2"] = Repository:new({"cp_ne_sr2", "cp_ne_sr_used"}, 'Central')
	
	
	
	
	assign('^fr_army%w+', 'Allied')
	assign('^fr_corps%w*', 'Allied')
	assign('^orient_army%w*', 'Allied')
	Repository["bef_army"] = Repository:new({"bef_army", "bef_army_back"}, 'Allied')
	assign('^bef_corps%w*', 'Allied')
	assign('^br_army%w+', 'Allied')
	assign('^mef_army%w*', 'Allied')
	assign('^br_corps%w*', 'Allied')
	assign('^cnd_corps%w*', 'Allied')
	assign('^aus_corps%w*', 'Allied')
	assign('^pt_corps%w*', 'Allied')
	assign('^ana_corps%w*', 'Allied')
	assign('^us_army%w+', 'Allied')
	assign('^us_corps%w*', 'Allied')
	
	
	assign('^be_army', 'Allied')
	assign('^be_corps%w*', 'Allied')
	
	assign('^ru_army%w+', 'Allied')
	assign('^cau_army%w*', 'Allied')
	assign('^ru_corps', 'Allied')
	assign('^ru_cav', 'Allied')
	Repository["ru_ne_move3"] = Repository:new({"ru_ne_move3", "ru_ne_move_used3"}, 'Allied')
	Repository["ru_ne_sr2"] = Repository:new({"ru_ne_sr2", "ru_ne_sr_used2"}, 'Allied')
	Repository["br_ne_sr2"] = Repository:new({"br_ne_sr2", "br_ne_sr_used2"}, 'Allied')
	assign('^czl_corps', 'Allied')
	assign('^sb_army%w+', 'Allied')
	assign('^sb_corps', 'Allied')	
	assign('^ro_corps', 'Allied')
	assign('^mn_corps', 'Allied')
	assign('^it_army%w+', 'Allied')
	assign('^it_corps', 'Allied')
	assign('^bu_corps', 'Central')
	assign('^gr_corps', 'Allied')
	assign('^pol_corps', 'Central')
	
	
	
	Repository["ap_trench1"] = Repository:new({"ap_trench1", "ap_trench2"})
	Repository["ap_trench2"] = Repository:new({"ap_trench2", "ap_trench1"})
	Repository["cp_trench1"] = Repository:new({"cp_trench1", "cp_trench2"})
	Repository["cp_trench2"] = Repository:new({"cp_trench2", "cp_trench1"})		
	Repository["oos_ap"] = Repository:new({"oos_ap", "oos_vp"})
	Repository["oos_cp"] = Repository:new({"oos_cp", "oos_ap"})	
	Repository["trench_mod"] = Repository:new({"trench_mod"})
	Repository["fort_besieged"] = Repository:new({"fort_besieged", "fort_destroyed"})
	Repository["fort_destroyed"] = Repository:new({"fort_destroyed", "fort_besieged"})
	Repository["fort_destroyed_mini"] = Repository:new({"fort_destroyed_mini"})
	Repository["activate_move"] = Repository:new({"activate_move", "activate_attack"})
	Repository["activate_attack"] = Repository:new({"activate_attack", "activate_move"})
	Repository["russianvp_current"] = Repository:new({"russianvp_current"})
	Repository["russianvp_tsarfell"] = Repository:new({"russianvp_tsarfell"})
	
		
	Repository["control_ap"] = Repository:new({"control_ap", "control_cp"})	
	Repository["control_cp"] = Repository:new({"control_cp", "control_ap"})
	
	
	
	Repository["cardback_cp"] = Repository:new({"cardback_cp"})
	Repository["cardback_ap"] = Repository:new({"cardback_ap"})
	
	
	
	
	Repository["card_cp01"] = Repository:new({"card_cp01", "cardback_cp"}, 'Central')
	Repository["card_cp02"] = Repository:new({"card_cp02", "cardback_cp"}, 'Central')
	Repository["card_cp03"] = Repository:new({"card_cp03", "cardback_cp"}, 'Central')
	Repository["card_cp04"] = Repository:new({"card_cp04", "cardback_cp"}, 'Central')
	Repository["card_cp05"] = Repository:new({"card_cp05", "cardback_cp"}, 'Central')
	Repository["card_cp06"] = Repository:new({"card_cp06", "cardback_cp"}, 'Central')
	Repository["card_cp07"] = Repository:new({"card_cp07", "cardback_cp"}, 'Central')
	Repository["card_cp08"] = Repository:new({"card_cp08", "cardback_cp"}, 'Central')
	Repository["card_cp09"] = Repository:new({"card_cp09", "cardback_cp"}, 'Central')
	Repository["card_cp10"] = Repository:new({"card_cp10", "cardback_cp"}, 'Central')
	Repository["card_cp11"] = Repository:new({"card_cp11", "cardback_cp"}, 'Central')
	Repository["card_cp12"] = Repository:new({"card_cp12", "cardback_cp"}, 'Central')
	Repository["card_cp13"] = Repository:new({"card_cp13", "cardback_cp"}, 'Central')
	Repository["card_cp14"] = Repository:new({"card_cp14", "cardback_cp"}, 'Central')
	
	Repository["card_cp15"] = Repository:new({"card_cp15", "cardback_cp"}, 'Central')
	Repository["card_cp16"] = Repository:new({"card_cp16", "cardback_cp"}, 'Central')
	Repository["card_cp17"] = Repository:new({"card_cp17", "cardback_cp"}, 'Central')
	Repository["card_cp18"] = Repository:new({"card_cp18", "cardback_cp"}, 'Central')
	Repository["card_cp19"] = Repository:new({"card_cp19", "cardback_cp"}, 'Central')
	Repository["card_cp20"] = Repository:new({"card_cp20", "cardback_cp"}, 'Central')
	Repository["card_cp21"] = Repository:new({"card_cp21", "cardback_cp"}, 'Central')
	Repository["card_cp22"] = Repository:new({"card_cp22", "cardback_cp"}, 'Central')
	Repository["card_cp23"] = Repository:new({"card_cp23", "cardback_cp"}, 'Central')
	Repository["card_cp24"] = Repository:new({"card_cp24", "cardback_cp"}, 'Central')
	Repository["card_cp25"] = Repository:new({"card_cp25", "cardback_cp"}, 'Central')
	Repository["card_cp26"] = Repository:new({"card_cp26", "cardback_cp"}, 'Central')
	Repository["card_cp27"] = Repository:new({"card_cp27", "cardback_cp"}, 'Central')
	Repository["card_cp28"] = Repository:new({"card_cp28", "cardback_cp"}, 'Central')
	Repository["card_cp29"] = Repository:new({"card_cp29", "cardback_cp"}, 'Central')
	Repository["card_cp30"] = Repository:new({"card_cp30", "cardback_cp"}, 'Central')
	Repository["card_cp31"] = Repository:new({"card_cp31", "cardback_cp"}, 'Central')
	Repository["card_cp32"] = Repository:new({"card_cp32", "cardback_cp"}, 'Central')
	Repository["card_cp33"] = Repository:new({"card_cp33", "cardback_cp"}, 'Central')
	Repository["card_cp34"] = Repository:new({"card_cp34", "cardback_cp"}, 'Central')
	
	Repository["card_cp35"] = Repository:new({"card_cp35", "cardback_cp"}, 'Central')
	Repository["card_cp36"] = Repository:new({"card_cp36", "cardback_cp"}, 'Central')
	Repository["card_cp37"] = Repository:new({"card_cp37", "cardback_cp"}, 'Central')
	Repository["card_cp38"] = Repository:new({"card_cp38", "cardback_cp"}, 'Central')
	Repository["card_cp39"] = Repository:new({"card_cp39", "cardback_cp"}, 'Central')
	Repository["card_cp40"] = Repository:new({"card_cp40", "cardback_cp"}, 'Central')
	Repository["card_cp41"] = Repository:new({"card_cp41", "cardback_cp"}, 'Central')
	Repository["card_cp42"] = Repository:new({"card_cp42", "cardback_cp"}, 'Central')
	Repository["card_cp43"] = Repository:new({"card_cp43", "cardback_cp"}, 'Central')
	Repository["card_cp44"] = Repository:new({"card_cp44", "cardback_cp"}, 'Central')
	Repository["card_cp45"] = Repository:new({"card_cp45", "cardback_cp"}, 'Central')
	Repository["card_cp46"] = Repository:new({"card_cp46", "cardback_cp"}, 'Central')
	Repository["card_cp47"] = Repository:new({"card_cp47", "cardback_cp"}, 'Central')
	Repository["card_cp48"] = Repository:new({"card_cp48", "cardback_cp"}, 'Central')
	Repository["card_cp49"] = Repository:new({"card_cp49", "cardback_cp"}, 'Central')
	Repository["card_cp50"] = Repository:new({"card_cp50", "cardback_cp"}, 'Central')
	Repository["card_cp51"] = Repository:new({"card_cp51", "cardback_cp"}, 'Central')
	Repository["card_cp52"] = Repository:new({"card_cp52", "cardback_cp"}, 'Central')
	Repository["card_cp53"] = Repository:new({"card_cp53", "cardback_cp"}, 'Central')
	Repository["card_cp54"] = Repository:new({"card_cp54", "cardback_cp"}, 'Central')
	Repository["card_cp55"] = Repository:new({"card_cp55", "cardback_cp"}, 'Central')
	
	
	Repository["card_ap01"] = Repository:new({"card_ap01", "cardback_ap"}, 'Allied')
	Repository["card_ap02"] = Repository:new({"card_ap02", "cardback_ap"}, 'Allied')
	Repository["card_ap03"] = Repository:new({"card_ap03", "cardback_ap"}, 'Allied')
	Repository["card_ap04"] = Repository:new({"card_ap04", "cardback_ap"}, 'Allied')
	Repository["card_ap05"] = Repository:new({"card_ap05", "cardback_ap"}, 'Allied')
	Repository["card_ap06"] = Repository:new({"card_ap06", "cardback_ap"}, 'Allied')
	Repository["card_ap07"] = Repository:new({"card_ap07", "cardback_ap"}, 'Allied')
	Repository["card_ap08"] = Repository:new({"card_ap08", "cardback_ap"}, 'Allied')
	Repository["card_ap09"] = Repository:new({"card_ap09", "cardback_ap"}, 'Allied')
	Repository["card_ap10"] = Repository:new({"card_ap10", "cardback_ap"}, 'Allied')
	Repository["card_ap11"] = Repository:new({"card_ap11", "cardback_ap"}, 'Allied')
	Repository["card_ap12"] = Repository:new({"card_ap12", "cardback_ap"}, 'Allied')
	Repository["card_ap13"] = Repository:new({"card_ap13", "cardback_ap"}, 'Allied')
	Repository["card_ap14"] = Repository:new({"card_ap14", "cardback_ap"}, 'Allied')
	
	Repository["card_ap15"] = Repository:new({"card_ap15", "cardback_ap"}, 'Allied')
	Repository["card_ap16"] = Repository:new({"card_ap16", "cardback_ap"}, 'Allied')
	Repository["card_ap17"] = Repository:new({"card_ap17", "cardback_ap"}, 'Allied')
	Repository["card_ap18"] = Repository:new({"card_ap18", "cardback_ap"}, 'Allied')
	Repository["card_ap19"] = Repository:new({"card_ap19", "cardback_ap"}, 'Allied')
	Repository["card_ap20"] = Repository:new({"card_ap20", "cardback_ap"}, 'Allied')
	Repository["card_ap21"] = Repository:new({"card_ap21", "cardback_ap"}, 'Allied')
	Repository["card_ap22"] = Repository:new({"card_ap22", "cardback_ap"}, 'Allied')
	Repository["card_ap23"] = Repository:new({"card_ap23", "cardback_ap"}, 'Allied')
	Repository["card_ap24"] = Repository:new({"card_ap24", "cardback_ap"}, 'Allied')
	Repository["card_ap25"] = Repository:new({"card_ap25", "cardback_ap"}, 'Allied')
	Repository["card_ap26"] = Repository:new({"card_ap26", "cardback_ap"}, 'Allied')
	Repository["card_ap27"] = Repository:new({"card_ap27", "cardback_ap"}, 'Allied')
	Repository["card_ap28"] = Repository:new({"card_ap28", "cardback_ap"}, 'Allied')
	Repository["card_ap29"] = Repository:new({"card_ap29", "cardback_ap"}, 'Allied')
	Repository["card_ap30"] = Repository:new({"card_ap30", "cardback_ap"}, 'Allied')
	Repository["card_ap31"] = Repository:new({"card_ap31", "cardback_ap"}, 'Allied')
	Repository["card_ap32"] = Repository:new({"card_ap32", "cardback_ap"}, 'Allied')
	Repository["card_ap33"] = Repository:new({"card_ap33", "cardback_ap"}, 'Allied')
	Repository["card_ap34"] = Repository:new({"card_ap34", "cardback_ap"}, 'Allied')
	
	Repository["card_ap35"] = Repository:new({"card_ap35", "cardback_ap"}, 'Allied')
	Repository["card_ap36"] = Repository:new({"card_ap36", "cardback_ap"}, 'Allied')
	Repository["card_ap37"] = Repository:new({"card_ap37", "cardback_ap"}, 'Allied')
	Repository["card_ap38"] = Repository:new({"card_ap38", "cardback_ap"}, 'Allied')
	Repository["card_ap39"] = Repository:new({"card_ap39", "cardback_ap"}, 'Allied')
	Repository["card_ap40"] = Repository:new({"card_ap40", "cardback_ap"}, 'Allied')
	Repository["card_ap41"] = Repository:new({"card_ap41", "cardback_ap"}, 'Allied')
	Repository["card_ap42"] = Repository:new({"card_ap42", "cardback_ap"}, 'Allied')
	Repository["card_ap43"] = Repository:new({"card_ap43", "cardback_ap"}, 'Allied')
	Repository["card_ap44"] = Repository:new({"card_ap44", "cardback_ap"}, 'Allied')
	Repository["card_ap45"] = Repository:new({"card_ap45", "cardback_ap"}, 'Allied')
	Repository["card_ap46"] = Repository:new({"card_ap46", "cardback_ap"}, 'Allied')
	Repository["card_ap47"] = Repository:new({"card_ap47", "cardback_ap"}, 'Allied')
	Repository["card_ap48"] = Repository:new({"card_ap48", "cardback_ap"}, 'Allied')
	Repository["card_ap49"] = Repository:new({"card_ap49", "cardback_ap"}, 'Allied')
	Repository["card_ap50"] = Repository:new({"card_ap50", "cardback_ap"}, 'Allied')
	Repository["card_ap51"] = Repository:new({"card_ap51", "cardback_ap"}, 'Allied')
	Repository["card_ap52"] = Repository:new({"card_ap52", "cardback_ap"}, 'Allied')
	Repository["card_ap53"] = Repository:new({"card_ap53", "cardback_ap"}, 'Allied')
	Repository["card_ap54"] = Repository:new({"card_ap54", "cardback_ap"}, 'Allied')
	Repository["card_ap55"] = Repository:new({"card_ap55", "cardback_ap"}, 'Allied')
	
	
	
	Repository["ev_guns_of_august"] = Repository:new({"ev_guns_of_august"})
	Repository["ev_falkenhayn"] = Repository:new({"ev_falkenhayn"})
	Repository["ev_oberost"] = Repository:new({"ev_oberost"})
	Repository["vp_reichstag"] = Repository:new({"vp_reichstag"})
	Repository["ev_entrench"] = Repository:new({"ev_entrench"})
	Repository["ev_race_to_the_sea"] = Repository:new({"ev_race_to_the_sea"})
	Repository["sud_army"] = Repository:new({"sud_army"})
	
	Repository["ev_bulgaria"] = Repository:new({"ev_bulgaria"})
	Repository["ev_walter_rathenau"] = Repository:new({"ev_walter_rathenau"})
	Repository["ev_place_of_execution"] = Repository:new({"ev_place_of_execution"})
	Repository["ev_tsar_takes_command"] = Repository:new({"ev_tsar_takes_command"})
	Repository["ev_war_in_africa"] = Repository:new({"ev_war_in_africa"})
	Repository["ev_zeppelin"] = Repository:new({"ev_zeppelin"})
	Repository["event_11tharmy"] = Repository:new({"event_11tharmy"})
	Repository["vp_fleet"] = Repository:new({"vp_fleet"})
	Repository["ev_haig"] = Repository:new({"ev_haig"})
	Repository["vp_polish"] = Repository:new({"vp_polish"})
	Repository["event_stavka"] = Repository:new({"event_stavka"})
	
	Repository["ev_hl_take_command"] = Repository:new({"ev_hl_take_command"})
	Repository["ev_uboats"] = Repository:new({"ev_uboats"})
	Repository["ev_hoffmann"] = Repository:new({"ev_hoffmann"})
	Repository["ev_michael"] = Repository:new({"ev_michael"})
	Repository["vp_fall_of_the_tsar_1"] = Repository:new({"vp_fall_of_the_tsar_1"})
	Repository["vp_fall_of_the_tsar_3"] = Repository:new({"vp_fall_of_the_tsar_3"})
	Repository["ev_bolshevik"] = Repository:new({"ev_bolshevik"})
	Repository["ev_brest_litovsk"] = Repository:new({"ev_brest_litovsk"})
	Repository["ev_french_mutiny"] = Repository:new({"ev_french_mutiny"})
	Repository["ev_prince_max"] = Repository:new({"ev_prince_max"})
	
	
	Repository["mef_beachhead"] = Repository:new({"mef_beachhead"})
	
	

	Repository["blockade_event"] = Repository:new({"blockade_event"})
	Repository["vp_rape"] = Repository:new({"vp_rape"})
	Repository["ev_british_1"] = Repository:new({"ev_british_1"})
	Repository["ev_british_2"] = Repository:new({"ev_british_2"})
	Repository["ev_entrench_ap"] = Repository:new({"ev_entrench_ap"})
	Repository["ev_moltke"] = Repository:new({"ev_moltke"})
	Repository["ev_italy"] = Repository:new({"ev_italy"})
	Repository["vp_lusitania"] = Repository:new({"vp_lusitania"})
	Repository["ev_great_retreat"] = Repository:new({"ev_great_retreat"})
	Repository["ev_mef"] = Repository:new({"ev_mef"})
	Repository["ev_romania"] = Repository:new({"ev_romania"})
	Repository["ev_salonika"] = Repository:new({"ev_salonika"})
	Repository["vp_grand_fleet"] = Repository:new({"vp_grand_fleet"})
	Repository["ev_landships"] = Repository:new({"ev_landships"})
	Repository["ev_brusilov"] = Repository:new({"ev_brusilov"})
	Repository["vp_zimmermann"] = Repository:new({"vp_zimmermann"})
	Repository["ev_independent_air_force"] = Repository:new({"ev_independent_air_force"})
	Repository["ev_greece"] = Repository:new({"ev_greece"})
	Repository["ev_allenby"] = Repository:new({"ev_allenby"})
	Repository["ev_everyone_into_battle"] = Repository:new({"ev_everyone_into_battle"})
	Repository["vp_14_points"] = Repository:new({"vp_14_points"})
	Repository["vp_convoy"] = Repository:new({"vp_convoy"})
	Repository["ev_overthere"] = Repository:new({"ev_overthere"})
	Repository["sinai_pipeline"] = Repository:new({"sinai_pipeline"})
	Repository["ev_sixtus_affair"] = Repository:new({"ev_sixtus_affair"})
	Repository["event_influenza"] = Repository:new({"event_influenza"})

	
	
	Repository["neutral"] = Repository:new({"neutral"})
	
		
	Repository["vp_1_blockade"] = Repository:new({"vp_1_blockade"})
	Repository["missed_mo_ap"] = Repository:new({"missed_mo_ap"})
	Repository["missed_mo_cp"] = Repository:new({"missed_mo_cp"})
	Repository["vp_italy"] = Repository:new({"vp_italy"})
	Repository["vp_mutiny"] = Repository:new({"vp_mutiny"})
	Repository["vp_peace_offensive"] = Repository:new({"vp_peace_offensive"})
	Repository["vp_war_in_africa"] = Repository:new({"vp_war_in_africa"})
	Repository["vp_peace_terms_minus"] = Repository:new({"vp_peace_terms_minus"})
	Repository["vp_peace_terms_plus"] = Repository:new({"vp_peace_terms_plus"})
	
	Repository["rp_ah"] = Repository:new({"rp_ah"})
	Repository["rp_allied"] = Repository:new({"rp_allied"})
	Repository["rp_br"] = Repository:new({"rp_br"})
	Repository["rp_bu"] = Repository:new({"rp_bu"})
	Repository["rp_fr"] = Repository:new({"rp_fr"})
	Repository["rp_ge"] = Repository:new({"rp_ge"})
	Repository["rp_it"] = Repository:new({"rp_it"})
	Repository["rp_ru"] = Repository:new({"rp_ru"})
	Repository["rp_tu"] = Repository:new({"rp_tu"})
	Repository["rp_us"] = Repository:new({"rp_us"})
	
	
	
	
	
	
-- set traits for all counters
	

	assignTraits('^ge_army%w+', 'CentralArmy', CentralArmy)
	assignTraits('^ge_corps%w*', 'CentralCorps', CentralCorps)
	assignTraits('^ah_army%w+', 'CentralArmy', CentralArmy)
	assignTraits('^ah_corps%w*', 'CentralCorps', CentralCorps)
	assignTraits('^fr_army%w+', 'AlliedArmy', AlliedArmy)
	assignTraits('^orient_army%w*', 'AlliedArmy', AlliedArmy)
	assignTraits('^fr_corps%w*', 'AlliedCorps', AlliedCorps)
	assignTraits('^bef_army%w*', 'AlliedArmy', AlliedArmy)
	assignTraits('^bef_corps%w*', 'AlliedCorps', AlliedCorps)
	assignTraits('^br_army%w+', 'AlliedArmy', AlliedArmy)
	assignTraits('^mef_army%w*', 'AlliedArmy', AlliedArmy)
	assignTraits('^br_corps%w*', 'AlliedCorps', AlliedCorps)
	assignTraits('^be_army', 'AlliedArmy', AlliedArmy)
	assignTraits('^be_corps%w*', 'AlliedCorps', AlliedCorps)
	assignTraits('^cnd_corps%w*', 'AlliedCorps', AlliedCorps)
	assignTraits('^aus_corps%w*', 'AlliedCorps', AlliedCorps)
	assignTraits('^pt_corps%w*', 'AlliedCorps', AlliedCorps)
	assignTraits('^ana_corps%w*', 'AlliedCorps', AlliedCorps)
	assignTraits('^us_army%w+', 'AlliedArmy', AlliedArmy)
	assignTraits('^us_corps%w*', 'AlliedCorps', AlliedCorps)
	
	
	assignTraits('^ru_army%w+', 'AlliedArmy', AlliedArmy)
	assignTraits('^cau_army%w*', 'AlliedArmy', AlliedArmy)
	assignTraits('^ru_corps', 'AlliedCorps', AlliedCorps)
	assignTraits('^ru_cav', 'AlliedCorps', AlliedCorps)
	assignTraits('^czl_corps', 'AlliedCorps', AlliedCorps)
	assignTraits('^sb_army%w+', 'AlliedArmy', AlliedArmy)
	assignTraits('^sb_corps', 'AlliedCorps', AlliedCorps)
	assignTraits('^ro_corps', 'AlliedCorps', AlliedCorps)
	assignTraits('^mn_corps', 'AlliedCorps', AlliedCorps)
	assignTraits('^it_army%w+', 'AlliedArmy', AlliedArmy)
	assignTraits('^it_corps', 'AlliedCorps', AlliedCorps)
	assignTraits('^bu_corps', 'CentralCorps', CentralCorps)
	assignTraits('^gr_corps', 'AlliedCorps', AlliedCorps)
	
	
	
	Repository:addTrait("ap_trench1", "Marker", Marker:new("ap_trench1", -20, 0, "Allied", "bottom"))
	Repository:addTrait("cp_trench1", "Marker", Marker:new("cp_trench1", -20, 0, "Central", "bottom"))
	Repository:addTrait("oos_ap", "Marker", Marker:new("oos_ap", 17, -15, "Allied", "top"))
	Repository:addTrait("oos_cp", "Marker", Marker:new("oos_cp", 17, -15, "Central", "top"))
	Repository:addTrait("trench_mod", "Marker", Marker:new("trench_mod", 2, -28, "", "top"))
	Repository:addTrait('fort_besieged', 'Marker', Marker:new('fort_besieged', 20, 0, '', 'bottom'))
	Repository:addTrait('fort_destroyed', 'Marker', Marker:new('fort_destroyed', 20, 0, '', 'bottom'))
	Repository:addTrait('fort_destroyed_mini', 'Marker', Marker:new('fort_destroyed_mini', -16, -16, '', 'bottom'))
	Repository:addTrait('control_ap', 'Marker', Marker:new('control_ap', 11, -9, 'Allied', 'bottom'))
	Repository:addTrait('control_cp', 'Marker', Marker:new('control_cp', 11, -9, 'Central', 'bottom'))
	Repository:addTrait('activate_move', 'Marker', Marker:new('activate_move', 30, -16, '', 'top'))
	Repository:addTrait('activate_attack', 'Marker', Marker:new('activate_attack', 30, -16, '', 'top'))			
	
	Repository:addTrait('ru_ne_move3', 'Flip', Flip:new('ru_ne_move3'))
	Repository:addTrait('ru_ne_sr2', 'Flip', Flip:new('ru_ne_sr2'))
	Repository:addTrait('cp_ne_sr2', 'Flip', Flip:new('cp_ne_sr2'))	
	Repository:addTrait('br_ne_sr2', 'Flip', Flip:new('br_ne_sr2'))	
	Repository:addTrait('ap_trench1', 'Flip', Flip:new('ap_trench1'))
	Repository:addTrait('cp_trench1', 'Flip', Flip:new('cp_trench1'))
	Repository:addTrait('oos_ap', 'Flip', Flip:new('oos_ap'))
	Repository:addTrait('oos_cp', 'Flip', Flip:new('oos_cp'))
	Repository:addTrait('fort_besieged', 'Flip', Flip:new('fort_besieged'))
	Repository:addTrait('fort_destroyed', 'Flip', Flip:new('fort_destroyed'))
	Repository:addTrait('control_ap', 'Flip', Flip:new('control_ap'))	
	Repository:addTrait('control_cp', 'Flip', Flip:new('control_cp'))
	Repository:addTrait('bid_cp', 'Flip', Flip:new('bid_cp'))
	Repository:addTrait('activate_move', 'Flip', Flip:new('activate_move'))
	Repository:addTrait('activate_attack', 'Flip', Flip:new('activate_attack'))
	
	Repository:addTrait('activate_move', 'Rotate', Rotate:new({facing = Facing}))
	Repository:addTrait('activate_attack', 'Rotate', Rotate:new({facing = Facing}))		
	
	
	local f = function(window, id) return true end
	Repository:addTrait('action_cp1', 'Move', Move:new('Move', 'main', 'cpa', 'Action_CP_1', 'Reset', f))
	Repository:addTrait('action_cp2', 'Move', Move:new('Move', 'main', 'cpa', 'Action_CP_2', 'Reset', f))
	Repository:addTrait('action_cp3', 'Move', Move:new('Move', 'main', 'cpa', 'Action_CP_3', 'Reset', f))
	Repository:addTrait('action_cp4', 'Move', Move:new('Move', 'main', 'cpa', 'Action_CP_4', 'Reset', f))
	Repository:addTrait('action_cp5', 'Move', Move:new('Move', 'main', 'cpa', 'Action_CP_5', 'Reset', f))
	Repository:addTrait('action_cp6', 'Move', Move:new('Move', 'main', 'cpa', 'Action_CP_6', 'Reset', f))
	
	Repository:addTrait("action_cp1", "MoveAll", MoveAll:new('cpa', 'Reset all'))
	Repository:addTrait("action_cp2", "MoveAll", MoveAll:new('cpa', 'Reset all'))
	Repository:addTrait("action_cp3", "MoveAll", MoveAll:new('cpa', 'Reset all'))
	Repository:addTrait("action_cp4", "MoveAll", MoveAll:new('cpa', 'Reset all'))
	Repository:addTrait("action_cp5", "MoveAll", MoveAll:new('cpa', 'Reset all'))
	Repository:addTrait("action_cp6", "MoveAll", MoveAll:new('cpa', 'Reset all'))
	
	
	
	
	Repository:addTrait('action_ap1', 'Move', Move:new('Move', 'main', 'caa', 'Action_AP_1', 'Reset', f))
	Repository:addTrait('action_ap2', 'Move', Move:new('Move', 'main', 'caa', 'Action_AP_2', 'Reset', f))
	Repository:addTrait('action_ap3', 'Move', Move:new('Move', 'main', 'caa', 'Action_AP_3', 'Reset', f))
	Repository:addTrait('action_ap4', 'Move', Move:new('Move', 'main', 'caa', 'Action_AP_4', 'Reset', f))
	Repository:addTrait('action_ap5', 'Move', Move:new('Move', 'main', 'caa', 'Action_AP_5', 'Reset', f))
	Repository:addTrait('action_ap6', 'Move', Move:new('Move', 'main', 'caa', 'Action_AP_6', 'Reset', f))

	Repository:addTrait("action_ap1", "MoveAll", MoveAll:new('caa', 'Reset all'))
	Repository:addTrait("action_ap2", "MoveAll", MoveAll:new('caa', 'Reset all'))
	Repository:addTrait("action_ap3", "MoveAll", MoveAll:new('caa', 'Reset all'))
	Repository:addTrait("action_ap4", "MoveAll", MoveAll:new('caa', 'Reset all'))
	Repository:addTrait("action_ap5", "MoveAll", MoveAll:new('caa', 'Reset all'))
	Repository:addTrait("action_ap6", "MoveAll", MoveAll:new('caa', 'Reset all'))
	
	
	Repository:addTrait("cardback_cp", "Card", Card:new())
	Repository:addTrait("cardback_ap", "Card", Card:new())
	assignTraits('^card_cp%w*', 'Card', Card)
	assignTraits('^card_ap%w*', 'Card', Card)
	
	Repository:addTrait('mandoffensive_cp', 'Flip', Flip:new('mandoffensive_cp'))
	Repository:addTrait('mandoffensive_ap', 'Flip', Flip:new('mandoffensive_ap'))
	Repository:addTrait('us_entry', 'Flip', Flip:new('us_entry'))
	
	Repository:addTrait('neutral', 'Delete', Delete)
	
	
	Repository:addTrait("warstatus_ap", "WhenMoved", WhenMoved:new())
	Repository:addTrait("warstatus_cp", "WhenMoved", WhenMoved:new())
	Repository:addTrait("warstatus_combined", "CantBeMoved", CantBeMoved)
	
	Repository:addTrait("neutral", "CantBeMoved", CantBeMoved)
	
	
	

	
-- set display name for all counters

	
	Repository['action_ap1'].name = 'Allied Action Marker 1'	
	Repository['action_ap2'].name = 'Allied Action Marker 2'
	Repository['action_ap3'].name = 'Allied Action Marker 3'
	Repository['action_ap4'].name = 'Allied Action Marker 4'
	Repository['action_ap5'].name = 'Allied Action Marker 5'
	Repository['action_ap6'].name = 'Allied Action Marker 6'


	Repository['action_cp1'].name = 'Central Action Marker 1'	
	Repository['action_cp2'].name = 'Central Action Marker 2'
	Repository['action_cp3'].name = 'Central Action Marker 3'
	Repository['action_cp4'].name = 'Central Action Marker 4'
	Repository['action_cp5'].name = 'Central Action Marker 5'
	Repository['action_cp6'].name = 'Central Action Marker 6'
		
	
	Repository["warstatus_ap"].name = 'Allied War Status'
	Repository["warstatus_cp"].name = 'Central War Status'
	Repository["warstatus_combined"].name = 'Combined War Status'
	Repository["bid_ap"].name = 'Allied Bid'
	Repository["bid_cp"].name = 'Central Bid'
	Repository["vp"].name = 'Victory Points'
	

	Repository["mandoffensive_cp"].name = 'Central Mandatory Offensive'
	Repository["mandoffensive_ap"].name = 'Allied Mandatory Offensive'
	Repository["turn"].name = 'Game Turn'
	Repository["us_entry"].name = 'US Entry Marker'
	Repository["russian_capit"].name = 'Russian Capitulation Marker'
	
	
	Repository['ge_army01'].name = '1 German Army'
	--Repository['ge_army01_back'].name = '1 German Army Reduced'	
	Repository['ge_army02'].name = '2 German Army'
	Repository['ge_army03'].name = '3 German Army'
	Repository['ge_army04'].name = '4 German Army'
	Repository['ge_army05'].name = '5 German Army'
	Repository['ge_army06'].name = '6 German Army' 
	Repository['ge_army07'].name = '7 German Army'
	Repository['ge_army08'].name = '8 German Army'
	Repository['ge_army09'].name = '9 German Army' 
	Repository['ge_army10'].name = '10 German Army'
	Repository['ge_army11'].name = '11 German Army'
	Repository['ge_army12'].name = '12 German Army'
	Repository['ge_army14'].name = '14 German Army'
	Repository['ge_army17'].name = '17 German Army'
	Repository['ge_army18'].name = '18 German Army'
	Repository['ge_corps'].name = 'German Corps'  		 
	
	Repository['ah_army01'].name = '1 Austro-Hungarian Army'
	Repository['ah_army02'].name = '2 Austro-Hungarian Army'
	Repository['ah_army03'].name = '3 Austro-Hungarian Army'
	Repository['ah_army04'].name = '4 Austro-Hungarian Army'
	Repository['ah_army05'].name = '5 Austro-Hungarian Army'
	Repository['ah_army06'].name = '6 Austro-Hungarian Army'
	Repository['ah_army07'].name = '7 Austro-Hungarian Army'
	Repository['ah_army10'].name = '10 Austro-Hungarian Army'
	Repository['ah_army11'].name = '11 Austro-Hungarian Army'
	Repository['ah_corps'].name = 'Austro-Hungarian Corps'
	
	Repository['sn_corps'].name = 'Turkish Cavalry Corps'
	Repository['yld_army'].name = 'Turkish Yilderim Army'
	Repository['aoi_army'].name = 'Turkish Army Of Islam'
	Repository['tu_corps'].name = 'Turkish Corps'
	
	Repository['fr_army01'].name = '1 French Army'
	Repository['fr_army02'].name = '2 French Army'
	Repository['fr_army03'].name = '3 French Army'
	Repository['fr_army04'].name = '4 French Army'
	Repository['fr_army05'].name = '5 French Army'
	Repository['fr_army06'].name = '6 French Army'
	Repository['fr_army07'].name = '7 French Army'
	Repository['fr_army09'].name = '9 French Army'
	Repository['fr_army10'].name = '10 French Army'
	Repository['fr_corps'].name = 'French Corps'
	Repository['orient_army'].name = 'French Orient Army'
	
	
	Repository['bef_army'].name = 'British Expeditionary Force'
	Repository['br_army01'].name = '1 British Army'
	Repository['br_army02'].name = '2 British Army'
	Repository['br_army03'].name = '3 British Army'
	Repository['br_army04'].name = '4 British Army'
	Repository['br_army05'].name = '5 British Army'
	Repository['mef_army'].name = 'British Middle East Force'
	Repository['br_corps'].name = 'British Corps'
	Repository['cnd_corps'].name = 'Canadian Corps'
	Repository['aus_corps'].name = 'Australian Corps'
	Repository['pt_corps'].name = 'Portuguese Corps'
	Repository['ana_corps'].name = 'Arab Northern Army'
	
	Repository['us_army01'].name = '1 US Army'
	Repository['us_army02'].name = '2 US Army'
	Repository['us_corps'].name = 'US Corps'
	
	Repository['be_army'].name = '1 Belgian Army'
	Repository['be_corps'].name = 'Belgian Corps'
	
	Repository['ru_army01'].name = '1 Russian Army'
	Repository['ru_army02'].name = '2 Russian Army'
	Repository['ru_army03'].name = '3 Russian Army'
	Repository['ru_army04'].name = '4 Russian Army'
	Repository['ru_army05'].name = '5 Russian Army'
	Repository['ru_army06'].name = '6 Russian Army'
	Repository['ru_army07'].name = '7 Russian Army'
	Repository['ru_army08'].name = '8 Russian Army'
	Repository['ru_army09'].name = '9 Russian Army'
	Repository['ru_army10'].name = '10 Russian Army'
	Repository['ru_army11'].name = '11 Russian Army'
	Repository['ru_army12'].name = '12 Russian Army'
	Repository['cau_army'].name = 'Russian Caucasus Army'
	Repository['ru_corps'].name = 'Russian Corps'
	Repository['ru_cav'].name = 'Russian Cavalry Corps'
	Repository['czl_corps'].name = 'Czech Legion'
	
	Repository['sb_army01'].name = '1 Serbian Army'
	Repository['sb_army02'].name = '2 Serbian Army'
	Repository['sb_corps'].name = 'US Corps'
	
	Repository['ro_corps'].name = 'Romanian Corps'
	
	Repository['mn_corps'].name = 'Montenegrian Corps'
	
	Repository['it_army01'].name = '1 Italian Army'
	Repository['it_army02'].name = '2 Italian Army'
	Repository['it_army03'].name = '3 Italian Army'
	Repository['it_army04'].name = '4 Italian Army'
	Repository['it_army05'].name = '5 Italian Army'
	Repository['it_corps'].name = 'Italian Corps'
	
	Repository['gr_corps'].name = 'Greek Corps'
	
	Repository['bu_corps'].name = 'Bulgarian Corps'
	
	Repository['pol_corps'].name = 'Polish Corps'
	
	
	Repository["ap_trench1"].name = 'Allied Trench Level 1'
	Repository["ap_trench2"].name = 'Allied Trench Level 2'
	Repository["cp_trench1"].name = 'Central Trench Level 1'
	Repository["cp_trench2"].name = 'Central Trench Level 2'
	Repository["activate_move"].name = 'Move Marker'
	Repository["activate_attack"].name = 'Attack Marker'
	Repository["ru_ne_move3"].name = 'Russian NE non-SR 1-per-turn'
	Repository["ru_ne_sr2"].name ='Russian NE SR 1-per-turn'
	Repository["cp_ne_sr2"].name = 'CP NE SR 1-per-turn'
	Repository["br_ne_sr2"].name = 'British NE SR'
	
	Repository["oos_ap"].name = 'Out Of Supply (Allied)'  
	Repository["oos_cp"].name = 'Out Of Supply (Central)'
	
	Repository["trench_mod"].name = 'Trench Roll Modification'
	Repository["fort_besieged"].name = 'Fort Besieged'
	Repository["fort_destroyed"].name = 'Fort Destroyed'
	Repository["fort_destroyed_mini"].name = 'Fort Destroyed (mini)'
		
	Repository["control_ap"].name = 'Allied Control'	
	Repository["control_cp"].name = 'Central Control'	
	
	
	
	Repository["cardback_cp"].name = 'Central Card'
	Repository["cardback_ap"].name = 'Allied Card'
	
	Repository["card_cp01"].name = 'Guns Of August'
	Repository["card_cp02"].name = 'Wireless Intercepts'
	Repository["card_cp03"].name = 'Von Francois'
	Repository["card_cp04"].name = 'Severe Weather'
	Repository["card_cp05"].name = 'Landwehr'
	Repository["card_cp06"].name = 'Entrench'
	Repository["card_cp07"].name = 'German Reinforcements'
	Repository["card_cp08"].name = 'Race To The Sea'
	Repository["card_cp09"].name = 'Reichstags Truce'
	Repository["card_cp10"].name = 'Sud Army'
	Repository["card_cp11"].name = 'Oberost'
	Repository["card_cp12"].name = 'German Reinforcements'
	Repository["card_cp13"].name = 'Falkenhayn'
	Repository["card_cp14"].name = 'Austria-Hungary Reinforcements'
	
	Repository["card_cp15"].name = 'Chlorine Gas'
	Repository["card_cp16"].name = 'Liman Von Sanders'
	Repository["card_cp17"].name = 'Mata Hari'
	Repository["card_cp18"].name = 'Fortified Machine Guns'
	Repository["card_cp19"].name = 'Flamethrowers'
	Repository["card_cp20"].name = 'Austria-Hungary Reinforcements'
	Repository["card_cp21"].name = 'German Reinforcements'
	Repository["card_cp22"].name = 'German Reinforcements'
	Repository["card_cp23"].name = 'Austria-Hungary Reinforcements'
	Repository["card_cp24"].name = 'Libyan revolt'
	Repository["card_cp25"].name = 'High Seas Fleet'
	Repository["card_cp26"].name = 'Place Of Execution'
	Repository["card_cp27"].name = 'Zeppelin Raids'
	Repository["card_cp28"].name = 'Tsar Takes Command'
	Repository["card_cp29"].name = '11th Army'
	Repository["card_cp30"].name = 'Alpenkorps'
	Repository["card_cp31"].name = 'Kemal'
	Repository["card_cp32"].name = 'War In Africa'
	Repository["card_cp33"].name = 'Walter Rathenau'
	Repository["card_cp34"].name = 'Bulgaria'
	
	Repository["card_cp35"].name = 'Mustard Gas'
	Repository["card_cp36"].name = 'U-Boats Unleashed'
	Repository["card_cp37"].name = 'Hoffmann'
	Repository["card_cp38"].name = 'German Reinforcements'
	Repository["card_cp39"].name = 'German Reinforcements'
	Repository["card_cp40"].name = 'Air Superiority'
	Repository["card_cp41"].name = 'German Reinforcements'
	Repository["card_cp42"].name = 'Turkish Reinforcements'
	Repository["card_cp43"].name = 'Von Below'
	Repository["card_cp44"].name = 'Von Hutier'
	Repository["card_cp45"].name = 'Treaty Of Brest Litovsk'
	Repository["card_cp46"].name = 'German Reinforcements'
	Repository["card_cp47"].name = 'French Mutiny'
	Repository["card_cp48"].name = 'Turkish Reinforcements'
	Repository["card_cp49"].name = 'Michael'
	Repository["card_cp50"].name = 'Blucher'
	Repository["card_cp51"].name = 'Peace Offensive'
	Repository["card_cp52"].name = 'Fall Of The Tsar'
	Repository["card_cp53"].name = 'Bolshevik Revolution'
	Repository["card_cp54"].name = 'H-L Take Command'
	Repository["card_cp55"].name = 'Lloyd George'
	
	
	
	
	Repository["card_ap01"].name = 'Britsh Reinforcements'
	Repository["card_ap02"].name = 'Blockade'
	Repository["card_ap03"].name = 'Russian Reinforcements'
	Repository["card_ap04"].name = 'Pleve'
	Repository["card_ap05"].name = 'Putnik'
	Repository["card_ap06"].name = 'Withdrawal'
	Repository["card_ap07"].name = 'Severe Weather'
	Repository["card_ap08"].name = 'Russian Reinforcements'
	Repository["card_ap09"].name = 'Moltke'
	Repository["card_ap10"].name = 'French Reinforcements'
	Repository["card_ap11"].name = 'Russian Reinforcements'
	Repository["card_ap12"].name = 'Entrench'
	Repository["card_ap13"].name = 'Rape Of Belgium'
	Repository["card_ap14"].name = 'British Reinforcements'
	
	Repository["card_ap15"].name = 'British Reinforcements'
	Repository["card_ap16"].name = 'Romania'
	Repository["card_ap17"].name = 'Italy'
	Repository["card_ap18"].name = 'Hurricane Barrage'
	Repository["card_ap19"].name = 'Air Superiority'
	Repository["card_ap20"].name = 'British Reinforcements'
	Repository["card_ap21"].name = 'Phosgene Gas'
	Repository["card_ap22"].name = 'Italian Reinforcements'
	Repository["card_ap23"].name = 'Cloak And Dagger'
	Repository["card_ap24"].name = 'French Reinforcements'
	Repository["card_ap25"].name = 'Russian Reinforcements'
	Repository["card_ap26"].name = 'Lusitania'
	Repository["card_ap27"].name = 'Great Retreat'
	Repository["card_ap28"].name = 'Landships'
	Repository["card_ap29"].name = 'Yudenitch'
	Repository["card_ap30"].name = 'Salonika'
	Repository["card_ap31"].name = 'MEF'
	Repository["card_ap32"].name = 'Russian Reinforcements'
	Repository["card_ap33"].name = 'Grand Fleet'
	Repository["card_ap34"].name = 'British Reinforcements'
	
	Repository["card_ap35"].name = 'Yanks And Tanks'
	Repository["card_ap36"].name = 'Mine Attack'
	Repository["card_ap37"].name = 'Independent Air Force'
	Repository["card_ap38"].name = 'USA Reinforcements'
	Repository["card_ap39"].name = 'They Shall Not pass'
	Repository["card_ap40"].name = '14 Points'
	Repository["card_ap41"].name = 'Arab Northern Army'
	Repository["card_ap42"].name = 'British Reinforcements'
	Repository["card_ap43"].name = 'USA Reinforcements'
	Repository["card_ap44"].name = 'Greece'
	Repository["card_ap45"].name = 'Kerensky Offensive'
	Repository["card_ap46"].name = 'Brusilov Offensive'
	Repository["card_ap47"].name = 'USA Reinforcements'
	Repository["card_ap48"].name = 'Royal Tank Corps'
	Repository["card_ap49"].name = 'Sinai Pipeline'
	Repository["card_ap50"].name = 'Allenby'
	Repository["card_ap51"].name = 'Everyone Into Battle'
	Repository["card_ap52"].name = 'Convoy'
	Repository["card_ap53"].name = 'Army Of The Orient'
	Repository["card_ap54"].name = 'Zimmermann Telegram'
	Repository["card_ap55"].name = 'Over There'
	

	
	
	Repository["ev_guns_of_august"].name = 'Guns Of August Marker'
	Repository["ev_falkenhayn"].name = 'Falkenhayn Marker'
	Repository["ev_oberost"].name = 'Oberost Marker'
	Repository["vp_reichstag"].name = 'Reichstag Truce Marker'
	Repository["ev_entrench"].name = 'Entrench Marker'
	Repository["ev_race_to_the_sea"].name = 'Race To The Sea Marker'
	Repository["sud_army"].name = 'Sud Army Marker'
	
	Repository["ev_bulgaria"].name = 'Bulgaria Marker'
	Repository["ev_walter_rathenau"].name = 'Walter Rathenau Marker'
	Repository["ev_place_of_execution"].name = 'Place Of Execution Marker'
	Repository["ev_tsar_takes_command"].name = 'Tsar Takes Command Marker'
	Repository["ev_war_in_africa"].name = 'War In Africa Marker'
	Repository["ev_zeppelin"].name = 'Zeppelin Marker'
	Repository["event_11tharmy"].name = '11th Army Marker'
	Repository["vp_fleet"].name = 'Fleet Marker'
	Repository["ev_haig"].name = 'Haig Marker'
	Repository["vp_polish"].name = 'Polish Marker'
	Repository["event_stavka"].name = 'Stavka Marker'
	
	Repository["ev_hl_take_command"].name = 'H-L Take Command Marker'
	Repository["ev_uboats"].name = 'U-Boats Marker'
	Repository["ev_hoffmann"].name = 'Hoffmann Marker'
	Repository["ev_michael"].name = 'Michael Marker'
	Repository["vp_fall_of_the_tsar_1"].name = 'Fall Of The Tsar 1 Marker'
	Repository["vp_fall_of_the_tsar_3"].name = 'Fall Of The Tsar 3 Marker'
	Repository["ev_bolshevik"].name = 'Bolshevik Marker'
	Repository["ev_brest_litovsk"].name = 'Brest-Litovsk Marker'
	Repository["ev_french_mutiny"].name = 'French Mutiny Marker'
	Repository["ev_prince_max"].name = 'Prince Max Marker'
	
	
	Repository["mef_beachhead"].name = 'Beach Head'
	
	
	
	Repository["blockade_event"].name = 'Blockade Marker'
	Repository["vp_rape"].name = 'Belgian Rape Marker'
	Repository["ev_british_1"].name = 'British 1st Army Marker'
	Repository["ev_british_2"].name = 'British 2nd Army Marker'
	Repository["ev_entrench_ap"].name = 'Entrench Marker'
	Repository["ev_moltke"].name = 'Moltke Marker'
	Repository["ev_italy"].name = 'Italy Marker'
	Repository["vp_lusitania"].name = 'Lusitania Marker'
	Repository["ev_great_retreat"].name = 'Great Retreat Marker'
	Repository["ev_mef"].name = 'MEF Marker'
	Repository["ev_romania"].name = 'Romania Marker'
	Repository["ev_salonika"].name = 'Salonika Marker'
	Repository["vp_grand_fleet"].name = 'Grand Fleet Marker'
	Repository["ev_landships"].name = 'Landships Marker'
	Repository["ev_brusilov"].name = 'Brusilov Marker'
	Repository["vp_zimmermann"].name = 'Zimmermann Marker'
	Repository["ev_independent_air_force"].name = 'Idependent Air Force Marker'
	Repository["ev_greece"].name = 'Greece Marker'
	Repository["ev_allenby"].name = 'Allenby Marker'
	Repository["ev_everyone_into_battle"].name = 'Everyone Into Battle Marker'
	Repository["vp_14_points"].name = '14 Points Marker'
	Repository["vp_convoy"].name = 'Convoy Marker'
	Repository["ev_overthere"].name = 'Over There Marker'
	Repository["sinai_pipeline"].name = 'Pipeline Marker'
	Repository["ev_sixtus_affair"].name = 'Sixtus Marker'
	Repository["event_influenza"].name = 'Influenza Marker'
	
	
	Repository["neutral"].name = 'Neutral Marker'
	
	
	Repository["vp_1_blockade"].name = 'Blockade VP Marker'
	Repository["missed_mo_ap"].name = 'Missed MO VP Marker (Allied)'
	Repository["missed_mo_cp"].name = 'Missed MO VP Marker (Central)'
	Repository["vp_italy"].name = 'Italy Late VP Marker'
	Repository["vp_mutiny"].name = 'French Mutiny VP Marker'
	Repository["vp_peace_offensive"].name = 'Peace Offensive VP Marker'
	Repository["vp_war_in_africa"].name = 'War In Africa VP Marker'
	Repository["vp_peace_terms_minus"].name = 'Peace Terms VP Marker (Allied)'
	Repository["vp_peace_terms_plus"].name = 'Peace Terms VP Marker (Central)'
	
	Repository["rp_ah"].name = 'Austro-Hungarian Replacements'
	Repository["rp_allied"].name = 'Allied Replacements'
	Repository["rp_br"].name = 'British Replacements'
	Repository["rp_bu"].name = 'Bulgarian Replacements'
	Repository["rp_fr"].name = 'French Replacements'
	Repository["rp_ge"].name = 'German Replacements'
	Repository["rp_it"].name = 'Italian Replacements'
	Repository["rp_ru"].name = 'Russian Replacements'
	Repository["rp_tu"].name = 'Turkish Replacements'
	Repository["rp_us"].name = 'US Replacements'
	
	
		

		
-- set the content of the repository window


set_root('Repository')


root(0)	
		
		
	tabs(1)
	
		tab(2, "Allied")
			
			tabs(3)
			
				tab(4, "Cards")
				
					listbox(5)
					
						listitem(6, "Mobilization")
						
							create_class(Repository, 'cardback_ap')
							create_class(Repository, 'card_ap01')
							create_class(Repository, 'card_ap02')
							create_class(Repository, 'card_ap03')
							create_class(Repository, 'card_ap04')
							create_class(Repository, 'card_ap05')
							create_class(Repository, 'card_ap06')
							create_class(Repository, 'card_ap07')
							create_class(Repository, 'card_ap08')
							create_class(Repository, 'card_ap09')
							create_class(Repository, 'card_ap10')
							create_class(Repository, 'card_ap11')
							create_class(Repository, 'card_ap12')
							create_class(Repository, 'card_ap13')
							create_class(Repository, 'card_ap14')
							
							scale_pane(0.2)
						
						listitem(6, "Limited War")
						
							create_class(Repository, 'card_ap15')
							create_class(Repository, 'card_ap16')
							create_class(Repository, 'card_ap17')
							create_class(Repository, 'card_ap18')
							create_class(Repository, 'card_ap19')
							create_class(Repository, 'card_ap20')
							create_class(Repository, 'card_ap21')
							create_class(Repository, 'card_ap22')
							create_class(Repository, 'card_ap23')
							create_class(Repository, 'card_ap24')
							create_class(Repository, 'card_ap25')
							create_class(Repository, 'card_ap26')
							create_class(Repository, 'card_ap27')
							create_class(Repository, 'card_ap28')
							create_class(Repository, 'card_ap29')
							create_class(Repository, 'card_ap30')
							create_class(Repository, 'card_ap31')
							create_class(Repository, 'card_ap32')
							create_class(Repository, 'card_ap33')
							create_class(Repository, 'card_ap34')
							
							scale_pane(0.2)
	
						listitem(6, "Total War")
						
							create_class(Repository, 'card_ap35')
							create_class(Repository, 'card_ap36')
							create_class(Repository, 'card_ap37')
							create_class(Repository, 'card_ap38')
							create_class(Repository, 'card_ap39')
							create_class(Repository, 'card_ap40')
							create_class(Repository, 'card_ap41')
							create_class(Repository, 'card_ap42')
							create_class(Repository, 'card_ap43')
							create_class(Repository, 'card_ap44')
							create_class(Repository, 'card_ap45')
							create_class(Repository, 'card_ap46')
							create_class(Repository, 'card_ap47')
							create_class(Repository, 'card_ap48')
							create_class(Repository, 'card_ap49')
							create_class(Repository, 'card_ap50')
							create_class(Repository, 'card_ap51')
							create_class(Repository, 'card_ap52')
							create_class(Repository, 'card_ap53')
							create_class(Repository, 'card_ap54')
							create_class(Repository, 'card_ap55')
							
							scale_pane(0.2)
							
							
				tab(4, "Counters")
				
					listbox(5)
					
						listitem(6, "British")
					
							create_class(Repository, 'bef_army')
							assignClass('^br_army%w+')
							create_class(Repository, 'mef_army')
							assignClass('^br_corps%w*')	
							assignClass('^bef_corps%w*')
							create_class(Repository, 'cnd_corps')
							create_class(Repository, 'aus_corps')
							create_class(Repository, 'pt_corps')
							create_class(Repository, 'ana_corps')
							
					
						listitem(6, "French")
				
							assignClass('^fr_army%w+')
							assignClass('^orient_army%w*')
							assignClass('^fr_corps%w*')
				
						listitem(6, "Russian")
						
							assignClass('^ru_army%w+')							
							create_class(Repository, 'cau_army')
							assignClass('^ru_corps')
							assignClass('^ru_cav')
							assignClass('^czl_corps')						
						
						listitem(6, "Belgian")
						
							assignClass('^be_army')
							assignClass('^be_corps%w*')
						
						listitem(6, "Serbian")
						
							assignClass('^sb_army%w+')
							assignClass('^sb_corps')
						
						listitem(6, "Montenegrian")
						
							assignClass('^mn_corps')
						
						listitem(6, "Italian")
						
							assignClass('^it_army%w+')
							assignClass('^it_corps')
						
						listitem(6, "Rumanian")
						
							assignClass('^ro_corps')
						
						listitem(6, "Greek")
						
							assignClass('^gr_corps')
						
						listitem(6, "American")
						
							assignClass('^us_army%w+')
							assignClass('^us_corps%w*')
						
						
				tab(4, "Markers")
				
						listbox(5)
						
							listitem(6, "General")
				
								create_class(Repository, 'ap_trench1')
								create_class(Repository, 'control_ap')
								create_class(Repository, 'ru_ne_move3')
								create_class(Repository, 'ru_ne_sr2')
								create_class(Repository, 'br_ne_sr2')									 
								
							listitem(6, "War Status")	
						
								create_class(Repository, "blockade_event")
								create_class(Repository, "vp_rape")
								create_class(Repository, "ev_british_1")
								create_class(Repository, "ev_british_2")
								create_class(Repository, "ev_entrench_ap")
								create_class(Repository, "ev_moltke")
								create_class(Repository, "ev_italy")
								create_class(Repository, "vp_lusitania")
								create_class(Repository, "ev_great_retreat")
								create_class(Repository, "ev_mef")
								create_class(Repository, "ev_romania")
								create_class(Repository, "ev_salonika")
								create_class(Repository, "vp_grand_fleet")
								create_class(Repository, "ev_landships")
								create_class(Repository, "ev_brusilov")
								create_class(Repository, "vp_zimmermann")
								create_class(Repository, "ev_independent_air_force")
								create_class(Repository, "ev_greece")
								create_class(Repository, "ev_allenby")
								create_class(Repository, "ev_everyone_into_battle")
								create_class(Repository, "vp_14_points")
								create_class(Repository, "vp_convoy")
								create_class(Repository, "ev_overthere")
								create_class(Repository, "sinai_pipeline")
								create_class(Repository, "ev_sixtus_affair")
								create_class(Repository, "event_influenza")
						
						

		
		tab(2, "Central Powers")
		
			tabs(3)
			
				tab(4, "Cards")
				
					listbox(5)
					
						listitem(6, "Mobilization")	
						
							create_class(Repository, 'cardback_cp')
							create_class(Repository, 'card_cp01')
							create_class(Repository, 'card_cp02')
							create_class(Repository, 'card_cp03')
							create_class(Repository, 'card_cp04')
							create_class(Repository, 'card_cp05')
							create_class(Repository, 'card_cp06')
							create_class(Repository, 'card_cp07')
							create_class(Repository, 'card_cp08')
							create_class(Repository, 'card_cp09')
							create_class(Repository, 'card_cp10')
							create_class(Repository, 'card_cp11')
							create_class(Repository, 'card_cp12')
							create_class(Repository, 'card_cp13')
							create_class(Repository, 'card_cp14')
							
							scale_pane(0.2)
						
						listitem(6, "Limited War")
						
							create_class(Repository, 'card_cp15')
							create_class(Repository, 'card_cp16')
							create_class(Repository, 'card_cp17')
							create_class(Repository, 'card_cp18')
							create_class(Repository, 'card_cp19')
							create_class(Repository, 'card_cp20')
							create_class(Repository, 'card_cp21')
							create_class(Repository, 'card_cp22')
							create_class(Repository, 'card_cp23')
							create_class(Repository, 'card_cp24')
							create_class(Repository, 'card_cp25')
							create_class(Repository, 'card_cp26')
							create_class(Repository, 'card_cp27')
							create_class(Repository, 'card_cp28')
							create_class(Repository, 'card_cp29')
							create_class(Repository, 'card_cp30')
							create_class(Repository, 'card_cp31')
							create_class(Repository, 'card_cp32')
							create_class(Repository, 'card_cp33')
							create_class(Repository, 'card_cp34')
							
							scale_pane(0.2)
						
						listitem(6, "Total War")
						
							create_class(Repository, 'card_cp35')
							create_class(Repository, 'card_cp36')
							create_class(Repository, 'card_cp37')
							create_class(Repository, 'card_cp38')
							create_class(Repository, 'card_cp39')
							create_class(Repository, 'card_cp40')
							create_class(Repository, 'card_cp41')
							create_class(Repository, 'card_cp42')
							create_class(Repository, 'card_cp43')
							create_class(Repository, 'card_cp44')
							create_class(Repository, 'card_cp45')
							create_class(Repository, 'card_cp46')
							create_class(Repository, 'card_cp47')
							create_class(Repository, 'card_cp48')
							create_class(Repository, 'card_cp49')
							create_class(Repository, 'card_cp50')
							create_class(Repository, 'card_cp51')
							create_class(Repository, 'card_cp52')
							create_class(Repository, 'card_cp53')
							create_class(Repository, 'card_cp54')
							create_class(Repository, 'card_cp55')
							
							scale_pane(0.2)
				
				tab(4, "Counters")
				
					listbox(5)
						
						listitem(6, "German")
					
							assignClass('^ge_army%w+')
							assignClass('^ge_corps%w*')
							
							scale_pane(0.7)
					
						listitem(6, "Austro-Hungarian")
				
							assignClass('^ah_army%w+')
							assignClass('^ah_corps%w*')
							
							scale_pane(0.7)
				
						listitem(6, "Bulgarian")
						
							assignClass('^bu_corps')
							
						
						listitem(6, "Turkish")
						
							assignClass('^yld_army')
							assignClass('^aoi_army')
							assignClass('^sn_corps')
							assignClass('^tu_corps')
							
						
						listitem(6, "Polish")
						
							assignClass('^pol_corps')
							
							
				
				
				tab(4, "Markers")
									
						listbox(5)
						
							listitem(6, "General")
				
								create_class(Repository, 'cp_trench1')
								create_class(Repository, 'control_cp')
								create_class(Repository, 'cp_ne_sr2')								
						
							listitem(6, "War Status")
							
								create_class(Repository, "ev_guns_of_august")
								create_class(Repository, "ev_falkenhayn")
								create_class(Repository, "ev_oberost")
								create_class(Repository, "vp_reichstag")
								create_class(Repository, "ev_entrench")
								create_class(Repository, "ev_race_to_the_sea")
								create_class(Repository, "sud_army")
								
								create_class(Repository, "ev_bulgaria")
								create_class(Repository, "ev_walter_rathenau")
								create_class(Repository, "ev_place_of_execution")
								create_class(Repository, "ev_tsar_takes_command")
								create_class(Repository, "ev_war_in_africa")
								create_class(Repository, "ev_zeppelin")
								create_class(Repository, "event_11tharmy")
								create_class(Repository, "vp_fleet")
								create_class(Repository, "ev_haig")
								create_class(Repository, "vp_polish")
								create_class(Repository, "event_stavka")
								
								create_class(Repository, "ev_hl_take_command")
								create_class(Repository, "ev_uboats")
								create_class(Repository, "ev_hoffmann")
								create_class(Repository, "ev_michael")
								create_class(Repository, "vp_fall_of_the_tsar_1")
								create_class(Repository, "vp_fall_of_the_tsar_3")
								create_class(Repository, "ev_bolshevik")
								create_class(Repository, "ev_brest_litovsk")
								create_class(Repository, "ev_french_mutiny")
								create_class(Repository, "ev_prince_max")
						
		
			
		tab(2, "Markers")
		
			listbox(3)
					
				listitem(4, "Actions")
				
					create_class(Repository, "action_ap1")
					create_class(Repository, "action_ap2")
					create_class(Repository, "action_ap3")
					create_class(Repository, "action_ap4")
					create_class(Repository, "action_ap5")
					create_class(Repository, "action_ap6")
		
					create_class(Repository, "action_cp1")
					create_class(Repository, "action_cp2")
					create_class(Repository, "action_cp3")
					create_class(Repository, "action_cp4")
					create_class(Repository, "action_cp5")
					create_class(Repository, "action_cp6")
		
			listitem(4, "General Records")
			
					create_class(Repository, "warstatus_ap")
					create_class(Repository, "warstatus_cp")
					create_class(Repository, "warstatus_combined")
					create_class(Repository, "bid_cp")
					create_class(Repository, "vp")
					
			listitem(4, "VP")
			
					create_class(Repository, "vp_1_blockade")					
					create_class(Repository, "missed_mo_ap")
					create_class(Repository, "missed_mo_cp")
					create_class(Repository, "vp_italy")
					create_class(Repository, "vp_mutiny")
					create_class(Repository, "vp_peace_offensive")
					create_class(Repository, "vp_war_in_africa")
					create_class(Repository, "vp_peace_terms_minus")
					create_class(Repository, "vp_peace_terms_plus")
					
			listitem(4, "Replacements")
					
					create_class(Repository, "rp_ah")
					create_class(Repository, "rp_allied")
					create_class(Repository, "rp_br")
					create_class(Repository, "rp_bu")
					create_class(Repository, "rp_fr")
					create_class(Repository, "rp_ge")
					create_class(Repository, "rp_it")
					create_class(Repository, "rp_ru")
					create_class(Repository, "rp_tu")
					create_class(Repository, "rp_us")			
						
			listitem(4, "Miscellaneous")
					
					create_class(Repository, "mandoffensive_cp")
					create_class(Repository, "mandoffensive_ap")
					create_class(Repository, "turn")
					create_class(Repository, "us_entry")
					create_class(Repository, "russian_capit")
					create_class(Repository, "mef_beachhead")
					create_class(Repository, "oos_cp")
					create_class(Repository, "oos_ap")
					create_class(Repository, "trench_mod")
					create_class(Repository, "fort_besieged")
					create_class(Repository, "fort_destroyed")
					create_class(Repository, "fort_destroyed_mini")
					create_class(Repository, "activate_move")
					create_class(Repository, "activate_attack")
					create_class(Repository, "russianvp_current")
					create_class(Repository, "russianvp_tsarfell")
					create_class(Repository, "neutral")
					
					
			

	
return Repository
