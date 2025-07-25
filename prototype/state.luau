-- State holds the base state and state transition stack
-- State implements UNDO and REDO

State = {

baseState = {},
stageStack = {},
stagePointer = 0,
-- used for saving logfile
savedPointer = 0

}



function State:addMove (id, trait, key, value)
	State.stagePointer = State.stagePointer + 1
	State.stageStack[State.stagePointer] = {id = id, trait = trait, key = key, value = value}
end

function State:addCreate (id, trait)
	State.stagePointer = State.stagePointer + 1
	State.stageStack[State.stagePointer] = { id = id, trait = trait, table = Counter:load(Counter[id]) }
end

function State:addStage ()
	State.stagePointer = State.stagePointer + 1
	State.stageStack[State.stagePointer] = {id = "end"}
	if not_recording() then
		toolbar_enable('__undo')
	end
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
	
	
	toolbar_disable('__redo')
	
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


function State:resetState()

	toolbar_disable('__undo')
	toolbar_disable('__redo')
	
	
	State.stageStack = nil
	State.stageStack = {}
	
	
	State.stagePointer = 0 
	
	State.baseState = nil
	State.baseState = {}
	
end


function State:reset()	
	
	-- reset map to state before stack was made 
		
	for key, value in pairs(Counter) do
			
		if type(key) == "string" and type(value) ~= 'function' and key ~= '__index' then
		
			delete_class(key)
			
			if State.baseState[key] ~= nil then
				setCounter(key, State.baseState[key])
			end	
			
		end	
	end
	
end


function State:executeUntil(stack, from, to)
	
	for i = from, to do
				
		local trait = stack[i].trait
		local id = stack[i].id
		local key
		local value
		
		
		if (trait ~= 'Create') then
			key = stack[i].key
			value = stack[i].value	
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
			setCounter(id, stack[i].table)
			dropped("Map", stack[i].table.Image.images[1], id, stack[i].table.x, stack[i].table.y)
		end
		
		if trait == 'Delete' then		
			delete_class(id)
		end

		
	end	

	
end


function State:undo()

	
	if #State.stageStack > 0 then
		if not_recording() then		
			toolbar_enable('__redo')
		end
	end
	
			
	local i = State:findLastStage ()
	
	-- reset counters to base state
	State:reset()								
	
	-- execute moves up to last stage
	State:executeUntil(State.stageStack, 1, i)	
	

	Counter:generateGUI()
	
	
	if i == 0 then
		toolbar_disable('__undo')
	end
	
end




function State:redo()
	
	if #State.stageStack > 0 then
		if not_recording() then	
			toolbar_enable('__undo')
		end
	end
	
	
	local i = State:findNextStage ()
	
	-- reset counters to base state
	State:reset()								
	
	-- execute moves up to next stage
	State:executeUntil(State.stageStack, 1, i)	
		
		
	Counter:generateGUI()
	
	
	if i == #State.stageStack then
		toolbar_disable('__redo')
	end	
	
end




Log = {
	stageStack = {},
	stagePointer = 0,
	stepPointer = 1
}


function Log:reset()
	Log.stageStack = nil
	Log.stageStack = {}
	Log.stagePointer = 0
	Log.stepPointer = 1
end


function Log:add(table)
	Log.stagePointer = Log.stagePointer + 1
	Log.stageStack[Log.stagePointer] = Counter:duplicate(table)
end


function Log:step(oneStep)

	local from = Log.stepPointer
	
	for i = Log.stepPointer + 1, #Log.stageStack do	
		value = Log.stageStack[i]
		if value.id == 'end' then
			Log.stepPointer = i
			if oneStep then
				break
			end
		end
	end
	
	State:executeUntil(Log.stageStack, from, Log.stepPointer)
	
	disable_all()
	
	-- if stepping done
	if  Log.stepPointer ==  #Log.stageStack then
		Log:abort()
	end
	
	Counter:generateGUI()
	
end


function Log:abort()

	toolbar_disable('__forward')
	toolbar_disable('__end')
	toolbar_disable('__abort')
	
	stop_stepping()
	
	State:resetState()
	
	for id, v in pairs(Counter) do
		if type(id) == "string" and id ~= '__index' and type(v) ~= 'function' then
			State.baseState[id] = Counter:duplicate(v)
		end		
	end
	
end



function undo()

	State:undo()
	
	-- hook
	undohook()
	
end

function redo()

	State:redo()
	
end


return State
