-- set alpha transparency for counters

setCounterDragAlpha(0.5)



--- definition of actions must happen before defintion of Traits


local function actionDelete (id)
	Counter:delete(id)
	delete_class(id)
end

local function actionFlip (id)
	local name = Counter:flip(id)
	set_counter_image(id, name)
end

local function actionRotate (id)
	Counter:rotate(id)
	rotate_counter(id, Counter[id].degrees)
end

local function actionMoved (id)	
	Counter:toggleMoved(id)
	set_moved_status(id, Counter[id].MarkMoved.offsetx, 
						 Counter[id].MarkMoved.offsety, 
						 Counter[id].MarkMoved.moved)
end



function action(id, action)

	--print('doing '..action..' on '..id)
	
	State:deleteNextStages()
	
	if action == 'actionDelete' then
		actionDelete(id)
	end	
	if action == 'actionFlip' then
		actionFlip(id)
	end	
	if action == 'actionRotate' then
		actionRotate(id)
	end	
	if action == 'actionMoved' then
		actionMoved(id)
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


function getTraits(name)

	--- find counter with key 'name'
	
	return Counter[name]

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

	print('doing '..event..' on '..id)
	
	--- new move, delete next moves on stack
	State:deleteNextStages()
	
	
	 
	if event == 'movetrigger' then
		--- find key 'movetrigger' for counter 'id' and call its function	
		local traits = getTraits(id)
		triggerEvent(id, traits, event)		
	end
	
	if event == 'move' then
		State:addMove(id, trait, key, value)
	end
	
	if event == 'end' then
		State:addStage()
	end
	
	
end





local Counter = counter()
local Traits = traits()
local State = state()









-- set rotate degrees for counters

Rotate.degrees = 45



-- create two counters


Counter["12R-51R"] = Counter:new({"12R-51R", "12R-51R_f"}, 130, 100)
Counter:addTrait("12R-51R", "MarkMoved", MarkMoved:new(72, 15))


Counter["BRIGADE-3"] = Counter:new({"BRIGADE-3", "BRIGADE-3_f"}, 250, 100)



---print(dump(Counter["12R-51R"]))
---print(dump(Counter["BRIGADE-3"]))







