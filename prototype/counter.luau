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

function Counter:move (window, id, x, y, cx, cy)
	

	local counter
	
	if Counter[id] ~= nil then
		counter = Counter[id]
	else
		counter = Repository[id]
	end
	
	
	
	if (counter[id].window == window) then
		
		updatePos(window, id, x, y, cx, cy)
		State:addMove(id, "Counter", "x", x)
		State:addMove(id, "Counter", "y", y)
		State:addMove(id, "Counter", "cx", cx)
		State:addMove(id, "Counter", "cy", cy)
		State:addMove(id, "Counter", "window", window)		
		update_class(Counter, id)
	
	elseif counter[id].window == 'Repository' then
	
		local toId = next_id()
		local zorder = top_zorder()
		copyCounter(id, toId, zorder, window, x, y, cx, cy)
		State:addCreate(toId, 'Create')
	
	else
	
		-- moved to another window
		
		-- delete counter in old window and create updated counter in new window
		
		Counter:delete(id)
		delete_class(id)
		
		
		updatePos(window, id, x, y, cx, cy)
		
		local zorder = top_zorder()	
		Counter[id].zorder = zorder
		Counter[id].deleted = false
		
		
		create_class_copy(Counter, id)			
		State:addCreate(id, 'Create')
		
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
				local dx, dy = get_size(v.Image.images[1])
				if v.x + dx == pos.x and v.y + dy == pos.y then
					res = res + 1
				end
			end
		end	
	end
	return res
end

function Counter:inStack(window, id, counter)
	local ex, ey = get_size(Counter[id].Image.images[1])	
	for k, v in pairs(Counter) do
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false then
			if Counter[id].window == Counter[k].window then
				if counter == v.Image.images[1] then		
					local dx, dy = get_size(v.Image.images[1])
					if v.x + dx == Counter[id].x + ex and v.y + dy == Counter[id].y + ey then
						return true
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

--[[
function Counter:refresh()
	
	local id
	local x
	local y
	local name
	local imageIndex
	local window
	local zorder
	local markmoved
	local degrees
	local opacity 
	local table
	local deleted
	
	
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
		 
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' then
		
			-- save state
		
			id = Counter[k].id
			x = Counter[k].x
			y = Counter[k].y
			name = Counter[k].Image.images[1]
			imageIndex = Counter[k].Image.imageIndex
			window = Counter[k].window
			zorder = Counter[k].zorder
			markmoved = Counter[k].MarkMoved ~= nil and Counter[k].MarkMoved.moved
			if Counter[k].Rotate ~= nil then
				degrees = Counter[k].Rotate
			else
				degrees = 0	 
			end
			if Counter[k].Visibility ~= nil then
				opacity = Counter[k].Visibility.opacity
			else
				opacity = 1.0	
			end
			deleted = Counter[k].deleted
		
	
	
			-- reset counter
			
			Counter[k] = nil			
			Counter[k] = Counter:duplicate(Repository[name])
			
			
			-- reinstall state

			Counter[k].id = id
			Counter[k].x = x 
			Counter[k].y = y 
			Counter[k].Image.imageIndex = imageIndex			
			Counter[k].window = window
			Counter[k].zorder = zorder
			
			if Counter[k].MarkMoved ~= nil then
				Counter[k].MarkMoved.moved = markmoved
			end	
			if Counter[k].Rotate ~= nil then		
				Counter[k].Rotate.degrees = degrees
			end
			if Counter[k].Visibility ~= nil then
				Counter[k].Visibility.opacity = opacity
			end	
			Counter[k].deleted = deleted 
		
			
		end
		
		
	end
	
	
end
--]]


function Counter:generateGUI()
	
	for k, v in pairs(Counter) do
		
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false then
		
			update_class(Counter, k)			
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

function Decks:get(id)
	return Decks[id]
end

function Decks:find(x, y)
	for k, v in pairs(Decks) do
		if type(v) ~= 'function' then	
			if v.x == x and v.y == y then
				return v
			end
		end
	end
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

function Deck:new (id, move, gridpoint)	
	local o = {}
	setmetatable(o, self)
	self.__index = self
	o.id = id
	o.cards = {}
	o.x = gridpoint.x
	o.y = gridpoint.y	
	o.moveto = move
	Decks:add(id, o)
	return o
end	


function Deck:add(card)
	table.insert(self.cards, card)
end

function Deck:remove(id)
	local n
	for k,v in pairs(self.cards) do
		if v == tonumber(id) then 	
			n = k
		end
	end
	table.remove(self.cards, n)	
end

function Deck:draw(andDelete)
	if #self.cards == 0 then
		return nil
	end	
	
	local n = math.random(1, #self.cards)
	local toId = self.cards[n]
	table.remove(self.cards, n)
	if andDelete then
		delete_class(toId)
	end 
	return toId
end

function Deck:load(id, table)
	local o = Deck:new(id, {}, {0,0})
	o = Counter:merge(o, table)
	return o
end




Card = {}

function Card:new(deck)
	local o = {}
	-- 1 face up, 2 face down (default)
	o.deck = deck
	o.face = 2
	return o 
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
