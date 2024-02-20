-- set alpha transparency for counters

setCounterDragAlpha(0.5)



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

local function actionRotate (window, id)
	if window == 'Map' then
		Counter:rotate(id)
		rotate_counter(id, Counter[id].degrees)
	else
		Repository:rotate(id)
		rotate_counter(id, Repository[id].degrees)
	end
end

local function actionMoved (window, id)
	if window == 'Map' then
		Counter:toggleMoved(id)
		set_moved_status(id, Counter[id].MarkMoved.offsetx, 
							 Counter[id].MarkMoved.offsety, 
							 Counter[id].MarkMoved.moved)
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
	if action == 'actionRotate' then
		actionRotate(window, id)
	end	
	if action == 'actionMoved' then
		actionMoved(window, id)
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

	--print('doing '..event..' on '..id)
	
	--- new move, delete next moves on stack
	State:deleteNextStages()
	
	
	 
	if event == 'movetrigger' then
		--- find key 'movetrigger' for counter 'id' and call its function	
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


function create(fromId, trait, toId, degrees, zorder, x, y)
	State:addCreate (fromId, trait, toId, degrees, zorder, x, y)
end





local Counter = counter()
local Traits = traits()
local State = state()









-- set rotate degrees for counters

Rotate.degrees = 45




-- create counter from C++

function copyCounter(fromId, toId, degrees, zorder, x, y)
	local images, imageIndex = Image:getImages(fromId)	
	Counter[toId] = Counter:new(toId, images, imageIndex, degrees, zorder, x, y)
end




-- create four counters in counter window


Repository["12R-51R"] = Repository:new({"12R-51R", "12R-51R_f"})
---Repository:addTrait("12R-51R", "MarkMoved", MarkMoved:new(72, 15))


Repository["BRIGADE-3"] = Repository:new({"BRIGADE-3", "BRIGADE-3_f"})

Repository["38-94"] = Repository:new({"38-94", "38-94_f"})
Repository["VII-40"] = Repository:new({"VII-40", "VII-40_f"})



---print(dump(Repository["12R-51R"]))
---print(dump(Counter["BRIGADE-3"]))


--Counter["1000"] = Counter:new("1000", {"12R-51R", "12R-51R_f"} , 1, 0, 100, 100)
---Counter["1001"] = Counter:new("12R-51R", {"12R-51R", "12R-51R_f"} , 1, 0, 105, 95)
--Counter["1001"] = Counter:new("1001", {"12R-51R", "12R-51R_f"} , 1, 0, 100, 100)




