
-- make repository window invisible

repository_visibility(false)


-- set default offset of stacks (6 = dx 2 = dy 2 = multiplier open stack 0 = unlimited shown)

setDefaultOffset(6, 2, 2, 0)


-- set map zoom range and default zoom (last value)

set_zoom_range(130, 120, 110, 100, 90, 80, 70, 60, 100)






-- create windows





create_window('Rf', 'Set-up / Reinforcements', 'Reinforcements', false, '')


-- black squares

local css = "border: 1px solid black;"
add_window_label('Rf', 'IPool', 113, 290, 54, 54, '', css)
add_window_label('Rf', 'IPile', 263, 290, 54, 54, '', css)
add_window_label('Rf', 'MPool', 486, 290, 54, 54, '', css)
add_window_label('Rf', 'MPile', 635, 290, 54, 54, '', css)


-- zone

local zone = Zone:new('Rf', 'rfzone')

zone:setBorder({x0 = 28, y0 = 50, x1 = 849, y1 = 908})

zone:addMembers('ge%-cav%-%w+%-*%d*$')
zone:addMembers('ge%-inf%-%d+$')
zone:addMembers('ge%-mot%-%w+$')
zone:addMembers('ge%-mot%-%w+%-%d*$')
zone:addMembers('ge%-pz%-%w+%-%-%d+pz%-*%d*$')
zone:addMembers('ge%-pz%-%w+-%d+pzgr-%d*$')

zone:addMembers('ru%-HQ%-%w+.+$')
zone:addMembers('ru%-inf%-%w+$')
zone:addMembers('ru%-arm%-%d+$')
zone:addMembers('ru%-mech%-%w+$')

zone:addMember('ru-inf-??')
zone:addMember('ru-mech-??')




zone:addGrid(59, 424, 'Turn 1', 0, 0, Offset:new(2, 2, 3, 3))
zone:addGrid(135, 424, 'Turn 2', 0, 0, Offset:new(2, 2, 3, 3))
zone:addGrid(135, 538, 'Turn 2', 0, 0, Offset:new(2, 2, 3, 3))

zone:addGrid(209, 424, 'Turn 3', 0, 0, Offset:new(2, 2, 3, 3))
zone:addGrid(209, 538, 'Turn 3', 0, 0, Offset:new(2, 2, 3, 3))
zone:addGrid(209, 593, 'Turn 3', 0, 0, Offset:new(2, 2, 3, 3))
zone:addGrid(209, 672, 'Turn 3', 0, 0, Offset:new(2, 2, 3, 3))

zone:addGrid(358, 424, 'Turn 5', 0, 0, Offset:new(2, 2, 3, 3))
zone:addGrid(432, 424, 'Turn 6', 0, 0, Offset:new(2, 2, 3, 3))
zone:addGrid(507, 424, 'Turn 7', 0, 0, Offset:new(2, 2, 3, 3))
zone:addGrid(582, 424, 'Turn 8', 0, 0, Offset:new(2, 2, 3, 3))
zone:addGrid(656, 424, 'Turn 9', 0, 0, Offset:new(2, 2, 3, 3))

zone:addGrid(60, 856, '24th Army HQ')
zone:addGrid(136, 856, '16th Army HQ')
zone:addGrid(209, 856, '19th Army HQ')
zone:addGrid(284, 856, '13th Army HQ')
zone:addGrid(358, 856, '20th Army HQ')

zone:addGrid(60, 89, 'Turn 1 X')
zone:addGrid(60, 182, 'Turn 1 V')
zone:addGrid(136, 90, 'Turn 2 W')
zone:addGrid(209, 90, 'Turn 3 Z')
zone:addGrid(284, 90, 'Turn 4 X')
zone:addGrid(507, 90, 'Turn 7 X')
zone:addGrid(582, 90, 'Turn 8 X')
zone:addGrid(656, 90, 'Turn 9 X')
zone:addGrid(731, 90, 'Turn 10 X')
zone:addGrid(806, 90, 'Turn 11 X')

zone:addGrid(139, 317, 'Soviet Infantry Pool', 0, 0, Offset:new(2, 2, 1, 6))
zone:addGrid(289, 317, 'Soviet Infantry Dead Pool', 0, 0, Offset:new(2, 2, 1, 3))
zone:addGrid(512, 317, 'Soviet Armor Pool', 0, 0, Offset:new(2, 2, 1, 6))
zone:addGrid(661, 317, 'Soviet Armor Dead Pool', 0, 0, Offset:new(2, 2, 1, 3))




create_window('Gy', 'Graveyard', 'Graveyard', false, '')



create_window('Ch', 'Charts', '#777777', false, 'Layout')
set_window('Ch', 70, 70, 870, 780)

root(0, 'Ch')

	tabs(1, 'Ch')

		tab(2, 'CRT', 'Ch')	
			image_item('CRT', 'Ch')
		
		tab(2, 'Terrain Effects', 'Ch')				
			image_item('Terrain Effects Chart', 'Ch')
			
		tab(2, 'Terrain Key', 'Ch')				
			image_item('terrain_key', 'Ch')
			
			
				
			

-- create custom toolbar and add buttons

create_toolbar('ct', 'Custom toolbar')


add_toolbar_label('ct', 'label1', 'russian_button', 20, 20, 'padding-left:10; padding-right:10')
toolbar_enable('ct', 'label1')

local css = 'font-size: 15px; border: 1px solid grey; height:20px; margin-right:4; border-radius: 2px;'
add_toolbar_label('ct', 'label2', '', 410, 22, css)
toolbar_enable('ct', 'label2')





-----------
local turns = {'Turn 1 (3-4 July)', 'Turn 2 (5-6 July)', 'Turn 3 (7-8 July)', 'Turn 4 (9-10 July)', 
               'Turn 5 (11-12 July)', 'Turn 6 (13-14 July)', 'Turn 7 (15-16 July)', 'Turn 8 (17-18 July)', 
               'Turn 9 (19-20 July)', 'Turn 10 (21-22 July)', 'Turn 11 (23-24 July)', 'Turn 12 (25-26 July)'}
local phases = {'Soviet Movement Phase', 'Soviet Combat Phase', 'Soviet Disruption Removal Phase', 
				'Soviet Interdiction Phase', 'German Initial Movement Phase', 'German Combat Phase',
				'German Mechanized Movement Phase', 'German Disruption Removal Phase', 'German Air Interdiction Phase'}



Globals:add('Turn', {turn = 1, phase = 1})
    
-- can't be local functions

function printturn()
	local turn = Globals['Turn'].turn
	local phase = Globals['Turn'].phase
	if phase >= 1 and phase <=4 then
		set_toolbar_button('ct', 'label1', 'russian_button', '')
	else
		set_toolbar_button('ct', 'label1', 'GermanControl', '')
	end
	set_toolbar_label('ct', 'label2', turns[turn] .. ' ' .. phases[phase])
end
                
function nextturn()
	local turn = Globals['Turn'].turn
	local phase = Globals['Turn'].phase

	if phase == 9 then
		if turn + 1 > 12 then
			turn = 1
			phase = 1	
		else
			turn = turn + 1
			phase = 1
		end
	else
		phase = phase + 1
	end
	
	Globals['Turn'].turn = turn
	Globals['Turn'].phase = phase
		
	printturn()
	
	State:addMove('Turn', 'Globals', 'turn', Globals['Turn'].turn)
	State:addMove('Turn', 'Globals', 'phase', Globals['Turn'].phase)
	State:addStage()
	
end

add_toolbar_button('ct', 'bs', 'button_step', '', 'Next turn', 'nextturn()')
toolbar_enable('ct', 'bs')

State.baseState['Turn'] = Counter:duplicate(Globals['Turn'])
printturn()


-----------

add_toolbar_separator('ct')



local callback = "toggle_window('Rf')"
add_toolbar_button('ct', 'rf', 'control_button', 'Set-up / Reinforcements', 'Show/Hide window', callback)
toolbar_enable('ct', 'rf')

local callback = "toggle_window('Gy')"
add_toolbar_button('ct', 'gy', 'button_headstone', 'Graveyard', 'Show/Hide window', callback)
toolbar_enable('ct', 'gy')

local callback = "toggle_window('Ch')"
add_toolbar_button('ct', 'ch', 'button_charts', 'Charts', 'Show/Hide window', callback)
toolbar_enable('ct', 'ch')

local callback = "toggle_window('Repository')"
add_toolbar_button('ct', 're', 'button_units', 'Markers', 'Show/Hide window', callback)
toolbar_enable('ct', 're')




-- define zone for main map ::  zones must have a unique tag across windows


local mainzone = Zone:new('main', 'main')

mainzone:setBorder({x0 = 36, y0 = 38, x1 = 4643, y1 = 2864})

	
mainzone:hexGrid(51.74, 89.61)

set_grid({mainzone.grid}, 1)

-- specify the visual appearance of grid (minscale, radius, r, g ,b, a)
specify_grid(0.4, 5, 255, 0, 0, 255 )

-- show the grid
show_grid(false)


mainzone:addGrid(477, 43, 'German Air Interdiction', 0, 0, Offset:new(2, 2, 3, 0))
mainzone:addGrid(634, 43, 'Soviet Interdiction')
mainzone:addGrid(786, 43, 'Soviet Rail Cut', 0, 0, Offset:new(2, 2, 3, 0))


local p1 = mainzone:getGrid('German Air Interdiction')
local p2 = mainzone:getGrid('Soviet Interdiction')
local p3 = mainzone:getGrid('Soviet Rail Cut')

Repository:addTrait('German Air Interdiction', 'Move', Move:new('Remove', p1))
Repository:addTrait('Soviet Interdiction', 'Move', Move:new('Remove', p2))
Repository:addTrait('Soviet Rail Cut', 'Move', Move:new('Remove', p3))





mainzone:addMembers('ge%-cav%-%w+%-*%d*$')
mainzone:addMembers('ge%-inf%-%d+$')
mainzone:addMembers('ge%-mot%-%w+$')
mainzone:addMembers('ge%-mot%-%w+%-%d*$')
mainzone:addMembers('ge%-pz%-%w+%-%-%d+pz%-*%d*$')
mainzone:addMembers('ge%-pz%-%w+-%d+pzgr-%d*$')

mainzone:addMembers('ru%-arm%-%d+$')
mainzone:addMembers('ru%-HQ%-%w+.+$')
mainzone:addMembers('ru%-inf%-%w+$')
mainzone:addMembers('ru%-mech%-%w+$')

mainzone:addMember('German Control')
mainzone:addMember('German Disruption')
mainzone:addMember('German Out of Supply')	
mainzone:addMember('German Air Interdiction')
mainzone:addMember('Soviet Disruption')
mainzone:addMember('Soviet Out of Supply')	
mainzone:addMember('Soviet Interdiction')
mainzone:addMember('Soviet Rail Cut')

mainzone:addMember('ru-inf-??')
mainzone:addMember('ru-mech-??')



-- position squares on top

local css = ' border: 1px solid black;'

add_window_label('main', 'German Air Interdiction', 450, 16, 54, 54, '', css)
add_window_label('main', 'Soviet Air Interdiction', 607, 16, 54, 54, '', css)
add_window_label('main', 'Soviet Rail Cut', 759, 16, 54, 54, '', css)



-- create four hidden decks that keep track of counters revealed/not revealed

Deck:new('ru-inf', {}, {0,0})
Deck:new('ru-inf-d', {}, {0,0})
Deck:new('ru-mech', {}, {0,0})
Deck:new('ru-mech-d', {}, {0,0})

State.baseState['ru-inf'] = Counter:duplicate(Decks['ru-inf'].cards)
State.baseState['ru-inf-d'] = Counter:duplicate(Decks['ru-inf-d'].cards)
State.baseState['ru-mech'] = Counter:duplicate(Decks['ru-mech'].cards)
State.baseState['ru-mech-d'] = Counter:duplicate(Decks['ru-mech-d'].cards)




-- loads the start position (done after all windows and grids defined)

load_setup('setup.vsav')








-- create 78 untried soviet infantry and 20 untried armor/mech units

-- IMPORTANT: the following code is just for setup

  --  after the setup is saved the following code must be removed --
--------------------------------------------------------------------

--[[




local deck = Decks:get('ru-inf')

for k, v in pairs(Repository) do
	if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' then	
		local key = string.match(k, 'ru%-inf%-%w+$')		
		if key ~= nil then	
			deck:add(k)
		end
	end
end

-- remove the unused (blank) counter
table.remove(deck.cards, 30)



local deck = Decks:get('ru-mech')

for k, v in pairs(Repository) do
	if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' then	
		local key = string.match(k, 'ru%-mech%-%w+$')		
		if key ~= nil then
			deck:add(k)
		end
	end
end

for k, v in pairs(Repository) do
	if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' then	
		local key = string.match(k, 'ru%-arm%-%d+$')		
		if key ~= nil then	
			deck:add(k)
		end
	end
end



-- create counters for infantry and armor pool (78 and 20)


local point = zone:getGrid('Soviet Infantry Pool')


for i = 1, 78 do
	local toId = next_id()
	local zorder = top_zorder()
	local dx, dy = get_size('ru-inf-??')
	copyCounter('ru-inf-??', toId, zorder, 'Rf', point.x - dx, point.y - dy)
end

point = zone:getGrid('Soviet Armor Pool')

for i = 1, 20 do
	local toId = next_id()
	local zorder = top_zorder()
	local dx, dy = get_size('ru-mech-??')
	copyCounter('ru-mech-??', toId, zorder, 'Rf', point.x - dx, point.y - dy)
end

		
--]]
