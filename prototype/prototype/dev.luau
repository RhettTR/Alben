-- loads a setup

load_setup('setup.vsav')


-- make repository window invisible

repository_visibility(false)


-- facilitate a right-click area around counters

createArea()


-- set map zoom range and default zoom (last value)

set_zoom_range(100, 67, 44, 30, 20, 30)




-- create a hand window with a background color and adds a button to toolbar with a callback



create_window('cphand', 'Central Powers Card Hand', '#999999')

local x = 200
local y = 400
local w = 900
local h = 600

set_window('cphand', x, y, w, h)



local css = "border:0; qproperty-alignment: AlignCenter;"

add_window_label('cphand', 'pos1', 10, 10, 0, 0, 'Icons/cardslot', css)
add_window_label('cphand', 'pos2', 400, 10, 0, 0, 'Icons/cardslot', css)


function draw(x, y)

	local drawpile = Decks:get('draw')
	
	
	if drawpile == nil then
		return
	end
	
	
	local card = drawpile:draw()

	if card == nil then
		return
	end
	
	-- drawn card face up
	Counter[card].Card.face = 1
	
	createToken(card, 'cphand', x, y)
	
end		


local action1 = "draw(197, 272)"
local action2 = "draw(587, 272)"


add_window_button('cphand', 'btn1', 78, 199, 'Icons/DrawCentral', action1)
add_window_button('cphand', 'btn2', 460, 199, 'Icons/DrawCentral', action2)



local callback = "toggle_window('cphand')"

add_toolbar_button('Icons/toolbar_cphand', 'Central Powers Hand', callback)





-- define zone for main map

local zone = Zone:new('main')


-- upper & lower corner of zone rectangle
zone:setBorder({x0 = 618, y0 = 952, x1 = 1242, y1 = 1514})


-- europe locations
zone:addGrid(1175, 1034, 'Essen')
zone:addGrid(1034, 1065, 'Aachen')
zone:addGrid(1117, 1213, 'Koblenz')
zone:addGrid(963, 1192, 'Liege')
zone:addGrid(865, 1306, 'Sedan')
zone:addGrid(973, 1407, 'Verdun')
zone:addGrid(797, 1176, 'Brussels')
zone:addGrid(709, 1313, 'Cambrai')
zone:addGrid(790, 1452, 'Chateau Thierry')

zone:addMember('German/ge_army01')
zone:addMember('German/ge_army02')
zone:addMember('German/ge_army03')

zone:addMember('French/fr_army03')
zone:addMember('French/fr_army04')
zone:addMember('French/fr_army05')

zone:addMember('British/bef_army')

zone:addMember('Allied/ap_trench1')






-- cp action round chart zone

local zonecpa = Zone:new('cpa')

zonecpa:setBorder({x0 = 1500, y0 = 20, x1 = 2071, y1 = 600})


zonecpa:addGrid(1593, 116, 'Neutral Entry (CP)')
zonecpa:addGrid(1680, 211, 'AH Reinforcements')
zonecpa:addGrid(1593, 211, 'GE Reinforcements')
zonecpa:addGrid(1767, 211, 'TU Reinforcements')
zonecpa:addGrid(1593, 308, 'SR (CP)')
zonecpa:addGrid(1680, 308, 'RP (CP)')
zonecpa:addGrid(1593, 405, 'OPS (CP)')
zonecpa:addGrid(1680, 405, 'Event (CP)')
zonecpa:addGrid(1767, 405, '1 OPS (CP)')
zonecpa:addGrid(1854, 405, 'Peace Terms (CP)')
zonecpa:addGrid(1593, 570, 'Action_CP_1')
zonecpa:addGrid(1680, 570, 'Action_CP_2')
zonecpa:addGrid(1767, 570, 'Action_CP_3')
zonecpa:addGrid(1854, 570, 'Action_CP_4')
zonecpa:addGrid(1933, 570, 'Action_CP_5')
zonecpa:addGrid(2017, 570, 'Action_CP_6')

Repository:addTrait("Markers/action_cp1", "Move", Move:new('Reset', zonecpa:getGrid('Action_CP_1')))
Repository:addTrait("Markers/action_cp2", "Move", Move:new('Reset', zonecpa:getGrid('Action_CP_2')))
Repository:addTrait("Markers/action_cp3", "Move", Move:new('Reset', zonecpa:getGrid('Action_CP_3')))
Repository:addTrait("Markers/action_cp4", "Move", Move:new('Reset', zonecpa:getGrid('Action_CP_4')))
Repository:addTrait("Markers/action_cp5", "Move", Move:new('Reset', zonecpa:getGrid('Action_CP_5')))
Repository:addTrait("Markers/action_cp6", "Move", Move:new('Reset', zonecpa:getGrid('Action_CP_6')))

Repository:addTrait("Markers/action_cp1", "MoveAll", MoveAll:new('cpa', 'Reset all'))
Repository:addTrait("Markers/action_cp2", "MoveAll", MoveAll:new('cpa', 'Reset all'))
Repository:addTrait("Markers/action_cp3", "MoveAll", MoveAll:new('cpa', 'Reset all'))
Repository:addTrait("Markers/action_cp4", "MoveAll", MoveAll:new('cpa', 'Reset all'))
Repository:addTrait("Markers/action_cp5", "MoveAll", MoveAll:new('cpa', 'Reset all'))
Repository:addTrait("Markers/action_cp6", "MoveAll", MoveAll:new('cpa', 'Reset all'))


zonecpa:addMember('Markers/action_cp1')
zonecpa:addMember('Markers/action_cp2')
zonecpa:addMember('Markers/action_cp3')
zonecpa:addMember('Markers/action_cp4')
zonecpa:addMember('Markers/action_cp5')
zonecpa:addMember('Markers/action_cp6')





-- general records track zone

local grt = Zone:new('grt')

grt:setBorder({x0 = 48, y0 = 2683, x1 = 1337, y1 = 3274})

grt:setAllowed(false)

grt:squareGrid(63, 2716, 115.0, 132.0)


grt:addMember('Markers/warstatus_ap')
grt:addMember('Markers/warstatus_cp')
grt:addMember('Markers/warstatus_combined')




-- decks zone

local decks = Zone:new('decks')

decks:setBorder({x0 = 3000, y0 = 3600, x1 = 3800, y1 = 4300})

decks:setAllowed(false)

decks:addGrid(3196, 3722, 'CP Draw')
decks:addGrid(3700, 3722, 'CP Discard')


decks:addMember('Central/card_cp02')
decks:addMember('Central/card_cp03')
decks:addMember('Central/card_cp06')
decks:addMember('Central/card_cp12')




-- create deck markers, captions and a draw deck and cards


add_window_label('main', 'd1', 3010, 3462, 0, 0, 'Icons/cardslot', css)
add_window_label('main', 'd2', 3514, 3462, 0, 0, 'Icons/cardslot', css)
add_window_label('main', 'nd1', 3010, 4000, 375, 36, '', css)
add_window_label('main', 'nd2', 3514, 4000, 375, 36, '', css)
set_label_text('main', 'nd1', 'Draw')
set_label_text('main', 'nd2', 'Discard Pile')



local zone = Zones.tags['decks']

local p1 = zone:getGrid('CP Draw')
local p2 = zone:getGrid('CP Discard')

local draw = Deck:new('draw',{Move = Move:new('Discard', p2)}, p1)
local discard = Deck:new('discard', {MoveAll = MoveAll:new('CP Draw', 'Reshuffle')}, p2)	
	
	
Repository:addTrait("Central/card_cp02", "Card", Card:new(draw))
Repository:addTrait("Central/card_cp03", "Card", Card:new(draw))
Repository:addTrait("Central/card_cp06", "Card", Card:new(draw))
Repository:addTrait("Central/card_cp12", "Card", Card:new(draw))



-- 2 is face down
createCard('Central/card_cp02', draw, 2)
createCard('Central/card_cp03', draw, 2)
createCard('Central/card_cp06', draw, 2)
createCard('Central/card_cp12', draw, 2)

