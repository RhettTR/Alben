
-- load the resource ids (keys)

ResourceKeys = {}

function setResourceKey(id)
	table.insert(ResourceKeys, id)
end

get_resource()


-- set number of counter columns in repository

set_row_length(4)



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

	local result = {}
	
	for k, v in pairs(Repository) do
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' then
			local key = string.match(k, pattern)
			if key ~= nil then
				table.insert(result, Repository[key])
				--create_class(Repository, k)
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

	for k, v in pairs(Repository) do
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' then
		
			local key = string.match(k, pattern)
			
			if key ~= nil then
			
				local name = ''
				
				local nation = string.match(key, '^(%w+)')
				if nation ~= nil then
					if nation == 'ge'  then
						name = 'German ' 
					elseif nation == 'ss' then
						name = 'German SS '
					elseif nation == 'fi' then
						name = 'Finnish '
					elseif nation == 'ro' then
						name = 'Romanian '
					elseif nation == 'hu' then
						name = 'Hungarian '
					elseif nation == 'it' then
						name = 'Italian '
					elseif nation == 'ru' then
						name = 'Soviet '
					elseif nation == 'Worker' then
						name = 'Worker '		
					end
				end
				
				local unit = string.match(key, '^%w+%-%w+%-(%w+)')	
				if unit ~= nil then
					unit = string.gsub(unit, '^0+', '')
					name = name .. unit .. ' '
				end
					
				local type = string.match(key, '^%w+%-(%w+)')
				if type ~= nil then
					if type == 'cav' then
						name = name .. 'Cavalery Corps'
					elseif type == 'inf' then
						name = name .. 'Infantry Corps'
					elseif type == 'pzg' then
						name = name .. 'Panzer Corps'
					elseif type == 'abn' then
						name = name .. 'Airborn Corps'
					elseif type == 'arm' then
						name = name .. 'Armoured Corps'
					elseif type == 'armc' then
						name = name .. 'Armoured Corps'	
					elseif type == 'mtn' then
						name = name .. 'Mountain Corps'
					elseif type == 'HQ' then
						name = name .. 'HQ'
					elseif type == 'art' then
						name = name .. 'Artillery Corps'
					elseif type == 'mec' then
						name = name .. 'Mechanized Corps'
					elseif type == 'Garm' then
						name = name .. 'Armored Army'
					elseif type == 'Gcav' then
						name = name .. 'Guards Cavalery Corps'
					elseif type == 'infa' then
						name = name .. 'Infantry Army'
					else
						name = name .. type								
					end
				end
			
				
				
				if Repository[k] ~= nil then
					Repository[k].name = name
					--print(k)
					--print(Repository[k].name)
				end 
				
				
			end
		end
	end


end



	
-- create repository
	
	
	Repository["choochoo"] = Repository:new({"choochoo"})
	Repository["invasion"] = Repository:new({"invasion"})
	Repository["paratroop_marker_57x57"] = Repository:new({"paratroop_marker_57x57"})
	Repository["germany-sm0"] = Repository:new({"germany-sm0"})
	Repository["russia-sm0"] = Repository:new({"russia-sm0"})
	Repository["Battle05"] = Repository:new({"Battle05"})
	
	assign('ge%-inf%-%d.$')
	assign('ge%-arm%-%w.$')
	assign('ge%-pzg%-%d.$')
	assign('ss%-%w*%-%d*$')
	assign('ss%-pzg%-%w*$')		
	assign('ge%-cav%-%d+$')
	assign('ge%-HQ%-%w+$')
	assign('ge%-luftwaffe$')
	assign('ge%-mtn%-%w*$')
	assign('it%-inf%-%d*$')
	assign('hu%-inf%-%d*$')
	assign('hu%-pzg%-%d*$')	
	assign('ro%-%w*%-%d*$')
	assign('ro%-cav$')
	assign('ro%-arm$')
	assign('fi%-inf%-%d*$')
	assign('ge%-abn%-%d*$')
	assign('ge%-art%-%d*$')
	assign('stuka_%w$')
	assign('Hitler$')
	
	assign('ru%-abn%-%d*$')
	Repository["ru-arm-1"] = Repository:new({"ru-arm-1"})
	Repository["ru-arm-2"] = Repository:new({"ru-arm-2"})
	Repository["ru-arm-3"] = Repository:new({"ru-arm-3"})
	Repository["ru-arm-4"] = Repository:new({"ru-arm-4"})
	Repository["ru-armc-01"] = Repository:new({"ru-armc-01"})
	Repository["ru-armc-02"] = Repository:new({"ru-armc-02"})
	Repository["ru-armc-03"] = Repository:new({"ru-armc-03"})
	Repository["ru-armc-04"] = Repository:new({"ru-armc-04"})
	Repository["ru-armc-05"] = Repository:new({"ru-armc-05"})
	Repository["ru-armc-06"] = Repository:new({"ru-armc-06"})
	Repository["ru-armc-07"] = Repository:new({"ru-armc-07"})
	Repository["ru-armc-08"] = Repository:new({"ru-armc-08"})
	Repository["ru-armc-09"] = Repository:new({"ru-armc-09"})
	Repository["ru-armc-10"] = Repository:new({"ru-armc-10"})
	Repository["ru-armc-11"] = Repository:new({"ru-armc-11"})
	Repository["ru-armc-12"] = Repository:new({"ru-armc-12"})
	
	assign('ru%-cav%-%d*$')
	assign('ru%-HQ$')
	assign('Stalin$')	
	assign('ru%-inf%-%d*$')
	assign('ru%-arm%-1Gd$')
	Repository["ru-cav-1Gd"] = Repository:new({"ru-cav-1Gd"})
	assign('ru%-Garm%-%w+$')
	assign('ru%-Gcav%-%d+$')
	assign('ru%-infa%-%w+$')
	assign('ru%-art%-%d+$')
	assign('sturm$')
	assign('Partisan%d$')
	assign('Worker%-%w+$')
	assign('Worker%-%w+%-%w+$')
	assign('Worker%-%w+%-%w+%-%w+$')
	
	
	
-- set traits for all counters
	
	
	Repository:addTraits("choochoo", Markers:new())
	Repository:addTraits("invasion", Markers:new())
	Repository:addTraits("paratroop_marker_57x57", Markers:new())
	Repository:addTrait("germany-sm0", "Delete", Delete)
	Repository:addTrait("germany-sm0", "Marker", Marker:new("germany-sm0", -20, 0, "German"))	
	Repository:addTrait("russia-sm0", "Delete", Delete)
	Repository:addTrait("russia-sm0", "Marker", Marker:new("russia-sm0", -20, 0, "Soviet"))	
	Repository:addTraits("Battle05", Odds:new())
	 	
	assignTraits('^ge%-%w*', Axis)
	assignTraits('^ss%w*', Axis)
	assignTraits('^it%w*', Axis)
	assignTraits('^hu%w*', Axis)
	assignTraits('^ro%w*', Axis)
	assignTraits('^fi%w*', Axis)
	assignTraits('^Hitler', Axis)
	assignTraits('^stuka_%w', Stukas)
	
	assignTraits('^ru%-%w*', Soviet)
	assignTraits('^Stalin', Soviet)
	assignTraits('^Partisan%d$', Soviet)
	assignTraits('Worker%-%w+$', Soviet)
	assignTraits('Worker%-%w+%-%w+$', Soviet)
	assignTraits('Worker%-%w+%-%w+%-%w+$', Soviet)
	assignTraits('sturm$', Stukas)
	
	
	

	
-- set display name for all counters

	assignName('^ge%-%w*%-%w*')
	assignName('^ss%-%w*%-%w*')
	assignName('^fi%-%w*%-%w*')
	assignName('^ro%-%w*%-%w*')
	assignName('^hu%-%w*%-%w*')
	assignName('^it%-%w*%-%w*')
	
	Repository['ge-abn-2'].name = 'German 2 Airborn Division'
	Repository['ge-luftwaffe'].name = 'German Luftwaffe Corps'
	Repository['stuka_c'].name = 'Stuka Army Group Center'
	Repository['stuka_n'].name = 'Stuka Army Group North'
	Repository['stuka_s'].name = 'Stuka Army Group South'
	
	Repository['hu-pzg-1'].name = 'Hungerian 1 Mechanized Corps'
	Repository['ro-cav'].name = 'Romanian Cavalery Corps'
	Repository['ro-arm'].name = 'Romanian Panzer Corps'
	
	
	
	assignName('^ru%-%w*%-%w*')
	
	assignName('^Worker%-%w*')
	
	Repository['Worker-Slalino'].name = 'Worker Stalino'
	Repository['ru-HQ'].name = 'Stavka'
	Repository['sturm'].name = 'Sturmovik'
	
	Repository['choochoo'].name = 'Railhead Marker'
	Repository['invasion'].name = 'Invasion Marker'
	Repository['paratroop_marker_57x57'].name = 'Paratroop Drop Marker'
	Repository['germany-sm0'].name = 'German Control Marker'
	Repository['russia-sm0'].name = 'Soviet Control Marker'
	Repository['Battle05'].name = 'Battle Marker'
	
	

		
-- set the content of the repository window


set_root('Repository')


root(0)	
		
		
	listbox(1)
	
		
		listitem(2, "Markers")
		
			listbox(3)
				
				listitem(4, "Railhead")			
					create_class(Repository, "choochoo")
				listitem(4, "Invasion")	
					create_class(Repository, "invasion")
				listitem(4, "AirDrop")		
					create_class(Repository, "paratroop_marker_57x57")
				listitem(4, "Axis Control")	
					create_class(Repository, "germany-sm0")
				listitem(4, "Soviet Control")
					create_class(Repository, "russia-sm0")
				listitem(4, "Battle")
					create_class(Repository, "Battle05")			
					
		listitem(2, "German")
			assignClass('Hitler$')
			assignClass('ge%-luftwaffe$')			
			assignClass('ge%-abn%-%d*$')
			assignClass('ge%-art%-%d*$')
			assignClass('stuka_%w$')
					
		listitem(2, "German Infantry")	
			assignClass('ge%-inf%-%d.$')
			
		listitem(2, "German Armor")	
			assignClass('ge%-arm%-%w.$')
			assignClass('ge%-pzg%-%d.$')
		
		listitem(2, "German SS")	
			assignClass('ss%-%w*%-%d*$')
			assignClass('ss%-pzg%-%w*$')	
			
		listitem(2, "German Cavalary")	
			assignClass('ge%-cav%-%d+$')	
			
		listitem(2, "German HQ")	
			assignClass('ge%-HQ%-%w+$')
			
		listitem(2, "German Mountain")			
			assignClass('ge%-mtn%-%w*$')
			
		listitem(2, "Italian")	
			assignClass('it%-inf%-%d*$')
			
		listitem(2, "Hugarian")	
			assignClass('hu%-inf%-%d*$')
			assignClass('hu%-pzg%-%d*$')
			
		listitem(2, "Rumanian")	
			assignClass('ro%-%w*%-%d*$')
			assignClass('ro%-cav$')
			assignClass('ro%-arm$')
			
		listitem(2, "Finnish")		
			assignClass('fi%-inf%-%d*$')
		
		
		listitem(2, "Soviet")	
			assignClass('ru%-abn%-%d*$')
			assignClass('ru%-art%-%d+$')
			assignClass('Partisan%d$')
			assignClass('ru%-HQ$')
			assignClass('Stalin$')
			assignClass('sturm$')	
			
		listitem(2, "Soviet Armor")			
			create_class(Repository, "ru-arm-1")
			create_class(Repository, "ru-arm-2")
			create_class(Repository, "ru-arm-3")
			create_class(Repository, "ru-arm-4")
			create_class(Repository, "ru-armc-01")
			create_class(Repository, "ru-armc-02")
			create_class(Repository, "ru-armc-03")
			create_class(Repository, "ru-armc-04")
			create_class(Repository, "ru-armc-05")
			create_class(Repository, "ru-armc-06")
			create_class(Repository, "ru-armc-07")
			create_class(Repository, "ru-armc-08")
			create_class(Repository, "ru-armc-09")
			create_class(Repository, "ru-armc-10")
			create_class(Repository, "ru-armc-11")
			create_class(Repository, "ru-armc-12")	
			
		listitem(2, "Soviet Cavalary")	
			assignClass('ru%-cav%-%d*$')
			
		listitem(2, "Soviet Infantry")	
			assignClass('ru%-inf%-%d*$')
			
		listitem(2, "Soviet Guard")	
			assignClass('ru%-arm%-1Gd$')
			create_class(Repository, "ru-cav-1Gd")
			assignClass('ru%-Garm%-%w+$')
			assignClass('ru%-Gcav%-%d+$')
			assignClass('ru%-infa%-%w+$')
			
		listitem(2, "Soviet Workers")			
			assignClass('Worker%-%w+$')
			assignClass('Worker%-%w+%-%w+$')
			assignClass('Worker%-%w+%-%w+%-%w+$')

	
return Repository
