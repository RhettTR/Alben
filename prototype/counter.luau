-- a counter is a table of traits
-- the order of traits is as they appear in the table with the lowest trait first


Repository = {}


function Repository:new (images)
	local o = {}	
	setmetatable(o, self)
	Repository.__index = self
	o.Image = Image:new(images, 1)
	o.x = 0
	o.y = 0
	return o
end


function Repository:addTrait (id, key, trait)
	Repository[id][key] = trait
end


function Repository:addTraits (id, traits)
	for k, v in pairs(traits) do
		Repository:addTrait(id, k, v)
    end   
end


function Repository:flip (id)
	local name = Image:Repository_nextImage(id)
	return name
end


function Repository:rotate (id, increment)
	if Repository[id].Rotate ~= nil then
		Repository[id].Rotate.degrees = Repository:incrementDegrees(id, increment)
	end
end


function Repository:incrementDegrees (id, increment)
	local newDegrees = Repository[id].Rotate.degrees + increment;	
	newDegrees = math.fmod(newDegrees, 360)	
	if newDegrees > 180 then
		newDegrees = -(360 - newDegrees)
	end	
	if newDegrees < -180 then
		newDegrees = 360 + newDegrees
	end	
	return newDegrees;	
end



Counter = {}


function Counter:new (images, imageIndex, zorder, x, y)	
	local o = Counter:duplicate(Repository[images[1]])	
	setmetatable(o, Counter)
	Counter.__index = Counter
	o.Image.imageIndex = imageIndex
	o.x = x
	o.y = y
	o.zorder = zorder	
	return o
end

function Counter:clone (id, zorder)	
	local o = Counter:duplicate(Counter[id])	
	setmetatable(o, Counter)
	Counter.__index = Counter
	o.zorder = zorder
	return o
end


function Counter:set (table)	
	local o = Counter:duplicate(table)	
	setmetatable(o, Counter)
	Counter.__index = Counter
	return o
end


function Counter:delete (id)
	State:addMove (id, "Delete", "", 0)	
	Counter[id] = nil
end


function Counter:flip (id)
	local name = Image:Counter_nextImage(id)
	State:addMove (id, "Image", "imageIndex", Counter[id].Image.imageIndex)
	return name
end


function Counter:toggleMoved (id)
	if Counter[id].MarkMoved ~= nil then
		Counter[id].MarkMoved.moved = not Counter[id].MarkMoved.moved	
		State:addMove(id, "MarkMoved", "moved", Counter[id].MarkMoved.moved)
	end	
end


function Counter:incrementDegrees (id, increment)
	local newDegrees = Counter[id].Rotate.degrees + increment;	
	newDegrees = math.fmod(newDegrees, 360)	
	if newDegrees > 180 then
		newDegrees = -(360 - newDegrees)
	end	
	if newDegrees < -180 then
		newDegrees = 360 + newDegrees
	end	
	return newDegrees;	
end


function Counter:rotate (id, increment)	
	Counter[id].Rotate.degrees = Counter:incrementDegrees(id, increment);
	State:addMove(id, "Rotate", "degrees", Counter[id].Rotate.degrees)	
end


function Counter:markMoved (id, moved)
	if Counter[id].MarkMoved ~= nil then	
		Counter[id].MarkMoved.moved = moved;	
		State:addMove(id, "MarkMoved", "moved", moved)
		set_moved_status(id, moved)
	end
end

function Counter:select (id)
	if Counter[id].Select ~= nil then	
		toggle_select_status(id)
	end
end



function Counter:duplicate(o)
	local res = {};
	for k, v in pairs(o) do
		if type(v) == "table" then
			res[k] = Counter:duplicate(v)
		else
			res[k] = v;
		end
	end
	return res;
end


function Counter:generateGUI()
	
	for k, v in pairs(Counter) do
		
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' then
			
			local name = Counter[k].Image.images[ Counter[k].Image.imageIndex ]
			local degrees = 0
			local opacity = 1.0			
			
			if Counter[k].Rotate ~= nil then
				degrees = Counter[k].Rotate.degrees
			end
			
			if Counter[k].Visibility ~= nil then
				opacity = Counter[k].Visibility.opacity
			end
			

			updateState(k,
						Counter[k].x, 
						Counter[k].y,
						Counter[k].zorder,  
						Counter[k].MarkMoved ~= nil and Counter[k].MarkMoved.moved,
						degrees,
						name,
						opacity,
						Counter[k],
						'Overlays')
			
		end
	end
	
	
	setGUI()

end
	


return Counter
