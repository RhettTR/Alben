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





--create_mask('MovedMarker', '__moved', 'left', 'hcenter')
create_mask('MovedMarker', '__moved', 'vleft', 'htop')



Overlays = {}

function Overlays:new ()
	local o = {}
	setmetatable(o, self)
	self.__index = self
	return o
end

function Overlays:add (id, value)
	self[id] = value
end


function Overlays:show (trait, id, name)
	local o = {}
	o.id = id
	o.menuname = name
	o.menuclick = 'actionToggleO'
	o.menutrait = trait
	o.menushow = function(window, id) return window == 'main' end
	return o
end


function Overlays:set(trait, id, name)
	local o = {}
	o.id = id
	o.menuname = name
	o.menuclick = 'actionInput'
	o.menutrait = trait
	o.menushow = function(window, id) return window == 'main' end
	return o
end


function Overlays:inc(trait, id, increment, name)
	local o = {}
	o.id = id
	o.inc = increment
	o.menuname = name
	o.menuclick = 'actionInc'
	o.menutrait = trait
	o.menushow = function(window, id) return window == 'main' end
	return o
end



Mask = {}

function Mask:new (mask, x, y)
	local o = {}
	setmetatable(o, self)
	self.__index = self
	o.mask = mask
	o.x = x
	o.y = y	
	o.apply = false
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
	o.menutrait = "Clone"
	o.menuclick = "actionClone"
	o.menushow = function(window, id) return window == 'main' end
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
	--o.Delete = Delete
	--o.OnlyOneInStack = OnlyOneInStack
	o.CantBeMoved = CantBeMoved
	return o
end


Move = {}

function Move:new(window, trait, name, point)
	local o = {}
	o.x = point.x
	o.y = point.y
	o.window = window
	o.menuname = name
	o.menuclick = "actionMove"
	o.menutrait = trait
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
	
function Offset:new(cx, cy, xm, ym, sh)
	local o = {}
	o.xoffsetClosed = cx	-- the offset for each counter in a stack
	o.yoffsetClosed = cy	
	o.xopenFactor = xm		-- the offset multiplier when stack opened
	o.yopenFactor = ym
	o.shown = sh			-- number of counters with offset in a stack, 0=unlimited
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




--- define menu table

Broom = {}

function Broom:new()
	local o = {}
	o.options = {
		supply = {menuname = "Remove All Out-Of-Supply Markers", menutrait = 'supply', menuclick = "actionToggleA"},
		freeRail = {menuname = "Remove All Free Rail Markers", menutrait = 'freeRail', menuclick = "actionToggleA"},
		rail = {menuname = "Remove All Rail Movement Markers", menutrait = 'rail', menuclick = "actionToggleR"},	
		autoVictory = {menuname = "Remove Automatic Victory Markers", menutrait = 'autoVictory', menuclick = "actionToggleA"},
		removeAll = {menuname = "Remove All Markers", menutrait = 'removeAll', menuclick = "actionToggleX"}	
	}
	return o
end

Globals:add('Broom', Broom:new())




--- define counter and card traits


Stukas = {}

function Stukas:new()
	local o = {}
	setmetatable(o, self)
	self.__index = self
	o.airfield = Move:new('Airfield', 'airfield', 'To Airfield', {x = 250 + math.random(-200, 200), y = 157 + math.random(-75, 75)})
	return o
end


Markers = {}

function Markers:new ()
	local o = {}
	setmetatable(self, self)
	self.__index = self
	o.Delete = Delete
	o.Clone = Clone:new()	
	return o
end



Odds = {}

function Odds:new()
	local o = Markers:new()
	setmetatable(o, self)
	self.__index = self
	o.Overlays = Overlays:new()
	o.Overlays:add('number', Label:new('1', 'sans', 24, 'black', 'vcenter', 'hcenter'))
	o.Overlays:add('odds', Label:new('1:1', 'arial', 12, 'black', 'vcenter', 47))
	o.Label = o.Overlays:set('Label', 'odds', 'Set Odds')
	o.Increase = o.Overlays:inc('Increase', 'number', 1, 'Increase')
	o.Decrease = o.Overlays:inc('Decrease', 'number', -1, 'Decrease')
	return o
end


Piece = {}

function Piece:new ()
	local o = {}
	setmetatable(self, self)
	self.__index = self
	o.MarkMoved = MarkMoved:new()
	o.Overlays = Overlays:new()
	o.Overlays:add('train_marker_black_50', Mask:new('train_marker_black_50', 4, 4))
	o.Overlays:add('train_marker_blue_50', Mask:new('train_marker_blue_50', 4, 4))
	o.Overlays:add('av_force_layer', Mask:new('av_force_layer', 0, 0))	
	o.Overlays:add('supply', Mask:new('supply', 0, 0))
	o.Overlays:add('top', Label:new('', 'arial', 12, 'black', 'vcenter', 0))
	o.freeRail = o.Overlays:show('freeRail', 'train_marker_black_50', 'Free Rail Movement')
	o.rail = o.Overlays:show('rail', 'train_marker_blue_50', 'Rail Movement (6/3)')
	o.autoVictory = o.Overlays:show('autoVictory', 'av_force_layer', 'Automatic Victory')
	o.supply = o.Overlays:show('supply', 'supply', 'Out Of Supply')
	o.label = o.Overlays:set('label', 'top', 'Change Label')
	o.prisoners = Move:new('POW', 'prisoners', 'Prisoners Of War', {x = 100 + math.random(-35, 35), y = 100 + math.random(-35, 35)})
	o.replace = Move:new('Repl', 'replace', 'To Replacement Pool', {x = 150, y = 150})

	return o
end


Axis = {}

function Axis:new ()
	local o = Piece:new()
	setmetatable(o, self)
	self.__index = self
	o.rail = o.Overlays:show('rail', 'train_marker_blue_50', 'Rail Movement (6/3)')
	return o
end


Soviet = {}

function Soviet:new ()
	local o = Piece:new()
	setmetatable(o, self)
	self.__index = self
	o.rail = o.Overlays:show('rail', 'train_marker_blue_50', 'Rail Movement (5)')
	return o
end



 
return Traits
