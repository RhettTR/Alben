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

function State:addText (key, text)
	State.stagePointer = State.stagePointer + 1
	State.stageStack[State.stagePointer] = {id = '', trait = 'text', key = key, value = text}
end

function State:addStage ()
	State.stagePointer = State.stagePointer + 1
	State.stageStack[State.stagePointer] = {id = "end"}
	toolbar_enable('main', '__undo')
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
	
	
	toolbar_disable('main', '__redo')
	
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

	toolbar_disable('main', '__undo')
	toolbar_disable('main', '__redo')
	
	
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
	
	for key, value in pairs(Decks) do
		
		if type(key) == "string" and type(value) ~= 'function' and key ~= '__index' then
			local deck = Decks[key]
			if State.baseState[key] ~= nil then
				deck.cards = Counter:duplicate(State.baseState[key])
			end	
		end
	end
	
	for key, value in pairs(Globals) do
	
		if type(key) == "string" and type(value) ~= 'function' and key ~= '__index' then
			if State.baseState[key] ~= nil then
				Globals[key] = Counter:duplicate(State.baseState[key])
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
		
		
		if trait ~= 'Create' then
			key = stack[i].key
			value = stack[i].value	
		end
		
			
		
		if trait == 'text' then
			if is_painting() then
				log_print(value)
			end	
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
		
		if trait == 'Globals' then
			Globals[id][key] = value
		end
		
		if trait == 'Decks' then
			local deck = Decks[id]
			deck.cards = Counter:duplicate(value)
		end
		
		if trait == 'Create' then
			setCounter(id, stack[i].table)
			dropped("main", stack[i].table.Image.images[1], id, stack[i].table.x, stack[i].table.y)
		end
		
		if trait == 'Delete' then		
			delete_class(id)
		end
		
		
	end	

	
end


function State:undo()

	
	local currentStage = State.stagePointer
			
	local i = State:findLastStage ()
	
	-- check whether a die roll is undone
	for j = i + 1, currentStage do
		if State.stageStack[j].key == 'roll' then
			log_print('ERROR: Can not undo a dieroll')
			State.stagePointer = currentStage
			return
		end 
		if State.stageStack[j].key == 'reveal' then
			log_print('ERROR: Can not undo a revealed unit')
			State.stagePointer = currentStage
			return
		end
	end
	
	
	if #State.stageStack > 0 then	
		toolbar_enable('main', '__redo')
	end
	
	
	
	toggle_painting()
	
	--print(os.clock())

	-- reset counters to base state
	State:reset()
		
	-- execute moves up to last stage
	State:executeUntil(State.stageStack, 1, i)
	
	
	toggle_painting()
	
	
	
	-- print log of redone move
	for j = i + 1, currentStage do
		if State.stageStack[j].trait == 'text' then
			log_print("UNDO: " .. State.stageStack[j].value)
		end 
	end
	
	
	Counter:generateGUI()
	

	if i == 0 then
		toolbar_disable('main', '__undo')
	end
	
end




function State:redo()
	
	if #State.stageStack > 0 then
		toolbar_enable('main', '__undo')
	end
	
	local currentStage = State.stagePointer + 1
	
	local i = State:findNextStage ()
	
	
	
	toggle_painting()
	

	-- reset counters to base state
	State:reset()								

	-- execute moves up to next stage
	State:executeUntil(State.stageStack, 1, i)	
	
	
	toggle_painting()
	
	
	
	-- print log of redone move
	for j = currentStage, i do
		if State.stageStack[j].trait == 'text' then
			log_print("REDO: " .. State.stageStack[j].value)
		end 
	end
		
		
	Counter:generateGUI()
	
	
	if i == #State.stageStack then
		toolbar_disable('main', '__redo')
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

	toolbar_disable('main', '__forward')
	toolbar_disable('main', '__end')
	toolbar_disable('main', '__abort')
	
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
