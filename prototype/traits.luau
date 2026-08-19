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



Side = {}

function Side:new(side)
	local o = {}
	if side == nil then
		o.side = 'none'
	else
		o.side = side
	end
	return o
end

function Side:findAllSides()
	local sides = {}
	for k, v in pairs(Repository) do
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' then				
			sides[Repository[k].Side.side] = true
		end
	end
	return sides
end



Flip = {}

function Flip:new(id)
	local o = {}
	if id ~= nil then	
		if #Repository[id].Image.images > 1 then
			o.menuname = "Flip"
			o.menuclick = "actionFlip"
		end
	end
	return o
end





create_mask('MovedMarker', '__moved', 'left', 'hcenter')




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
	o.action = 
		function()
		
			local ap = Counter:findId('warstatus_ap')
			local cp = Counter:findId('warstatus_cp')
			local combined = Counter:findId('warstatus_combined')
		
			if not ap or not cp or not combined then
				return
			end
			
			
			local zone = Zones:get('grt')
			
			-- find turn for warstatus_ap and warstatus_cp
			local n = tonumber(zone:getCoordinate(Counter[ap].cx, Counter[ap].cy))		
			if n == nil then
				return
			end									
			local m = tonumber(zone:getCoordinate(Counter[cp].cx, Counter[cp].cy)) 
			if m == nil then
				return
			end	
			
						
			-- find position for sum
			
			local cx, cy = zone:setCoordinate(n + m)
			
			if cx == -1 then
				return
			end
				
			-- move warstatus_combined
				
			local dx, dy = get_corner(Counter[combined].Image.images[1])
			local zorder = top_zorder()
			
			reportmove(combined, Counter[combined].name, 'main', 'main', 
					   Counter[combined].cx, Counter[combined].cy, cx, cy)
					
			Counter:move('main', combined, zorder, cx - dx, cy - dy, cx, cy)
			
			update_class(Counter, combined)
			
				
			
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


Facing = {
	menuname = "Adjust Facing",
	menuclick = "actionFacing"
}


			
	
Delete = {
	menuname = "Delete",
	menutrait = "Delete",
	menufield =  "Delete",
	menuclick = "actionDelete",
	menushow = function(window, id) return not (window == 'Repository') end
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

function Rotate:new (menu)
	local o = {}
	for k,v in pairs(menu) do
		o[k] = v
	end
	o.degrees = 0
	return o
end




Global = {}	

function Global:addTrait (key, trait)
	Global[key] = trait
end




Marker = {}

function Marker:new(name, dx, dy, side, stackplace)
	local o = {}
	o.marker = name
	o.place = stackplace
	o.renderOffset = {dx = dx, dy = dy}
	o.Side = side
	o.Delete = Delete
	o.OnlyOneInStack = OnlyOneInStack
	o.CantBeMoved = CantBeMoved
	return o
end


Move = {}

function Move:new(field, window, zone, point, name, action)
	local o = {}
	setmetatable(self, self)
	self.__index = self
	o.trait = "Move"
	o.zone = zone
	o.point = point
	o.window = window
	o.menuname = name
	o.menuclick = "actionMove"
	o.menutrait = o.trait
	o.menufield = field
	if action == nil then
		o.menushow = function(window, id) return window == 'main' end
	else
		o.menushow = action
	end	
	return o
end


MoveAll = {}

function MoveAll:new(tag, name)
	local o = {}
	-- tag is zone moved to
	o.tag = tag	
	o.menuname = name
	o.menuclick = "actionMoveAll"
	return o
end


MoveStack = {}

function MoveStack:new(from, to, name, action)
	local o = {}
	-- from is grid or deck id moved from
	-- to is the position moved to
	o.id = from
	o.cx = to.x
	o.cy = to.y
	o.menuname = name
	o.menuclick = "actionMoveStack"
	o.menutrait = "MoveStack"
	o.menufield = "MoveStack"
	o.menushow = action
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


Card = {}

function Card:new()
	local o = {}
	o.deckId = ''
	o.actionUp = {}
	o.actionDown = {}
	return o 
end


Draw = {}

function Draw:new(action)
	local o = {}
	o.menuname = 'Draw'
	o.menuclick = 'actionDraw'
	o.menutrait = 'Draw'
	o.menushow = action
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


Shadow = {}

function Shadow:new(shadow)
	local o = {}
	setmetatable(o, self)
	self.__index = self
	o.shadow = shadow
	return o
end	




SubMenu = {}

function SubMenu:new(title, actions)
	local o = {}
	setmetatable(self, self)
	self.__index = self
	o.menuname = title
	o.menuactions = actions
	o.menutrait = "SubMenu"
	o.menufield = "SubMenu"
	o.menushow = function(window, id) return window == 'main' end
	return o 
end


PlaceMarker = {}

function PlaceMarker:new(name, field, marker)
	local o = {}
	setmetatable(self, self)
	self.__index = self
	o.menuname = name
	o.menuclick = "actionPlace"
	o.menutrait = "PlaceMarker"
	o.menufield = field
	o.menuid = marker
	o.menushow = function(window, id) return window == 'main' end
	return o
end









--- define counter and card traits



areaActions = [[Central Control, control_cp, actionPlace, Central:
				Allied Control, control_ap, actionPlace, Allied:
				Place Central Trench, cp_trench1, actionPlace, Central:
				Place Allied Trench, ap_trench1, actionPlace, Allied:
				Trench Modification, trench_mod, actionPlace:
				Fort Besieged, fort_besieged, actionPlace:
				Fort Destroyed, fort_destroyed, actionPlace:
				Fort Destroyed (mini), fort_destroyed_mini, actionPlace:
				Out of Supply (Allied), oos_ap, actionPlace, Allied:
				Out of Supply (Central), oos_cp, actionPlace, Central:
				Delete all markers, , actionRemoveMarkers]]


Piece = {}

function Piece:new (id)
	local o = {}
	setmetatable(self, self)
	self.__index = self
	o.Flip = Flip:new(id)
	o.SubMenu = SubMenu:new('Area', areaActions)
	o.MoveMarker = PlaceMarker:new('Move', 'MoveMarker', 'activate_move')
	o.AttackeMarker = PlaceMarker:new('Attack', 'AttackeMarker', 'activate_attack')	
	return o
end


Army = {}

function Army:new (id)
	local o = Piece:new(id)
	setmetatable(self, {__index = Piece})
	self.__index = self
	o.Shadow = Shadow:new('shadow_lg_blurred')
	return o
end


Corps = {}

function Corps:new (id)
	local o = Piece:new(id)
	setmetatable(self, {__index = Piece})
	self.__index = self
	o.Shadow = Shadow:new('shadow_blurred')
	return o
end


CentralArmy = {}

function CentralArmy:new (id)
	local o = Army:new(id)
	setmetatable(self, {__index = Army})
	self.__index = self
	-- need to send to more places than just 1
	o.Elim = Move:new('Elim', 'main', 'cel', 1, 'Eliminated', nil)
	o.Perm = Move:new('Perm', 'pelim', '', {x=1032,y=253}, 'Permanently Eliminated')
	return o
end


CentralCorps = {}

function CentralCorps:new (id)
	local o = Corps:new(id)
	setmetatable(self, {__index = Corps})
	self.__index = self
	o.Elim = Move:new('Elim', 'main', 'cel', 7, 'Eliminated', nil)
	o.Perm = Move:new('Perm', 'pelim', '', {x=1215,y=285}, 'Permanently Eliminated')
	return o
end


AlliedArmy = {}

function AlliedArmy:new (id)
	local o = Army:new(id)
	setmetatable(self, {__index = Army})
	self.__index = self
	o.Elim = Move:new('Elim', 'main', 'ael', 1, 'Eliminated', nil)
	o.Perm = Move:new('Perm', 'pelim', '', {x=306,y=253}, 'Permanently Eliminated')
	return o
end


AlliedCorps = {}

function AlliedCorps:new (id)
	local o = Corps:new(id)
	setmetatable(self, {__index = Corps})
	self.__index = self
	o.Elim = Move:new('Elim', 'main', 'ael', 7, 'Eliminated', nil)
	o.Perm = Move:new('Perm', 'pelim', '', {x=483,y=285}, 'Permanently Eliminated')
	return o
end





 
return Traits
