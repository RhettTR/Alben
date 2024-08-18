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




Mask = {}

function Mask:new ()
	local o = {}
	setmetatable(o, self)
	self.__index = self
	o.mask = ""
	o.x = 0
	o.y = 0
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
	o.movetrigger = function(id) Counter:markMoved(id) end	
	return o
end



--- since basic Rotate and Delete has the same function everywhere only one instance may exist


BaseRotate = {	
	increment = 0,
	menuname = "Rotate",
	menuclick = "actionRotate",
}

			
	
Delete = {
	menuname = "Delete",
	menuclick = "actionDelete"
} 



--- define the rotation trait of one particular counter

Rotate = {}

function Rotate:new ()
	local o = BaseRotate
	setmetatable(o, self)
	self.__index = self	
	o.degrees = 0
	return o
end







Base = {}

function Base:new ()
	local o = {}
	setmetatable(o, self)
	self.__index = self
	o.Delete = Delete
	o.Side = ""
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
	o.Mask = Mask:new()
	o.Mask.mask = "German/Mask"
	o.Mask.x = 46
	o.Mask.y = 44
	return o   	
end  


French = {}

function French:new()
	local o = Piece:new ()
	setmetatable(self, {__index = Piece})
	self.__index = self
	o.Side = "French"
	return o   	
end 



 
return Traits
