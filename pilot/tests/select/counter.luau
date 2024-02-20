-- a counter is a table of traits
-- the order of traits is as they appear in the table with the lowest trait first


Repository = {}


function Repository:new (images)
	local o = Traits:new(images, 1)
	setmetatable(o, Repository)
	Repository.__index = Repository
	o.degrees = 0
	o.x = 0
	o.y = 0
	create_class(images[1])
	return o
end


function Repository:addTrait (id, key, trait)
	Repository[id][key] = trait
end


function Repository:flip (id)
	print(id)
	local name =  Image:Repository_nextImage(id)
	return name
end


function Repository:rotate (id)	
	Repository[id].degrees = Repository:incrementDegrees(id)
end


function Repository:incrementDegrees (id)
	local newDegrees = Repository[id].degrees + Rotate.degrees;	
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


function Counter:new (id, images, imageIndex, degrees, zorder, x, y)
	local o = Traits:new(images, imageIndex)
	setmetatable(o, Counter)
	Counter.__index = Counter
	o.degrees = degrees
	o.x = x
	o.y = y
	o.zorder = zorder
	create_class_copy(id, images[imageIndex], degrees, zorder, x, y)
	return o
end


function Counter:delete (id)
	State:addMove (id, "Delete", "", 0)	
	State:addStage()
	Counter[id] = nil
end


function Counter:flip (id)
	local name = Image:Counter_nextImage(id)
	State:addMove (id, "Image", "imageIndex", Counter[id].Image.imageIndex)
	State:addStage()
	return name
end


function Counter:toggleMoved (id)
	Counter[id].MarkMoved.moved = not Counter[id].MarkMoved.moved;	
	local flag = Counter[id].MarkMoved ~= nil and Counter[id].MarkMoved.moved
	State:addMove(id, "MarkMoved", "moved", flag)
	State:addStage()	
end


function Counter:incrementDegrees (id)
	local newDegrees = Counter[id].degrees + Rotate.degrees;	
	newDegrees = math.fmod(newDegrees, 360)	
	if newDegrees > 180 then
		newDegrees = -(360 - newDegrees)
	end	
	if newDegrees < -180 then
		newDegrees = 360 + newDegrees
	end	
	return newDegrees;	
end


function Counter:rotate (id)	
	Counter[id].degrees = Counter:incrementDegrees(id);
	State:addMove(id, "Counter", "degrees", Counter[id].degrees)
	State:addStage()	
end


function Counter:markMoved (id)	
	Counter[id].MarkMoved.moved = true;	
	State:addMove(id, "MarkMoved", "moved", 1)
	set_moved_status(id, Counter[id].MarkMoved.offsetx, Counter[id].MarkMoved.offsety, true)
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
			
			updateState(k, 
						Counter[k].x, 
						Counter[k].y,
						Counter[k].zorder,  
						Counter[k].MarkMoved ~= nil and Counter[k].MarkMoved.moved,
						Counter[k].degrees,
						name)
		end
	end
	
	
	setGUI()

end
	


return Counter
