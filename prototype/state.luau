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

function State:addCreate (fromId, trait, toId, degrees, zorder, x, y)
	State.stagePointer = State.stagePointer + 1
	State.stageStack[State.stagePointer] = {fromId = fromId, trait = trait, toId = toId, degrees = degrees, zorder = zorder, x = x, y = y}
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
				Counter[key] = Counter:new(key, value.Image.images, value.Image.imageIndex, value.degrees, value.zorder, value.x, value.y)
			else
				Counter[key] = Counter:duplicate(value)
			end
		end
	end
	
end


function State:executeUntil(stage)
	
	for i = 1, stage do
		
		local id
		local trait = State.stageStack[i].trait
		local key
		local value
		local fromId
		local toId
		local degrees
		local zorder
		local x
		local y
				
		if (trait == 'Create') then
			fromId = State.stageStack[i].fromId
			toId = State.stageStack[i].toId
			degrees = State.stageStack[i].degrees
			zorder = State.stageStack[i].zorder
			x = State.stageStack[i].x
			y = State.stageStack[i].y
		else
			id = State.stageStack[i].id
			key = State.stageStack[i].key
			value = State.stageStack[i].value	
		end
		
		
		
		if trait == 'Counter' then
			Counter[id][key] = value		 
		end
		
		if trait == 'MarkMoved' then
			Counter[id][trait][key] = value		 
		end
		
		if trait == 'Image' then
			Counter[id][trait][key] = value 
		end
		
		if trait == 'Create' then
			copyCounter(fromId, toId, degrees, zorder, x, y) 			
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
