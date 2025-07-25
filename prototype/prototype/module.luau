--- set side

PlayerSide = "German"


-- set alpha transparency for counters

setCounterDragAlpha(0.5)


-- set map 

setBackgroundMapID("map_classic_vassal")

hoover_ShowMap(false)
hoover_ShowPlace(true)


-- set seed for randowm numbers

math.randomseed(os.time())


--- definition of actions must happen before defintion of Traits


local function actionDelete (window, id)
	if window == 'Map' then
		Counter:delete(id)
		delete_class(id)
	end
end

local function actionFlip (window, id)
	local name
	if window == 'Map' then
		name = Counter:flip(id)
		set_counter_image(id, name)
	elseif window == 'Repository' then
		name = Repository:flip(id)
		set_counter_image(id, name)
	else
		local name = Token:flip(id)
		set_token_image(window, id, name)
	end
	
end

local function actionRotate (window, id, sign)
	if window == 'Map' then
		Counter:rotate(id, sign*Rotate.increment)
		rotate_counter(id, Counter[id].Rotate.degrees)
	else
		Repository:rotate(id, sign*Rotate.increment)
		rotate_counter(id, Repository[id].Rotate.degrees)
	end
end

local function actionMoved (window, id)
	if window == 'Map' then
		if Counter[id].MarkMoved ~= nil then
			Counter:toggleMoved(id)
			set_moved_status(id, Counter[id].MarkMoved.moved)
		end
	end
end

local function actionClone (window, id)
	if window == 'Map' then
		if Counter[id].Clone ~= nil then
			toId = next_id()
			zorder = top_zorder()
			cloneCounter(id, toId, zorder)
		end
	end
end

local function actionSelect (window, id)
	if window == 'Map' then
		if Counter[id].Select ~= nil then
			Counter:select(id)
		end
	end
end

local function actionMove (window, id)
	if window == 'Map' then
	
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
			
			Counter:move(id, move.x, move.y)
		end
		
	elseif Token[id] ~= nil then
	
		-- move card from hand to map window
			
		local deck = Token[id].Card.deck
				
		local toId = next_id()
		local dx, dy = get_size(Token[id].Image.images[1])
		local zorder = top_zorder()		
		copyCard(Token[id].name, toId, id, zorder, deck.moveto.Move.x - dx, deck.moveto.Move.y - dy)
		local newdeck = Decks:find(deck.moveto.Move.x, deck.moveto.Move.y)
		if newdeck ~= nil then
			newdeck:add(toId)
			Counter[toId].Card.deck = newdeck
		end
		delete_token(window, id)
				
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
						
						Counter:move(k, point.x, point.y)
						
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
			if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' then		
				if (v.x == x and v.y == y and v.Marker ~= nil and v.deleted == false) then
					Counter:delete(k)
					delete_class(k)
				end
			end
		end		
	end	
end




function action(window, id, action)

	--print('doing '..action..' on '..id)
	
	if window == 'Map' then 
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
	elseif window == 'Map' then	
		return Counter[id] == nil and {} or Counter[id]
	elseif window == 'State' then
		return State.stageStack[id] == nil and {} or State.stageStack[id]
	elseif window == 'Global' then
		return Global[id] == nil and {} or Global[id]
	else
		return Token[id] == nil and {} or Token[id]			
	end	

end


function getState(id)
	
	return Counter[id].x, Counter[id].y
	
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
		local traits = getTraits("Map", id)
		triggerEvent(id, traits, event)		
	end
	
	if event == 'move' then
		State:addMove(id, trait, key, value)
	end

	if event == 'end' then
		State:addStage()
	end
	
	
end



function create(fromId, trait)
	State:addCreate (fromId, trait)
end






local Counter = counter() 
local Traits = traits()
local State = state()
local Zone = zone()
local Repository = repository()












-- set rotate degrees for counters

Rotate.increment = 45






-- create Luau and C++ counter

function copyCounter(fromId, toId, zorder, x, y)
	local images, imageIndex = Image:getImages(fromId)
	Counter[toId] = Counter:new(toId, images, imageIndex, zorder, x, y)
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







-- cards


function copyCard(fromId, toId, oldtoId, zorder, x, y)
	copyCounter(fromId, toId, zorder, x, y)
	Token[oldtoId] = nil
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


function createToken(id, window, x, y)
	local toId = next_id()
	local dx, dy = get_size(Counter[id].Image.images[1])	
	Token[toId] = Token:copy (Counter, id, x - dx, y - dy)
	create_token(Token, toId, window)
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




function mapPlace(dx, dy, x, y)	
	if Zones.coordinatesUsed then
		return Zones:printCoordinates(x, y)
	end
	return Zones:getPlace(x + dx, y + dy)
end	

function updatePos(id, x, y)
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
	end
	
	return true
	
end


function afterDrag(id, dx, dy, x, y)

	return Zones:snapTo(id, dx, dy, x, y)
	
end



function dropped(name, fromid, toid, x, y)
		
	
end


function moved(name, toid, x, y)
	
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
		
	if name == 'Delete all markers here' then
		if Counter[id] ~= nil then
			local x = Counter[id].x
			local y = Counter[id].y
			for k, v in pairs(Counter) do
				if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false then
				--print(v.Marker, v.x, x , v.y, y, k, id)	
					if v.x == x and v.y == y and v.Marker ~= nil then
						return true
					end
				end
			end
		end
	end
	


	if name == 'Place AP Trench' then	
		if Counter[id] ~= nil and Repository[marker] ~= nil then
			if Counter[id].Side ~= nil and Repository[marker].Marker ~= nil then
				if Counter[id].Side == Repository[marker].Marker.Side then
					-- make sure no double marker
					local x = Counter[id].x
					local y = Counter[id].y 
					for k, v in pairs(Counter) do
						if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false then		
							if (k ~= id and v.x == x and v.y == y and v.Marker ~= nil ) then
								if v.Marker.marker == marker then		
									return false
								end
							end
						end
					end
							
					return true
								
				end
			end
		end
	end
	

	return false

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


