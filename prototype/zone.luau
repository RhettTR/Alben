-- zones and snap-to functionality

Zone = {}


function Zone:new(window, tag, name)
	local o = {}
	setmetatable(o, self)
	self.__index = self
	o.tag = tag
	o.name = (name == nil and 'Off-grid') or name
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
	o.coordinatesUsed = false
	Zones:add(tag, o)
	return o
end


function Zone:isEmpty()
	return #self.grid == 0
end

function Zone:setAllowed(value)
	self.allowUndefined = value
end

function Zone:addBorder(point)
	table.insert(self.border, point)
end

function Zone:isWithinBorder(x, y)

	
	-- https://stackoverflow.com/questions/217578/how-can-i-determine-whether-a-2d-point-is-within-a-polygon
	
	
	local j = #self.border
	local inside = false
	
	for i = 1 , #self.border do 
	
		local a = (self.border[i].y > y) ~= (self.border[j].y > y)
		
		local b = false;
		
		if a then
			b = x < (self.border[j].x - self.border[i].x) * (y - self.border[i].y) / (self.border[j].y - self.border[i].y) + self.border[i].x;
		end	
		
		if a and b then 
			inside = not inside
		end	
		
		j = i
	
	end
	
	return inside
	  	   
end

function Zone:addGrid(x, y, name, hx, hy, offset)
	if self:isWithinBorder(x, y) then
		table.insert(self.grid, {name = name == nil and '' or name, 
								 hx = hx == nil and 0 or hx,
								 hy = hy == nil and 0 or hy,
								 offset = offset == nil and {} or offset,
								 x = x, y = y})
		return true
	else
		return false	
	end							 
end

function Zone:getGrid(p)
	if type(p) == "string" then
		for k, v in pairs(self.grid) do
			if v.name ~= nil then
				if p == v.name then				
					return v
				end
			end
		end
	else
		for k, v in pairs(self.grid) do	
			if p == k then			
				return v
			end
		end
	end
	error(6, p)
	return nil
end

function Zone:getGridName(x, y)
	if self:isWithinBorder(x, y) then
		for k, v in pairs(self.grid) do
			if v.x == x and v.y == y then			
				return v.name
			end
		end
	end
	error(34, x, y)
	return ''							 
end

function Zone:setGridName(n, name)
	if self.grid[n] ~= nil then
		self.grid[n].name = name
		return
	end
	error(12, n, name)
end

function Zone:setGridOffset(n, offset)
	if self.grid[n] ~= nil then
		self.grid[n].offset = offset
		return
	end
	error(12, n, name)
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


function Zone:makeSquareGrid(minx, miny, width, height)	

	local x
	local y

	
	local j = 0
	local place = 0	
	
	
	while (true) do
		 
		y = math.round(self.dy/2) + j*self.dy
		
		if y >= miny + height then		
			return
		end	
		
	
		local i = 0
		local flag = true
		
		while (flag) do	
	
			x = math.round(self.dx/2) + i*self.dx
			
							
			if x >= minx + width then
				flag = false
			else									
				if self:addGrid(math.round(minx + x), 
								math.round(miny + y),
								place) then
								 
					 -- the default grid name is the numerical sequence
					place = place + 1
					
				end			 
				i = i + 1
			end	
					
		end
		
		j = j + 1
		
	end
end


function Zone:squareGrid(dx, dy)

	self.gridType = 'square'
	self.dx = dx
	self.dy = dy
	
	-- find width/height of zone
	local minx = self.border[1].x
    local maxx = self.border[1].x
    local miny = self.border[1].y
    local maxy = self.border[1].y
    
    for i = 1 , #self.border do         
        minx = math.min( self.border[i].x, minx )
        maxx = math.max( self.border[i].x, maxx )
        miny = math.min( self.border[i].y, miny )
        maxy = math.max( self.border[i].y, maxy )
    end
    
    
	self:makeSquareGrid(minx, miny, maxx - minx, maxy - miny)
	
	return maxx - minx, maxy - miny

end



function Zone:makeHexagonalGrid(width, height)

	local x 
	local y

	
	-- 60 degrees is 1.0471975512 radians
	local c = self.b / ( 2*math.tan(1.0471975512) )	
	
	local i = 0	
	
	
	while (true) do
		 
		x = self.a + i*(self.a + c)
				
		if x >= width then
			return
		end	
		
	
		local j = 0
		local flag = true
		
		while (flag) do	
				
			local fy
			
			if math.fmod(i,2) == 0 then
				fy = 0
			else
				fy = -math.round(self.b / 2)	
			end 
	
			y = math.round(self.b / 2) + j*self.b + fy
			
					
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
				--self:addGrid(self.border.x0 + math.round(x), 
				--			 self.border.y0 + math.round(y), 
				--			 cx .. i + 1 .. cy .. j + 1, 
				--			 math.round(x), 
				--			 math.round(y - (0.45*self.b)))
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
	--self:makeHexagonalGrid(self.border.x1-self.border.x0, self.border.y1-self.border.y0)
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


function Zone:assignPlace(x, y, width, height)

	

	return 0

end


function Zone:getCoordinate(cx, cy)

	if self.gridType == 'square' or self.gridType == 'hex' then
			
		for k, v in pairs(self.grid) do
			if v.x == cx and v.y == cy then
				return v.name
			end
		end
		
		return self.name
		
	elseif self.gridType == 'custom' then
	
		for k, v in pairs(self.grid) do
			if v.x == cx and v.y == cy then
				return v.name
			end
		end
			
		return self.name
		
	else
	
		return self.window	
		
	end

end



function Zone:setCoordinate(n)

	if self.gridType == 'square' then
	
		for k, v in pairs(self.grid) do
			if v.name == n then
				return v.x, v.y
			end
		end
	
	end
	
	return -1
	
end



local function char(num)
	return string.char(string.byte("A") + num - 1)
end


function Zone:printCoordinates(cx, cy)
	
	if self.gridType == 'square' then
	
		return 'test'
	
	elseif self.gridType == 'hex' then
	
		for k, v in pairs(self.grid) do
			if v.x == cx and v.y == cy then
				local letter1 = string.sub(v.name, 1, 2)	
				local number = string.sub(v.name, 3, 4)
				number = string.gsub(number, '^0+', '')
				if tonumber(letter1) > 26 then
					letter1 = tostring(tonumber(letter1) - 26)
					return char(letter1) .. char(letter1) .. number
				else
					return char(letter1) .. number
				end
			end
		end
	
	else
	
		return self.window
	
	end
	
	
	
end








Zones = {}

Zones.tags = {}


function Zones:add(tag, o)
	Zones.tags[tag] = o
end

function Zones:get(tag)
	if Zones.tags[tag] ~= nil then
		return Zones.tags[tag]	
	end
	error(5, tag)
	return nil
end

function Zones:is(tag)
	return Zones.tags[tag] ~= nil
end

function Zones:findZone(window, x, y)

	for k, v in pairs(Zones.tags) do
					
		if v.window == window and v:isWithinBorder(x, y) then	
			return v
		end
	end	
	
	return nil

end


function Zones:getPlace(window, cx, cy)

	local zone = Zones:findZone(window, cx, cy)

	if zone == nil then
		return 'Offboard'
	end

	if zone.coordinatesUsed then
		return zone:printCoordinates(cx, cy)
	else
		return zone:getCoordinate(cx, cy)
	end

end



function Zones:snaptoDefault(window, id, x, y, dx, dy)

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
						return false, x , y , x + dx , y + dy
					end
					if v["DoesNotStack"] ~= nil then
						return false, x , y , x + dx , y + dy
					end 
					
					-- a card only stacks with other cards
					if counter["Card"] ~= nil and v["Card"] == nil then
						return false, x , y , x + dx , y + dy
					end
					if counter["Card"] == nil and v["Card"] ~= nil then
						return false, x , y , x + dx , y + dy
					end
					
					
					return true, v["x"] , v["y"] , v["cx"] , v["cy"]
				   
				end 
				  	
			end	 
		end	
	end

	return true, x , y , x + dx , y + dy
	
end




function Zones:doSnap(zone, dx, dy, x, y)
	
	-- math.maxinteger not supported in 5.1

	local minimum_distance = 100000

	local minimumX, minimumY
	
	local cx = x + dx
	local cy = y + dy

	for k, v in pairs(zone.grid) do	
	
		-- simplification of sqr(dx² + dy²)
		
		local sum = math.abs(cx - v["x"]) + math.abs(cy - v["y"])
		
		if sum < minimum_distance then
			minimum_distance = sum
			minimumX = v["x"]
			minimumY = v["y"]
		end
	
	end

	
	if minimum_distance < 100000 then
		x = minimumX - dx
		y = minimumY - dy
		cx = minimumX
		cy = minimumY			
	end
	

	return true, x , y , cx, cy
	
end



function Zones:snapTo(window, id, dx, dy, x, y)

	-- dx dy locates the center from the top-left counter corner
	
	local zone = Zones:findZone(window, x + dx, y + dy)

	if zone == nil then
		-- problem with retreats!!
		--return false, x , y
		--return true, x , y , x + dx , y + dy
		return Zones:snaptoDefault(window, id, x, y, dx, dy)
	end
	
	local counter
	
 	if Counter[id] ~= nil then
		counter = Counter[id]
	else
		counter = Repository[id]
	end
	
	if zone:isWithinBorder(x + dx, y + dy) then
		
		if zone:hasMember(counter.Image.images[1]) then
			return Zones:doSnap(zone, dx, dy, x, y)
		elseif zone.allowUndefined then			
			return Zones:snaptoDefault(window, id, x, y, dx, dy)
		else
			return false, x , y , x + dx, y + dy
		end
	end
	
	return Zones:snaptoDefault(window, id, x, y, dx, dy)
	
end







return Zone
