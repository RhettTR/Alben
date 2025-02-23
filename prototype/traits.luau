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




Masks = {}

Masks["MovedMarker"] = {
	mask = "__moved", 
	x = 72, 
	y = 15
}


create_mask("MovedMarker", "__moved", 72, 15)




Overlays = {}

function Overlays:new ()
	local o = {}
	setmetatable(o, self)
	self.__index = self
	return o
end


function Overlays:add (value, apply)
	value["apply"] = apply
	table.insert(self, value)
end



Mask = {}

function Mask:new (mask, x, y)
	local o = {}
	setmetatable(o, self)
	self.__index = self
	o.mask = mask
	o.x = x
	o.y = y	
	o.apply = true	
	return o
end

Label = {}

function Label:new (text, font, size, color, x, y)
	local o = {}
	setmetatable(o, self)
	self.__index = self
	o.text = text
	o.font = font
	o.size = size
	o.color = color
	o.x = x
	o.y = y
	o.apply = true
	return o
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



--- since basic Rotate, Delete and DoesNotStack have the same function everywhere only one instance need exist


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







Base = {}

function Base:new ()
	local o = {}
	setmetatable(o, self)
	self.__index = self
	o.Delete = Delete
	return o
end


Piece = {}

function Piece:new ()
	local o = Base:new ()
	setmetatable(self, {__index = Base})
	self.__index = self 
	o.MarkMoved = MarkMoved:new()
	o.Rotate = Rotate:new()
	return o
end


German = {}

function German:new()
	local o = Piece:new ()
	setmetatable(self, {__index = Piece})
	self.__index = self
	o.Side = "German"
	o.Overlays = Overlays:new ()
	o.Overlays:add(Mask:new("German/Mask", 46, 44), (o.Side ~= PlayerSide))
	o.Overlays:add(Label:new("7", "times", 28, "white", 45, 63), (o.Side ~= PlayerSide))
	return o   	
end  


French = {}

function French:new()
	local o = Piece:new ()
	setmetatable(self, {__index = Piece})
	self.__index = self
	o.Side = "French"
	o.Overlays = Overlays:new ()
	o.Overlays:add(Mask:new("French/Mask", 46, 45), (o.Side ~= PlayerSide))
	o.Overlays:add(Label:new("R", "times", 24, "black", 45, 64), (o.Side ~= PlayerSide))
	return o   	
end 



 
return Traits
