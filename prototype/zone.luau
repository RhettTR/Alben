-- zones and snap-to functionality

Zone = {}


function Zone:new(window, tag)
	local o = {}
	setmetatable(o, self)
	self.__index = self
	o.tag = tag
	o.border = {}
	o.grid = {}
	o.gridType = 'custom'
	o.a = 0
	o.b = 0
	o.dx = 0
	o.dy = 0
	o.members = {}
	o.allowUndefined = true
	o.window = window
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

function Zone:addGrid(x, y, p, hx, hy, offset)
	table.insert(self.grid, {p = p == nil and '' or p, 
							 hx = hx == nil and 0 or hx,
							 hy = hy == nil and 0 or hy,
							 offset = offset == nil and {} or offset,
							 x = x, y = y})
end

function Zone:getGrid(p)
	for k, v in pairs(self.grid) do	
		if v.p == p then
			return v
		end	
	end
end

function Zone:isGrid(x, y)
	for k, v in pairs(self.grid) do
		-- a bit dirty, takes into consideration rounding errors
		if x <= v.x + 2 and x >= v.x - 2 and y <= v.y + 2 and y >= v.y - 2 then
		   return true
		end   		
	end
	return false
end

function Zone:getOffset(x, y)
	for k, v in pairs(self.grid) do		
		if v.offset ~= nil then
			if v.x == x and v.y == y then
				return v.offset
			end
		end	
	end
	return nil
end

function Zone:addMember(id)
	table.insert(self.members, id)
end

function Zone:addMembers(pattern)
	for k, v in pairs(Repository) do
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' then	
			local key = string.match(k, pattern)		
			if key ~= nil then		
				table.insert(self.members, k)
			end
		end
	end
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
	self.a = startX
	self.b = startY
	self.dx = dx
	self.dy = dy
end



function Zone:makeHexagonalGrid(width, height)

	local x 
	local y

	
	-- 60 degrees is 1.0471975512 radians
	local c = self.b / ( 2*math.tan(1.0471975512) )	
	
	local i = 0	
	
	
	while (true) do
		 
		x = leftPadding + self.border.x0 + self.a + i*(self.a + c)
				
		if x >= width then
			return
		end	
		
	
		local j = 0
		local flag = true
		
		while (flag) do	
				
			local fy
			
			if math.fmod(i,2) == 0 then
				fy = -math.round(self.b / 2)
			else
				fy = 0	
			end 
	
			y = topPadding + self.border.y0 + self.b + j*self.b + fy
			
					
			if y >= height then
				flag = false
			else
				local cx = ''
				local cy = ''
				if i + 1 < 10 then
					cx = '0'
				end
				if j + 1 < 10 then
					cy = '0'
				end
				self:addGrid(math.round(x), 
							 math.round(y), 
							 cx .. i + 1 .. cy .. j + 1, 
							 math.round(x), 
							 math.round(y - (0.45*self.b)))
				j = j + 1
			end	
					
		end
		
		i = i + 1
		
	end
	
end

function Zone:hexGrid(a, b)
	self.gridType = 'hex'
	self.a = a
	self.b = b	
	self:makeHexagonalGrid(self.border.x1-self.border.x0, self.border.y1-self.border.y0)
end




local vi = {{x = 0.5, y = 0.5}, {x = 1, y = 0}, {x = 0.5, y = -0.5}, {x = -0.5, y = -0.5}, {x = -1, y = 0}, {x = -0.5, y = 0.5}}
local hi = {{x = 0, y = 1} , {x = 1.5, y = 0.5}, {x = 1.5, y = -0.5}, {x = 0, y = -1}, {x = -1.5, y = -0.5}, {x = -1.5, y = 0.5}}

local vertices = {}
local hexes = {}

local function inHexes(x, y)
	for k, v in ipairs(hexes) do
		if v.x == x and v.y == y then
			return true
		end
	end
	return false
end

local function verticePrune()

	for i = 1, #vertices - 1 do
		for j = i + 1, #vertices do			
			if vertices[i].p1.x == vertices[j].p2.x and vertices[i].p1.y == vertices[j].p2.y and 
			   vertices[i].p2.x == vertices[j].p1.x and vertices[i].p2.y == vertices[j].p1.y then
				vertices[i].deleted = true
				vertices[j].deleted = true	
			end
		end		
	end
	
end

local undeletedpoints = {}

function Zone:undeletedAreaOfEffect()

	undeletedpoints = {}
	
	for _,v in ipairs(vertices) do
		if not v.deleted then
			table.insert(undeletedpoints, {p1 = v.p1, p2 = v.p2})
		end
	end

end


local points = {}

function Zone:seqAreaOfEffect(level, total, p1, p2)
	
	if level < total then
		
		-- +1 correction for rounding errors ?
		local x = math.round(p1.x * self.a) + 1
		local y = math.round(p1.y * self.b) + 1
		table.insert(points, {x = x, y = y})
		
	
		for _,v in ipairs(undeletedpoints) do
			if v.p1.x == p2.x and v.p1.y == p2.y then
				self:seqAreaOfEffect(level + 1, total, p2, v.p2)
				return
			end
		end
	end
	
end


function Zone:hexAreaOfEffect(x, y, level, radius, sx, sy)

	if level > radius then
		return
	end
	
	
	local ax = sx + math.round(x * self.a)
	local ay = sy + math.round(y * self.b)
	
	if not self:isGrid(ax, ay) then
		return
	end	
	
	
	
	if not inHexes(x, y) then
	
		for i = 1, 6 do
		
			local j = i + 1
			
			if j == 7 then
				j = 1
			end
		
			local p1 = {x = x + vi[i].x, y = y + vi[i].y}
			local p2 = {x = x + vi[j].x, y = y + vi[j].y}
			local ver = {p1 = p1, p2 = p2}
			
			table.insert(vertices, ver)
			
			
		end
		
		table.insert(hexes, {x = x, y = y})	
		
	end
	
		
	for k, v in ipairs(hi) do	
		self:hexAreaOfEffect(x + v.x, y + v.y, level + 1, radius, sx, sy)
	end
	
	
end


function Zone:makeAreaOfEffect(x, y, radius)
	vertices = {}
	hexes = {}
	points = {}
	for i = 0, radius do
		self:hexAreaOfEffect(0, 0, 0, i, x, y)
	end
	verticePrune()
	if #vertices > 0 then
		self:undeletedAreaOfEffect()
		self:seqAreaOfEffect(0, #undeletedpoints, undeletedpoints[1].p1, undeletedpoints[1].p2)
	end
	return points
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



function Zones:snaptoDefault(window, id, x, y)

	local counter
 		
 	if Counter[id] ~= nil then
		counter = Counter[id]
	else
		counter = Repository[id]
	end
	
	
	local maxDistance = 8;
			
	for k, v in pairs(Counter) do
	
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' and v.deleted == false and v.window == window then
	
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


function Zones:findZone(window, x, y)

	for k, v in pairs(Zones.tags) do
					
		if v.window == window and v:isWithinBorder(x, y) then	
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
	
	local ax = zone.dx * math.floor((x + dx - zone.a) / zone.dx)
	local ay = zone.dy * math.floor((y + dy - zone.b) / zone.dy)
	
	local nx = -(dx - zone.a) + hx + ax
	local ny = -(dy - zone.b) + hy + ay
	
	if zone:isWithinBorder(nx, ny) then
		return true, nx, ny
	else
		return true, x , y  
	end
	
end


function Zones:doSnapHex(zone, dx, dy, x, y)
	
	return Zones:doSnap(zone, dx, dy, x, y)  
	
end


function Zones:snapTo(window, id, dx, dy, x, y)

	-- dx dy locates the center from the top-left counter corner
	
	local zone = Zones:findZone(window, x + dx, y + dy)

	if zone == nil then
		-- problem with retreats!!
		--return false, x , y
		return true, x , y
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
			elseif zone.gridType == 'hex' then
				return Zones:doSnapHex(zone, dx, dy, x, y)	
			else
				return Zones:doSnap(zone, dx, dy, x, y)
			end
		elseif zone.allowUndefined then			
			return Zones:snaptoDefault(window, id, x, y)
		else
			return false, x , y
		end
	end
	
	return Zones:snaptoDefault(window, id, x, y)
	
end





function Zones:getPlace(window, x, y)

	local squareLength = 10

	local zone = Zones:findZone(window, x, y)

	if zone == nil then
		return 'Offboard'
	end
	
	if zone.gridType == 'square' then
		local ax = math.floor((x - zone.a) / zone.dx)
		local ay = math.floor((y - zone.b) / zone.dy)
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





local function char(num)
	return string.char(string.byte("A") + num - 1)
end


function Zones:printCoordinates(x, y)

	x = math.floor((x + 20) / 113)
	y = math.floor((y + 20) / 113)
	
	return tostring(char(y + 1)) .. tostring(x + 1)
	
end







return Zone
