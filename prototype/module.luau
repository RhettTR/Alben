

-- set alpha transparency for counters

setCounterDragAlpha(0.5)


-- set map 

set_background_mapID("main", "map_classic_vassal")

hoover_ShowMap(true)
hoover_ShowPlace(true)






	

-- set log window toolbar icons

local callback = "toggle_pin('logbar', 'pin')"
add_toolbar_button('logbar', 'pin', '__unpin', '', '', callback)
add_toolbar_separator('logbar')
local callback = "roll_die()"
add_toolbar_button('logbar', 'dieroll', '__die', '', 'dr', callback)

toolbar_enable('logbar', 'dieroll')







-- set toolbar zoom range and default zoom (last value)

set_toolbar_zoom(100, 75, 63, 56, 50, 44, 40, 33, 25, 20, 20)

-- set main map zoom

set_map_zoom('main', 0.1, 1.0)







--- definition of actions must happen before defintion of Traits


local function actionDelete (window, id)
	if window ~= 'Repository' then
		setNoAction(false)
		reporttext('move', '<span style="color:DarkSlateGray;">' .. Counter[id].name  .. 
						   '</span> in <span style="color:DarkSlateGray;">' ..
						   Zones:getPlace(window, Counter[id].cx, Counter[id].cy) .. '</span> is deleted')
		Counter:delete(id)
		delete_class(id)
		setGUI(window)		
	end
end

local function actionFlip (window, id)

	if window == 'Repository' then
		local newname = Repository:flip(id)	
		set_counter_image(id, newname)	
		return
	else	
		local name = Counter[id].Image.images[Counter[id].Image.imageIndex]
		local newname = Counter:flip(id)
		if name ~= newname then
			setNoAction(false)
			set_counter_image(id, newname)
			
			local aName = Repository[newname] ~= nil and Repository[newname].name
			local unit = Repository[Counter[id].Image.images[1]].name
					 
			if not aName then
				reporttext('move', unit .. ' flipped')				
			else			 						 
				reporttext('move', '<span style="color:DarkSlateGray;">' .. unit .. 
						   '</span> flipped to <span style="color:DarkSlateGray;">' .. 
					       aName .. '</span>' .. ' in <span style="color:DarkSlateGray;">' ..
						   Zones:getPlace(window, Counter[id].cx, Counter[id].cy) .. '</span>')
			end			
		end	
	end	
	
end

local function actionToggle (window, id, trait)
	if Counter[id][trait].apply ~= nil then
		Counter[id][trait].apply = not Counter[id][trait].apply
		if trait == 'AreaOfEffect' then
			if Counter[id][trait].apply then
				local zone = Zones:get( Counter[id].window)
				--local dx, dy = get_size(Counter[id].Image.images[1])
				Counter[id][trait].area =
					zone:makeAreaOfEffect(Counter[id].cx, Counter[id].cy, Counter[id]['AreaOfEffect'].radius)	
			else
				Counter[id][trait].area = {}
			end
		end
		setNoAction(false)
		update_class(Counter, id)		
	end	
end

local function actionRotate (window, id, sign)
	if window == 'Repository' then
		if Repository:hasTrait(Repository[id], 'Rotate') then
			Repository:rotate(id, sign*Rotate.increment)
			rotate_counter(id, Repository[id].Rotate.degrees)	
		end
	else
		if Counter:hasTrait(Counter[id], 'Rotate') then
			setNoAction(false)
			Counter:rotate(id, sign*Rotate.increment)
			rotate_counter(id, Counter[id].Rotate.degrees)			
		end
	end
end

local function actionMoved (window, id)
	if window == 'Repository' then
		return
	else
		if Counter:hasTrait(Counter[id], 'MarkMoved') then
			setNoAction(false)
			Counter:toggleMoved(id)
			set_moved_status(id, Counter[id].MarkMoved.moved)
		end
	end
end

local function actionClone (window, id)
	if window == 'Repository' then
		return 
	else
		if Counter:hasTrait(Counter[id], 'Clone') then
			if not Counter:hasTrait(Counter[id], 'OnlyOneInStack') then
				setNoAction(false)
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
	if window == 'Repository' then
		return
	else	
		if Counter:hasTrait(Counter[id], 'Select') then
			Counter:select(id)
		end	
	end
end


local function actionMove (window, id, field, cx, cy)


	local move = Counter:findTrait(Counter[id], field, 'Move')
	
	if move == nil then
		error(4, 'Move')
		return 
	end
		
	
	local zone = Zones:get(move.zone)
	local point
	
	if zone == nil then
		point = move.point
	else
		point = zone:getGrid(move.point)
	end
		
			
	local dx, dy = get_corner(Counter[id].Image.images[1])
	
	if cx ~= nil and cy ~= nil then
		-- cx, cy is the allowed place to move to
		if point.x ~= cx or point.y ~= cy then
			return 
		end
	end
	
	
	-- only actual movement
	
	if Counter[id].cx == point.x and Counter[id].cy == point.y and Counter[id].window == window then
		return
	end
		
	
	setNoAction(false)
	
	local zorder = top_zorder()
	
		
	
	Counter:move(move.window, id, zorder, point.x - dx, point.y - dy, point.x,  point.y)				
	
	
	update_class(Counter, id)
	
	if zone == nil then
		reporttext('move', '<span style="color:DarkSlateGray;">' .. Counter[id].name .. 
						   '</span> moves to <span style="color:DarkSlateGray;">' ..
						   get_title(move.window) .. '</span>')
	else
		reporttext('move', '<span style="color:DarkSlateGray;">' .. Counter[id].name .. 
						   '</span> moves to <span style="color:DarkSlateGray;">' ..
						   point.name .. '</span> in <span style="color:DarkSlateGray;">' ..
						   zone.name .. '</span>')
	end
		 	 
				
end



local function actionMoveAll (window, id)

	-- move counters in zone or cards in deck
	
	local move = Counter:findTrait(Counter[id], 'MoveAll')
	if move == nil then
		error(4, 'MoveAll')
		return 
	end
		
	if Zones:is(move.tag) then
	
		local zone = Zones:get(move.tag)
				
		for k, v in pairs(Counter) do
			if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false then	
				if zone:hasMember(Counter[k].Image.images[1]) then			
					actionMove(window, k, 'Move') 
				end
			end
		end
		
		Counter:generateGUI()
		
		return 
		
	end

	error(16, move.tag)
	
end


local function actionRemoveMarkers (window, id)
	local result = false
	if Counter[id] ~= nil then
		local cx = Counter[id].cx
		local cy = Counter[id].cy
		for k, v in pairs(Counter) do	
			if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false then	
				if v.cx == cx and v.cy == cy then
					local value = Counter:findTrait(v, 'marker')
					if value ~= nil then
						reporttext('move', 'A <span style="color:DarkSlateGray;">' .. Counter[k].name  .. 
								   '</span> is deleted in <span style="color:DarkSlateGray;">' .. 
								   Zones:getPlace(window, Counter[id].cx, Counter[id].cy) .. '</span>')
						setNoAction(false)
						Counter:delete(k)
						delete_class(k)						
					end	
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
	
	-- move cards to deck; source and destination given in move
	
	local move = Counter:findTrait(Counter[id], 'MoveStack')
	if move == nil then
		error(4, 'MoveStack')
		return 
	end
	

	for k, v in pairs(Counter) do
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false then
			
			--local trait = Counter:findTrait(Counter[k], 'Card')
			local trait = Counter[k].Card
				
			if trait ~= nil then	
				
				if trait.deckId == move.id then
				
					setNoAction(false)
					
					local deck = Decks:get(move.id)
						
					Counter[k].Image.imageIndex = 2
					
					State:addMove (k, 'Image', 'imageIndex', Counter[k].Image.imageIndex)	
					
					local dx, dy = get_corner(k)
					local zorder = top_zorder()	
						
					Counter:move(window, k, zorder, move.cx - dx, move.cy - dy, move.cx, move.cy)
						
					
				end
										
			end
		end
	end
	
	
	Counter:generateGUI()	
	
	
end


local function actionDraw (window, id)

	local trait = Counter:findTrait(Counter[id], 'Card')
				
	if trait ~= nil then
	
		local deck = Decks:get(trait.deckId)
		local zone = Zones:findZone(window, deck.cx, deck.cy)	
		local name = zone:getGridName(deck.cx, deck.cy)
		
		
		if zone == nil then
			return 
		end
		
		deck:draw(window, zone.tag, name)
		
	end
	
end



--[=====[
--]=====]


local function actionPlace (window, id, trait, actionid)	

	
	local table = Repository[actionid]
	
	if table == nil then
		return 
	end
	
	if Counter:hasTrait(table, 'OnlyOneInStack') then
		if Counter:MarkerInStack(window, Counter[id].cx, Counter[id].cy, 'OnlyOneInStack', actionid) then
			return 
		end
	end
	
	
	setNoAction(false)
	
	  
	local toId = next_id()
	local zorder
	
	
	if Counter:hasTrait(table, 'Marker') then
	
		-- set what markers come below counters
		
		if table.Marker.place == 'bottom' then 
		
			zorder = bottom_zorder()
		else
			zorder = top_zorder()
		end
		
	else
		zorder = top_zorder()		
	end	
	
	
	copyCounter(actionid, toId, zorder, Counter[id].window, 
				Counter[id].x, Counter[id].y, Counter[id].cx, Counter[id].cy)
	
	
	State:addCreate(toId, 'Create')
	
	
	reporttext('move', '<span style="color:DarkSlateGray;">' .. Counter[toId].name  .. 
			   '</span> is placed in <span style="color:DarkSlateGray;">' .. 
			   Zones:getPlace(window, Counter[id].cx, Counter[id].cy) .. '</span>')			
		
		
	-- has to be here for zorder			
	setGUI(window)
	
				
	
end



local function actionFacing (window, id)

	-- set global, triggers feeding of points
	
	startFeed(id)

end




------


NoAction = true

function setNoAction(value)

	-- if action detected delete stages
	if NoAction == true and value == false then
		State:deleteNextStages()
		if net_connected() then
			local table = { trait = "deletestages" }
			send_table({table}, 1, 1)
		end
	end
	
	NoAction = value
	
end

function ifAction()

	if NoAction == false then
		State:addStage()
	end
	
	NoAction = true
	
end



function action(window, id, trait, field, action, actionid)

	--print('doing '..action..' on '..id)
	
	
	if action == 'actionDelete' then
		actionDelete(window, id)
	elseif action == 'actionSelect' then
		actionSelect(window, id)
	elseif action == 'actionFlip' then
		actionFlip(window, id)
	elseif action == 'actionRotateCw' then
		actionRotate(window, id, 1)
	elseif action == 'actionRotateCcw' then
		actionRotate(window, id, -1)
	elseif action == 'actionMoved' then
		actionMoved(window, id)
	elseif action == 'actionToggle' then	
		actionToggle(window, id, trait)
	elseif action == 'actionClone' then
		actionClone(window, id)
	elseif action == 'actionMove' then
		actionMove(window, id, field)
	elseif action == 'actionMoveAll' then
		actionMoveAll(window, id)
	elseif action == 'actionRemoveMarkers' then
		actionRemoveMarkers(window, id)
	elseif action == 'actionReveal' then
		actionReveal(window, id)
	elseif action == 'actionStepLoss' then
		actionStepLoss(window, id)
	elseif action == 'actionInput' then
		actionInput(window, id, trait)
	elseif action == 'actionInc' then
		actionInc(window, id, trait)
	elseif action == 'actionDraw' then
		actionDraw(window, id)
	elseif action == 'actionMoveStack' then
		actionMoveStack(window, id, trait)
	elseif action == 'actionPlace' then
		actionPlace(window, id, trait, actionid)
	elseif action == 'actionFacing' then
		actionFacing (window, id) 
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


function findTrait(id, trait, field)

	local found = Counter:hasTrait(Counter[id], trait)
	
	if found then
		local trait = Counter:findTrait(Counter[id], trait)
		return trait[field], true
	else
		return '', false
	end 
	
end


function getKeys()
	return Decks:getKeys()
end

function getDeck(id)
	return Decks:get(id)
end

function getTurn()
	if Globals['Turn'] == nil then
		return 0,0
	end
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
	if net_connected() then
		local table = { trait = "deletestages" }
		send_table({table}, 1, 1)
	end
 
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
	nick = get_nick()
	log_print(nick .. text)
	State:addText(type, nick .. text)
end



function doTest(window, field, id, o)

	for k, v in pairs(o) do
	
		if type(v) == 'table' then
			if v.menufield ~= nil then
				if v.menufield == field then
					if v.menushow ~= nil then
						if type(v.menushow) == 'function' then
							return v.menushow(window, id)
						end	
					end		
				end			
			end
		end
		
		if type(v) == 'table' then
			if doTest(window, field, id, v) == true then
				return true
			end 
		end
		
	end	
	
	return false
	
end


function tests(window, field, id)
	
	local traits = getTraits(window, id)	
	return doTest(window, field, id, traits)	
	
end




function create(fromId, trait)
	State:addCreate (fromId, trait)
end

function moveCounter(window, id, zorder, x, y, cx, cy)
	Counter:move (window, id, zorder, x, y, cx, cy)
end

function delete(id)
	Counter:delete (id)
end




local Report = report()
local Counter = counter()
local State = state()
local Zone = zone()
local Traits = traits()
local Repository = repository()








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
	Counter[id] = nil
	Counter[id] = Counter:load(table)
	local trait = Counter:findTrait(Counter[id], 'Card')
	if trait ~= nil then
		local deck = getDeck(trait.deckId)
		trait.actionUp = deck.actionUp
		trait.actionDown = deck.actionDown
	end
	create_class_copy(Counter, id)
	State.baseState[id] = Counter:duplicate(Counter[id])
	return id
end

function updateCounter(table)
	State:recieveTable(table)
end

function loadDeck(table)
	local id = table.id
	local deck = Deck:load(id, table)
	Decks:add(id, deck)
	State.baseState[id] = Counter:duplicate(Decks[id])
	return id
end

function updatePos(window, id, zorder, x, y, cx, cy)
	Counter[id].window = window
	Counter[id].zorder = zorder
	Counter[id].x = x
	Counter[id].y = y
	Counter[id].cx = cx
	Counter[id].cy = cy
end

function updateMoved(id, moved)
	Counter:markMoved (id, moved)
end

function refresh()
	Counter:refresh()
end



local k

function sync()

	if net_connected() then
				
		
		local v
		
		k, v  = next(Counter, nil)
		
		if k == nil then
			local table = { trait = "syncend" }	
			send_table({table}, 1, 1)
			return
		end
		
		
		while (not (type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false)) do
			l, v  = next(Counter, k)
			k = l
		end
		
		-- 2 is sync
		send_table(Counter, k, 2)
		
		State.baseState[k] = Counter:duplicate(Counter[k])
				
				
				
	end
		
		--for k, v in pairs(Globals) do
		--	if type(key) == "string" and type(value) ~= 'function' and key ~= '__index' then
		--		if State.baseState[key] ~= nil then
		--			Globals[key] = Counter:duplicate(State.baseState[key])
		--		end
		--	end
		--end
		
	

end

function nextsync()

	if net_connected() then
	
		local v

		repeat
		
			l, v  = next(Counter, k)
			
			if l == nil then
				local table = { trait = "syncend" }	
				send_table({table}, 1, 1)
				return
			end
			
			k = l
			
		until (type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false)	
		
		-- 2 is sync			
		send_table(Counter, k, 2)
		
		State.baseState[k] = Counter:duplicate(Counter[k])
	
	end

end

function done()

	if net_connected() then
	
		local table = { trait = "done" }	
		send_table({table}, 1, 1)

	end
	
end

function ask()

	if net_connected() then
	
		local table = { trait = "confirm" }
		send_table({table}, 1, 1)
		
	end
	
end		
		
function confirm()
	
	if net_connected() then
	
		local answer = ask_sync()
		if answer then
			State:resetState()
			toggle_painting()
			local table = { trait = "sync" }
			send_table({table}, 1, 1)
		end
	
	end

end


function updateid()
	if net_connected() then
		local table = { trait = "updateid" }
		send_table({table}, 1, 1)
	end
end


function updatetopzorder()
	if net_connected() then
		local table = { trait = "updatetopzorder" }
		send_table({table}, 1, 1)
	end
end


function updatebottomzorder()
	if net_connected() then	
		local table = { trait = "updatebottomzorder" }
		send_table({table}, 1, 1)	
	end
end


function findallsides()
	sides = Side:findAllSides()
	return sides
end





-- cards


function copyCard(fromId, toId, oldtoId, zorder, window, x, y)
	copyCounter(fromId, toId, zorder, window, x, y, x, y)
end


function createCard(window, deckId, id, face)

	local toId = next_id()
	local dx, dy = get_corner(id)
	
	
	local zorder = top_zorder()	
	
	local images, imageIndex = Image:getImages(id)
	
	
	local deck = Decks:get(deckId)
	
	-- 2 = face down 1 = face up
	Counter[toId] = Counter:new(toId, images, face, zorder, window, deck.cx - dx, deck.cy - dy, deck.cx, deck.cy)
	Counter[toId].Card.deckId = deck.id
	Counter[toId].Card.actionUp = deck.actionUp
	Counter[toId].Card.actionDown = deck.actionDown

	
	create_class_copy(Counter, toId)
	
	State.baseState[toId] = Counter:duplicate(Counter[toId])
	
end


function createDeck(id, actionUp, actionDown, gridpoint)

	local deck = Deck:new (id, actionUp, actionDown, gridpoint)
	State.baseState[id] = Counter:duplicate(deck)

end





function saveStage()
	State.savedPointer = State.stagePointer
end

function deleteAll()
	for id, v in pairs(Counter) do
		if type(id) == "string" and id ~= '__index' and type(v) ~= 'function' and v.deleted == false then
			delete_class(id)
		end
	end
	for id, v in pairs(Counter) do
		if type(id) == "string" and id ~= '__index' and type(v) ~= 'function' then
			Counter[id] = nil
		end
	end
end

function resetState()
	State:resetState()
end

function resetBase()
	State:resetBase()
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



function mapPlace(window, cx, cy)

	return Zones:getPlace(window, cx, cy)
	
end	


function reportmove(id, name, fromtag, totag, fromcx, fromcy, tocx, tocy)
	
	local text = ''
	
	
	if fromtag == 'Repository' then
		
		text = Repository[name].name .. ' placed on ' .. Zones:getPlace(totag, tocx, tocy)
		
	else
	
		local table = Counter[tostring(id)]
		local frontName = Repository[table.Image.images[1]]
		local flipName = Repository[table.Image.images[table.Image.imageIndex]]
		
		local display = ''
		
		if flipName == nil then
			display = frontName.name
		else
			display = flipName.name
		end
		
		local zonedisplay = ''
			
		if fromtag ~= totag then
			zonedisplay = ' in ' .. get_title(fromtag) 	
		end	
			
		text = display .. zonedisplay .. ' moves from <span style="color:DarkSlateGray;">' .. 
			   Zones:getPlace(fromtag, fromcx, fromcy) .. '</span> to <span style="color:DarkSlateGray;">' .. 
			   Zones:getPlace(totag, tocx, tocy) .. '</span>'
		
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
		if Counter:hasTrait(Counter[id], 'CantBeMoved') then
			return false
		end	
		if Counter:hasTrait(Counter[id], 'AreaOfEffect') then
			Counter[id].AreaOfEffect.area = {}
			Counter[id].AreaOfEffect.apply = false
			update_class(Counter, id)
		end
	end
	
	return true
	
end


function afterDrag(window, id, dx, dy, x, y)

	local table

	if Counter[id] ~= nil then	
		table = Counter[id]
	else
		table = Repository[id]
	end
	
	local nx
	local ny
	local cx
	local cy
	local snapped
	
	snapped, nx , ny , cx, cy = Zones:snapTo(window, id, dx, dy, x, y)
	
	if Counter:hasTrait(table, 'OnlyOneInStack') then
	
		if Counter:MarkerInStack(window, cx, cy, 'OnlyOneInStack', table.Image.images[1]) then
			return false
		end
	end
	
	return snapped, nx , ny , cx, cy
	
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
	
	return not Counter:hasTrait(Counter[id], 'CantBeMoved')
	
end


function rotatable(id)
	
	return Counter:hasTrait(Counter[id], 'Rotate')
	
end



-- cursor feed hook;  x,y are cursor mouse coordinates realative to widget

function feed(x, y)

	local id = _G['Rotated']
	
	if id == nil then
		return
	end		



	local w, h = get_size(Counter[id].Image.images[1])

	local degrees = math.deg( math.atan2( y - math.floor(h/2) , x - math.floor(w/2) ) )
	
	degrees = math.floor(degrees + 90)
	
	if degrees > 180 then
		degrees = degrees - 360
	elseif degrees < -180 then
		degrees = degrees + 360
	end
	
	
	Counter[id].Rotate.degrees = degrees
	rotate_counter(id, degrees)
	
end


function startFeed(id)
	_G['Rotated'] = id
	reporttext('move', 'Started ajusting facing')
end


function isFeeding()
	if _G['Rotated'] then
		return true
	end
	return false
end	


function stopFeed()
	_G['Rotated'] = nil
	reporttext('move', 'Stopped ajusting facing')
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


function pointoffset(window, cx, cy)
	
	local zone = Zones:findZone(window, cx, cy)

	if zone == nil then
		return defaultOffset()
	end
	
	local offset = zone:getOffset(cx, cy)
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


