--- set side

PlayerSide = "German"


-- set alpha transparency for counters

setCounterDragAlpha(0.5)


-- set map 

setBackgroundMapID("VAGLmap")


--- definition of actions must happen before defintion of Traits


local function actionDelete (window, id)
	if window == 'Map' then
		Counter:delete(id)
		delete_class(id)
	end
end

local function actionFlip (window, id)
	local name
	if window == 'Map' then
		name = Counter:flip(id)
	else
		name = Repository:flip(id) 
	end
	set_counter_image(id, name)
end

local function actionRotate (window, id, sign)
	if window == 'Map' then
		Counter:rotate(id, sign*Rotate.increment)
		rotate_counter(id, Counter[id].Rotate.degrees)
	else
		Repository:rotate(id, sign*Rotate.increment)
		rotate_counter(id, Repository[id].Rotate.degrees)
	end
end

local function actionMoved (window, id)
	if window == 'Map' then
		if Counter[id].MarkMoved ~= nil then
			Counter:toggleMoved(id)
			set_moved_status(id, Counter[id].MarkMoved.moved)
		end
	end
end

local function actionClone (window, id)
	if window == 'Map' then
		if Counter[id].Clone ~= nil then
			toId = next_id()
			zorder = top_zorder()
			cloneCounter(id, toId, zorder)
		end
	end
end

local function actionSelect (window, id)
	if window == 'Map' then
		if Counter[id].Select ~= nil then
			Counter:select(id)
		end
	end
end


function action(window, id, action)

	--print('doing '..action..' on '..id)
	
	if window == 'Map' then 
		State:deleteNextStages()
	end
	
	if action == 'actionDelete' then
		actionDelete(window, id)
	end	
	if action == 'actionFlip' then
		actionFlip(window, id)
	end	
	if action == 'actionRotateCw' then
		actionRotate(window, id, 1)
	end	
	if action == 'actionRotateCcw' then
		actionRotate(window, id, -1)
	end	
	if action == 'actionMoved' then
		actionMoved(window, id)
	end
	if action == 'actionClone' then
		actionClone(window, id)
	end 
	if action == 'actionSelect' then
		actionSelect(window, id)
	end
	if action == 'actionEnd' then
		State:addStage()
	end  
	
end


function dump(o)
   if type(o) == 'table' then
      local s = '{ '
      for k,v in pairs(o) do
         if type(k) ~= 'number' then k = '"'..k..'"' end
         s = s .. '['..k..'] = ' .. dump(v) .. ','
      end
      return s .. '} '
   else
      return tostring(o)
   end
end


function getTraits(window, id)

	--- find counter with key 'id'
	
	if window == 'Repository' then
		return Repository[id]
	else
		return Counter[id]
	end	

end


function getState(id)
	
	return Counter[id].x, Counter[id].y
	
end


function triggerEvent(id, o, event)
	
	if type(o) == 'table' then
		for k, v in pairs(o) do
			if type(k) == 'string' then
				if k == event and type(v) == 'function' then
					v(id)					
				end 
			end
			triggerEvent(id, v, event) 
		end
	end
end



function events(event, id, trait, key, value)
	
	-- new move, delete next moves on stack
	State:deleteNextStages()
	
	 
	if event == 'movetrigger' then
		-- find key 'movetrigger' for counter 'id' and call its function	
		local traits = getTraits("Map", id)
		triggerEvent(id, traits, event)		
	end
	
	if event == 'move' then
		State:addMove(id, trait, key, value)
	end

	if event == 'end' then
		State:addStage()
	end
	
	
end



function create(fromId, trait)
	State:addCreate (fromId, trait)
end





local Counter = counter() 
local Traits = traits()
local State = state()
local Repository = repository()









-- set rotate degrees for counters

Rotate.increment = 45






-- create Luau and C++ counter

function copyCounter(fromId, toId, zorder, x, y)
	local images, imageIndex = Image:getImages(fromId)
	Counter[toId] = Counter:new(images, imageIndex, zorder, x, y)
	create_class_copy(Counter, toId)
end

function cloneCounter(fromId, toId, zorder)
	Counter[toId] = Counter:clone(fromId, zorder)
	create_class_copy(Counter, toId)
end

function setCounter(id, table)
	Counter[id] = Counter:set(table)
	create_class_copy(Counter, id)
end

function updatePos(id, x, y)
	Counter[id]["x"] = x
	Counter[id]["y"] = y
end

function updateMoved(id, moved)
	Counter:markMoved (id, moved)
end




-- event hooks

function beforeDrag(id)

	--local counter
	
	--if Counter[id] ~= nil then
	--	counter = Counter[id]
	--else
	--	counter = Repository[id]
	--end
	
	--if counter["Side"] == PlayerSide or counter["Side"] == "" then
		return true
	--else
	--	return false
	--end
	
end

function afterDrag(id, x, y)
	
 	local counter
 		
 	if Counter[id] ~= nil then
		counter = Counter[id]
	else
		counter = Repository[id]
	end
	
	
	local maxDistance = 8;
		
	for k, v in pairs(Counter) do
	
		if type(k) == "string" and k ~= '__index' and type(v) ~= 'function' then
	
			if k ~= id then
				
				if math.abs(v["x"] - x) < maxDistance and math.abs(v["y"] - y) < maxDistance then
				   				
					if counter["DoesNotStack"] ~= nil then
						return false, x , y
					end
					if v["DoesNotStack"] ~= nil then
						return false, x , y
					end
					
					x = v["x"]
					y = v["y"]					
					return true, x , y
				   
				end 
				  	
			end	 
		end	
	end
			
	
	return true, x , y
	
end



-- handlers

function handlers(id, func)
	_G[func](id)	
end




--print(dump(Repository["German/12R-51R"]))




--copyCounter("French/BRIGADE-3", "1000", -1, 100, 100)
--copyCounter("German/12R-51R", "1001", -2, 200, 100)

--copyCounter("German/38-94", "1002", -3, 300, 100)
--copyCounter("French/VII-40", "1003", -4, 30, 30)



--local ok = beforeDrag("1002")
--if ok then
--	local x,y
--	ok, x, y = afterDrag("1002", 37, 27)
--	if ok then
--		updatePos("1002", x, y)
--		Counter:generateGUI()
--	end
--end 





function buttonHandler(id)	
	delete_button(id)
	create_button(id, id, "buttonHandler", 200+math.random(100), 100+math.random(100), 70, 30)	
end

function buttonHandler2(id)
	print('do something')
end	


--create_button("button 1", "button 1", "buttonHandler", 100, 100, 70, 30)
--create_button("button 2", "button 2", "buttonHandler2", 200, 100, 70, 30)









