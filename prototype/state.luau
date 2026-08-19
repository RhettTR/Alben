-- State holds the base state and state transition stack
-- State implements UNDO and REDO

State = {

baseState = {},
stageStack = {},
stagePointer = 0,
-- used for saving logfile
savedPointer = 0

}



function State:addMove (id, trait, key, value, subkey)
	State.stagePointer = State.stagePointer + 1
	State.stageStack[State.stagePointer] = { id = id, trait = trait, key = key, value = value, subkey = subkey }
end

function State:addCreate (id, trait)
	State.stagePointer = State.stagePointer + 1
	State.stageStack[State.stagePointer] = { id = id, trait = trait, table = Counter:load(Counter[id]) }
end

function State:addText (key, text)
	State.stagePointer = State.stagePointer + 1
	State.stageStack[State.stagePointer] = { id = '', trait = 'text', key = key, value = text }
end

function State:addStage ()
	State.stagePointer = State.stagePointer + 1
	State.stageStack[State.stagePointer] = { id = "end" }
	toolbar_enable('main', '__undo')
	if net_connected() then
		State:sendStage()
	end
end




function State:sendStage ()


	local currentStage = State.stagePointer
		
	local i = State:findLastStage()
	
	
	stagesync(i + 1, currentStage)
	 
end



local next
local currentStage

function stagesync(start, current)

	if net_connected() then
	
		next = start + 1
		currentStage = current	
		
		if start > current then
			State.stagePointer = currentStage
			local table = { trait = "stagesyncend" }	
			send_table({table}, 1, 1)			
			return
		end
		
		-- 3 is stage sync
		
		send_table(State.stageStack, start, 3)
									
	end

end


function nextstagesync()

	if net_connected() then
		
		if next > currentStage then
			State.stagePointer = currentStage
			local table = { trait = "stagesyncend" }	
			send_table({table}, 1, 1)			
			return
		end
		
		
		send_table(State.stageStack, next, 3)
			
		next = next + 1
	
	end

end



function stagedone()

	if net_connected() then
	
		local table = { trait = "stagedone" }	
		send_table({table}, 1, 1)

	end
	
end





function State:recieveTable (table)

	if table.trait == 'text' then
		State:addText(table.key, table.value)
		return
	end
	

	
	
	if table.id == 'end' then
	
		State.stagePointer = State.stagePointer + 1
		State.stageStack[State.stagePointer] = {id = "end"}
		
		local currentStage = State.stagePointer
			
		local i = State:findLastStage()
	
		-- execute moves of recieved stage
		State:executeUntil(State.stageStack, i + 1, currentStage)
		
		State.stagePointer = currentStage
		
		
		Counter:generateGUI()
		
		
		toolbar_enable('main', '__undo')
		
		

		
	elseif table.trait == 'Counter' then
		State:addMove (table.id, table.trait, table.key, table.value)
	

	
	elseif table.trait == 'Rotate' then		
		State:addMove (table.id, table.trait, table.key, table.value)

	
	elseif table.trait == 'MarkMoved' then
		State:addMove (table.id, table.trait, table.key, table.value)

	
	elseif table.trait == 'Overlays' then	
		Counter[id].Overlays[Counter[id][key].id][subkey] = value

	
	elseif table.trait == 'Image' then
		State:addMove (table.id, table.trait, table.key, table.value)

	
	elseif table.trait == 'Globals' then
		State:addMove (table.id, table.trait, table.key, table.value)

	
	elseif table.trait == 'Create' then
		State.stagePointer = State.stagePointer + 1	
		State.stageStack[State.stagePointer] = { id = table.id, trait = table.trait, table = Counter:duplicate(table.table) }

	
	elseif table.trait == 'Delete' then		
		State:addMove (table.id, table.trait)

	
	
	elseif table.trait == 'deletestages' then
		State:deleteNextStages()

	
	elseif table.trait == 'updateid' then
		inc_id()

	
	elseif table.trait == 'updatetopzorder' then
		inc_top_zorder()

	
	elseif table.trait == 'updatebottomzorder' then
		inc_bottom_zorder()

	
	elseif table.trait == 'undo' then
		State:undo()

	
	elseif table.trait == 'redo' then
		State:redo()

	
	elseif table.trait == 'confirm' then
		local answer = ask_sync()
		if answer then
			State:resetState()
			toggle_painting()
			local table = { trait = "sync" }
			send_table({table}, 1, 1)
		end

	
	elseif table.trait == 'sync' then
		State:resetState()
		sync()
	
	
	elseif table.trait == 'stagesync' then
		stagesync()	

	
	elseif table.trait == 'done' then
		nextsync()
		
	
	elseif table.trait == 'stagedone' then
		nextstagesync()	

	
	elseif table.trait == 'syncend' then
		reset_idz()
		toggle_painting()
		Counter:generateGUI()
		
	
	elseif table.trait == 'stagesyncend' then
		Counter:generateGUI()
			

	end
	
	
	 
	
end



function State:findLastStage ()
	
	for i = State.stagePointer - 1, 1, -1 do
		local value = State.stageStack[i]
		if  value.id == 'end' then
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

end


function State:resetBase()
	
	State.baseState = nil
	State.baseState = {}
	
end




function State:reset()	
	
	-- reset map to state before stack was made 
	
	for key, value in pairs(Counter) do		
		if type(key) == "string" and type(value) ~= 'function' and key ~= '__index' and value.deleted == false then
			delete_class(key)
		end	
	end
	
	for key, value in pairs(State.baseState) do
		setCounter(key, State.baseState[key])
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
		local subkey

	
		
		if trait ~= 'Create' then
			key = stack[i].key
			value = stack[i].value
			subkey = stack[i].subkey	
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
		
		if trait == 'Overlays' then	
			Counter[id].Overlays[Counter[id][key].id][subkey] = value
		end
		
		if trait == 'Image' then
			Counter[id][trait][key] = value
		end
		
		if trait == 'Globals' then
			Globals[id][key] = value
		end
		
		if trait == 'Card' then
			Counter[id][trait][key] = value
		end
		
		if trait == 'Create' then
			setCounter(id, stack[i].table)
			-- hook
			dropped(stack[i].table.window, stack[i].table.Image.images[1], id, stack[i].table.x, stack[i].table.y)
		end
		
		if trait == 'Delete' then		
			delete_class(id)
		end
		
		
	end	

	
end


function State:undo()
	
	
	local currentStage = State.stagePointer
			
	local i = State:findLastStage ()
	

	
	
	-- check whether a die roll/reveal is undone
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
		if State.stageStack[j].key == 'draw' then
			log_print('ERROR: Can not undo a drawn card')
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
	State:resetBase()
	
	for id, v in pairs(Counter) do
		if type(id) == "string" and id ~= '__index' and type(v) ~= 'function' then
			State.baseState[id] = Counter:duplicate(v)
		end		
	end
	
end



function undo()

	State:undo()
	
	-- net
	local table = {id = '', trait = 'undo'}
	send_table({table}, 1, 1)
	
	-- hook
	undohook()
	
end

function redo()

	State:redo()
	
	-- net
	local table = {id = '', trait = 'redo'}
	send_table({table}, 1, 1)
	
end


return State
