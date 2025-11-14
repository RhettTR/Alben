
-- loads the keys

ResourceKeys = {}

function setResourceKey(id)
	table.insert(ResourceKeys, id)
end

get_resource()



-- repository assignment for regular expressions

local function assign(prefix)

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
		
	
	for k, v in ipairs(result) do
		--print('Repository["' .. v[1] .. '"] = ' .. dump(v))
		Repository[v[1]] = Repository:new(v)
	end
	
	for k, v in ipairs(used) do
		-- NOTE: a key in 'used' is not necessaraly a key in 'ResourceKeys'
		local n = table.find(ResourceKeys, v)
		if n ~= nil then
			table.remove(ResourceKeys, n)
		end 	
	end 
	
	
	
end


local function assignTraits(pattern, trait)

	for k, v in pairs(Repository) do
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' then
			local key = string.match(k, pattern)
			if key ~= nil then
				Repository:addTraits(k, trait:new())
			end
		end
	end
	
end


local function assignClass(pattern)

	for k, v in pairs(Repository) do
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' then
			local key = string.match(k, pattern)
			if key ~= nil then
				create_class(Repository, k)
			end
		end
	end
	
end


local function assignName(pattern)

	for k, v in pairs(Repository) do
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' then
			local key = string.match(k, pattern)
			if key ~= nil then
				local name = ''
				if string.match(key, '^ge')then
					name = 'German ' 
				elseif string.match(key, '^ru')then
					name = 'Soviet '
				end
				local unit = string.match(key, '%-%w+%-(%d+)PzDiv')
				local subunit = string.match(key, '%-%w+%-%d+PzDiv%-+(%d+)')
				if unit ~= nil and subunit ~= nil then
					unit = string.gsub(unit, '^0+', '')
					subunit = string.gsub(subunit, '^0+', '')
					name = name .. subunit .. '/' .. unit .. ' '
				else
					local unit = string.match(key, '%-%w+%-(%d+)Mot')
					local subunit = string.match(key, '%-%w+%-%d+Mot%-(%d+)')
					if unit ~= nil and subunit ~= nil then
						unit = string.gsub(unit, '^0+', '')
						subunit = string.gsub(subunit, '^0+', '')
						name = name .. subunit .. '/' .. unit .. ' '
					else
						local unit = string.match(key, '%-%w+%-(%d+Mos)')
						if unit ~= nil then
							unit = string.gsub(unit, '^0+', '')		
							name = name .. unit .. ' '
						else	 
							local unit = string.match(key, '%-%w+%-(%d+)')
							if unit ~= nil then
								unit = string.gsub(unit, '^0+', '')		
								name = name .. unit .. ' ' 
							else
								local unit = string.match(key, '%-%w+%-(%w+)')
								if unit ~= nil then
									unit = string.gsub(unit, '^0+', '')
									name = name .. unit .. ' '
								end
							end
						end		
					end	
				end	
				
				local type = string.match(key, '%-(%w+)%-')
				if type == 'cav' then
					name = name .. 'Cavalery Division'
				end
				if type == 'inf' then
					name = name .. 'Infantry Division'
				end
				if type == 'pz' then
					name = name .. 'Panzer Division'
				end
				if type == 'mot' then
					name = name .. 'Motorized Division'
				end
				if type == 'arm' then
					name = name .. 'Armoured Division'
				end
				if type == 'mech' then
					name = name .. 'Mechanized Division'
				end
				if type == 'HQ' then
					name = name .. 'Army Headquarters'
				end
				if Repository[key] ~= nil then
					Repository[key].name = name
					--print(key)
					--print(Repository[key].name)
				end 
			end
		end
	end


end



	
-- create repository
	
	
	assign('ge%-cav%-%w+%-*%d*$')
	assign('ge%-inf%-%d+$')
	assign('ge%-mot%-%w+$')
	assign('ge%-mot%-%w+%-%d*$')
	assign('ge%-pz%-%w+%-%-%d+pz%-*%d*$')
	assign('ge%-pz%-%w+-%d+pzgr-%d*$')
	
	assign('ru%-arm%-%d+$')
	assign('ru%-HQ%-%w+.+$')
	assign('ru%-inf%-%w+$')
	assign('ru%-mech%-%w+$')
	
	
	
	--Repository["ge-inf-005"] = Repository:new({"ge-inf-005", "ge-inf-005-1", "ge-inf-005-2", "ge-inf-005-3"})
	
	
	Repository["control_button"] = Repository:new({"control_button"})
	
	Repository["German Control"] = Repository:new({"German Control"})
	Repository["German Disruption"] = Repository:new({"German Disruption"})
	Repository["German Out of Supply"] = Repository:new({"German Out of Supply"})	
	Repository["German Air Interdiction"] = Repository:new({"German Air Interdiction"})
	
	Repository["Soviet Disruption"] = Repository:new({"Soviet Disruption"})
	Repository["Soviet Out of Supply"] = Repository:new({"Soviet Out of Supply"})	
	Repository["Soviet Interdiction"] = Repository:new({"Soviet Interdiction"})
	Repository["Soviet Rail Cut"] = Repository:new({"Soviet Rail Cut"})
	
	Repository["SW-Front-1-Unit"] = Repository:new({"SW-Front-1-Unit"})
	Repository["SW-Front-2-Units"] = Repository:new({"SW-Front-2-Units"})
	Repository["SW-Front-3-Units"] = Repository:new({"SW-Front-3-Units"})
	Repository["SW-Front-4-Units"] = Repository:new({"SW-Front-4-Units"})
	Repository["SW-Front-5-Units"] = Repository:new({"SW-Front-5-Units"})
	
	
	Repository["ru-inf-??"] = Repository:new({"ru-inf-??"})
	Repository["ru-mech-??"] = Repository:new({"ru-mech-??"})
	
	
	
-- set traits for all counters
	
	
	assignTraits('ge%-cav%-%w+%-*%d*$', German)
	assignTraits('ge%-inf%-%d+$', German)
	assignTraits('ge%-mot%-%w+$', German)
	assignTraits('ge%-mot%-%w+%-%d*$', GermanMot)
	assignTraits('ge%-pz%-%w+%-%-%d+pz%-*%d*$', GermanArm)
	assignTraits('ge%-pz%-%w+-%d+pzgr-%d*$', GermanArm)
	
	assignTraits('ru%-arm%-%d+$', SovietArm)
	assignTraits('ru%-HQ%-%w+.+$', SovietHQ)
	assignTraits('ru%-inf%-%w+$', SovietInf)
	assignTraits('ru%-mech%-%w+$', SovietArm)

	
	--Repository:addTraits("ge-inf-005", German:new())
	
	
	
	
	
	Repository:addTrait('ru-HQ-13th-Army-Remezov', 'AreaOfEffect', AreaOfEffect:new('hex', 4, 'red', 0.2))
	Repository:addTrait('ru-HQ-16th-Army-Lukin', 'AreaOfEffect', AreaOfEffect:new('hex', 3, 'red', 0.2))
	Repository:addTrait('ru-HQ-19th-Army-Koniev', 'AreaOfEffect', AreaOfEffect:new('hex', 5, 'red', 0.2))
	Repository:addTrait('ru-HQ-20th-Army-Kurochkin', 'AreaOfEffect', AreaOfEffect:new('hex', 4, 'red', 0.2))
	Repository:addTrait('ru-HQ-21st-Army-Gerisamenko', 'AreaOfEffect', AreaOfEffect:new('hex', 2, 'red', 0.2))
	Repository:addTrait('ru-HQ-22th-Army-Yershakov', 'AreaOfEffect', AreaOfEffect:new('hex', 3, 'red', 0.2))
	Repository:addTrait('ru-HQ-24th-Army-Rakutin', 'AreaOfEffect', AreaOfEffect:new('hex', 2, 'red', 0.2))
	Repository:addTrait('ru-HQ-28th-Army-Kashalov', 'AreaOfEffect', AreaOfEffect:new('hex', 2, 'red', 0.2))
	Repository:addTrait('ru-HQ-29th-Army-Maslennikov', 'AreaOfEffect', AreaOfEffect:new('hex', 3, 'red', 0.2))
	Repository:addTrait('ru-HQ-30th-Army-Khomenko', 'AreaOfEffect', AreaOfEffect:new('hex', 3, 'red', 0.2))
	Repository:addTrait('ru-HQ-31st-Army-Dolmatov', 'AreaOfEffect', AreaOfEffect:new('hex', 2, 'red', 0.2))
	Repository:addTrait('ru-HQ-32nd-Army-Vishnevsky', 'AreaOfEffect', AreaOfEffect:new('hex', 2, 'red', 0.2))
	Repository:addTrait('ru-HQ-33th-Army-Onuprienko', 'AreaOfEffect', AreaOfEffect:new('hex', 2, 'red', 0.2))
	Repository:addTrait('ru-HQ-49th-Army-Zakharkin', 'AreaOfEffect', AreaOfEffect:new('hex', 3, 'red', 0.2))
	Repository:addTrait('ru-HQ-Reserve-Army-Rokassovski', 'AreaOfEffect', AreaOfEffect:new('hex', 5, 'red', 0.2))
	
	
	
	Repository:addTrait('ru-inf-??', 'Reveal', Reveal:new())
	Repository:addTrait('ru-inf-??', 'MarkMoved', MarkMoved:new())
	Repository:addTrait('ru-inf-??', 'Take2', Take:new(2))
	Repository:addTrait('ru-inf-??', 'Take3', Take:new(3))
	Repository:addTrait('ru-mech-??', 'Reveal', Reveal:new())
	Repository:addTrait('ru-mech-??', 'MarkMoved', MarkMoved:new())
	
	
	Repository:addTrait('German Disruption', 'Delete', Delete)
	Repository:addTrait('German Disruption', 'OnlyOneInStack', OnlyOneInStack)
	Repository:addTrait('Soviet Disruption', 'Delete', Delete)
	Repository:addTrait('Soviet Disruption', 'OnlyOneInStack', OnlyOneInStack)
	Repository:addTrait('German Out of Supply', 'Delete', Delete)
	Repository:addTrait('German Out of Supply', 'OnlyOneInStack', OnlyOneInStack)
	Repository:addTrait('Soviet Out of Supply', 'Delete', Delete)
	Repository:addTrait('Soviet Out of Supply', 'OnlyOneInStack', OnlyOneInStack)
	
	
	Repository:addTrait('German Control', 'Marker', Marker:new('German Control', -20, -10, 'German'))
	

	
-- set display name for all counters

	
	assignName('ge%-cav%-%w+%-*%d*$')
	assignName('ge%-inf%-%d+$')
	assignName('ge%-mot%-%w+$')
	assignName('ge%-mot%-%w+%-%d*$')
	assignName('ge%-pz%-%w+%-%-%d+pz%-*%d*$')
	assignName('ge%-pz%-%w+-%d+pzgr-%d*$')
	
	assignName('ru%-arm%-%d+$')
	assignName('ru%-HQ%-%w+.+$')
	assignName('ru%-inf%-%w+$')
	assignName('ru%-mech%-%w+$')
	
	Repository['ge-mot-DR-02'].name = '2/DR Motorized Division'
	Repository['ge-mot-DR-03'].name = '3/DR Motorized Division'
	Repository['ge-mot-DR-04'].name = '4/DR Motorized Division'
	
	Repository['ru-inf-??'].name = 'Soviet Unrevealed Infantry Division'
	Repository['ru-mech-??'].name = 'Soviet Unrevealed Armored Division'
	

		
-- set the content of the repository window


set_root_window('Repository')


root(0)	
		
		
	listbox(1)
	
		listitem(2, "Markers")
			create_class(Repository, "German Control")
			create_class(Repository, "German Disruption")
			create_class(Repository, "German Out of Supply")	
			create_class(Repository, "German Air Interdiction")
			create_class(Repository, "Soviet Disruption")
			create_class(Repository, "Soviet Out of Supply")	
			create_class(Repository, "Soviet Interdiction")
			create_class(Repository, "Soviet Rail Cut")
			create_class(Repository, "ru-inf-??")
			create_class(Repository, "ru-mech-??")
			create_class(Repository, "SW-Front-1-Unit")
			create_class(Repository, "SW-Front-2-Units")
			create_class(Repository, "SW-Front-3-Units")
			create_class(Repository, "SW-Front-4-Units")
			create_class(Repository, "SW-Front-5-Units")
					
		listitem(2, "German Panzer")	
			assignClass('ge%-pz%-%w+%-%-%d+pz%-*%d*$')
			
		listitem(2, "German PzGr")	
			assignClass('ge%-pz%-%w+-%d+pzgr-%d*$')
			
		listitem(2, "German Motorized")
			assignClass('ge%-mot%-%w+$')
			assignClass('ge%-mot%-%w+%-%d*$')
			
		listitem(2, "German Cavalry")
			assignClass('ge%-cav%-%w+%-*%d*$')	
			
		listitem(2, "German Infantry")
			assignClass('ge%-inf%-%d+$')
			
		listitem(2, "Soviet Armor")
			assignClass('ru%-arm%-%d+$')
			
		listitem(2, "Soviet Mechanized")
			assignClass('ru%-mech%-%w+$')	
	
		listitem(2, "Soviet Infantry")
			assignClass('ru%-inf%-%w+$')
	
		listitem(2, "Soviet HQ")
			assignClass('ru%-HQ%-%w+.+$')
			





	
return Repository
