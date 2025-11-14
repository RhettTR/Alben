--- set side

PlayerSide = "German"


-- set alpha transparency for counters

setCounterDragAlpha(0.5)


-- set map 

setBackgroundMapID("PGG-map-ver1")

hoover_ShowMap(false)
hoover_ShowPlace(true)


-- padding of main map

leftPadding = 20
topPadding = 20
rightPadding = 20
bottomPadding = 20

set_padding("PGG-map-ver1", leftPadding, topPadding, rightPadding ,bottomPadding)
	



-- set seed for random numbers

math.randomseed(os.time())


--- definition of actions must happen before defintion of Traits


local function actionDelete (window, id)
	if window == 'main' then
		Counter:delete(id)
		delete_class(id)
		setGUI()
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
			local toId = next_id()
			local zorder = top_zorder()
			cloneCounter(id, toId, zorder)
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

local function actionMove (window, id)

	if window == 'main' then
	
		local move = Counter:findTrait(Counter[id], 'Move')
		
		if move ~= nil then
		
			if Counter[id].Card ~= nil then	
					
				local deck = Decks:find(Counter[id].Card.deck.x, Counter[id].Card.deck.y) 	
				deck:remove(id)
							
				local newdeck = Decks:find(deck.moveto.Move.x, deck.moveto.Move.y)	
				newdeck:add(id)
				Counter[id].Card.deck = Counter:duplicate(newdeck)
				if newdeck.id == 'draw' then				
					Counter[id].Image.imageIndex = 2
					Counter[id].Card.face = 2
				end
						
			end
			
			local dx, dy = get_size(Counter[id].Image.images[1])
	
						
			Counter:move(window, id, move.x - dx, move.y - dy)
			
			reporttext('move', '<span style="color:gray;">' .. Repository[Counter[id].Image.images[1]].name .. ' moves to ' .. 
					   Zones:getPlace(window, move.x,  move.y) .. '</span>') 
			
		end
				
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
		
		
	else
	
		-- move without a move trait (reshuffel)
		
		-- find the moveall trait for this counter
		local moveall = Counter:findTrait(Counter[id], 'MoveAll')
		
		-- get the zone
		local zone = Zones:get('decks')
		
		-- find point to move to
		local point = zone:getGrid(moveall.zoneTag)
	
		
		
		for k, v in pairs(Counter) do
			if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false then
				
				-- for all counters find its moveall trait if any
				local mall = Counter:findTrait(Counter[k], 'MoveAll')
	
								
				if mall ~= nil then	
				
					-- test if moveall is to same zone
					if mall.zoneTag == moveall.zoneTag then	
				
						
						if Counter[k].Card ~= nil then
						
							-- find its deck
							local deck = Decks:find(Counter[k].Card.deck.x, Counter[k].Card.deck.y)
							if deck == nil then 
								return
							end
							-- find deck to be moved to			
							local newdeck = Decks:find(point.x, point.y)
							if newdeck == nil then 	
								return
							end
							
							
							-- remove card from deck
							
							deck:remove(k)
							
							
							
							
							--make sure the counter belongs to the new deck
							
							newdeck:add(k)
							
							
							
							Counter[k].Card.deck = Decks:find(point.x, point.y)
							
							
							--  all reshuffled cards are face down
							if newdeck.id == 'draw' then				
								Counter[k].Image.imageIndex = 2
								Counter[k].Card.face = 2								
							end
						end
						
						Counter:move(Counter[k].window, k, point.x, point.y)
						
					end		
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

	local drawpile
	
	if Counter[id].Image.images[1] == 'ru-inf-??' then
		drawpile = Decks:get('ru-inf')
	else
		drawpile = Decks:get('ru-mech')
	end	

	if drawpile == nil then
		return
	end
	
	-- false is to not delete cpp counter in deck
	local drawn = drawpile:draw(false)

	if drawn == nil then
		return
	end
	
	
	local toId = next_id()
	
	copyCounter(drawn, toId, Counter[id].zorder, Counter[id].window, Counter[id].x, Counter[id].y)
	State:addCreate(toId, 'Create')
	
	local dx, dy = get_size(Counter[id].Image.images[1])
	
	reporttext('reveal', Repository[Counter[id].Image.images[1]].name .. ' in ' .. 
						 Zones:getPlace(window,  Counter[id].x + dx, Counter[id].y + dy) ..
						 ' is revealed as ' ..
						 Repository[Counter[toId].Image.images[1]].name)
	
	local cards = Counter:duplicate(drawpile.cards)
	
	State:addMove(drawpile.id, 'Decks', '', cards)
	
	
	Counter:delete(id)
	delete_class(id)
	
		
end


local function actionStepLoss (window, id)

	if Counter[id].Side == 'German' then
		if Counter[id].Image.imageIndex < #Counter[id].Image.images then
			reporttext('steploss', Repository[Counter[id].Image.images[1]].name .. ' suffers step loss')		
			actionFlip(window, id)
		else
			reporttext('steploss', Repository[Counter[id].Image.images[1]].name .. ' goes to graveyard')
			Counter:move('Gy', id, 480 + math.random(-100, 100), 270 + math.random(-100, 100))
		end
		
	else -- soviet
	
		if Counter[id].Type == 'HQ' then
			reporttext('steploss', Repository[Counter[id].Image.images[1]].name .. ' goes to graveyard')
			Counter:move('Gy', id, 180 + math.random(-100, 100), 130 + math.random(-100, 100))
			return
		end
		
		local zone = Zones:get('rfzone')
		local toId = next_id()
		local zorder = top_zorder()
		
		
		if Counter[id].Type == 'Inf' then	
			
			reporttext('steploss', Repository[Counter[id].Image.images[1]].name .. ' goes to Soviet Infantry Dead Pool')
				
			local pos = zone:getGrid('Soviet Infantry Dead Pool')
			local dx, dy = get_size('ru-inf-??')
			copyCounter('ru-inf-??', toId, zorder, 'Reinforcements', pos.x - dx, pos.y - dy)
			
			Counter:setTrait(Counter[toId], 'MoveStack', MoveStack:new('Reuse pile', zone:getGrid('Soviet Infantry Pool')))
			Counter:setTrait(Counter[toId].MoveStack, 'menutrait', 'MoveStack')
			Counter:setTrait(Counter[toId].MoveStack, 'menushow', 
				function(window, id)				
					local number = Counter:getStackSize('Reinforcements', zone:getGrid('Soviet Infantry Pool'))
					return number == 0 				
				end)
				
			State:addCreate(toId, 'Create')
			
			Decks['ru-inf-d']:add(Counter[id].Image.images[1])
			local cards = Counter:duplicate(Decks['ru-inf-d'].cards)
			State:addMove('ru-inf-d', 'Decks', '', cards)
			
			
		else -- type is 'Arm'
		
			reporttext('steploss', Repository[Counter[id].Image.images[1]].name .. ' goes to Soviet Armor Dead Pool')
			
			local pos = zone:getGrid('Soviet Armor Dead Pool')
			local dx, dy = get_size('ru-mech-??')
			copyCounter('ru-mech-??', toId, zorder, 'Reinforcements', pos.x - dx, pos.y - dy)
			
			Counter:setTrait(Counter[toId], 'MoveStack', MoveStack:new('Reuse pile', zone:getGrid('Soviet Armor Pool')))
			Counter:setTrait(Counter[toId].MoveStack, 'menutrait', 'MoveStack')
			Counter:setTrait(Counter[toId].MoveStack, 'menushow', 
				function(window, id)				
					local number = Counter:getStackSize('Reinforcements', zone:getGrid('Soviet Armor Pool'))
					return number == 0 					
				end)
				
			State:addCreate(toId, 'Create')
			
			Decks['ru-mech-d']:add(Counter[id].Image.images[1])
			local cards = Counter:duplicate(Decks['ru-mech-d'].cards)
			State:addMove('ru-mech-d', 'Decks', '', cards)
				
		end
		
		Counter:setTrait(Counter[toId], "CantBeMoved", CantBeMoved)	
		Counter:removeTrait(Counter[toId], "Reveal")
		
		Counter:delete(id)
		delete_class(id)
			
		
	end
	
	
end


local function actionMoveStack (window, id)
	
	local toPos
	local fromPos
	local zone = Zones:get('rfzone')

	if Counter[id].Image.images[1] == 'ru-inf-??' then
		toPos = zone:getGrid('Soviet Infantry Pool')
		fromPos = zone:getGrid('Soviet Infantry Dead Pool')
		
		Decks:merge('ru-inf', 'ru-inf-d')
		
		local deck = Decks:get('ru-inf')
		State:addMove('ru-inf', 'Decks', '', deck.cards)
		local deck = Decks:get('ru-inf-d')
		State:addMove('ru-inf-d', 'Decks', '', deck.cards)
	else
		toPos = zone:getGrid('Soviet Armor Pool')
		fromPos = zone:getGrid('Soviet Armor Dead Pool')
		
		Decks:merge('ru-mech', 'ru-mech-d')

		local deck = Decks:get('ru-mech')
		State:addMove('ru-mech', 'Decks', '', deck.cards)
		local deck = Decks:get('ru-mech-d')
		State:addMove('ru-mech-d', 'Decks', '', deck.cards)
	end
	
	
	
	local number = Counter:getStackSize(Counter[id].window, fromPos)

	for i = 1, number do
		local toId = next_id()
		local zorder = top_zorder()
		local dx, dy = get_size(Counter[id].Image.images[1])		
		copyCounter(Counter[id].Image.images[1], toId, zorder, Counter[id].window, toPos.x - dx, toPos.y - dy)
		State:addCreate(toId, 'Create')
	end
	
	
	actionDeleteAll (window, id)
	
	State:addStage()
	
	
end



local function retreat(window, id, zone, incx, incy)
	local dx, dy = get_size(Counter[id].Image.images[1])		
	local ok, x, y = afterDrag(window, id, dx, dy, Counter[id].x + dx + incx - zone.border.x0, 
												   Counter[id].y + dy + incy - zone.border.y0)									   
	if ok and zone:isWithinBorder(x, y) then
		reporttext('move', Repository[Counter[id].Image.images[1]].name .. ' retreats to ' .. 
				   Zones:getPlace(window, x + dx, y + dy)) 
		Counter:move(window, id, x, y )
	end
end

local function actionMoveN (window, id)
	local dx, dy = get_size(Counter[id].Image.images[1])
	local zone = Zones:findZone(window, Counter[id].x + dx, Counter[id].y + dy)	
	retreat(window, id, zone, 0, -math.round(zone.b))
end

local function actionMoveS (window, id)
	local dx, dy = get_size(Counter[id].Image.images[1])
	local zone = Zones:findZone(window, Counter[id].x + dx, Counter[id].y + dy)
	retreat(window, id, zone, 0, math.round(zone.b))
end

local function actionMoveNW (window, id)
	local dx, dy = get_size(Counter[id].Image.images[1])
	local zone = Zones:findZone(window, Counter[id].x + dx, Counter[id].y + dy)
	retreat(window, id, zone, -math.round(zone.a / 2), -math.round(zone.b / 2))
end

local function actionMoveSW (window, id)
	local dx, dy = get_size(Counter[id].Image.images[1])
	local zone = Zones:findZone(window, Counter[id].x + dx, Counter[id].y + dy)
	retreat(window, id, zone, -math.round(zone.a / 2), math.round(zone.b / 2))
end

local function actionMoveNE (window, id)
	local dx, dy = get_size(Counter[id].Image.images[1])
	local zone = Zones:findZone(window, Counter[id].x + dx, Counter[id].y + dy)
	retreat(window, id, zone, math.round(zone.a / 2), -math.round(zone.b / 2))
end

local function actionMoveSE (window, id)
	local dx, dy = get_size(Counter[id].Image.images[1])
	local zone = Zones:findZone(window, Counter[id].x + dx, Counter[id].y + dy)
	retreat(window, id, zone, math.round(zone.a / 2), math.round(zone.b / 2))
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
				Counter[id].x - dx + ex, Counter[id].y - dy + ex)
	State:addCreate(toId, 'Create')			
		
		
	-- has to be here for zorder			
	setGUI()			
	
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

------




function action(window, id, trait, action)

	--print('doing '..action..' on '..id)
	
	if window == 'main' then 
		State:deleteNextStages()
	end
	
	--if not action == 'actionEnd' then
	--	handlers(id, action)
	--end	
	
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
	if action == 'actionClone' then
		actionClone(window, id)
	end 
	if action == 'actionSelect' then
		actionSelect(window, id)
	end
	if action == 'actionMove' then
		actionMove(window, id)
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
	if action == 'actionMoveStack' then
		actionMoveStack(window, id)
	end
	if action == 'actionMoveN' then
		actionMoveN(window, id)
	end
	if action == 'actionMoveNW' then
		actionMoveNW(window, id)
	end
	if action == 'actionMoveS' then
		actionMoveS(window, id)
	end
	if action == 'actionMoveSW' then
		actionMoveSW(window, id)
	end
	if action == 'actionMoveSE' then
		actionMoveSE(window, id)
	end
	if action == 'actionMoveNE' then
		actionMoveNE(window, id)
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
		return Global[id] == nil and {} or Global[id]
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

function copyCounter(fromId, toId, zorder, window, x, y)
	local images, imageIndex = Image:getImages(fromId)
	Counter[toId] = Counter:new(toId, images, imageIndex, zorder, window, x, y)
	create_class_copy(Counter, toId)
end

function cloneCounter(fromId, toId, zorder)
	Counter[toId] = Counter:clone(fromId, zorder)
	create_class_copy(Counter, toId)
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





-- cards


function copyCard(fromId, toId, oldtoId, zorder, window, x, y)
	copyCounter(fromId, toId, zorder, window, x, y)
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

	if Zones.coordinatesUsed then
		return Zones:printCoordinates(x, y)
	end
	return Zones:getPlace(window, x + dx, y + dy)
end	


function updatePos(window, id, x, y)
	Counter[id]["window"] = window
	Counter[id]["x"] = x
	Counter[id]["y"] = y
end

function updateMoved(id, moved)
	Counter:markMoved (id, moved)
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

function setDefaultOffset(dx, dy, multiplier, shown)
	defaultStack = Offset:new(dx, dy, multiplier, shown)
end

function defaultOffset()
	return defaultStack['xoffsetClosed'], defaultStack['yoffsetClosed'], defaultStack['openFactor'], defaultStack['shown']
end




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



function dropped(tag, fromid, toid, x, y)
	
	if Counter[toid]['Take2'] ~= nil then
		Counter[toid]['Take2'] = nil 	
	end
	
	if Counter[toid]['Take3'] ~= nil then
		Counter[toid]['Take3'] = nil 	
	end

end


function moved(tag, toid, x, y)
	
	-- moved is also called from Counter:generateGUI()
	
		
end


function deleted(id)

	-- very important -- must always be here
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
	

	return offset['xoffsetClosed'], offset['yoffsetClosed'], offset['openFactor'], offset['shown']
	
end


------





-- handlers

function handlers(id, func)
	_G[func](id)	
end






--local ok = beforeDrag("1002")
--if ok then
--	local x,y
--	ok, x, y = afterDrag("main", "1002", 45, 45, 37, 27)
--	if ok then
--		updatePos("1002", x, y)
--		Counter:generateGUI()
--	end
--end 






-- execute module

dev()


