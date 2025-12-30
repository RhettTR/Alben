
-- repository window visiblity

repository_visibility(false)



-- specify the visual appearance of grid (minscale, radius, r, g ,b, a)
specify_grid(0.4, 5, 255, 0, 0, 125 )


-- show the grid
show_grid(false)



-- set default offset of stacks (2 = dx 4 = dy, 2 = x-multiplier 5 = y-multiplier for open stack, 0 = unlimited shown)

setDefaultOffset(2, 4, 2, 5, 0)





-- create windows


create_window('Axis OOB', 'Axis OOB', 'G-OOB-TRC3-071104', true, '')
create_window('Russian OOB', 'Russian OOB', 'R-OOB-72-2x-071102', true, '')
create_window('POW', 'Prisoners Of War', 'pow_box', false, '')
create_window('Repl', 'Replacements', 'trc3_replacement_pool', false, '')
create_window('Airfield', 'Airfield', 'Flugplatz_box', false, '')
create_window('Para', 'Paratroops', 'paratroop_box', false, '')
create_window('Part', 'Partisans', 'partisan_box', false, '')
create_window('Ch', 'Charts', '#777777', false, 'Layout')
create_window('Op', 'Optional Units', 'TRC_optionalUnits', false, '')




-- define zone for main map


local mainzone = Zone:new('main', 'main')

local a = 52.75
local b = 90.24

mainzone:setBorder({x0 = leftPadding, y0 = topPadding, 
				    x1 = leftPadding + 3 + 3409, y1 = topPadding + 3 + 2796})

	
mainzone:hexGrid(a, b)
mainzone.coordinatesUsed = true

set_grid({mainzone.grid}, 1, 'main')




mainzone:addMember('choochoo')
mainzone:addMember('invasion')
mainzone:addMember('paratroop_marker_57x57')
mainzone:addMember('germany-sm0')
mainzone:addMember('russia-sm0')
mainzone:addMember('Battle05')


mainzone:addMembers('^ge%w*')
mainzone:addMembers('^ss%w*')
mainzone:addMembers('^it%w*')
mainzone:addMembers('^hu%w*')
mainzone:addMembers('^ro%w*')
mainzone:addMembers('^fi%w*')
mainzone:addMembers('^Hitler')
mainzone:addMembers('^stuka%w*')

mainzone:addMembers('^ru%w*')
mainzone:addMembers('ru%-HQ$')
mainzone:addMembers('Stalin$')
mainzone:addMembers('Partisan%d$')
mainzone:addMembers('Worker%-%w+$')
mainzone:addMembers('Worker%-%w+%-%w+$')
mainzone:addMembers('Worker%-%w+%-%w+%-%w+$')
mainzone:addMembers('sturm')




-- zones in OOB

local azone = Zone:new('Axis OOB', 'azone')

azone:setBorder({x0 = 56, y0 = 116, x1 = 834, y1 = 1716})

azone:squareGrid(59.2, 59.1, 3, 1)

-- needs more work (squaregrid to get custom places)




set_grid({azone.grid}, 1, 'Axis OOB')


azone:addMembers('^ge%w*')
azone:addMembers('^ss%w*')
azone:addMembers('^it%w*')
azone:addMembers('^hu%w*')
azone:addMembers('^ro%w*')
azone:addMembers('^fi%w*')
azone:addMembers('^Hitler')



local bzone = Zone:new('Russian OOB', 'bzone')

bzone:setBorder({x0 = 56, y0 = 114, x1 = 895, y1 = 1535})

bzone:squareGrid(59.2, 59.1, 3, 1)


set_grid({bzone.grid}, 1, 'Russian OOB')



bzone:addMembers('^ru%w*')
bzone:addMembers('ru%-HQ$')
bzone:addMembers('Stalin$')
bzone:addMembers('Partisan%d$')
bzone:addMembers('Worker%-%w+$')
bzone:addMembers('Worker%-%w+%-%w+$')
bzone:addMembers('Worker%-%w+%-%w+%-%w+$')






-- set the tabbed Charts window


set_window('Ch', 70, 70, 600, 550)

set_root('Ch')

root(0)

	tabs(1)

		tab(2, 'Combat')	
			html_item('combat.html')
		
		tab(2, 'Movement')				
			html_item('movement.html')
			
		tab(2, 'TRC3-Rail && Sea Movement')				
			image_item('RailSeaMovement')
				
		tab(2, 'Weather')				
			html_item('weather.html')
		
		tab(2, 'Historical Weather')				
			image_item('TRC3-Historical-Weather')					
	
	
			

-- create custom toolbar and add buttons

create_toolbar('ct', 'Custom toolbar')


add_toolbar_label('ct', 'label1', 'gr_oob_button', 20, 20, 'padding-left:10; padding-right:10')
toolbar_enable('ct', 'label1')

local css = 'font-size: 15px; border: 1px solid grey; height:20px; margin-right:4; border-radius: 2px;'
add_toolbar_label('ct', 'label2', '', 310, 22, css)
toolbar_enable('ct', 'label2')







-- set turn track


local turns = {'Turn May/Jun 1941', 'Turn Jul/Aug 1941', 'Turn Sep/Oct 1941', 'Turn Nov/Dec 1941', 
               'Turn Jan/Feb 1942', 'Turn Mar/Apr 1942', 'Turn May/Jun 1942', 'Turn Jul/Aug 1942', 'Turn Sep/Oct 1942', 'Turn Nov/Dec 1942', 
               'Turn Jan/Feb 1943', 'Turn Mar/Apr 1943', 'Turn May/Jun 1943', 'Turn Jul/Aug 1943', 'Turn Sep/Oct 1943', 'Turn Nov/Dec 1943',
               'Turn Jan/Feb 1944', 'Turn Mar/Apr 1944', 'Turn May/Jun 1944', 'Turn Jul/Aug 1944', 'Turn Sep/Oct 1944', 'Turn Nov/Dec 1944',
               'Turn Jan/Feb 1945', 'Turn Mar/Apr 1945', 'Turn May/Jun 1945'}
local phases = {' - Axis First Impulse', ' - Axis Second Impulse', ' - Soviet First Impulse', ' - Soviet Second Impuse'}



Globals:add('Turn', {turn = 1, phase = 1})
    
-- can't be local functions

function printturn()
	local turn = Globals['Turn'].turn
	local phase = Globals['Turn'].phase
	if phase >= 1 and phase <=2 then
		set_toolbar_button('ct', 'label1', 'gr_oob_button', '')
	else
		set_toolbar_button('ct', 'label1', 'russian_button', '')
	end
	set_toolbar_label('ct', 'label2', turns[turn] .. ' ' .. phases[phase])
end
                
function nextturn()
	local turn = Globals['Turn'].turn
	local phase = Globals['Turn'].phase

	if phase == 4 then
		if turn + 1 > 25 then
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

	reporttext('move', '<span style="color:gray;">*** ' .. turns[turn] .. ' ' .. phases[phase] .. '</span>')
	
	State:addStage()
	
end

add_toolbar_button('ct', 'bs', 'StepForward32', '', 'Next turn', 'nextturn()')
toolbar_enable('ct', 'bs')

State.baseState['Turn'] = Counter:duplicate(Globals['Turn'])
printturn()

add_toolbar_separator('ct')



-- define tool bar buttons

local callback = "toggle_window('Axis OOB')"
add_toolbar_button('ct', 'Axis OOB', 'gr_oob_button', 'Axis OOB', 'Show/Hide window', callback)
toolbar_enable('ct', 'Axis OOB')

local callback = "toggle_window('Russian OOB')"
add_toolbar_button('ct', 'Russian OOB', 'russian_button', 'Russian OOB', 'Show/Hide window', callback)
toolbar_enable('ct', 'Russian OOB')

local callback = "toggle_window('Airfield')"
add_toolbar_button('ct', 'Airfield', 'stuka_button', '', 'Show/Hide Airfield', callback)
toolbar_enable('ct', 'Airfield')

local callback = "toggle_window('Repl')"
add_toolbar_button('ct', 'Repl', 'replacement_button', '', 'Show/Hide Replacements', callback)
toolbar_enable('ct', 'Repl')

local callback = "toggle_window('POW')"
add_toolbar_button('ct', 'POW', 'pow_button', '', 'Show/Hide Prisoners Of War', callback)
toolbar_enable('ct', 'POW')

local callback = "toggle_window('Para')"
add_toolbar_button('ct', 'Para', 'paratroop_button', '', 'Show/Hide Paratroopers', callback)
toolbar_enable('ct', 'Para')

local callback = "toggle_window('Part')"
add_toolbar_button('ct', 'Part', 'partisan_button', '', 'Show/Hide Partisans', callback)
toolbar_enable('ct', 'Part')

local callback = "toggle_window('Ch')"
add_toolbar_button('ct', 'Ch', 'charts_button', '', 'Show/Hide Charts', callback)
toolbar_enable('ct', 'Ch')

local callback = "toggle_window('Op')"
add_toolbar_button('ct', 'Op', 'optional_button', '', 'Show/Hide Optional Units', callback)
toolbar_enable('ct', 'Op')

local callback = "show_menu('ct', 'Broom')"
add_toolbar_button('ct', 'Br', 'whiskbroom_button', '', 'Show menu', callback)
toolbar_enable('ct', 'Br')









-- loads the start position (only called after all windows and zones defined)

load_setup('setup.vsav')


	
