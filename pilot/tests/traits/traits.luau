-- a trait is a table
-- traits have common table-fields but extra custom fields may be added



Image = {}
	
function Image:new (images)
	local o = {}
	setmetatable(o, self)
	self.__index = self
	--- fields
	o.images = images
	o.menuname = "Flip"
	o.menuclick = "actionFlip"
	return o
end


--- since Rotate and Delete has the same function everywhere only one instance may exist


Rotate = {	
	degrees = 60,
	menuname = "Rotate",
	menuclick = "actionRotate"
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
	o.Delete = Delete
	return o
end

	
return Traits
