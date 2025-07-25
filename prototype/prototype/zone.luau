-- zones and snap-to functionality

Zone = {}


function Zone:new(tag)
	local o = {}
	setmetatable(o, self)
	self.__index = self
	o.tag = tag
	o.border = {}
	o.grid = {}
	o.gridType = 'custom'
	o.sx = 0
	o.sy = 0
	o.dx = 0
	o.dy = 0
	o.members = {}
	o.allowUndefined = true
	Zones:add(tag, o)
	return o
end


function Zone:isEmpty()
	return #self.grid == 0
end

function Zone:setAllowed(value)
	self.allowUndefined = value
end

function Zone:setBorder(square)
	self.border = square
end

function Zone:isWithinBorder(x, y)
	return x >= self.border.x0 and x <= self.border.x1 and 
		   y >= self.border.y0 and y <= self.border.y1	   
end

function Zone:addGrid(x, y, p)
	table.insert(self.grid, {p = p == nil and '' or p, x = x, y = y})
end

function Zone:getGrid(p)
	for k, v in pairs(self.grid) do	
		if v.p == p then
			return v
		end	
	end
end

function Zone:addMember(id)
	table.insert(self.members, id)
end

function Zone:hasMember(id)
	for k, v in pairs(self.members) do	
		if v == id then
			return true
		end	
	end
	return false
end

function Zone:squareGrid(startX, startY, dx, dy)
	self.gridType = 'square'
	self.sx = startX
	self.sy = startY
	self.dx = dx
	self.dy = dy
end




Zones = {}

-- does grid have x, y coordinates
Zones.coordinatesUsed = false

Zones.tags = {}


function Zones:add(tag, o)
	Zones.tags[tag] = o
end

function Zones:get(tag)
	return Zones.tags[tag]
end



function Zones:snaptoDefault(id, x, y)

	local counter
 		
 	if Counter[id] ~= nil then
		counter = Counter[id]
	else
		counter = Repository[id]
	end
	
	
	local maxDistance = 8;
		
	for k, v in pairs(Counter) do
	
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false then
	
			if k ~= id then
				
				if math.abs(v["x"] - x) < maxDistance and math.abs(v["y"] - y) < maxDistance then
				   	
				   	-- a DoesNotStack counter never stacks			
					if counter["DoesNotStack"] ~= nil then
						return false, x , y
					end
					if v["DoesNotStack"] ~= nil then
						return false, x , y
					end
					
					-- a card only stacks with other cards
					if counter["Card"] ~= nil and v["Card"] == nil then
						return false, x , y
					end
					if counter["Card"] == nil and v["Card"] ~= nil then
						return false, x , y
					end
					
					x = v["x"]
					y = v["y"]					
					return true, x , y
				   
				end 
				  	
			end	 
		end	
	end
	
	return true, x , y 
	
end


function Zones:findZone(x, y)

	for k, v in pairs(Zones.tags) do					
		if v:isWithinBorder(x, y) then
			return v
		end
	end	
	
	return nil

end


function Zones:doSnap(zone, dx, dy, x, y)
	
	-- math.maxinteger not supported in 5.1
	
	local minimum_distance = 100000

	local minimumX, minimumY
		
	for k, v in pairs(zone.grid) do	
	
		-- simplification of sqr(dx² + dy²)
		
		local sum = math.abs(x + dx - v["x"]) + math.abs(y + dy - v["y"])
		
		if sum < minimum_distance then
			minimum_distance = sum
			minimumX = v["x"]
			minimumY = v["y"]
		end
	
	end
	
	
	if minimum_distance < 100000 then
		x = minimumX - dx
		y = minimumY - dy			
	end
	
	
	return true, x , y
	
end


function Zones:doSnapSquare(zone, dx, dy, x, y)
	
	local hx = math.floor(zone.dx/2)
	local hy = math.floor(zone.dy/2)
	
	local ax = zone.dx * math.floor((x + dx - zone.sx) / zone.dx)
	local ay = zone.dy * math.floor((y + dy - zone.sy) / zone.dy)
	
	local nx = -(dx - zone.sx) + hx + ax
	local ny = -(dy - zone.sy) + hy + ay
	
	if zone:isWithinBorder(nx, ny) then
		return true, nx, ny
	else
		return true, x , y  
	end
	
end


function Zones:snapTo(id, dx, dy, x, y)

	-- dx dy locates the center from the top-left counter corner
	
	local zone = Zones:findZone(x + dx, y + dy)

	if zone == nil then
		return Zones:snaptoDefault(id, x, y)
	end
	
	local counter
 		
 	if Counter[id] ~= nil then
		counter = Counter[id]
	else
		counter = Repository[id]
	end

	
	if zone:isWithinBorder(x + dx, y + dy) then
		
		if zone:hasMember(counter.Image.images[1]) then
			if zone.gridType == 'square' then
				return Zones:doSnapSquare(zone, dx, dy, x, y)
			else
				return Zones:doSnap(zone, dx, dy, x, y)
			end
		elseif zone.allowUndefined then			
			return Zones:snaptoDefault(id, x, y)
		else
			return false, x , y
		end
	end
	
	return Zones:snaptoDefault(id, x, y)
	
end





function Zones:getPlace(x, y)

	local squareLength = 10

	local zone = Zones:findZone(x, y)
	
	if zone == nil then
		return ''
	end
	
	if zone.gridType == 'square' then
		local ax = math.floor((x - zone.sx) / zone.dx)
		local ay = math.floor((y - zone.sy) / zone.dy)
		local n = ax + squareLength * ay		
		return tostring(n)
	else
		for k, v in pairs(zone.grid) do
			if v.x == x and v.y == y then
				return v.p
			end
		end
	end
	
	return ''	

end


function Zones:getCoordinates(tag, place)

	local zone = Zones.tags[tag]
	
	if zone == nil then
		return -1
	end
	
	if zone.gridType == 'square' then
	
		local squareLength = 10
		
		if place > 40 then
			place = 40
		end
		
		local x = place % squareLength
		local y = math.floor(place / squareLength)
		
		if place == 40 then
			x = 10
			y = 3
		end
		
		x = zone.sx + (x * zone.dx) + math.floor(zone.dx / 2)
		y = zone.sy + (y * zone.dy) + math.floor(zone.dy / 2)
		
		
		return x , y 
			   
		
	end
	
	return -1
	
end



local function char(num)
	return string.char(string.byte("A") + num - 1)
end


function Zones:printCoordinates(x, y)

	x = math.floor((x + 20) / 113)
	y = math.floor((y + 20) / 113)
	
	return tostring(char(y + 1)) .. tostring(x + 1)
	
end







return Zone
