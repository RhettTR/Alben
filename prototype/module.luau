--- set side

PlayerSide = "German"


-- set alpha transparency for counters

setCounterDragAlpha(0.5)


-- set map 

setBackgroundMapID("TRCMapD")

hoover_ShowMap(false)
hoover_ShowPlace(true)


-- padding of main map

leftPadding = 50
topPadding = 50
rightPadding = 50
bottomPadding = 50

set_padding("TRCMapD", "white", leftPadding, topPadding, rightPadding ,bottomPadding)


-- hack to erase '5' on the odds counter
set_mask("Battle05", "#feff5f", 22, 17, 16 ,18)

-- redefine the __moved system image
set_image("__moved", "moved")
	

-- set log window toolbar icons

local callback = "toggle_pin('logbar', 'pin')"
add_toolbar_button('logbar', 'pin', '__unpin', '', '', callback)
add_toolbar_separator('logbar')
local callback = "roll_die()"
add_toolbar_button('logbar', 'dieroll', '1d6', '', 'dr', callback)

toolbar_enable('logbar', 'dieroll')






-- set map zoom range and default zoom (last value)

set_zoom_range(125, 100, 75, 50, 100)




--- definition of actions must happen before defintion of Traits


local function actionDelete (window, id)
	if window == 'main' then
		Counter:delete(id)
		delete_class(id)
		setGUI('main')
	end
end

local function actionFlip (window, id)
	local name
	if window == 'main' then
		name = Counter:flip(id)
		set_counter_image(id, name)
	elseif window == 'Repository' then
		name = Repository:flip(id)
		set_counter_image(id, name)
	end
	
end

local function actionToggle (window, id, trait)
	Counter[id][trait].apply = not Counter[id][trait].apply
	if trait == 'AreaOfEffect' then
		if Counter[id][trait].apply then
			local zone = Zones:get( Counter[id].window)
			local dx, dy = get_size(Counter[id].Image.images[1])
			Counter[id][trait].area =
				zone:makeAreaOfEffect(Counter[id].x + dx, Counter[id].y + dy, Counter[id]['AreaOfEffect'].radius)	
		else
			Counter[id][trait].area = {}
		end
	end
	update_class(Counter, id)	
end


local function actionToggleO (id, trait)
	local overlay = Counter[id].Overlays[Counter[id][trait].id]
	if overlay ~= nil then
		overlay.apply = not overlay.apply
		State:addMove(id, "Overlays", trait, overlay.apply, "apply")
		local status = Counter[id][trait].menuname 
		if overlay.apply == false then
			status = 'not ' .. status
		end 
		reporttext('move', Repository[Counter[id].Image.images[1]].name  .. ' becomes <span style="color:DarkSlateGray;">'
		 .. status .. '</span>')
		update_class(Counter, id)
		setGUI('main')
	end
end


local function actionToggleA (trait)
	for k, v in pairs(Counter) do
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false then
			local overlays = Counter[k].Overlays
			if overlays ~= nil then
				local overlay = overlays[Counter[k][trait].id]
				if overlay ~= nil then
					if overlay.apply then										
						actionToggleO(k, trait) 
					end
				end
			end
		end
	end
end


local function actionToggleR ()
	actionToggleA('rail')
	actionToggleA('freeRail')
end


local function actionToggleX ()
	actionToggleA('rail')
	actionToggleA('freeRail')
	actionToggleA('supply')
	actionToggleA('autoVictory')
end


local function actionRotate (window, id, sign)
	if window == 'main' then
		Counter:rotate(id, sign*Rotate.increment)
		rotate_counter(id, Counter[id].Rotate.degrees)
	else
		Repository:rotate(id, sign*Rotate.increment)
		rotate_counter(id, Repository[id].Rotate.degrees)
	end
end

local function actionMoved (window, id)
	if window == 'main' then
		if Counter[id].MarkMoved ~= nil then
			Counter:toggleMoved(id)
			set_moved_status(id, Counter[id].MarkMoved.moved)
		end
	end
end

local function actionClone (window, id)
	if window == 'main' then
		if Counter[id].Clone ~= nil then
			if Counter[id].OnlyOneInStack == nil then
				local toId = next_id()
				local zorder = top_zorder()
				cloneCounter(id, toId, zorder)
				reporttext('move', Counter[id].name  .. ' in <span style="color:DarkSlateGray;">' ..
								   Zones:getPlace(window, Counter[id].cx, Counter[id].cy) .. '</span> is cloned') 
			end	
		end
	end
end

local function actionSelect (window, id)
	if window == 'main' then
		if Counter[id].Select ~= nil then
			Counter:select(id)
		end
	end
end

local function actionMove (window, id, trait)
	
	local move = Counter:findTrait(Counter[id], trait)

	if move ~= nil then		
		
		local dx, dy = get_size(Counter[id].Image.images[1])	
						
		Counter:move(move.window, id, move.x - dx, move.y - dy, move.x,  move.y)
	
		reporttext('move', Repository[Counter[id].Image.images[1]].name  .. ' moves to <span style="color:DarkSlateGray;">'
		 .. move.window .. '</span>') 	
		
	end
				
end

local function actionMoveAll (window, id)
	
	if Counter[id].Move ~= nil then
		
		local tag = Counter[id].MoveAll.zoneTag
		local zone = Zones:get(tag)
		
		for k, v in pairs(Counter) do
			if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false then	
				if zone:hasMember(Counter[k].Image.images[1]) then					
					actionMove(window, k) 
				end
			end
		end	
	
	end
	
end

local function actionDeleteAll (window, id)
	if Counter[id] ~= nil then
		local x = Counter[id].x
		local y = Counter[id].y
		for k, v in pairs(Counter) do
			if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false then		
				if v.x == x and v.y == y then
					Counter:delete(k)
					delete_class(k)
				end
			end
		end		
	end	
end


local function actionReveal (window, id)
	
		
end


local function actionStepLoss (window, id)
	
	
end


local function actionMoveStack (window, id)
	
	
end





local function actionPlace (window, id, trait)	

	local table = Repository[Counter[id][trait].marker]
	
	if table.OnlyOneInStack ~= nil then
		if Counter:inStack(window, id, Counter[id][trait].marker) then
			return
		end
	end
	 
	local toId = next_id()
	local zorder
	if table.Marker ~= nil then
		zorder = bottom_zorder()
	else
		zorder = top_zorder()		
	end	
	
	local dx, dy = get_size(Counter[id][trait].marker)
	local ex, ey = get_size(Counter[id].Image.images[1])
	
	copyCounter(Counter[id][trait].marker, toId, zorder, Counter[id].window, 
				Counter[id].x - dx + ex, Counter[id].y - dy + ey, Counter[id].x + ex, Counter[id].y + ey)
	State:addCreate(toId, 'Create')			
		
		
	-- has to be here for zorder			
	setGUI(window)			
	
end


local function actionTake (window, id, number)

	-- set 'number' on top of stack as selected
	
	local stack = {}
	
	for k, v in pairs(Counter) do
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false then
			if v.window == Counter[id].window then
				if v.x == Counter[id].x and v.y == Counter[id].y then
					table.insert(stack, k)
					local top = stack[#stack]
					stack[top] = k
				end
			end
		end
	end
	
	table.sort(stack)
	
	local top = #stack
	

	for i = 0, number - 1 do
		toggle_select_status(stack[top - i])
		update_class(Counter, stack[top - i])
	end	
	
	 
end



local function actionInput(window, id, trait)

	local input = Counter:findTrait(Counter[id], trait)
	
	if input ~= nil then
	
		local before = Counter[id].Overlays[input.id].text
		
		local text = get_text_input(input.menuname, Counter[id].Overlays[input.id].text)
		
		Counter[id].Overlays[input.id].text = text
		
		State:addMove(id, "Overlays", trait, text, "text")
		update_class(Counter, id)
		setGUI(window)		

		reporttext('move', 'Label on ' .. Counter[id].name .. ' set from <span style="color:DarkSlateGray;">' ..
					       before .. '</span> to <span style="color:DarkSlateGray;">' .. text .. '</span>') 
		
	end

end



local function actionInc(window, id, trait)

	local table = Counter:findTrait(Counter[id], trait)
	
	if table ~= nil then
	
		local text = Counter[id].Overlays[table.id].text
		
		local number = tonumber(text)
		
		if number == nil then
			return
		end
		
		number = number + table.inc
		
		Counter[id].Overlays[table.id].text = tostring(number)
		
		State:addMove(id, "Overlays", trait, tostring(number), "text")
		update_class(Counter, id)
		setGUI(window)
		

		reporttext('move', 'Number on ' .. Counter[id].name .. ' changes from <span style="color:DarkSlateGray;">' ..
					text .. '</span> to <span style="color:DarkSlateGray;">' .. tostring(number) .. '</span>') 
		
	end
	
end


------




function action(window, id, trait, action)

	--print('doing '..action..' on '..id)
	
	if window == 'main' then 
		State:deleteNextStages()
	end
	

	
	if action == 'actionDelete' then
		actionDelete(window, id)
	end	
	if action == 'actionFlip' then
		actionFlip(window, id)
	end	
	if action == 'actionRotateCw' then
		actionRotate(window, id, 1)
	end	
	if action == 'actionRotateCcw' then
		actionRotate(window, id, -1)
	end	
	if action == 'actionMoved' then
		actionMoved(window, id)
	end
	if action == 'actionToggle' then	
		actionToggle(window, id, trait)
	end
	if action == 'actionToggleO' then
		actionToggleO(id, trait)
	end
	if action == 'actionToggleA' then
		actionToggleA(trait)
	end
	if action == 'actionToggleR' then
		actionToggleR()
	end
	if action == 'actionToggleX' then
		actionToggleX()
	end
	if action == 'actionClone' then
		actionClone(window, id)
	end 
	if action == 'actionSelect' then
		actionSelect(window, id)
	end
	if action == 'actionMove' then
		actionMove(window, id, trait)
	end
	if action == 'actionMoveAll' then
		actionMoveAll(window, id)
	end
	if action == 'actionDeleteAll' then
		actionDeleteAll(window, id)
	end
	if action == 'actionReveal' then
		actionReveal(window, id)
	end
	if action == 'actionStepLoss' then
		actionStepLoss(window, id)
	end
	if action == 'actionInput' then
		actionInput(window, id, trait)
	end	
	if action == 'actionInc' then
		actionInc(window, id, trait)
	end	
	if action == 'actionMoveStack' then
		actionMoveStack(window, id)
	end
	if action == 'actionPlace' then
		actionPlace(window, id, trait)
	end
	if string.sub(action, 1, action:len()-1) == 'actionTake' then
		actionTake(window, id, string.sub(action, -1))
	end
	
	if action == 'actionEnd' then
		State:addStage()
	end  
	
end


function dump(o)
   if type(o) == 'table' then
      local s = '{ '
      for k,v in pairs(o) do
         if type(k) ~= 'number' then k = '"'..k..'"' end
         s = s .. '['..k..'] = ' .. dump(v) .. ','
      end
      return s .. '} '
   else
      return tostring(o)
   end
end


function getTraits(window, id)

	--- find counter with key 'id'
	
	if window == 'Repository' then
		return Repository[id] == nil and {} or Repository[id]
	elseif window == 'State' then
		return State.stageStack[id] == nil and {} or State.stageStack[id]
	elseif window == 'Global' then
		return Globals[id] == nil and {} or Globals[id]
	elseif window == 'Deck'	then
		return Decks[id] == nil and {} or Decks[id]
	else
		return (Counter[id] == nil or Counter[id].deleted == true) and {} or Counter[id]			
	end	

end

function getKeys()
	return Decks:getKeys()
end

function getDeck(id)
	return Decks:get(id)
end

function getTurn()
	return Globals['Turn'].turn, Globals['Turn'].phase
end

function setTurn(t, p)
	Globals['Turn'].turn = t
	Globals['Turn'].phase = p
	State.baseState['Turn'] = Counter:duplicate(Globals['Turn'])
	printturn()
end



function triggerEvent(id, o, event)	
	if type(o) == 'table' then
		for k, v in pairs(o) do
			if type(k) == 'string' then
				if k == event and type(v) == 'function' then
					v(id)					
				end 
			end
			triggerEvent(id, v, event) 
		end
	end
end



function events(event, id, trait, key, value)
	
	-- new move, delete next moves on stack
	State:deleteNextStages()
	
 
	if event == 'movetrigger' then
		-- find key 'movetrigger' for counter 'id' and call its function
		local traits = getTraits("", id)
		triggerEvent(id, traits, event)		
	end
	
	if event == 'move' then
		State:addMove(id, trait, key, value)
	end

	if event == 'end' then
		State:addStage()
	end
	
	
end


function reporttext(type, text)
	logging(type, text)
end



function logging(type, text)

	State:addText(type, text)
	log_print(text)

end



function doTest(window, trait, id, o)
	
	for k, v in pairs(o) do
		if type(k) == 'string' then	
			if k == trait then			
				if type(v) == 'table' then
					if v.menushow ~= nil then
						return v.menushow(window, id)
					end
				end
			end
		end
		if type(v) == 'table' then
			if doTest(window, trait, id, v) == true then
				return true
			end 
		end
	end	
	
	return false
	
end


function tests(window, trait, id)
	
	local traits = getTraits(window, id)
	local res = doTest(window, trait, id, traits)
	return doTest(window, trait, id, traits)	
	
end




function create(fromId, trait)
	State:addCreate (fromId, trait)
end

function moveCounter(window, id, x, y, cx, cy)
	Counter:move (window, id, x, y, cx, cy)
end

function delete(id)
	Counter:delete (id)
end





local Counter = counter() 
local Traits = traits()
local State = state()
local Repository = repository()
local Zone = zone()










-- set rotate degrees for counters

Rotate.increment = 45






-- create Luau and C++ counter

function copyCounter(fromId, toId, zorder, window, x, y, cx, cy)
	local images, imageIndex = Image:getImages(fromId)
	Counter[toId] = Counter:new(toId, images, imageIndex, zorder, window, x, y, cx, cy)
	create_class_copy(Counter, toId)
end

function cloneCounter(fromId, toId, zorder)
	Counter[toId] = Counter:clone(fromId, zorder)	
	create_class_copy(Counter, toId)
	State:addCreate(toId, 'Create')
end

function setCounter(id, table)
	Counter[id] = Counter:load(table)
	create_class_copy(Counter, id)
end

function loadCounter(table)
	local id = table.id
	Counter[id] = Counter:load(table)
	create_class_copy(Counter, id)
	State.baseState[id] = Counter:duplicate(Counter[id])
	return id
end

function loadDeck(table)
	local id = table.id
	local deck = Deck:load(id, table)
	Decks:add(id, deck)
	State.baseState[id] = Counter:duplicate(Decks[id].cards)
	return id
end

function updatePos(window, id, x, y, cx, cy)
	Counter[id]["window"] = window
	Counter[id]["x"] = x
	Counter[id]["y"] = y
	Counter[id]["cx"] = cx
	Counter[id]["cy"] = cy
end

function updateMoved(id, moved)
	Counter:markMoved (id, moved)
end




-- cards


function copyCard(fromId, toId, oldtoId, zorder, window, x, y)
	copyCounter(fromId, toId, zorder, window, x, y, x, y)
end


function createCard(fromId, deck, face)

	local toId = next_id()
	local dx, dy = get_size(fromId)
	local zorder = top_zorder()	
	local images, imageIndex = Image:getImages(fromId)
	-- add card to deck
	deck:add(toId)
	Counter[toId] = Counter:new(toId, images, face, zorder, deck.x - dx, deck.y - dy)
	Counter[toId].Card.face = face
	create_class_copy(Counter, toId)
	State.baseState[toId] = Counter:duplicate(Counter[toId])
end





function updateSide(side)
	PlayerSide = side
end

function saveStage()
	State.savedPointer = State.stagePointer
end

function deleteAll()
	for id, v in pairs(Counter) do
		if type(id) == "string" and id ~= '__index' and type(v) ~= 'function' then
			delete_class(id)
		end
	end
end

function resetState()
	State:resetState()
end

function getRange()
	return State.stagePointer, State.savedPointer
end

function createArea()
	Global:addTrait("Area", Area)
	create_area()
end

function setDefaultOffset(dx, dy, xmultiplier, ymultiplier, shown)
	defaultStack = Offset:new(dx, dy, xmultiplier, ymultiplier, shown)
end

function defaultOffset()
	return defaultStack['xoffsetClosed'], defaultStack['yoffsetClosed'], defaultStack['xopenFactor'], defaultStack['yopenFactor'], defaultStack['shown']
end





-- logging

function logReset()
	Log:reset()
end

function loadLog(table)
	Log:add(table)
end

function logStep(oneStep)
	Log:step(oneStep)
end

function logAbort()
	Log:abort()
end



function mapPlace(window, dx, dy, x, y)

	return Zones:getPlace(window, x + dx, y + dy)
	
end	


function reportmove(id, name, fromtag, totag, fromx, fromy, tox, toy)

	local dx, dy = get_size(name)
	local display = Repository[name].name
	local text = ''
	
	if fromtag == 'Repository' then	
		text = display .. ' placed on ' .. Zones:getPlace(totag, tox + dx, toy + dy)
	else
	
		local zonedisplay = ''	
		if fromtag ~= totag then
			zonedisplay = ' in ' .. fromtag 	
		end		
		text = display .. zonedisplay .. ' moves from <span style="color:DarkSlateGray;">' .. 
			   Zones:getPlace(fromtag, fromx + dx, fromy + dy) .. '</span> to <span style="color:DarkSlateGray;">' .. 
			   Zones:getPlace(totag, tox + dx, toy + dy) .. '</span>'
	   
	end			 
	logging("html", text)
	
end




---


function runScript(func)

	_G[func]()
	
end



-- event hooks

function beforeDrag(id)

	local counter
	
	if Counter[id] ~= nil then
		counter = Counter[id]
	else
		counter = Repository[id]
	end
	
	-- if counter in repository
	if Counter[id] == nil then
		if counter[id]['OnlyOneInstance'] ~= nil then
			-- check if counter already exists
			for k, v in pairs(Counter) do		
				if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false then
					if v.Image.images[1] == id and Counter[k].OnlyOneInstance ~= nil then
						return false
					end
				end
			end
		end
	end
	
	-- if counter not in repository
	if Counter[id] ~= nil then
		if Counter:findTrait(Counter[id], 'CantBeMoved') ~= nil then
			return false
		end	
		if Counter:findTrait(Counter[id], 'AreaOfEffect') ~= nil then
			Counter[id].AreaOfEffect.area = {}
			Counter[id].AreaOfEffect.apply = false
			update_class(Counter, id)
		end
	end
	
	return true
	
end


function afterDrag(window, id, dx, dy, x, y)

	return Zones:snapTo(window, id, dx, dy, x, y)
	
end



function dropped(tag, id, x, y)


end


function moved(tag, toid, x, y)
	
	-- moved is also called from Counter:generateGUI()
	
		
end


function deleted(id)

	-- IMPORTANT  must always be here
	Counter[id].deleted = true

end


function flipped(id, name)
	
	
end


function undohook()

	
end


function selectable(id)
	
	return Counter:findTrait(Counter[id], 'CantBeMoved') == nil
	
end


function menu(id, name, marker)

	-- returns whether menu entry shown or not .. false = not
	

	return true

end


function menucounter(id)

	-- returns whether counter should have a menu
				
	if Counter[id] ~= nil then
	
		local zone = Zones:findZone(Counter[id].x, Counter[id].y)
	
		if zone == nil then
			return false
		end
		
		if zone.tag == 'main' then
			return true
		end
			
	end

	return false

end


function pointoffset(window, x, dx, y, dy)
	
	local zone = Zones:findZone(window, x + dx, y + dy)
	
	if zone == nil then
		return defaultOffset()
	end
	
	local offset = zone:getOffset(x + dx, y + dy)
	local next = next
	
	if offset == nil or next(offset) == nil then
		return defaultOffset()
	end
	

	return offset['xoffsetClosed'], offset['yoffsetClosed'], offset['xopenFactor'], offset['yopenFactor'], offset['shown']
	
end


------




-- handlers

function handlers(id, func)
	_G[func](id)	
end





-- execute module

dev()


