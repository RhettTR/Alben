-- set rotate degrees for counters

setCounterRotateDegrees(45)





--- definition of actions must happen before defintion of Traits


local function actionDelete (id)
	Counter:delete(id)
	delete_class(id)
end


local function actionFlip (id)
	local name = Counter:nextImage(id)
	set_counter_image(id, name)
end

local function actionRotate (id)
	rotate_counter(id)
end



function action(id, action)

	print('doing '..action..' on '..id)
	
	if action == 'actionDelete' then
		actionDelete(id)
	end 
	
	if action == 'actionFlip' then
		actionFlip(id)
	end
	
	if action == 'actionRotate' then
		actionRotate(id)
	end 
	
	
	---print(dump(Counter["12R-51R"]))
	---print(dump(Counter["BRIGADE-3"]))
	
end





local Counter = counter()
local Traits = traits()




-- create two counters


Counter["12R-51R"] = Counter:new({"12R-51R", "12R-51R_f"})
Counter["BRIGADE-3"] = Counter:new({"BRIGADE-3", "BRIGADE-3_f"})





function getTraits(name)

	--- find counter with key 'name'
	
	return Counter[name]

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


---print(dump(Counter["12R-51R"]))
---print(dump(Counter["BRIGADE-3"]))






