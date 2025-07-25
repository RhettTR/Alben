-- a trait is a table
-- traits have common table-fields but extra custom fields may be added



Image = {}
	
function Image:new (images, imageIndex)
	local o = {}
	setmetatable(o, self)
	self.__index = self
	--- fields
	o.images = images
	o.imageIndex = imageIndex
	if #images > 1 then
		o.menuname = "Flip"
		o.menuclick = "actionFlip"
	end
	return o
end


function Image:Repository_nextImage (id)
	Repository[id].Image.imageIndex = Repository[id].Image.imageIndex + 1
	local length = #(Repository[id].Image.images)
	if Repository[id].Image.imageIndex > length then
		Repository[id].Image.imageIndex = 1
	end
	return Repository[id].Image.images[Repository[id].Image.imageIndex]
end


function Image:Counter_nextImage (id)
	Counter[id].Image.imageIndex = Counter[id].Image.imageIndex + 1
	local length = #(Counter[id].Image.images)
	if Counter[id].Image.imageIndex > length then
		Counter[id].Image.imageIndex = 1
	end
	return Counter[id].Image.images[Counter[id].Image.imageIndex]
end
	

function Image:getImages (id)
	return Repository[id].Image.images, Repository[id].Image.imageIndex	
end





create_mask('MovedMarker', '__moved', 'left', 'hcenter')




Overlays = {}

function Overlays:new ()
	local o = {}
	setmetatable(o, self)
	self.__index = self
	return o
end


function Overlays:add (value)
	value["apply"] = value.test(value)
	table.insert(self, value)
end



Mask = {}

function Mask:new (mask, x, y, func)
	local o = {}
	setmetatable(o, self)
	self.__index = self
	o.mask = mask
	o.x = x
	o.y = y	
	o.test = func
	return o
end

Label = {}

function Label:new (text, font, size, color, x, y, func)
	local o = {}
	setmetatable(o, self)
	self.__index = self
	o.text = text
	o.font = font
	o.size = size
	o.color = color
	o.x = x
	o.y = y
	o.test = func
	return o
end

function Label:setText (value)
	self.text = value
	self.apply = self.test(self)
end




AreaOfEffect = {}

function AreaOfEffect:new (radius, color, opacity)
	local o = {}
	setmetatable(o, self)
	self.__index = self
	o.radius = radius
	o.color = color
	o.opacity = opacity
	o.apply = true
	return o
end


Clone = {}

function Clone:new ()
	local o = {}
	setmetatable(o, self)
	self.__index = self	
	o.menuname = "Clone"
	o.menuclick = "actionClone"
	return o
end


Select = {}

function Select:new ()
	local o = {}
	setmetatable(o, self)
	self.__index = self	
	o.menuname = "Selected"
	o.menuclick = "actionSelect"
	return o
end


Visibility = {}

function Visibility:new (opacity)
	local o = {}
	setmetatable(o, self)
	self.__index = self	
	o.opacity = opacity
	o.apply = true
	return o
end


MarkMoved = {}
	
function MarkMoved:new ()
	local o = {}
	setmetatable(o, self)
	self.__index = self
	o.mask = "MovedMarker"
	o.moved = false 	
	o.menuname = "Moved"
	o.menuclick = "actionMoved"
	o.movetrigger = function(id) Counter:markMoved(id, true) end	
	return o
end




WhenMoved = {}


function WhenMoved:new()
	local o = {}
	setmetatable(o, self)
	self.__index = self
	o.action = 
		function()
			local ap = Counter:findId("Markers/warstatus_ap")
			local cp = Counter:findId("Markers/warstatus_cp")
			local combined = Counter:findId("Markers/warstatus_combined")
		
			if not ap or not cp or not combined then
				return
			end
			-- find turn for warstatus_ap and warstatus_cp
			local n = tonumber(Zones:getPlace(Counter[ap].x, Counter[ap].y)) 		
			if n == nil then
				return
			end									
			local m = tonumber(Zones:getPlace(Counter[cp].x, Counter[cp].y)) 
			if m == nil then
				return
			end				
			-- find position for sum
			local x, y = Zones:getCoordinates("grt", n + m)
			if x == -1 then
				return
			end
			-- move warstatus_combined			
			Counter:move(combined, x, y)
		end
	o.movetrigger = function(id) Counter[id].WhenMoved.action() end
	return o
end




-- Rotate,Delete,DoesNotStack,OnlyOneInstance have the same function everywhere so only one instance need exist


RotateCW = {	
	menuname = "Rotate cw",
	menuclick = "actionRotateCw"
}


RotateCCW = {	
	menuname = "Rotate ccw",
	menuclick = "actionRotateCcw"
}

			
	
Delete = {
	menuname = "Delete",
	menuclick = "actionDelete"
} 


DoesNotStack = {
}


OnlyOneInstance = {
}


CantBeMoved = {
}

--[[
AllCards = {
	hooverstack = false,           
	openstack = false,              
	offsetlessNumber = false,       
	offsetMax = 3,				    
	offsetAmount = 3,              							 
	faceupIndex = 1,              
	faceDownAlwaysShuffled = true   
}
--]]


--- define the rotation trait of one particular counter

Rotate = {}

function Rotate:new ()
	local o = {}
	setmetatable(o, self)
	self.__index = self
	o.rotatecw = RotateCW
	o.rotateccw = RotateCCW
	o.degrees = 0
	return o
end




Global = {}	

function Global:addTrait (key, trait)
	Global[key] = trait
end


Area = {
	menuDelete = {menuname = "Delete all markers here", menuclick = "actionDeleteAll"},
	menuATrench = {menuname = "Place AP Trench", menuid = "Allied/ap_trench1"}
}


Marker = {}

function Marker:new(name, dx, dy, side)
	local o = {}
	o.marker = name
	o.renderOffset = {dx = dx, dy = dy}
	o.Side = side
	o.Delete = Delete
	o.CantBeMoved = CantBeMoved
	return o
end


Move = {}

function Move:new(name, point)
	local o = {}
	o.x = point.x
	o.y = point.y
	o.menuname = name
	o.menuclick = "actionMove"
	return o
end


MoveAll = {}

function MoveAll:new(zone, name)
	local o = {}
	o.zoneTag = zone	
	o.menuname = name
	o.menuclick = "actionMoveAll"
	return o
end







--- define counter and card playing pieces


Piece = {}

function Piece:new ()
	local o = {}
	setmetatable(self, self)
	self.__index = self 
	o.MarkMoved = MarkMoved:new()
	return o
end


German = {}

function German:new()
	local o = Piece:new ()
	setmetatable(self, {__index = Piece})
	self.__index = self
	o.Side = "Central"
	return o   	
end  


French = {}

function French:new()
	local o = Piece:new ()
	setmetatable(self, {__index = Piece})
	self.__index = self
	o.Side = "Allied"
	return o   	
end 

British = {}

function British:new()
	local o = Piece:new ()
	setmetatable(self, {__index = Piece})
	self.__index = self
	o.Side = "Allied"
	return o   	
end




 





 
return Traits
