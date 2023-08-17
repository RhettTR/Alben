-- State holds the base state and state transition stack
-- State implements UNDO and REDO

State = {

stageStack = {},
stagePointer = 0

}


function State:add (id, o)
	State[id] = o
end


function State:addMove (id, trait, key, value)
	State.stagePointer = State.stagePointer + 1
	State.stageStack[State.stagePointer] = {id = id, trait = trait, key = key, value = value}
end


function State:addStage ()
	State.stagePointer = State.stagePointer + 1
	State.stageStack[State.stagePointer] = {id = "end"}
end


function State:findLastStage ()
	
	for i = State.stagePointer - 1, 1, -1 do	
		value = State.stageStack[i]
		if value.id == 'end' then
			State.stagePointer = i;
			return i
		end
	end
	
	State.stagePointer = 0 
	return 0

end


function State:deleteNextStages()
	
	local start = State.stagePointer + 1
	local upto = #State.stageStack
	
	
	for i = start, upto do
		State.stageStack[i] = nil
	end
	
	--print(#State.stageStack)
	
end


function State:findNextStage ()

	for i = State.stagePointer + 1, #State.stageStack do	
		value = State.stageStack[i]
		if value.id == 'end' then
			State.stagePointer = i;
			return i
		end
	end
	
	return State.stagePointer

end


function State:reset()
	
	for key, value in pairs(State) do
		if type(value) ~= 'function' and key ~= 'stageStack' and key ~= 'stagePointer' then
			if Counter[key] == nil then			
				create_class(value.Image.images[1], value.x, value.y)
				--Counter[key] = Counter:new(value.Image.images, value.x, value.y)				
			end
			Counter[key] = Counter:copy(value)	
		end
	end
	
end


function State:executeUntil(stage)
	
	for i = 1, stage do
		
		local id = State.stageStack[i].id
		local trait = State.stageStack[i].trait
		local key = State.stageStack[i].key
		local value = State.stageStack[i].value	
		
		print(State.stageStack[i].trait)
		
		if trait == 'Counter' then
			Counter[id][key] = value		 
		end
		
		if trait == 'MarkMoved' then
			Counter[id][trait][key] = value		 
		end
		
		if trait == 'Image' then
			Counter[id][trait][key] = value 
		end
		
		if trait == 'Delete' then			
			Counter[id] = nil	
		end
		
	end
	
end


function State:undo()
	
	local i = State:findLastStage ()
	
	State:reset()			-- reset Counter with saved State	
	
	State:executeUntil(i)	-- execute moves up to last stage
	
	
	Counter:generateGUI()
	
end





function State:redo()

	local i = State:findNextStage ()
	
	State:executeUntil(i)	-- execute moves up to next stage
		
	Counter:generateGUI()
	
end



function undo()

	State:undo()
	
end


function redo()

	State:redo()
	
end


return State
