-- define the error texts
-- error arguments are coded with _1_ , _2_ and so on


local errors = {}



errors[1] = 'No counters found for pattern _1_.'
errors[2] = 'create_class() First parameter not a table.'
errors[3] = 'create_class() Second parameter not a valid id.'
errors[4] = 'Trait _1_ not found.'
errors[5] = 'Zone _1_ not found.'
errors[6] = 'Grid point _1_ not found.'
errors[7] = 'Resource _1_ not found.'
errors[8] = 'Window _1_ not found.'
errors[9] = 'Font _1_ not found.'

errors[10] = 'Font-weight _1_ not found.'
errors[11] = 'Color _1_ not valid.'
errors[12] = 'Grid point _1_ does not exist. _2_ could not be set.'
errors[13] = 'Deck _1_ not found.'
errors[14] = 'Deck could not be identified.'
errors[15] = 'Deck is nil.'
errors[16] = 'Tag _1_ not found.'
errors[17] = 'Id _1_ not found in deck.'
errors[18] = 'Window widget _1_ not found.'
errors[19] = 'Duplicate zorder _1_ found in counter _2_.'
errors[20] = 'Duplicate id _1_ found for counter _2_.'
errors[21] = 'No deck found at _1_,_2_.'
errors[22] = 'No port number.'
errors[23] = 'The port is not a number.'
errors[24] = 'The port number is outside legal range.'
errors[25] = 'IP address _1_ is not valid.'
errors[26] = 'Server is already listening.'
errors[27] = 'Client is already connected.'
errors[28] = 'There is no connection.'
errors[29] = 'Invalid argument to get_size.'
errors[30] = 'Resource _1_ not defined in counters.'
errors[31] = 'No table with id _1_.'
errors[32] = 'Coordinates _1_, _2_ not found in zone _3_.'
errors[33] = 'Id _1_ not found.'
errors[34] = 'Could not get grid name for _1_,_2_.'
errors[35] = 'Connection timed out after _1_ milliseconds.'
errors[36] = 'Zone _1_ already exists.'
errors[37] = 'Vertical alignment _1_ not found.'
errors[38] = 'Horizontal alignment _1_ not found.'
errors[39] = 'Id _1_ not found in window _2_.'
errors[40] = 'Choice page _1_ not found.'
errors[41] = 'Choice page _1_ defined more than once.'
errors[42] = 'Failed to open file _1_.'
errors[43] = 'Key not safe.'
errors[44] = 'Prime key generation failed.'
errors[45] = 'No user key exists.'





function error(no, ...)

	local message = errors[no]

	if message == nil then
		print('wrong error number')
		return
	end
	
	local args = {...}
	
	if #args > 0 then
	
		local i = 1
		local eos = false
	
		repeat	
			
			local arg = string.gsub(args[i], '%%', '%%%%')
				
			s = string.gsub(message, '_' .. i .. '_', arg, 1)
			
			if s ~= message then
				if i < #args then
					i = i + 1
				end
				message = s
			else
				eos = true
			end
			
				
		until eos
	
	else
	
		-- case when arg is nil
		message = message:gsub('_%d_', '')
		
	end
	
	print('Error ' .. no .. '  ' .. message)
	
end





return Report
