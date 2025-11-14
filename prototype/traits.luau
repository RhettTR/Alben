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
	--if #images > 1 then
	--	o.menuname = "Flip"
	--	o.menuclick = "actionFlip"
	--end
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

function AreaOfEffect:new (type, radius, color, opacity)
	local o = {}
	setmetatable(o, self)
	self.__index = self
	o.type = type
	o.radius = radius
	o.color = color
	o.opacity = opacity
	o.menuname = "Show Radius"
	o.menutrait = "AreaOfEffect"
	o.menuclick = "actionToggle"
	o.menushow = function(window, id) return window == 'main' end
	o.area = {}
	o.apply = false
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
	o.menutrait = "MarkMoved"
	o.menuclick = "actionMoved"
	o.movetrigger = function(id) Counter:markMoved(id, true) end
	o.menushow = function(window, id) return window == 'main' end	
	return o
end




WhenMoved = {}


function WhenMoved:new()
	local o = {}
	setmetatable(o, self)
	self.__index = self
	o.action = 
		function()
			
		end
	o.movetrigger = function(id) Counter[id].WhenMoved.action() end
	return o
end




-- These traits have the same function everywhere so only one instance need exist


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
	menutrait = "Delete",
	menuclick = "actionDelete",
	menushow = function(window, id) return window == 'main' end
} 


DoesNotStack = {
}


OnlyOneInstance = {
}


CantBeMoved = {
}


OnlyOneInStack = {
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
	o.OnlyOneInStack = OnlyOneInStack
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
	o.menutrait = "Move"
	o.menushow = function(window, id) return window == 'main' end
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


MoveStack = {}

function MoveStack:new(name, point)
	local o = {}
	o.x = point.x
	o.y = point.y
	o.menuname = name
	o.menuclick = "actionMoveStack"
	return o
end


Offset = {}
	
function Offset:new(cx, cy, of, sh)
	local o = {}
	o.xoffsetClosed = cx	-- the offset for each counter in a stack
	o.yoffsetClosed = cy	
	o.openFactor = of		-- the offset multiplier when stack opened
	o.shown = sh			-- number of counters with offset in a stack
	return o
end


Reveal = {}

function Reveal:new()
	local o = {}
	setmetatable(self, self)
	self.__index = self 	
	o.menuname = "Reveal"
	o.menutrait = "Reveal"
	o.menuclick = "actionReveal"
	o.menushow = function(window, id) return window == 'main' end
	return o
end


StepLoss = {}

function StepLoss:new()
	local o = {}
	setmetatable(self, self)
	self.__index = self 
	o.menuname = "Step Loss"
	o.menutrait = "StepLoss"
	o.menuclick = "actionStepLoss"
	o.menushow = function(window, id) return window == 'main' end
	return o
end


SubMenu = {}

function SubMenu:new(title)
	local o = {}
	setmetatable(self, self)
	self.__index = self
	o.menuname = title
	o.menuactions = [[North, ,actionMoveN:
					  North-West, ,actionMoveNW:
					  South-West, ,actionMoveSW:
					  South, ,actionMoveS:
					  South-East, ,actionMoveSE:
					  North-East, ,actionMoveNE]]
	o.menutrait = "SubMenu"
	o.menushow = function(window, id) return window == 'main' end
	return o 
end


PlaceMarker = {}

function PlaceMarker:new(name, trait, marker)
	local o = {}
	setmetatable(self, self)
	self.__index = self
	o.marker = marker
	o.menuname = name
	o.menuclick = "actionPlace"
	o.menutrait = trait
	o.menushow = function(window, id) return window == 'main' end
	return o
end


Take = {}

function Take:new(number)
	local o = {}
	setmetatable(self, self)
	self.__index = self
	o.menuname = 'Take '..number
	o.menuclick = 'actionTake'..number
	o.menutrait = 'Take'..number
	o.menushow = function(window, id) return window == 'Reinforcements' end
	return o
end




--- define counter and card playing pieces


Piece = {}

function Piece:new ()
	local o = {}
	setmetatable(self, self)
	self.__index = self
	o.StepLoss = StepLoss:new()
	o.MarkMoved = MarkMoved:new()
	o.SubMenu = SubMenu:new('Retreat')
	return o
end


German = {}

function German:new()
	local o = Piece:new()
	setmetatable(o, self)
	self.__index = self
	o.Side = "German"
	o.Disrupted = PlaceMarker:new('Disrupted', 'Disrupted', 'German Disruption')
	o.Control = PlaceMarker:new('Place German Control Marker', 'Control', 'German Control')
	return o   	
end

GermanArm = {}

function GermanArm:new()
	local o = German:new()
	setmetatable(o, self)
	self.__index = self
	o.Take3 = Take:new(3)
	return o   	
end
 
GermanMot = {}

function GermanMot:new()
	local o = German:new()
	setmetatable(o, self)
	self.__index = self
	o.Take2 = Take:new(2)
	return o   	
end 

Soviet = {}

function Soviet:new()
	local o = Piece:new()
	setmetatable(o, self)
	self.__index = self
	o.Side = "Soviet"
	o.Disrupted = PlaceMarker:new('Disrupted', 'Disrupted', 'Soviet Disruption')
	return o   	
end 


SovietHQ = {}

function SovietHQ:new()
	local o = Soviet:new()
	setmetatable(o, self)
	self.__index = self
	o.Type = "HQ"
	o.StepLoss.menuname = "Graveyard"
	
	return o   	
end


SovietInf = {}

function SovietInf:new()
	local o = Soviet:new()
	setmetatable(o, self)
	self.__index = self
	o.Type = "Inf"
	return o   	
end 


SovietArm = {}

function SovietArm:new()
	local o = Soviet:new()
	setmetatable(o, self)
	self.__index = self
	o.Type = "Arm"
	return o   	
end  







 
return Traits
