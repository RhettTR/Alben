-- a counter is a table of traits
-- the order of traits is as they appear in the table with the lowest trait first


Repository = {}




function Repository:new (images)
	local o = {}	
	setmetatable(o, self)
	Repository.__index = self
	o.Image = Image:new(images, 1)
	o.x = 0
	o.y = 0
	o.scale = 1.0
	o.name = images[1]
	o.window = 'Repository'
	return o
end


function Repository:addTrait (id, key, trait)
	if Repository[id][key] == nil then
		Repository[id][key] = {}
	end
	Repository[id][key] = trait
end


function Repository:addTraits (id, traits)
	for k, v in pairs(traits) do
		Repository:addTrait(id, k, v)
    end   
end


function Repository:flip (id)
	local name = Image:Repository_nextImage(id)
	return name
end


function Repository:rotate (id, increment)
	if Repository[id].Rotate ~= nil then
		Repository[id].Rotate.degrees = Repository:incrementDegrees(id, increment)
	end
end


function Repository:incrementDegrees (id, increment)
	local newDegrees = Repository[id].Rotate.degrees + increment;	
	newDegrees = math.fmod(newDegrees, 360)	
	if newDegrees > 180 then
		newDegrees = -(360 - newDegrees)
	end	
	if newDegrees < -180 then
		newDegrees = 360 + newDegrees
	end	
	return newDegrees;	
end


function Repository:hasTrait(table, trait)
	for k, v in pairs(table) do	
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' then			
			if k == trait then
				return true
			end
			if type(v) == 'table' and (v.deleted == nil or v.deleted == false) then
				local result = Repository:hasTrait(v, trait)
				if result == true then
					return true
				end
			end
		end	
	end
	return false
end



Counter = {}


function Counter:new (id, images, imageIndex, zorder, window, x, y, cx, cy)	
	local o = Counter:duplicate(Repository[images[1]])	
	setmetatable(o, Counter)
	Counter.__index = Counter
	o.Image.imageIndex = imageIndex
	o.id = id
	o.x = x
	o.y = y
	o.cx = cx
	o.cy = cy
	o.zorder = zorder
	o.deleted = false
	o.window = window	
	return o
end

function Counter:load (table)
	local o = Counter:duplicate(Repository[table.Image.images[1]])
	o = Counter:merge(o, table)	
	setmetatable(o, Counter)
	Counter.__index = Counter
	return o
end

function Counter:clone (id, zorder)	
	local o = Counter:duplicate(Counter[id])	
	setmetatable(o, Counter)
	Counter.__index = Counter
	o.zorder = zorder
	return o
end

function Counter:delete (id)
	State:addMove (id, "Delete", "", 0)	
end


function Counter:flip (id)
	local name = Image:Counter_nextImage(id)
	State:addMove (id, "Image", "imageIndex", Counter[id].Image.imageIndex)
	return name
end


function Counter:toggleMoved (id)
	if Counter[id].MarkMoved ~= nil then
		Counter[id].MarkMoved.moved = not Counter[id].MarkMoved.moved
		State:addMove(id, "MarkMoved", "moved", Counter[id].MarkMoved.moved)
	end	
end


function Counter:incrementDegrees (id, increment)
	local newDegrees = Counter[id].Rotate.degrees + increment;	
	newDegrees = math.fmod(newDegrees, 360)	
	if newDegrees > 180 then
		newDegrees = -(360 - newDegrees)
	end	
	if newDegrees < -180 then
		newDegrees = 360 + newDegrees
	end	
	return newDegrees;	
end


function Counter:rotate (id, increment)	
	Counter[id].Rotate.degrees = Counter:incrementDegrees(id, increment);
	State:addMove(id, "Rotate", "degrees", Counter[id].Rotate.degrees)	
end


function Counter:markMoved (id, moved)
	if Counter[id].MarkMoved ~= nil then	
		Counter[id].MarkMoved.moved = moved;
		State:addMove(id, "MarkMoved", "moved", moved)
		set_moved_status(id, moved)
	end
end

function Counter:select (id)
	if Counter[id].Select ~= nil then	
		toggle_select_status(id)
	end
end


function Counter:move (window, id, zorder, x, y, cx, cy)

	local counter

	if Counter[id] ~= nil then	
		counter = Counter[id]
	else
		counter = Repository[id]
	end
	

	
	
	if (counter.window == window) then
		
		
		updatePos(window, id, zorder, x, y, cx, cy)
		
		State:addMove(id, "Counter", "x", x)
		State:addMove(id, "Counter", "y", y)
		State:addMove(id, "Counter", "cx", cx)
		State:addMove(id, "Counter", "cy", cy)
		State:addMove(id, "Counter", "zorder", zorder)
		State:addMove(id, "Counter", "window", window)
	
	
	else
	
		
		if Counter:hasTrait(counter, 'Marker') then		
			if counter.Marker.place == 'bottom' then 
				zorder = bottom_zorder()
			else
				zorder = top_zorder()
			end
		else
			zorder = top_zorder()		
		end	
	
	
		if (counter.window == 'Repository') then

			local toId = next_id()
			copyCounter(id, toId, zorder, window, x, y, cx, cy)
			State:addCreate(toId, 'Create')
			
			-- has to be here for zorder			
			setGUI(window)
			
		else
		
			-- moved to another window
			
			-- delete counter in old window and create updated counter in new window
		
			Counter:delete(id)
			delete_class(id)
			
			--local zorder = top_zorder()
			
			updatePos(window, id, zorder, x, y, cx, cy)
		
			Counter[id].deleted = false		
			
			create_class_copy(Counter, id)
			
					
			State:addCreate(id, 'Create')
			
		end
	end
	
	
	if Counter:hasTrait(counter[id], 'Card') then
		local deck = Decks:find(cx, cy)
		if deck ~= nil then
			counter[id].Card.deckId = deck.id
			counter[id].Card.actionUp = deck.actionUp
			counter[id].Card.actionDown = deck.actionDown
			State:addMove (id, 'Card', 'deckId', deck.id)
			State:addMove (id, 'Card', 'actionUp', deck.actionUp)	
			State:addMove (id, 'Card', 'actionDown', deck.actionDown)
		end
	end

		
end


function Counter:findId(stringId)
	for k, v in pairs(Counter) do	
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false then
			local name = Counter[k].Image.images[1]
			if name == stringId then
				return k
			end
		end
	end
	return nil
end

function Counter:findTrait(table, trait)
	for k, v in pairs(table) do	
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' then			
			if k == trait then
				return v
			end
			if type(v) == 'table' and (v.deleted == nil or v.deleted == false) then
				local table = Counter:findTrait(v, trait)
				if table ~= nil then
					return table
				end
			end
		end	
	end
	return nil
end

function Counter:hasTrait(table, trait)
	for k, v in pairs(table) do	
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' then			
			if k == trait then
				return true
			end
			if type(v) == 'table' and (v.deleted == nil or v.deleted == false) then
				local result = Counter:hasTrait(v, trait)
				if result == true then
					return true
				end
			end
		end	
	end
	return false
end


function Counter:setTrait(table, name, trait)
	table[name] = trait
end

function Counter:removeTrait(table, trait)
	table[trait] = nil	
end

function Counter:getStackSize(window, pos)
	local res = 0
	for k, v in pairs(Counter) do
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false then
			if Counter[k].window == window then		
				--local dx, dy = get_size(v.Image.images[1])
				if v.cx == pos.x and v.cy + dy == pos.y then
					res = res + 1
				end
			end
		end	
	end
	return res
end

function Counter:MarkerInStack(window, cx, cy, trait, resourceid)	
	for k, v in pairs(Counter) do	
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false then

			if Counter[k].window == window then
				if cx == v.cx and cy == v.cy then		
					if Counter:hasTrait(Counter[k], 'Marker') then
						local table = Counter:findTrait(Counter[k], 'Marker')
						if Counter:hasTrait(table, trait) and table.marker == resourceid then
							return true
						end
					end
				end
			end
			
		end	
	end
	return false
end

function Counter:duplicate(o)
	local res = {}
	for k, v in pairs(o) do
		if type(v) == "table" then
			res[k] = Counter:duplicate(v)
		else
			res[k] = v;
		end
	end
	return res
end

function Counter:merge(o, loaded)
    for k, v in pairs(loaded) do
		if type(v) == "table" and type(o[k] or false) == "table" then	
			o[k] = Counter:merge(o[k], v)
		else
			o[k] = v
		end
    end
    return o
end


function Counter:refresh()
	
	local id
	local x
	local y
	local cx
	local cy
	local name
	local imageIndex
	local deleted
	local window
	local zorder
	
	
	
	local keys = {}
	local n = 0

	for k,v in pairs(Counter) do
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' then
			n = n + 1
			keys[n] = k
		end
	end

	
	

	
	for i = 1 , #keys do
	
		local k = keys[i]
		 
		
		
		-- save state
	
		id = Counter[k].id
		x = Counter[k].x
		y = Counter[k].y
		cx = Counter[k].cx
		cy = Counter[k].cy
		name = Counter[k].Image.images[1]
		imageIndex = Counter[k].Image.imageIndex
		deleted = Counter[k].deleted
		window = Counter[k].window
		zorder = Counter[k].zorder
		
	


		-- reset counter
		
		Counter[k] = nil
		Counter[k] = Counter:duplicate(Repository[name])
		
		
		-- reinstall state

		Counter[k].id = id
		Counter[k].x = x 
		Counter[k].y = y
		Counter[k].cx = cx 
		Counter[k].cy = cy  
		Counter[k].Image.imageIndex = imageIndex
		Counter[k].deleted = deleted			
		Counter[k].window = window
		Counter[k].zorder = zorder
		
			
			
		
			
		
		
		
	end
	
	
end



function Counter:generateGUI()
	
	for k, v in pairs(Counter) do
		
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false then
		
			update_class(Counter, k)
			-- hook			
			moved(Counter[k].window, k, Counter[k].x, Counter[k].y)
					
		end
	end
	
	setGUI('all')
	
	
	for k, v in pairs(Globals) do
	
		if k == 'Turn' then
			printturn()
		end
	
	end

end


	
	
------	
	

	
	

Decks = {}

function Decks:add(id, o)
	Decks[id] = o
end

function Decks:is(id)
	return Decks[id] ~= nil
end

function Decks:get(id)
	if Decks[id] ~= nil then
		return Decks[id]
	else
		error(13, id)
		return nil
	end
end

function Decks:find(x, y)
	for k, v in pairs(Decks) do
		if type(v) ~= 'function' then	
			if v.cx == x and v.cy == y then
				return v
			end
		end
	end
	--error(21, x, y)
	return nil
end

function Decks:merge(merged, from)
	local mergedeck = Decks:get(merged)
	local fromdeck = Decks:get(from)
	for k, v in ipairs(fromdeck.cards) do
		table.insert(mergedeck.cards, v)
	end	
	for k, v in ipairs(fromdeck.cards) do
		fromdeck.cards[k] = nil
	end
end

function Decks:getKeys()
	local result = {}
	for k, v in pairs(self) do
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' then
			table.insert(result, k)
		end
	end
	return result
end



Deck = {}

function Deck:new (id, actionUp, actionDown, gridpoint)	
	local o = {}
	setmetatable(o, self)
	self.__index = self
	o.id = id
	o.cx = gridpoint.x
	o.cy = gridpoint.y
	o.actionUp = actionUp
	o.actionDown = actionDown
	Decks:add(id, o)
	return o
end	


function Deck:load(id, table)
	local o = Deck:new(id, {}, {}, {x = 0,y = 0})
	o = Counter:merge(o, table)
	return o
end


function Deck:draw(window, zone, pos)

	
	-- the available cards to draw from
	
	local cards = {}

	for k,v in pairs(Counter) do
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false then
		
			local trait = Counter[v.id].Card
			
			if trait ~= nil then
				-- 2 face down
				if trait.deckId == self.id and Counter[v.id].Image.imageIndex == 2 then
					table.insert(cards, v.id)
				end
			end
			
		end
	end



	if #cards == 0 then
		return 
	end
	
	setNoAction(false)
	
	
	local n = math.random(1, #cards)
	local card = cards[n]

		
	
	local z = Zones:get(zone)
	local p = z:getGrid(pos)
		
	
			
	local dx, dy = get_corner(card)
	local zorder = top_zorder()
	
	
			
	Counter[card].Image.imageIndex = 1
	
	State:addMove (card, 'Image', 'imageIndex', Counter[card].Image.imageIndex)
	
	
	Counter:move(window, card, zorder, p.x - dx, p.y - dy, p.x, p.y)

	
		
	
	reporttext('move', Repository[Counter[card].Image.images[2]].name .. ' is drawn in the <span style="color:DarkSlateGray;">'
				   .. get_title(window) .. '</span> window')
				   
		
	
		
	Counter:generateGUI()	
	
	
end		






function refresh()

	Counter:refresh()
	
end


-----------


Globals = {}

function Globals:add(id, o)
	Globals[id] = o
end







return Counter
