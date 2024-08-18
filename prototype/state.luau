-- State holds the base state and state transition stack
-- State implements UNDO and REDO

State = {

stageStack = {},
stagePointer = 0

}


function State:add (id, o)
	State[id] = o
	print('added'..id)
end


function State:addMove (id, trait, key, value)
	State.stagePointer = State.stagePointer + 1
	State.stageStack[State.stagePointer] = {id = id, trait = trait, key = key, value = value}
end

function State:addCreate (id, trait)
	State.stagePointer = State.stagePointer + 1
	State.stageStack[State.stagePointer] = { id = id, trait = trait, table = Counter:set(Counter[id]) }
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
	
	
	-- remove all Counter not in State
	
	for key, value in pairs(Counter) do
		
		if type(value) ~= 'function' and key ~= '__index' then			
			if State[key] == nil then
				Counter[key] = nil			
				delete_class(key)
			end
		end
	end
	
	
	-- reset Counter
		
	for key, value in pairs(State) do
		
		if type(value) ~= 'function' and key ~= 'stageStack' and key ~= 'stagePointer' then
			if Counter[key] == nil then
				Counter[key] = Counter:duplicate(value)
			end
		end
	end
	
end


function State:executeUntil(stage)
	
	for i = 1, stage do
		
		
		local trait = State.stageStack[i].trait
		local id = State.stageStack[i].id
		local key
		local value
		
		
		if (trait ~= 'Create') then
			key = State.stageStack[i].key
			value = State.stageStack[i].value	
		end
		
			
		
		if trait == 'Counter' then
			Counter[id][key] = value		 
		end
		
		if trait == 'Rotate' then		
			Counter[id][trait][key] = value		 
		end
		
		if trait == 'MarkMoved' then
			Counter[id][trait][key] = value		 
		end
		
		if trait == 'Image' then
			Counter[id][trait][key] = value 
		end
		
		if trait == 'Create' then
			setCounter(id, State.stageStack[i].table)			
		end
		
		if trait == 'Delete' then			
			Counter[id] = nil			
			delete_class(id)
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
	
	State:reset()			-- reset Counter with saved State
	
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
