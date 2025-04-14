
-- create fire table window and add button to toolbar with callback

create_window("German artillery table", "artillery-window", "GermanArtilleryB")


local callback = "toggle_window()"

add_toolbar_button("Icons/GermanArtillery", "German artillery table", callback)




-- define and set custom grid coordinates

local grid = {[1] = {}}

function grid:add(x, y)
	table.insert(grid[1], {x = x, y = y})
end

-- storage locations
grid:add(104, 136)
grid:add(192, 136)
grid:add(280, 136)
grid:add(368, 136)
grid:add(456, 136)
grid:add(544, 136)
grid:add(632, 136)


grid:add(104, 234)
grid:add(192, 234)
grid:add(280, 234)
grid:add(368, 234)
grid:add(456, 234)
grid:add(544, 234)
grid:add(632, 234)

-- fire calculation locations
grid:add(128, 376)
grid:add(128, 471)


set_grid(grid, 1)



--- define fire location, key = target

local FireLocations = { {x = 128, y = 376, gridx = 1, gridy = 1}, 
		                {x = 128, y = 471, gridx = 1, gridy = 1}}
		               

function setTarget(k, x, y)
	FireLocations[k].gridx = x
	FireLocations[k].gridy = y
end



-- create boxes for target coordinate and calculated firepower

create_label('coor1', 180, 355, 78, 38, 'border:1px solid black; background:white; font:bold 30px; qproperty-alignment: AlignCenter;')		
create_label('valu1', 268, 355, 58, 38, 'border:1px solid black; background:white; font:bold 30px; qproperty-alignment: AlignCenter; color:red;')	
	
set_label_text('coor1', '---')		
set_label_text('valu1', '0')

create_label('coor2', 180, 450, 78, 38, 'border:1px solid black; background:white; font:bold 30px; qproperty-alignment: AlignCenter;')		
create_label('valu2', 268, 450, 58, 38, 'border:1px solid black; background:white; font:bold 30px; qproperty-alignment: AlignCenter; color:red;')	
	
set_label_text('coor2', '---')		
set_label_text('valu2', '0');


-- create check box for setting counter battery fire

callback = "recalculate()"

create_checkbox('cb', 100, 20, 200, 28, 'Counter Battery Fire', callback,
				'QCheckBox:checked {color: black;} QCheckBox {font:bold 16px Arial,sans-serif; color:white;}');






-- create Luau tokens


local Token = {}

function Token:new (fromid, toid)
	local o = Counter:duplicate(Repository[fromid])
	o = Counter:merge(o, Artillery[fromid])
	o.FullStrength = (o.Image.imageIndex == 1)
	o.counter = toid
	o.gridx = 0
	o.gridy = 0	
	o.Overlays = Overlays:new ()
	o.Overlays:add(Label:new('', "arial", 18, "black", 0, 0, function(o) return o.text ~= '' end))
	return o
end


function Token:flip (id)
	Token[id].Image.imageIndex = Token[id].Image.imageIndex + 1
	local length = #(Token[id].Image.images)
	if Token[id].Image.imageIndex > length then
		Token[id].Image.imageIndex = 1
	end
	return Token[id].Image.images[Token[id].Image.imageIndex]
end


function Token:pos (id, x ,y)
	Token[id].gridx = x
	Token[id].gridy = y
end


Artillery = {}

function Artillery:new (x, y, bfull, cfull, breduced, creduced, range)
	local o = {}
	-- placement in artillery window
	o.x = x
	o.y = y
	o.Bombardment = bfull
	o.CounterBattery = cfull
	o.Bomb2nd = breduced
	o.Counter2nd = creduced
	o.Range = range	
	return o
end


Artillery["German/Artillery/5-77-2"] = Artillery:new(104, 136, 2, 2, 2, 2, 6)
Artillery["German/Artillery/5-150H"] = Artillery:new(192, 136, 3, 4, 2, 2, 8)
Artillery["German/Artillery/5-420H-6"] = Artillery:new(280, 136, 6, 2, 3, 1, 14)





function updateGUI(k)
		
	local image = Token[k].Image.images[ Token[k].Image.imageIndex ]	

	update_token(k,
				 Token[k].x, 
				 Token[k].y,
				 image,
				 Token[k],
				 'Overlays')
 
end




local function char(num)
	return string.char(string.byte("A") + num - 1)
end


function printCoordinates(x, y)

	x = math.floor((x + 20) / 113)
	y = math.floor((y + 20) / 113)
	
	return tostring(char(y + 1)) .. tostring(x + 1)
	
end


function xyCoordinates(x, y)

	x = math.floor((x + 20) / 113)
	y = math.floor((y + 20) / 113)
	
	return (x + 1), (y + 1) 
	
end


function recalculate()

	local counterBattery = get_checkbox_checked('artillery-window', 'cb')
	

	for k, v in ipairs(FireLocations) do
	
		local text = get_label_text('coor' .. k)
		
		-- if target counter on map
		
		if text ~= '---' then
			
			-- find tokens in target slot and add firepower
			
			local power = 0
			
			for key, token in pairs(Token) do
			
				if type(key) == "string" and key ~= '__index' and type(token) ~= 'function' then				
					if  v.x == token.x  and  v.y == token.y  then
						if  math.abs(v.gridx - token.gridx) <= token.Range and
							math.abs(v.gridy - token.gridy) <= token.Range then
							if token.FullStrength then
								if counterBattery then
									power = power + token.CounterBattery
								else
									power = power + token.Bombardment
								end
							else
								if counterBattery then
									power = power + token.Counter2nd
								else
									power = power + token.Bomb2nd
								end	
							end
						end
					end					
				end
			end
			
			set_label_text('valu' .. k, tostring(power))
			
		end
				
	end
	
end


function resetValues()
	
	for k, v in ipairs(FireLocations) do
		set_label_text('valu' .. k, '0')	
	end
	
end



return Token 
