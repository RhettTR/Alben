-- a counter is a table of traits
-- the order of traits is as they appear in the table with the lowest trait first

Counter = {}


function Counter:new (images, x, y)
	local o = Traits:new(images)
	setmetatable(o, Counter)
	Counter.__index = Counter
	o.degrees = 0
	o.x = x
	o.y = y
	--table.insert(Counter, o)
	create_class(images[1], x, y)
	State:add(images[1], Counter:copy(o))
	return o
end


function Counter:delete (id)
	State:addMove (id, "Delete", "", 0)	
	State:addStage()
	Counter[id] = nil
end


function Counter:flip (id)
	local name = Image:nextImage(id)
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






function Counter:addTrait (id, key, trait)
	Counter[id][key] = trait
	State[id][key] = Counter:copy(trait)
end


function Counter:copy(o)
	local res = {};
	for k, v in pairs(o) do
		if type(v) == "table" then
			res[k] = Counter:copy(v)
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
			
			generateGUI(k, 
						Counter[k].x, 
						Counter[k].y,  
						Counter[k].MarkMoved ~= nil and Counter[k].MarkMoved.moved,
						Counter[k].degrees,
						name)
		end
	end

end


return Counter
