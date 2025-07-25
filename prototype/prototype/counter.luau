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
	return o
end


function Repository:addTrait (id, key, trait)
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


function Counter:new (id, images, imageIndex, zorder, x, y)	
	local o = Counter:duplicate(Repository[images[1]])	
	setmetatable(o, Counter)
	Counter.__index = Counter
	o.Image.imageIndex = imageIndex
	o.id = id
	o.x = x
	o.y = y
	o.zorder = zorder
	o.deleted = false
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

function Counter:move (id, x, y)
	local dx, dy = get_size(id)
	updatePos(id, x - dx, y - dy)
	State:addMove(id, "Counter", "x", x - dx)
	State:addMove(id, "Counter", "y", y - dy)
	Counter:generateGUI()	
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

function Counter:merge(o, n)
    for k, v in pairs(n) do
        if type(v) == "table" and type(o[k] or false) == "table" then
			o[k] = Counter:merge(o[k], v)
		else
			o[k] = v;
		end
    end
    return o
end



function Counter:generateGUI()
	
	for k, v in pairs(Counter) do
		
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false then
			
			local name = Counter[k].Image.images[ Counter[k].Image.imageIndex ]
			local degrees = 0
			local opacity = 1.0			
			
			if Counter[k].Rotate ~= nil then
				degrees = Counter[k].Rotate.degrees
			end
			
			if Counter[k].Visibility ~= nil then
				opacity = Counter[k].Visibility.opacity
			end
			

			updateState(k,
						Counter[k].x, 
						Counter[k].y,
						Counter[k].zorder,  
						Counter[k].MarkMoved ~= nil and Counter[k].MarkMoved.moved,
						degrees,
						name,
						opacity,
						Counter[k],
						'Overlays')
						
					
			moved("Map", k, Counter[k].x, Counter[k].y)
			
		end
	end
	
	
	setGUI()

end





-- create Luau tokens


Token = {}

function Token:new (fromId, toId)
	local o = Counter:duplicate(Repository[fromId])
	o.name = fromId
	o.id = toId
	o.x = 0
	o.y = 0
	return o
end

function Token:copy (table, id, x, y)
	local o = Counter:duplicate(table[id])
	o.name = table[id].Image.images[1]
	o.id = id
	o.x = x
	o.y = y
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


function Token:setPos (id, x ,y)
	Token[id].gridx = x
	Token[id].gridy = y
end


function Token:updateGUI(id) 
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



Deck = {}

function Deck:new (id, move, gridpoint)	
	local o = {}
	setmetatable(o, Deck)
	Deck.__index = Deck
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

function Deck:draw()
	if #self.cards == 0 then
		return nil
	end	
	local n = math.random(1, #self.cards)
	local toId = self.cards[n]
	table.remove(self.cards, n)
	delete_class(toId)
	return toId
end




Card = {}

function Card:new(deck)
	local o = {}
	-- 1 face up, 2 face down (default)
	o.deck = deck
	o.face = 2
	return o 
end






return Counter
