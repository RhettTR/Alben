-- a counter is a table of traits
-- the order of traits is as they appear in the table with the lowest trait first

Counter = {}


function Counter:new (images)
	local o = Traits:new(images)
	setmetatable(o, Counter)
	Counter.__index = Counter
	o.imageIndex = 1
	table.insert(Counter, o)
	create_class(images[1])
	return o
end


function Counter:delete (id)	
	Counter[id] = nil	
end


function Counter:nextImage (id)
	Counter[id].imageIndex = Counter[id].imageIndex + 1
	local length = #(Counter[id].Image.images)
	if Counter[id].imageIndex > length then
		Counter[id].imageIndex = 1
	end
	return Counter[id].Image.images[Counter[id].imageIndex]
end



---function Counter:addTrait (name)

---end


--function Counter:addTraitAt ()
	
--end


--function Counter:removeTrait ()
	
--end


--function Counter:moveTraitUp ()
	
--end


---function Counter:moveTraitDown ()
	
---end


return Counter
