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
	o.menuname = "Flip"
	o.menuclick = "actionFlip"
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

function Traits:new (images, imageIndex)
	local o = {}
	setmetatable(o, self)
	self.__index = self
	--- fields
	o.Rotate = Rotate	
	o.Image = Image:new(images, imageIndex)
	o.MarkMoved = MarkMoved:new()
	o.Delete = Delete
	return o
end

	
return Traits
