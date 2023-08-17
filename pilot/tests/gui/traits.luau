-- a trait is a table
-- traits have common table-fields but extra custom fields may be added



Image = {}
	
function Image:new (images)
	local o = {}
	setmetatable(o, self)
	self.__index = self
	--- fields
	o.images = images
	o.imageIndex = 1
	o.menuname = "Flip"
	o.menuclick = "actionFlip"
	return o
end


function Image:nextImage (id)
	Counter[id].Image.imageIndex = Counter[id].Image.imageIndex + 1
	local length = #(Counter[id].Image.images)
	if Counter[id].Image.imageIndex > length then
		Counter[id].Image.imageIndex = 1
	end
	return Counter[id].Image.images[Counter[id].Image.imageIndex]
end


Mask = {}
	
function Mask:new (mask, x, y)
	local o = {}
	setmetatable(o, self)
	self.__index = self
	--- fields
	o.mask = mask
	o.offsetx = x
	o.offsety = y
	return o
end



MarkMoved = {}
	
function MarkMoved:new (x, y)
	local o = Mask:new("_Moved", x, y)
	setmetatable(self, {__index = Mask})
	self.__index = self
	o.moved = false 	
	o.menuname = "Moved"
	o.menuclick = "actionMoved"
	o.movetrigger = function(id) Counter:markMoved(id) end	
	return o
end



--- since Rotate and Delete has the same function everywhere only one instance may exist


Rotate = {	
	degrees = 0,
	menuname = "Rotate",
	menuclick = "actionRotate",
}

				
	
Delete = {
	menuname = "Delete",
	menuclick = "actionDelete"
} 





Traits = {}

function Traits:new (images)
	local o = {}
	setmetatable(o, self)
	self.__index = self
	--- fields
	o.Rotate = Rotate	
	o.Image = Image:new(images)
	---o.MarkMoved = MarkMoved:new(72, 14)
	o.Delete = Delete
	return o
end

	
return Traits
