
-- repository window visiblity

repository_visibility(false)
log_visibility(false)



-- specify the visual appearance of grid (minscale, radius, r, g ,b, a)

specify_grid(0.2, 5, 255, 0, 0, 125 )
show_grid(false)



-- set default offset of stacks (dx, dy, dx-multiplier, dy-multiplier, 0 = unlimited shown)

setDefaultOffset(3.0, 10, 2, 3, 0)





-- create windows



-- card central power hand window

create_window('cphand', 'Central Powers Card Hand', '#999999', true, '', 1000, 272)


local cpzone = Zone:new('cphand', 'cph')

cpzone:addBorder({x = 5, y = 5})
cpzone:addBorder({x = 3061, y = 5})
cpzone:addBorder({x = 3061, y = 536})
cpzone:addBorder({x = 5, y = 536})


local w, h = cpzone:squareGrid(382.0, 531.0)

set_frame('cphand', 200, 400, w + 10, h + 10)
scale_frame('cphand', 0.5)


set_grid({cpzone.grid}, 1, 'cphand', 'cph')
set_borders({cpzone.border}, 1, 'cphand', 'cph')






add_window_image('cphand', 'pos1', cpzone:getGrid(1).x, cpzone:getGrid(1).y, 0, 0, 'cardslot')
add_window_image('cphand', 'pos2', cpzone:getGrid(2).x, cpzone:getGrid(2).y, 0, 0, 'cardslot')
add_window_image('cphand', 'pos3', cpzone:getGrid(3).x, cpzone:getGrid(3).y, 0, 0, 'cardslot')
add_window_image('cphand', 'pos4', cpzone:getGrid(4).x, cpzone:getGrid(4).y, 0, 0, 'cardslot')
add_window_image('cphand', 'pos5', cpzone:getGrid(5).x, cpzone:getGrid(5).y, 0, 0, 'cardslot')
add_window_image('cphand', 'pos6', cpzone:getGrid(6).x, cpzone:getGrid(6).y, 0, 0, 'cardslot')
add_window_image('cphand', 'pos7', cpzone:getGrid(7).x, cpzone:getGrid(7).y, 0, 0, 'cardslot')
add_window_image('cphand', 'pos8', cpzone:getGrid(8).x, cpzone:getGrid(8).y, 0, 0, 'cardslot')



local script = 'local deck = Decks:get("cpdraws") deck:draw("cphand", "cph", 1) ifAction()'
add_window_button('cphand', 'btn1', cpzone:getGrid(1).x, cpzone:getGrid(1).y, 'DrawCentral', script)

local script = 'local deck = Decks:get("cpdraws") deck:draw("cphand", "cph", 2) ifAction()'
add_window_button('cphand', 'btn2', cpzone:getGrid(2).x, cpzone:getGrid(2).y, 'DrawCentral', script)

local script = 'local deck = Decks:get("cpdraws") deck:draw("cphand", "cph", 3) ifAction()'
add_window_button('cphand', 'btn3', cpzone:getGrid(3).x, cpzone:getGrid(3).y, 'DrawCentral', script)

local script = 'local deck = Decks:get("cpdraws") deck:draw("cphand", "cph", 4) ifAction()'
add_window_button('cphand', 'btn4', cpzone:getGrid(4).x, cpzone:getGrid(4).y, 'DrawCentral', script)

local script = 'local deck = Decks:get("cpdraws") deck:draw("cphand", "cph", 5) ifAction()'
add_window_button('cphand', 'btn5', cpzone:getGrid(5).x, cpzone:getGrid(5).y, 'DrawCentral', script)

local script = 'local deck = Decks:get("cpdraws") deck:draw("cphand", "cph", 6) ifAction()'
add_window_button('cphand', 'btn6', cpzone:getGrid(6).x, cpzone:getGrid(6).y, 'DrawCentral', script)

local script = 'local deck = Decks:get("cpdraws") deck:draw("cphand", "cph", 7) ifAction()'
add_window_button('cphand', 'btn7', cpzone:getGrid(7).x, cpzone:getGrid(7).y, 'DrawCentral', script)

local script = 'local deck = Decks:get("cpdraws") deck:draw("cphand", "cph", 8) ifAction()'
add_window_button('cphand', 'btn8', cpzone:getGrid(8).x, cpzone:getGrid(8).y, 'DrawCentral', script)


cpzone:addMembers('^card_cp%w*')
cpzone:addMember('cardback_cp')





-- card allied power hand window

create_window('aphand', 'Allied Powers Card Hand', '#e26363', true, '', 1000, 272)


local apzone = Zone:new('aphand', 'aph')

apzone:addBorder({x = 5, y = 5})
apzone:addBorder({x = 3061, y = 5})
apzone:addBorder({x = 3061, y = 536})
apzone:addBorder({x = 5, y = 536})


local w, h = apzone:squareGrid(382.0, 531.0)

set_frame('aphand', 200, 400, w + 10, h + 10)
scale_frame('aphand', 0.5)


set_grid({apzone.grid}, 1, 'aphand', 'cph')
set_borders({apzone.border}, 1, 'aphand', 'cph')






add_window_image('aphand', 'pos1', apzone:getGrid(1).x, apzone:getGrid(1).y, 0, 0, 'cardslot')
add_window_image('aphand', 'pos2', apzone:getGrid(2).x, apzone:getGrid(2).y, 0, 0, 'cardslot')
add_window_image('aphand', 'pos3', apzone:getGrid(3).x, apzone:getGrid(3).y, 0, 0, 'cardslot')
add_window_image('aphand', 'pos4', apzone:getGrid(4).x, apzone:getGrid(4).y, 0, 0, 'cardslot')
add_window_image('aphand', 'pos5', apzone:getGrid(5).x, apzone:getGrid(5).y, 0, 0, 'cardslot')
add_window_image('aphand', 'pos6', apzone:getGrid(6).x, apzone:getGrid(6).y, 0, 0, 'cardslot')
add_window_image('aphand', 'pos7', apzone:getGrid(7).x, apzone:getGrid(7).y, 0, 0, 'cardslot')
add_window_image('aphand', 'pos8', apzone:getGrid(8).x, apzone:getGrid(8).y, 0, 0, 'cardslot')



local script = 'local deck = Decks:get("apdraws") deck:draw("aphand", "aph", 1) ifAction()'
add_window_button('aphand', 'btn1', apzone:getGrid(1).x, apzone:getGrid(1).y, 'DrawAllied', script)

local script = 'local deck = Decks:get("apdraws") deck:draw("aphand", "aph", 2) ifAction()'
add_window_button('aphand', 'btn2', apzone:getGrid(2).x, apzone:getGrid(2).y, 'DrawAllied', script)

local script = 'local deck = Decks:get("apdraws") deck:draw("aphand", "aph", 3) ifAction()'
add_window_button('aphand', 'btn3', apzone:getGrid(3).x, apzone:getGrid(3).y, 'DrawAllied', script)

local script = 'local deck = Decks:get("apdraws") deck:draw("aphand", "aph", 4) ifAction()'
add_window_button('aphand', 'btn4', apzone:getGrid(4).x, apzone:getGrid(4).y, 'DrawAllied', script)

local script = 'local deck = Decks:get("apdraws") deck:draw("aphand", "aph", 5) ifAction()'
add_window_button('aphand', 'btn5', apzone:getGrid(5).x, apzone:getGrid(5).y, 'DrawAllied', script)

local script = 'local deck = Decks:get("apdraws") deck:draw("aphand", "aph", 6) ifAction()'
add_window_button('aphand', 'btn6', apzone:getGrid(6).x, apzone:getGrid(6).y, 'DrawAllied', script)

local script = 'local deck = Decks:get("apdraws") deck:draw("aphand", "aph", 7) ifAction()'
add_window_button('aphand', 'btn7', apzone:getGrid(7).x, apzone:getGrid(7).y, 'DrawAllied', script)

local script = 'local deck = Decks:get("apdraws") deck:draw("aphand", "aph", 8) ifAction()'
add_window_button('aphand', 'btn8', apzone:getGrid(8).x, apzone:getGrid(8).y, 'DrawAllied', script)


cpzone:addMembers('^card_ap%w*')
cpzone:addMember('cardback_ap')




-- reinforcement windows


create_window('cpreinf', 'Central Powers Reinforments and Card Markers', 'reinforcementsCP2', true, '', 1100, 510)


-- 1554, 714 is the size of the image 'reinforcementsCP2'

set_frame('cpreinf', 200, 400, 1554, 714)
scale_frame('cpreinf', 0.7)



local cprzone = Zone:new('cpreinf', 'cpr')

cprzone:addBorder({x = 2, y = 2})
cprzone:addBorder({x = 1550, y = 2})
cprzone:addBorder({x = 1550, y = 710})
cprzone:addBorder({x = 2, y = 710})



cprzone:addGrid(311, 128, "9'th Army")
cprzone:addGrid(423, 128, "10'th Army")
cprzone:addGrid(738, 128, "11'th Army")
cprzone:addGrid(851, 128, "12'th Army")
cprzone:addGrid(1171, 130, "14'th Army")
cprzone:addGrid(1283, 129, "17/18'th Army")
cprzone:addGrid(442, 86, "10'th Army/Corps")
cprzone:addGrid(757, 86, "11'th Army/Corps")
cprzone:addGrid(870, 86, "12'th Army/Corps")
cprzone:addGrid(1388, 140, "TW1")
cprzone:addGrid(1482, 141, "TW1")
cprzone:addGrid(311, 259, "AH7")
cprzone:addGrid(739, 259, "AH10")
cprzone:addGrid(850, 259, "AH11")
cprzone:addGrid(335, 217, "AHc")
cprzone:addGrid(1173, 262, "TU-YLD")
cprzone:addGrid(1283, 260, "TU-Aol")
cprzone:addGrid(953, 269, "TUc")
cprzone:addGrid(1047, 270, "BUc")
cprzone:addGrid(954, 139, "Polc")


cprzone:addGrid(942, 499, "11'th Army")
cprzone:addGrid(1375, 499, "Bolshevik Revolution")
cprzone:addGrid(737, 396, "Bularian Entry")
cprzone:addGrid(306, 499, "Entrench")
cprzone:addGrid(407, 397, "Falkenhayn")
cprzone:addGrid(1171, 499, "Fall ot the Tsar")
cprzone:addGrid(1273, 499, "Fall ot the Tsar")
cprzone:addGrid(1171, 599, "French Mutiny")
cprzone:addGrid(306, 398, "Guns of August")
cprzone:addGrid(738, 598, "Haig")
cprzone:addGrid(1043, 499, "High Seas Fleet")
cprzone:addGrid(1171, 396, "H-L Take Command")
cprzone:addGrid(1375, 397, "Hoffman")
cprzone:addGrid(1477, 398, "Michael")
cprzone:addGrid(510, 398, "Oberost")
cprzone:addGrid(942, 397, "Place of Execution")
cprzone:addGrid(839, 598, "Polish Restoration")
cprzone:addGrid(1273, 599, "Prince Max")
cprzone:addGrid(407, 499, "Race to the Sea")
cprzone:addGrid(612, 398, "Reichstag Truce")
cprzone:addGrid(942, 598, "STAVKA Timity")
cprzone:addGrid(510, 499, "Sud Army")
cprzone:addGrid(1478, 499, "Treaty of Brest-Litovsk")
cprzone:addGrid(1043, 396, "Tsar Takes Command")
cprzone:addGrid(1273, 398, "U-Boats Unleashed")
cprzone:addGrid(839, 398, "Walter Rathenau")
cprzone:addGrid(738, 499, "War in Africa")
cprzone:addGrid(839, 499, "Zeppelin Raids")
cprzone:addGrid(1171, 499, "Fall ot the Tsar")


set_grid({cprzone.grid}, 1, 'cpreinf', 'cpr')
set_borders({cprzone.border}, 1, 'cpreinf', 'cpr')


cprzone:addMembers('^ge_army%w+')
cprzone:addMembers('^ge_corps%w*')
cprzone:addMembers('^ah_army%w+')
cprzone:addMembers('^ah_corps%w*')
cprzone:addMembers('^bu_corps')
cprzone:addMembers('^pol_corps')
cprzone:addMembers('^sn_corps')
cprzone:addMembers('^yld_army')
cprzone:addMembers('^aoi_army')


cprzone:addMember("ev_guns_of_august")
cprzone:addMember("ev_falkenhayn")
cprzone:addMember("ev_oberost")
cprzone:addMember("vp_reichstag")
cprzone:addMember("ev_entrench")
cprzone:addMember("ev_race_to_the_sea")
cprzone:addMember("sud_army")

cprzone:addMember("ev_bulgaria")
cprzone:addMember("ev_walter_rathenau")
cprzone:addMember("ev_place_of_execution")
cprzone:addMember("ev_tsar_takes_command")
cprzone:addMember("ev_war_in_africa")
cprzone:addMember("ev_zeppelin")
cprzone:addMember("event_11tharmy")
cprzone:addMember("vp_fleet")
cprzone:addMember("ev_haig")
cprzone:addMember("vp_polish")
cprzone:addMember("event_stavka")

cprzone:addMember("ev_hl_take_command")
cprzone:addMember("ev_uboats")
cprzone:addMember("ev_hoffmann")
cprzone:addMember("ev_michael")
cprzone:addMember("vp_fall_of_the_tsar_1")
cprzone:addMember("vp_fall_of_the_tsar_3")
cprzone:addMember("ev_bolshevik")
cprzone:addMember("ev_brest_litovsk")
cprzone:addMember("ev_french_mutiny")
cprzone:addMember("ev_prince_max")





create_window('apreinf', 'Allied Powers Reinforments and Card Markers', 'reinforcementsAP2', true, '', 1100, 550)

set_frame('apreinf', 220, 420, 1553, 773)
scale_frame('apreinf', 0.7)



local aprzone = Zone:new('apreinf', 'apr')

aprzone:addBorder({x = 2, y = 2})
aprzone:addBorder({x = 1550, y = 2})
aprzone:addBorder({x = 1550, y = 770})
aprzone:addBorder({x = 2, y = 770})



aprzone:addGrid(310, 107, "1'th Army")
aprzone:addGrid(420, 107, "2'nd Army")
aprzone:addGrid(738, 107, "3'rd Army")
aprzone:addGrid(847, 107, "4'th Army")
aprzone:addGrid(1171, 107, "5'th Army")
aprzone:addGrid(958, 107, "MEF Army")
aprzone:addGrid(1280, 107, "BRNE Army")

aprzone:addGrid(332, 83, "BRc")
aprzone:addGrid(443, 83, "BRc")
aprzone:addGrid(760, 83, "BRc")
aprzone:addGrid(871, 83, "BRc")
aprzone:addGrid(1192, 83, "BRc")

aprzone:addGrid(1056, 119, "CNDc")
aprzone:addGrid(1381, 118, "ANAc")
aprzone:addGrid(310, 219, "FR10")
aprzone:addGrid(738, 219, "FR7")
aprzone:addGrid(1171, 219, "Orient")
aprzone:addGrid(310, 332, "RU10")
aprzone:addGrid(420, 332, "RU11")


aprzone:addGrid(737, 332, "RU6/7")
aprzone:addGrid(847, 332, "RU12")
aprzone:addGrid(956, 332, "CAU")
aprzone:addGrid(444, 310, "RUc")
aprzone:addGrid(524, 341, "RUc")
aprzone:addGrid(760, 310, "RUc")
aprzone:addGrid(1056, 343, "CzLc")
aprzone:addGrid(614, 343, "RU Cav")
aprzone:addGrid(847, 219, "IT5")
aprzone:addGrid(1171, 332, "US1")
aprzone:addGrid(1281, 332, "US2")
aprzone:addGrid(1195, 310, "USc")
aprzone:addGrid(1302, 310, "USc")
aprzone:addGrid(1383, 343, "USc")
aprzone:addGrid(1469, 343, "USc")
aprzone:addGrid(1056, 230, "ROc")
aprzone:addGrid(800, 500, "GRc")
aprzone:addGrid(956, 219, "MEF Bh")


aprzone:addGrid(1375, 555, "14 Points")
aprzone:addGrid(1171, 555, "Allenby")
aprzone:addGrid(305, 453, "Blockade")
aprzone:addGrid(509, 453, "British Reinforcements 1")
aprzone:addGrid(611, 453, "British Reinforcements 2")
aprzone:addGrid(1171, 453, "Brusilov Offensive")
aprzone:addGrid(1476, 555, "Convoy")
aprzone:addGrid(304, 555, "Entrench")
aprzone:addGrid(1272, 555, "Everyone Into Battle")
aprzone:addGrid(941, 555, "Grand Fleet")
aprzone:addGrid(941, 453, "Great Retreat")

aprzone:addGrid(1477, 453, "Greek Entry")
aprzone:addGrid(1375, 453, "Independent Air Force")
aprzone:addGrid(1476, 657, "Influenza")
aprzone:addGrid(737, 454, "Italian Entry")
aprzone:addGrid(1043, 555, "Landships")
aprzone:addGrid(839, 453, "Lusitania")
aprzone:addGrid(1043, 453, "MEF")
aprzone:addGrid(407, 555, "Moltke")

aprzone:addGrid(1171, 657, "Over There")
aprzone:addGrid(407, 453, "Rape of Belgium")
aprzone:addGrid(737, 555, "Romania")
aprzone:addGrid(839, 555, "Salonika")
aprzone:addGrid(1375, 657, "Sixtus Affair")
aprzone:addGrid(1272, 657, "Sinai Pipeline")
aprzone:addGrid(1272, 453, "Zimmermann Telegram")



set_grid({aprzone.grid}, 1, 'apreinf', 'apr')
set_borders({aprzone.border}, 1, 'apreinf', 'apr')


aprzone:addMembers('^br_army%w+')
aprzone:addMembers('^br_corps%w*')
aprzone:addMember('bef_army')
aprzone:addMember('cnd_corps')
aprzone:addMember('aus_corps')
aprzone:addMember('pt_corps')
aprzone:addMember('ana_corps')
aprzone:addMember('mef_army')

aprzone:addMembers('^fr_army%w+')
aprzone:addMember('orient_army')
aprzone:addMember('it_army05')
aprzone:addMember('ro_corps')
aprzone:addMember('mef_beachhead')

aprzone:addMembers('^ru_army%w+')
aprzone:addMembers('^ru_corps')
aprzone:addMembers('^ru_cav')
aprzone:addMember('cau_army')
aprzone:addMember('czl_corps')

aprzone:addMembers('^us_army%w+')
aprzone:addMembers('^us_corps%w*')


aprzone:addMembers("blockade_event")
aprzone:addMembers("vp_rape")
aprzone:addMembers("ev_british_1")
aprzone:addMembers("ev_british_2")
aprzone:addMembers("ev_entrench_ap")
aprzone:addMembers("ev_moltke")
aprzone:addMembers("ev_italy")
aprzone:addMembers("vp_lusitania")
aprzone:addMembers("ev_great_retreat")
aprzone:addMembers("ev_mef")
aprzone:addMembers("ev_romania")
aprzone:addMembers("ev_salonika")
aprzone:addMembers("vp_grand_fleet")
aprzone:addMembers("ev_landships")
aprzone:addMembers("ev_brusilov")
aprzone:addMembers("vp_zimmermann")
aprzone:addMembers("ev_independent_air_force")
aprzone:addMembers("ev_greece")
aprzone:addMembers("ev_allenby")
aprzone:addMembers("ev_everyone_into_battle")
aprzone:addMembers("vp_14_points")
aprzone:addMembers("vp_convoy")
aprzone:addMembers("ev_overthere")
aprzone:addMembers("sinai_pipeline")
aprzone:addMembers("ev_sixtus_affair")
aprzone:addMembers("event_influenza")




create_window('ctabwin', 'Fire Tables and Terrian Effects', 'CombatTables', true, '', 1133, 827)

set_frame('ctabwin', 200, 400, 1131, 827)




create_window('aids', 'Player Aids', '#ffffff', false, 'Layout', 1100, 800)

set_frame('aids', 70, 70, 1100, 800)

set_root('aids')

root(0)

	tabs(1)

		tab(2, 'War Status')	
			image_item('POG PAC-3 War Status')
			scale_pane(0.6)
		
		tab(2, 'VP Track')	
			image_item('POG PAC-3-VP track')
			scale_pane(0.6)
			
		tab(2, 'Replacements / VP / 1914')	
			image_item('POG PAC-1 TablesBack')
			scale_pane(0.45)	
			
		tab(2, 'Sequence / Peace Terms')	
			image_item('PeaceTermsTable')
			scale_pane(0.9)	
			

			
create_window('pelim', 'Permanently Eliminated', 'permelim', true, '', 1110, 583)

set_frame('pelim', 100, 100, 1391, 583)
scale_frame('pelim', 0.8)
			



-- create custom toolbar


create_toolbar('main', 'ct', 'Custom toolbar', 42)


local callback = "toggle_window('cphand')"
add_toolbar_button('ct', 'tbcphand', 'toolbar_cphand', '', 'Show/Hide window', callback)
toolbar_enable('ct', 'tbcphand')


local callback = "toggle_window('aphand')"
add_toolbar_button('ct', 'tbahand', 'toolbar_aphand', '', 'Show/Hide window', callback)
toolbar_enable('ct', 'tbaphand')


add_toolbar_separator('ct')


local callback = "toggle_window('cpreinf')"
add_toolbar_button('ct', 'cprein', 'toolbar_cpforces', '', 'Show/Hide window', callback)
toolbar_enable('ct', 'cprein')

local callback = "toggle_window('apreinf')"
add_toolbar_button('ct', 'aprein', 'toolbar_apforces', '', 'Show/Hide window', callback)
toolbar_enable('ct', 'aprein')


add_toolbar_separator('ct')


local callback = "toggle_window('ctabwin')"
add_toolbar_button('ct', 'ctables', 'CombatTablesNewIcon2', '', 'Show/Hide window', callback)
toolbar_enable('ct', 'ctables')

local callback = "toggle_window('aids')"
add_toolbar_button('ct', 'caid', 'ChartsIcon1', '', 'Show/Hide window', callback)
toolbar_enable('ct', 'caid')

local callback = "toggle_window('pelim')"
add_toolbar_button('ct', 'elim', 'toolbar_oos4', '', 'Show/Hide window', callback)
toolbar_enable('ct', 'elim')





-- define zones



-- define europe zone for main map

local zone = Zone:new('main', 'pog', 'European Map')


-- border map coordinates

zone:addBorder({x = 100, y = 966})
zone:addBorder({x = 1178, y = 966})
zone:addBorder({x = 1178, y = 601})
zone:addBorder({x = 2300, y = 601})
zone:addBorder({x = 2662, y = 32})
zone:addBorder({x = 3784, y = 32})
zone:addBorder({x = 4106, y = 406})
zone:addBorder({x = 4106, y = 1774})
zone:addBorder({x = 3584, y = 1954})
zone:addBorder({x = 3584, y = 2808})
zone:addBorder({x = 3304, y = 3198})
zone:addBorder({x = 2892, y = 3138})
zone:addBorder({x = 2092, y = 2710})
zone:addBorder({x = 1852, y = 2710})
zone:addBorder({x = 834, y = 2382})
zone:addBorder({x = 834, y = 2188})
zone:addBorder({x = 100, y = 2188})



-- europe locations
zone:addGrid(315, 1077, 'London')
zone:addGrid(544, 1180, 'Calais')
zone:addGrid(671, 1089, 'Ostend')
zone:addGrid(860, 1055, 'Antwerp')
zone:addGrid(1175, 1034, 'Essen')
zone:addGrid(1034, 1065, 'Aachen')
zone:addGrid(1373, 1046, 'Kassel')
zone:addGrid(1321, 866, 'Bremen')
zone:addGrid(1241, 721, 'Wilhelmshaven')
zone:addGrid(1455, 649, 'Kiel')
zone:addGrid(1667, 686, 'Rostock')
zone:addGrid(1944, 721, 'Stettin')
zone:addGrid(2153, 665, 'Kolberg')
zone:addGrid(1791, 913, 'Berlin')
zone:addGrid(1576, 960, 'Hannover')
zone:addGrid(1703, 1102, 'Leipzig')
zone:addGrid(1944, 1011, 'Cottbus')
zone:addGrid(1838, 1134, 'Dresden')
zone:addGrid(1553, 1228, 'Erfurt')
zone:addGrid(1686, 1449, 'Regensburg')
zone:addGrid(1636, 1570, 'Munich')
zone:addGrid(1756, 1685, 'Spittal')
zone:addGrid(1778, 1824, 'Maggiore')
zone:addGrid(1646, 1844, 'Asiago')
zone:addGrid(1792, 1943, 'Udine')
zone:addGrid(1678, 1996, 'Venice')
zone:addGrid(1716, 2183, 'Ravenna')
zone:addGrid(1573, 2109, 'Bologna')
zone:addGrid(1566, 2230, 'Florence')
zone:addGrid(1646, 2364, 'Viterbo')
zone:addGrid(1832, 2308, 'Ancona')
zone:addGrid(1708, 2509, 'Rome')
zone:addGrid(1897, 2448, 'Pescara')
zone:addGrid(1900, 2667, 'Naples')
zone:addGrid(2218, 2716, 'Taranto')
zone:addGrid(2068, 2598, 'Foggia')
zone:addGrid(2290, 801, 'Thorn')
zone:addGrid(2370, 647, 'Danzig')
zone:addGrid(2547, 749, 'Tannenberg')
zone:addGrid(2562, 580, 'Koningsberg')
zone:addGrid(2473, 880, 'Plock')
zone:addGrid(2458, 1031, 'Lodz')
zone:addGrid(2640, 960, 'Warzaw')
zone:addGrid(2759, 818, 'Lomza')
zone:addGrid(2717, 667, 'Insterberg')
zone:addGrid(2859, 562, 'Kovno')
zone:addGrid(2832, 385, 'Szawli')
zone:addGrid(2662, 446, 'Memel')
zone:addGrid(2665, 305, 'Libau')
zone:addGrid(2978, 267, 'Riga')
zone:addGrid(3196, 109, 'Reval')
zone:addGrid(3023, 570, 'Vilna')
zone:addGrid(2936, 714, 'Grodno')
zone:addGrid(3203, 622, 'Moldechno')
zone:addGrid(3246, 429, 'Dvinsk')
zone:addGrid(3474, 321, 'Opochka')
zone:addGrid(3460, 138, 'Pskov')
zone:addGrid(3678, 105, 'Petrograd')
zone:addGrid(3660, 321, 'Velikiye Luki')
zone:addGrid(3439, 542, 'Polotsk')
zone:addGrid(3661, 503, 'Vitebsk')
zone:addGrid(3861, 591, 'Smolensk')
zone:addGrid(3661, 618, 'Orsha')
zone:addGrid(3671, 735, 'Mogilev')
zone:addGrid(4020, 544, 'Moscow')
zone:addGrid(3911, 795, 'Roslavl')
zone:addGrid(3376, 721, 'Minsk')
zone:addGrid(3461, 865, 'Slutsk')
zone:addGrid(3182, 783, 'Baranovichi')
zone:addGrid(3742, 936, 'Gomel')
zone:addGrid(2981, 843, 'Bialystok')
zone:addGrid(2885, 976, 'Brest Litovsk')
zone:addGrid(3134, 917, 'Pinsk')
zone:addGrid(2911, 1138, 'Lublin')
zone:addGrid(3066, 1047, 'Kovel')
zone:addGrid(3283, 1004, 'Sarny')
zone:addGrid(3541, 1050, 'Mozyr')
zone:addGrid(3772, 1090, 'Chrenigov')
zone:addGrid(3685, 1231, 'Kiev')
zone:addGrid(4021, 1229, 'Kharkov')
zone:addGrid(3504, 1223, 'Zhitomir')
zone:addGrid(3345, 1168, 'Rovno')
zone:addGrid(3252, 1304, 'Dubno')
zone:addGrid(3125, 1185, 'Lutsk')
zone:addGrid(2696, 1138, 'Ivangorod')
zone:addGrid(2669, 1282, 'Tarnow')
zone:addGrid(2544, 1158, 'Czestochowa')
zone:addGrid(2507, 1297, 'Cracow')
zone:addGrid(2619, 1411, 'Gorlice')
zone:addGrid(2568, 1542, 'Miskolcz')
zone:addGrid(2371, 1475, 'Martin')
zone:addGrid(2437, 1669, 'Budapest')
zone:addGrid(2341, 1888, 'Pecs')
zone:addGrid(2713, 1669, 'Debrecen')
zone:addGrid(2537, 1824, 'Szeged')
zone:addGrid(2830, 1297, 'Przemysl')
zone:addGrid(2985, 1314, 'Lemberg')
zone:addGrid(2779, 1511, 'Uzhgorod')
zone:addGrid(2950, 1470, 'Stanislaw')
zone:addGrid(3108, 1418, 'Tarnopol')
zone:addGrid(3256, 1474, 'Kamenets-Podolski')
zone:addGrid(3105, 1579, 'Czernowitz')
zone:addGrid(2940, 1613, 'Munkacs')
zone:addGrid(3470, 1419, 'Vinnista')
zone:addGrid(3714, 1409, 'Belaya Tserkov')
zone:addGrid(3716, 1596, 'Uman')
zone:addGrid(4022, 1660, 'Caucasus')
zone:addGrid(3715, 1814, 'Odessa')
zone:addGrid(3542, 1912, 'Ismail')
zone:addGrid(3511, 1742, 'Kishinev')
zone:addGrid(3393, 1593, 'Zhmerinka')
zone:addGrid(3236, 1685, 'Jassy')
zone:addGrid(3275, 1825, 'Barlad')
zone:addGrid(3184, 1979, 'Ploesti')
zone:addGrid(3364, 1996, 'Galatz')
zone:addGrid(3201, 2119, 'Bucharest')
zone:addGrid(3062, 1895, 'Kronstadt')
zone:addGrid(3062, 1765, 'Schossburg')
zone:addGrid(2908, 1763, 'Cluj')
zone:addGrid(2904, 1891, 'Hermannstadt')
zone:addGrid(2802, 2034, 'Targu Jiu')
zone:addGrid(2678, 1935, 'Timisvar')
zone:addGrid(2499, 1983, 'Novi Sad')
zone:addGrid(2621, 2113, 'Belgrade')
zone:addGrid(2544, 2252, 'Valjevo')
zone:addGrid(2361, 2201, 'Sarajevo')
zone:addGrid(2224, 2078, 'Banja Luka')
zone:addGrid(2088, 1922, 'Zagreb')
zone:addGrid(2210, 2297, 'Mostar')
zone:addGrid(2398, 2406, 'Cetinje')
zone:addGrid(2507, 2546, 'Tirana')
zone:addGrid(2497, 2726, 'Valona')
zone:addGrid(2697, 2473, 'Skopje')
zone:addGrid(2691, 2284, 'Nis')
zone:addGrid(2913, 2507, 'Strumitsa')
zone:addGrid(2979, 2648, 'Kavalla')
zone:addGrid(2704, 2612, 'Monastir')
zone:addGrid(2828, 2717, 'Salonika')
zone:addGrid(2702, 2769, 'Florina')
zone:addGrid(2935, 3090, 'Athens')
zone:addGrid(2798, 2873, 'Larisa')
zone:addGrid(3123, 2602, 'Philippoli')
zone:addGrid(2893, 2365, 'Sofia')
zone:addGrid(3155, 2448, 'Kazanlik')
zone:addGrid(3065, 2306, 'Plevna')
zone:addGrid(2988, 2164, 'Caracal')
zone:addGrid(2957, 2026, 'Carte de Arges')
zone:addGrid(3436, 2134, 'Constanta')
zone:addGrid(3388, 2290, 'Varna')
zone:addGrid(3358, 2428, 'Burgas')
zone:addGrid(3366, 2578, 'Adrianople')
zone:addGrid(3230, 2710, 'Gallipoli')
zone:addGrid(3248, 2843, 'Cana Kale')
zone:addGrid(3413, 2866, 'Balikesir')
zone:addGrid(3330, 3019, 'Izmir')
zone:addGrid(3101, 2796, 'MEF1')
zone:addGrid(3119, 2944, 'MEF2')
zone:addGrid(3220, 3148, 'MEF3')
zone:addGrid(3535, 2627, 'Constantinople Virtual Space')
zone:addGrid(3538, 2763, 'Bursa Virtual Space')

zone:addGrid(2190, 945, 'Posen')
zone:addGrid(2198, 1137, 'Breslau')
zone:addGrid(1917, 1277, 'Prague')
zone:addGrid(2047, 1354, 'Kolin')
zone:addGrid(2167, 1427, 'Brun')
zone:addGrid(2302, 1321, 'Olmutz')
zone:addGrid(2357, 1188, 'Oppeln')
zone:addGrid(2125, 1574, 'Vienna')
zone:addGrid(1881, 1574, 'Linz')
zone:addGrid(1906, 1780, 'Villach')
zone:addGrid(2033, 1732, 'Graz')
zone:addGrid(1926, 1950, 'Trieste')
zone:addGrid(2088, 1922, 'Zagreb')
zone:addGrid(2224, 2078, 'Banja Luka')
zone:addGrid(2210, 2297, 'Mostar')

zone:addGrid(1454, 774, 'Hamburg')
zone:addGrid(1271, 1242, 'Frankfurt')
zone:addGrid(1275, 1366, 'Mannheim')
zone:addGrid(1361, 1481, 'Stuttgart')
zone:addGrid(1486, 1505, 'Augsburg')
zone:addGrid(1571, 1687, 'Innsbruck')
zone:addGrid(1227, 1634, 'Mulhouse')
zone:addGrid(1117, 1213, 'Koblenz')
zone:addGrid(1123, 1358, 'Metz')
zone:addGrid(1056, 1528, 'Nancy')
zone:addGrid(1078, 1678, 'Belfort')
zone:addGrid(963, 1192, 'Liege')
zone:addGrid(865, 1306, 'Sedan')
zone:addGrid(973, 1407, 'Verdun')
zone:addGrid(920, 1569, 'Bar le Duc')
zone:addGrid(949, 1757, 'Dijon')
zone:addGrid(797, 1176, 'Brussels')
zone:addGrid(709, 1313, 'Cambrai')
zone:addGrid(626, 1473, 'Paris')
zone:addGrid(482, 1430, 'Rouen')
zone:addGrid(362, 1357, 'Le Havre')
zone:addGrid(578, 1309, 'Amiens')
zone:addGrid(732, 1604, 'Melun')
zone:addGrid(765, 1774, 'Nevers')
zone:addGrid(877, 1940, 'Lyon')
zone:addGrid(834, 2120, 'Avignon')
zone:addGrid(923, 2300, 'Marseilles')
zone:addGrid(1092, 2262, 'Nice')
zone:addGrid(1025, 2007, 'Grenoble')
zone:addGrid(1177, 2025, 'Turin')
zone:addGrid(1345, 1970, 'Milan')
zone:addGrid(1316, 2131, 'Genoa')
zone:addGrid(1528, 1964, 'Verona')
zone:addGrid(1502, 1798, 'Trent')
zone:addGrid(568, 1628, 'Orleans')
zone:addGrid(602, 1801, 'St. Amand')
zone:addGrid(415, 1698, 'Tours')
zone:addGrid(362, 1574, 'Le Mans')
zone:addGrid(248, 1461, 'Caen')
zone:addGrid(155, 1348, 'Cherbourg')
zone:addGrid(168, 1585, 'Rennes')
zone:addGrid(155, 1719, 'Nantes')
zone:addGrid(235, 1871, 'La Rochelle')
zone:addGrid(272, 2048, 'Bordeaux')
zone:addGrid(407, 1847, 'Poitiers')
zone:addGrid(790, 1452, 'Chateau Thierry')
zone:addGrid(1200, 1500, 'Strasbourg')
zone:addGrid(1553, 1375, 'Nuremberg')


zone:addGrid(3080, 2138, 'Romania Neutral')
zone:addGrid(3026, 2356, 'Bulgaria Neutral')
zone:addGrid(2694, 2945, 'Greece Neutral')
zone:addGrid(1446, 2047, 'Italy Neutral')





set_grid({zone.grid}, 1, 'main', 'pog')
set_borders({zone.border}, 1, 'main', 'pog')



zone:addMembers('^ge_army%w+')
zone:addMembers('^ge_corps%w*')
zone:addMembers('^ah_army%w+')
zone:addMembers('^ah_corps%w*')
zone:addMembers('^fr_army%w+')
zone:addMembers('^fr_corps%w*')
zone:addMember('bef_army')
zone:addMembers('^br_army%w+')
zone:addMembers('^br_corps%w*')
zone:addMembers('^bef_corps%w*')
zone:addMembers('^be_army')
zone:addMembers('^be_corps*')

zone:addMembers('^ru_army%w+')
zone:addMembers('^ru_corps')
zone:addMembers('^ru_cav')
zone:addMembers('^sb_army%w+')
zone:addMembers('^sb_corps')	
zone:addMembers('^ro_corps')
zone:addMembers('^bu_corps')
zone:addMembers('^mn_corps')
zone:addMembers('^it_army%w+')
zone:addMembers('^it_corps')
zone:addMembers('^gr_corps')
zone:addMembers('^tu_corps')



zone:addMember('ap_trench1')
zone:addMember('cp_trench1')
zone:addMember('oos_cp')
zone:addMember('oos_ap')
zone:addMember('trench_mod')
zone:addMember('fort_besieged')
zone:addMember('fort_destroyed')
zone:addMember('fort_destroyed_mini')
zone:addMember('control_ap') 
zone:addMember('control_cp')


zone:addMember('neutral')


zone:addMember('rp_ah')
zone:addMember('rp_allied')
zone:addMember('rp_br')
zone:addMember('rp_bu')
zone:addMember('rp_fr')
zone:addMember('rp_ge')
zone:addMember('rp_it')
zone:addMember('rp_ru')
zone:addMember('rp_tu')
zone:addMember('rp_us')			




-- near east

local zonene = Zone:new('main', 'ne', 'Near East Map')


-- border map coordinates

zonene:addBorder({x = 4134, y = 1872})
zonene:addBorder({x = 5052, y = 1872})
zonene:addBorder({x = 5052, y = 3278})
zonene:addBorder({x = 3498, y = 3278})
zonene:addBorder({x = 3498, y = 2984})
zonene:addBorder({x = 3662, y = 2830})
zonene:addBorder({x = 3662, y = 2104})

zonene:addGrid(3710, 2164, 'Constantinople')
zonene:addGrid(3717, 2318, 'Bursa')
zonene:addGrid(3831, 2157, 'Adapazari')
zonene:addGrid(3834, 2303, 'Eskidor')
zonene:addGrid(3948, 2109, 'Sinope')
zonene:addGrid(3954, 2266, 'Ankara')
zonene:addGrid(4058, 2088, 'Samsun')
zonene:addGrid(4112, 2251, 'Sivas')
zonene:addGrid(4158, 2126, 'Giresun')
zonene:addGrid(4274, 2175, 'Trebizond')
zonene:addGrid(4257, 2288, 'Erzingan')
zonene:addGrid(4406, 2154, 'Rize')
zonene:addGrid(4519, 2097, 'Batum')
zonene:addGrid(4622, 2148, 'Kars')
zonene:addGrid(4434, 1927, 'Poti')
zonene:addGrid(4657, 1937, 'Grozny')
zonene:addGrid(4868, 1979, 'Petrovsk')
zonene:addGrid(4745, 2094, 'Tbilisi')
zonene:addGrid(4863, 2180, 'Elizabethpol')
zonene:addGrid(4987, 2264, 'Baku')
zonene:addGrid(4750, 2230, 'Erivan')
zonene:addGrid(4587, 2288, 'Eleskirt')
zonene:addGrid(4458, 2282, 'Erzerum')
zonene:addGrid(4745, 2384, 'Dilman')
zonene:addGrid(4861, 2469, 'Tabriz')
zonene:addGrid(4606, 2405, 'Van')
zonene:addGrid(4490, 2408, 'Bitlis')
zonene:addGrid(4373, 2402, 'Diyarbakir')
zonene:addGrid(4259, 2400, 'Kharput')
zonene:addGrid(4148, 2400, 'Kayseri')
zonene:addGrid(4008, 2420, 'Konya')
zonene:addGrid(4124, 2524, 'Adana')
zonene:addGrid(4250, 2580, 'Aleppo')
zonene:addGrid(4367, 2536, 'Urfa')
zonene:addGrid(4491, 2535, 'Mardin')
zonene:addGrid(4601, 2553, 'Mosul')
zonene:addGrid(4912, 2630, 'Hamadan')
zonene:addGrid(4783, 2706, 'Kermanshah')
zonene:addGrid(4615, 2676, 'Kirkuk')
zonene:addGrid(4263, 2695, 'Damascus')
zonene:addGrid(4145, 2664, 'Beirut')
zonene:addGrid(3893, 2668, 'MEF4')
zonene:addGrid(4095, 2802, 'Nablus')
zonene:addGrid(4218, 2814, 'Amman')
zonene:addGrid(4665, 2809, 'Baghdad')
zonene:addGrid(4768, 2855, 'Kut')
zonene:addGrid(4926, 2776, 'Khorramabad')
zonene:addGrid(4940, 2923, 'Ahwaz')
zonene:addGrid(4825, 2962, 'Qurna')
zonene:addGrid(4616, 2954, 'Samawah')
zonene:addGrid(4908, 3067, 'Basra')
zonene:addGrid(4739, 3114, 'Al Nasiriya')
zonene:addGrid(4379, 3070, 'Arabia')
zonene:addGrid(4223, 3239, 'Median')
zonene:addGrid(4076, 3148, 'Aqaba')
zonene:addGrid(4131, 3044, 'Beersheba')
zonene:addGrid(4159, 2925, 'Jerusalem')
zonene:addGrid(4042, 2956, 'Gaza')
zonene:addGrid(3943, 3042, 'Sinai')
zonene:addGrid(3838, 3100, 'Cairo')
zonene:addGrid(3818, 2985, 'Port Said')
zonene:addGrid(3702, 3033, 'Alexandria')
zonene:addGrid(3557, 3023, 'Libya')


zonene:addGrid(3902, 2539, 'Turkey Neutral')



set_grid({zonene.grid}, 1, 'main', 'ne')
set_borders({zonene.border}, 1, 'main', 'ne')


zonene:addMembers('^br_corps%w*')
zonene:addMembers('^tu_corps')
zonene:addMembers('^ru_corps')
zonene:addMembers('^sn_corps')
zonene:addMembers('^yld_army')
zonene:addMembers('^aoi_army')


zonene:addMember('ap_trench1')
zonene:addMember('cp_trench1')
zonene:addMember('oos_cp')
zonene:addMember('oos_ap')
zonene:addMember('trench_mod')
zonene:addMember('fort_besieged')
zonene:addMember('fort_destroyed')
zonene:addMember('fort_destroyed_mini')
zonene:addMember('control_ap') 
zonene:addMember('control_cp')

zonene:addMember('neutral')




-- cp action round chart zone

local zonecpa = Zone:new('main', 'cpa')


zonecpa:addBorder({x = 1500, y = 20})
zonecpa:addBorder({x = 2071, y = 20})
zonecpa:addBorder({x = 2071, y = 600})
zonecpa:addBorder({x = 1500, y = 600})

zonecpa:setAllowed(false)


zonecpa:addGrid(1593, 116, 'Neutral Entry (CP)')
zonecpa:addGrid(1680, 211, 'AH Reinforcements')
zonecpa:addGrid(1593, 211, 'GE Reinforcements')
zonecpa:addGrid(1767, 211, 'TU Reinforcements')
zonecpa:addGrid(1593, 308, 'SR (CP)')
zonecpa:addGrid(1680, 308, 'RP (CP)')
zonecpa:addGrid(1593, 405, 'OPS (CP)')
zonecpa:addGrid(1680, 405, 'Event (CP)')
zonecpa:addGrid(1767, 405, '1 OPS (CP)')
zonecpa:addGrid(1854, 405, 'Peace Terms (CP)')
zonecpa:addGrid(1593, 570, 'Action_CP_1')
zonecpa:addGrid(1680, 570, 'Action_CP_2')
zonecpa:addGrid(1767, 570, 'Action_CP_3')
zonecpa:addGrid(1854, 570, 'Action_CP_4')
zonecpa:addGrid(1933, 570, 'Action_CP_5')
zonecpa:addGrid(2017, 570, 'Action_CP_6')

set_grid({zonecpa.grid}, 1, 'main', 'cpa')
set_borders({zonecpa.border}, 1, 'main', 'cpa')

zonecpa:addMember('action_cp1')
zonecpa:addMember('action_cp2')
zonecpa:addMember('action_cp3')
zonecpa:addMember('action_cp4')
zonecpa:addMember('action_cp5')
zonecpa:addMember('action_cp6')



-- ap action round chart zone

local zonecaa = Zone:new('main', 'caa')


zonecaa:addBorder({x = 1352, y = 2712})
zonecaa:addBorder({x = 1901, y = 2712})
zonecaa:addBorder({x = 1901, y = 2828})
zonecaa:addBorder({x = 1832, y = 2828})
zonecaa:addBorder({x = 1832, y = 3258})
zonecaa:addBorder({x = 1352, y = 3258})

zonecaa:setAllowed(false)


zonecaa:addGrid(1412, 2901, 'Neutral Entry (AP)')
zonecaa:addGrid(1412, 2998, 'French Reinforcements')
zonecaa:addGrid(1500, 2998, 'British Reinforcements')
zonecaa:addGrid(1586, 2998, 'Russian Reinforcements')
zonecaa:addGrid(1672, 2998, 'Italian Reinforcements')
zonecaa:addGrid(1759, 2998, 'US Reinforcements')
zonecaa:addGrid(1412, 3095, 'SR (AP)')
zonecaa:addGrid(1500, 3095, 'RP (AP)')
zonecaa:addGrid(1412, 3192, 'OPS (AP)')
zonecaa:addGrid(1500, 3192, 'Event (AP)')
zonecaa:addGrid(1586, 3192, '1 OPS (AP)')
zonecaa:addGrid(1672, 3192, 'Peace Terms (AP)')
zonecaa:addGrid(1414, 2745, 'Action_AP_1')
zonecaa:addGrid(1500, 2745, 'Action_AP_2')
zonecaa:addGrid(1586, 2745, 'Action_AP_3')
zonecaa:addGrid(1672, 2745, 'Action_AP_4')
zonecaa:addGrid(1758, 2745, 'Action_AP_5')
zonecaa:addGrid(1838, 2745, 'Action_AP_6')


set_grid({zonecaa.grid}, 1, 'main', 'caa')
set_borders({zonecaa.border}, 1, 'main', 'caa')


zonecaa:addMember('action_ap1')
zonecaa:addMember('action_ap2')
zonecaa:addMember('action_ap3')
zonecaa:addMember('action_ap4')
zonecaa:addMember('action_ap5')
zonecaa:addMember('action_ap6')




-- general records track zone

local grt = Zone:new('main', 'grt')


grt:addBorder({x = 61, y = 2716})
grt:addBorder({x = 1211, y = 2716})
grt:addBorder({x = 1211, y = 3114})
grt:addBorder({x = 1326, y = 3114})
grt:addBorder({x = 1326, y = 3248})
grt:addBorder({x = 61, y = 3248})



grt:setAllowed(false)

grt:squareGrid(115.0, 132.0)

set_grid({grt.grid}, 1, 'main', 'grt')
set_borders({grt.border}, 1, 'main', 'grt')


grt:addMember('warstatus_ap')
grt:addMember('warstatus_cp')
grt:addMember('warstatus_combined')
grt:addMember('bid_ap')
grt:addMember('vp')



-- deck zones



local cpdecks = Zone:new('main', 'cpdecks')


cpdecks:addBorder({x = 2553, y = 3438})
cpdecks:addBorder({x = 5084, y = 3438})
cpdecks:addBorder({x = 5084, y = 3963})
cpdecks:addBorder({x = 2553, y = 3963})



cpdecks:setAllowed(false)

local w, h = cpdecks:squareGrid(423.0, 531.0)

set_grid({cpdecks.grid}, 1, 'main', 'cpdecks')
set_borders({cpdecks.border}, 1, 'main', 'cpdecks')



--add_window_image('main', 'pos1', cpdecks:getGrid(1).x, cpdecks:getGrid(2).y, 0, 0, 'cardslot')
add_window_image('main', 'pos2', cpdecks:getGrid(2).x, cpdecks:getGrid(2).y, 0, 0, 'cardslot')
add_window_image('main', 'pos3', cpdecks:getGrid(3).x, cpdecks:getGrid(3).y, 0, 0, 'cardslot')
add_window_image('main', 'pos4', cpdecks:getGrid(4).x, cpdecks:getGrid(4).y, 0, 0, 'cardslot')
add_window_image('main', 'pos5', cpdecks:getGrid(5).x, cpdecks:getGrid(5).y, 0, 0, 'cardslot')
add_window_image('main', 'pos6', cpdecks:getGrid(6).x, cpdecks:getGrid(5).y, 0, 0, 'cardslot')



cpdecks:setGridName(1, 'CP Combat Cards')
cpdecks:setGridName(2, 'CP Draw')
cpdecks:setGridName(3, 'CP Discard Pile')
cpdecks:setGridName(4, 'CP Limited War Deck')
cpdecks:setGridName(5, 'CP Total War Deck')
cpdecks:setGridName(6, 'CP Removed Cards')


local offset = Offset:new(2, 2, 1, 1, 12)
cpdecks:setGridOffset(1, offset)
cpdecks:setGridOffset(2, offset)
cpdecks:setGridOffset(3, offset)
cpdecks:setGridOffset(4, offset)
cpdecks:setGridOffset(5, offset)
cpdecks:setGridOffset(6, offset)


cpdecks:addMembers('^card_cp%w*')
cpdecks:addMember('cardback_cp')


----


local apdecks = Zone:new('main', 'apdecks')



apdecks:addBorder({x = 13, y = 3438})
apdecks:addBorder({x = 2597, y = 3438})
apdecks:addBorder({x = 2597, y = 3963})
apdecks:addBorder({x = 13, y = 3963})



apdecks:setAllowed(false)

local w, h = apdecks:squareGrid(423.0, 531.0)

set_grid({apdecks.grid}, 1, 'main', 'apdecks')
set_borders({apdecks.border}, 1, 'main', 'apdecks')



add_window_image('main', 'pos7', apdecks:getGrid(1).x, apdecks:getGrid(1).y, 0, 0, 'cardslot')
add_window_image('main', 'pos8', apdecks:getGrid(2).x, apdecks:getGrid(2).y, 0, 0, 'cardslot')
add_window_image('main', 'pos9', apdecks:getGrid(3).x, apdecks:getGrid(3).y, 0, 0, 'cardslot')
add_window_image('main', 'pos10', apdecks:getGrid(4).x, apdecks:getGrid(4).y, 0, 0, 'cardslot')
add_window_image('main', 'pos11', apdecks:getGrid(5).x, apdecks:getGrid(5).y, 0, 0, 'cardslot')
--add_window_image('main', 'pos12', apdecks:getGrid(6).x, apdecks:getGrid(6).y, 0, 0, 'cardslot')



apdecks:setGridName(1, 'AP Draw')
apdecks:setGridName(2, 'AP Discard Pile')
apdecks:setGridName(3, 'AP Limited War Deck')
apdecks:setGridName(4, 'AP Total War Deck')
apdecks:setGridName(5, 'AP Removed Cards')
apdecks:setGridName(6, 'AP Combat Cards')



apdecks:setGridOffset(1, offset)
apdecks:setGridOffset(2, offset)
apdecks:setGridOffset(3, offset)
apdecks:setGridOffset(4, offset)
apdecks:setGridOffset(5, offset)
apdecks:setGridOffset(6, offset)


apdecks:addMembers('^card_ap%w*')
apdecks:addMember('cardback_ap')









-- deckcaptions zone


local deckcaptions = Zone:new('main', 'deckcaptions') 

deckcaptions:addBorder({x = 13, y = 3983})
deckcaptions:addBorder({x = 5087, y = 3983})
deckcaptions:addBorder({x = 5087, y = 4023})
deckcaptions:addBorder({x = 13, y = 4023})

deckcaptions:setAllowed(false)

local w, h = deckcaptions:squareGrid(423.0, 40.0)

set_grid({deckcaptions.grid}, 1, 'main', 'deckcaptions')
set_borders({deckcaptions.border}, 1, 'main', 'deckcaptions')


add_window_image('main', 'pos13', deckcaptions:getGrid(1).x, deckcaptions:getGrid(1).y, 375, 40, '')
add_window_image('main', 'pos14', deckcaptions:getGrid(2).x, deckcaptions:getGrid(2).y, 375, 40, '')
add_window_image('main', 'pos15', deckcaptions:getGrid(3).x, deckcaptions:getGrid(3).y, 375, 40, '')
add_window_image('main', 'pos16', deckcaptions:getGrid(4).x, deckcaptions:getGrid(4).y, 375, 40, '')
add_window_image('main', 'pos17', deckcaptions:getGrid(5).x, deckcaptions:getGrid(5).y, 375, 40, '')
add_window_image('main', 'pos18', deckcaptions:getGrid(6).x, deckcaptions:getGrid(6).y, 375, 40, '')
add_window_image('main', 'pos19', deckcaptions:getGrid(7).x, deckcaptions:getGrid(7).y, 375, 40, '')
add_window_image('main', 'pos20', deckcaptions:getGrid(8).x, deckcaptions:getGrid(8).y, 375, 40, '')
add_window_image('main', 'pos21', deckcaptions:getGrid(9).x, deckcaptions:getGrid(9).y, 375, 40, '')
add_window_image('main', 'pos22', deckcaptions:getGrid(10).x, deckcaptions:getGrid(10).y, 375, 40, '')
add_window_image('main', 'pos23', deckcaptions:getGrid(11).x, deckcaptions:getGrid(11).y, 375, 40, '')
add_window_image('main', 'pos24', deckcaptions:getGrid(12).x, deckcaptions:getGrid(12).y, 375, 40, '')


set_font_options('main', 'Serif', 28, 'Bold', '#cc0000')

set_label_text('main', 'pos13', 'Draw')
set_label_text('main', 'pos14', 'Discard Pile')
set_label_text('main', 'pos15', 'Limited War Deck')
set_label_text('main', 'pos16', 'Total War Deck')
set_label_text('main', 'pos17', 'Removed Cards')
set_label_text('main', 'pos18', 'AP Combat Cards')


set_font_options('main', 'Serif', 28, 'Bold', '#333333')

set_label_text('main', 'pos19', 'CP Combat Cards')
set_label_text('main', 'pos20', 'Draw')
set_label_text('main', 'pos21', 'Discard Pile')
set_label_text('main', 'pos22', 'Limited War Deck')
set_label_text('main', 'pos23', 'Total War Deck')
set_label_text('main', 'pos24', 'Removed Cards')





-- central powers mandated offensive table zone


local cpoffensive = Zone:new('main', 'cpoffensive') 

cpoffensive:addBorder({x = 2027, y = 85})
cpoffensive:addBorder({x = 2511, y = 85})
cpoffensive:addBorder({x = 2511, y = 180})
cpoffensive:addBorder({x = 2027, y = 180})

cpoffensive:setAllowed(false)

cpoffensive:addGrid(2121, 133, 'AH)')
cpoffensive:addGrid(2207, 133, 'AH (IT)')
cpoffensive:addGrid(2294, 133, 'TU')
cpoffensive:addGrid(2379, 133, 'GE')
cpoffensive:addGrid(2465, 133, 'None or Made (CP)')

set_grid({cpoffensive.grid}, 1, 'main', 'cpoffensive')
set_borders({cpoffensive.border}, 1, 'main', 'cpoffensive')


cpoffensive:addMember('mandoffensive_cp')



-- allied powers mandated offensive table zone


local apoffensive = Zone:new('main', 'apoffensive') 

apoffensive:addBorder({x = 2356, y = 3109})
apoffensive:addBorder({x = 2787, y = 3109})
apoffensive:addBorder({x = 2787, y = 3204})
apoffensive:addBorder({x = 2356, y = 3204})


apoffensive:setAllowed(false)

apoffensive:addGrid(2398, 3156, 'FR')
apoffensive:addGrid(2485, 3156, 'BR')
apoffensive:addGrid(2571, 3156, 'IT')
apoffensive:addGrid(2657, 3156, 'RU')
apoffensive:addGrid(2743, 3156, 'None or Made (AP)')

set_grid({apoffensive.grid}, 1, 'main', 'apoffensive')
set_borders({apoffensive.border}, 1, 'main', 'apoffensive')


apoffensive:addMember('mandoffensive_ap')



-- turn track zone


local turntrack = Zone:new('main', 'turntrack') 

turntrack:addBorder({x = 4132, y = 108})
turntrack:addBorder({x = 4997, y = 108})
turntrack:addBorder({x = 4997, y = 565})
turntrack:addBorder({x = 4132, y = 565})


turntrack:setAllowed(false)

turntrack:addGrid(4218, 175, 'Turn 1')
turntrack:addGrid(4389, 175, 'Turn 2')
turntrack:addGrid(4561, 175, 'Turn 3')
turntrack:addGrid(4735, 175, 'Turn 4')
turntrack:addGrid(4908, 175, 'Turn 5')
turntrack:addGrid(4218, 288, 'Turn 6')
turntrack:addGrid(4389, 288, 'Turn 7')
turntrack:addGrid(4561, 288, 'Turn 8')
turntrack:addGrid(4735, 288, 'Turn 9')
turntrack:addGrid(4908, 288, 'Turn 10')
turntrack:addGrid(4218, 403, 'Turn 11')
turntrack:addGrid(4389, 403, 'Turn 12')
turntrack:addGrid(4561, 403, 'Turn 13')
turntrack:addGrid(4735, 403, 'Turn 14')
turntrack:addGrid(4908, 403, 'Turn 15')
turntrack:addGrid(4218, 520, 'Turn 16')
turntrack:addGrid(4389, 520, 'Turn 17')
turntrack:addGrid(4561, 520, 'Turn 18')
turntrack:addGrid(4735, 520, 'Turn 19')
turntrack:addGrid(4908, 520, 'Turn 20')


set_grid({turntrack.grid}, 1, 'main', 'turntrack')
set_borders({turntrack.border}, 1, 'main', 'turntrack')


turntrack:addMember('turn')
turntrack:addMember('blockade_event')




-- us commitment track zone


local ustrack = Zone:new('main', 'ustrack') 

ustrack:addBorder({x = 63, y = 2320})
ustrack:addBorder({x = 523, y = 2320})
ustrack:addBorder({x = 523, y = 2457})
ustrack:addBorder({x = 63, y = 2457})


ustrack:setAllowed(false)

ustrack:addGrid(115, 2405, 'US Neutral')
ustrack:addGrid(230, 2405, 'Zimmermann Telegram Allowed')
ustrack:addGrid(345, 2405, 'Zimmermann Telegram')
ustrack:addGrid(460, 2405, 'Over There')



set_grid({ustrack.grid}, 1, 'main', 'ustrack')
set_borders({ustrack.border}, 1, 'main', 'ustrack')


ustrack:addMember('us_entry')




-- russian capitulation track zone


local russian = Zone:new('main', 'russian') 

russian:addBorder({x = 63, y = 2518})
russian:addBorder({x = 982, y = 2518})
russian:addBorder({x = 982, y = 2652})
russian:addBorder({x = 63, y = 2652})


russian:setAllowed(false)

russian:addGrid(115, 2605, 'God Save the Tsar')
russian:addGrid(230, 2605, 'Tsar Takes Command Allowed')
russian:addGrid(345, 2605, 'Tsar Takes Command')
russian:addGrid(460, 2605, 'Fall of the Tsar Allowed')
russian:addGrid(575, 2605, 'Fall of the Tsar')
russian:addGrid(690, 2605, 'Bolshevik Revolution Allowed')
russian:addGrid(805, 2605, 'Bolshevik Revolution')
russian:addGrid(920, 2605, 'Treaty of Brest Litovsk')



set_grid({russian.grid}, 1, 'main', 'russian')
set_borders({russian.border}, 1, 'main', 'russian')


russian:addMember('russian_capit')



-- central powers reserve box zone


local creserve = Zone:new('main', 'crb', 'Central Powers Reserve Box') 

creserve:addBorder({x = 4240, y = 675})
creserve:addBorder({x = 4994, y = 675})
creserve:addBorder({x = 4994, y = 1056})
creserve:addBorder({x = 4240, y = 1056})

set_grid({creserve.grid}, 1, 'main', 'crb')
set_borders({creserve.border}, 1, 'main', 'crb')



-- allied reserve box zone
		 

local areserve = Zone:new('main', 'arb', 'Allied Reserve Box') 

areserve:addBorder({x = 4240, y = 1139})
areserve:addBorder({x = 4994, y = 1139})
areserve:addBorder({x = 4994, y = 1526})
areserve:addBorder({x = 4240, y = 1526})

set_grid({areserve.grid}, 1, 'main', 'arb')
set_borders({areserve.border}, 1, 'main', 'arb')





-- create all decks


-- show functions
local fdown = function(window, id) return Counter[id].Image.imageIndex == 2 end
local fup = function(window, id) return Counter[id].Image.imageIndex == 1 end

createDeck('cpdraws', 
		  {Move = Move:new('Move', 'main', 'cpdecks', 'CP Discard Pile', 'Discard', fup)},	
		  {Draw = Draw:new(fdown)},			  
		  cpdecks:getGrid('CP Draw'))

createDeck('cpdiscard',		 
		  {MoveStack = MoveStack:new('cpdiscard', cpdecks:getGrid('CP Draw'), 'Reshuffle')},
		  {}, 
		  cpdecks:getGrid('CP Discard Pile'))
		 	 
createDeck('cplimited',		 
		  {MoveStack = MoveStack:new('cplimited', cpdecks:getGrid('CP Draw'), 'Move to draw pile')},
		  {}, 
		  cpdecks:getGrid('CP Limited War Deck'))
		 
createDeck('cptotal',		 
		  {MoveStack = MoveStack:new('cptotal', cpdecks:getGrid('CP Draw'), 'Move to draw pile')},
		  {}, 
		  cpdecks:getGrid('CP Total War Deck'))		 		 




createDeck('apdraws', 
		  {Move = Move:new('Move', 'main', 'apdecks', 'AP Discard Pile', 'Discard', fup)},	
		  {Draw = Draw:new(fdown)}, 
		  --{},		  
		  apdecks:getGrid('AP Draw'))


createDeck('apdiscard',		 
		  {MoveStack = MoveStack:new('apdiscard', apdecks:getGrid('AP Draw'), 'Reshuffle')},
		  {}, 
		  apdecks:getGrid('AP Discard Pile'))
		 
createDeck('aplimited',		 
		  {MoveStack = MoveStack:new('aplimited', apdecks:getGrid('AP Draw'), 'Move to draw pile')},
		  {}, 
		  apdecks:getGrid('AP Limited War Deck'))
		 
createDeck('aptotal',		 
		  {MoveStack = MoveStack:new('aptotal', apdecks:getGrid('AP Draw'), 'Move to draw pile')},
		  {}, 
		  apdecks:getGrid('AP Total War Deck'))		








-- loads the start position (only called after all windows and zones defined)

load_setup('setup.gsav')






-- create cards (code only for setup; deleted after saved setup)

		 


--[[

createCard('main', 'cpdiscard', 'card_cp01', 1)

createCard('main', 'cpdraws', 'card_cp02', 2)

createCard('main', 'cpdraws', 'card_cp03', 2)
createCard('main', 'cpdraws', 'card_cp04', 2)
createCard('main', 'cpdraws', 'card_cp05', 2)
createCard('main', 'cpdraws', 'card_cp06', 2)
createCard('main', 'cpdraws', 'card_cp07', 2)
createCard('main', 'cpdraws', 'card_cp08', 2)
createCard('main', 'cpdraws', 'card_cp09', 2)
createCard('main', 'cpdraws', 'card_cp10', 2)
createCard('main', 'cpdraws', 'card_cp11', 2)
createCard('main', 'cpdraws', 'card_cp12', 2)
createCard('main', 'cpdraws', 'card_cp13', 2)
createCard('main', 'cpdraws', 'card_cp14', 2)

createCard('main', 'cplimited', 'card_cp15', 2)
createCard('main', 'cplimited', 'card_cp16', 2)
createCard('main', 'cplimited', 'card_cp17', 2)
createCard('main', 'cplimited', 'card_cp18', 2)
createCard('main', 'cplimited', 'card_cp19', 2)
createCard('main', 'cplimited', 'card_cp20', 2)
createCard('main', 'cplimited', 'card_cp21', 2)
createCard('main', 'cplimited', 'card_cp22', 2)
createCard('main', 'cplimited', 'card_cp23', 2)
createCard('main', 'cplimited', 'card_cp24', 2)
createCard('main', 'cplimited', 'card_cp25', 2)
createCard('main', 'cplimited', 'card_cp26', 2)
createCard('main', 'cplimited', 'card_cp27', 2)
createCard('main', 'cplimited', 'card_cp28', 2)
createCard('main', 'cplimited', 'card_cp29', 2)
createCard('main', 'cplimited', 'card_cp30', 2)
createCard('main', 'cplimited', 'card_cp31', 2)
createCard('main', 'cplimited', 'card_cp32', 2)
createCard('main', 'cplimited', 'card_cp33', 2)
createCard('main', 'cplimited', 'card_cp34', 2)

createCard('main', 'cptotal', 'card_cp35', 2)
createCard('main', 'cptotal', 'card_cp36', 2)
createCard('main', 'cptotal', 'card_cp37', 2)
createCard('main', 'cptotal', 'card_cp38', 2)
createCard('main', 'cptotal', 'card_cp39', 2)
createCard('main', 'cptotal', 'card_cp40', 2)
createCard('main', 'cptotal', 'card_cp41', 2)
createCard('main', 'cptotal', 'card_cp42', 2)
createCard('main', 'cptotal', 'card_cp43', 2)
createCard('main', 'cptotal', 'card_cp44', 2)
createCard('main', 'cptotal', 'card_cp45', 2)
createCard('main', 'cptotal', 'card_cp46', 2)
createCard('main', 'cptotal', 'card_cp47', 2)
createCard('main', 'cptotal', 'card_cp48', 2)
createCard('main', 'cptotal', 'card_cp49', 2)
createCard('main', 'cptotal', 'card_cp50', 2)
createCard('main', 'cptotal', 'card_cp51', 2)
createCard('main', 'cptotal', 'card_cp52', 2)
createCard('main', 'cptotal', 'card_cp53', 2)
createCard('main', 'cptotal', 'card_cp54', 2)
createCard('main', 'cptotal', 'card_cp55', 2)





createCard('main', 'apdraws', 'card_ap01', 2)
createCard('main', 'apdraws', 'card_ap02', 2)
createCard('main', 'apdraws', 'card_ap03', 2)
createCard('main', 'apdraws', 'card_ap04', 2)
createCard('main', 'apdraws', 'card_ap05', 2)
createCard('main', 'apdraws', 'card_ap06', 2)
createCard('main', 'apdraws', 'card_ap07', 2)
createCard('main', 'apdraws', 'card_ap08', 2)
createCard('main', 'apdraws', 'card_ap09', 2)
createCard('main', 'apdraws', 'card_ap10', 2)
createCard('main', 'apdraws', 'card_ap11', 2)
createCard('main', 'apdraws', 'card_ap12', 2)
createCard('main', 'apdraws', 'card_ap13', 2)
createCard('main', 'apdraws', 'card_ap14', 2)

createCard('main', 'aplimited', 'card_ap15', 2)
createCard('main', 'aplimited', 'card_ap16', 2)
createCard('main', 'aplimited', 'card_ap17', 2)
createCard('main', 'aplimited', 'card_ap18', 2)
createCard('main', 'aplimited', 'card_ap19', 2)
createCard('main', 'aplimited', 'card_ap20', 2)
createCard('main', 'aplimited', 'card_ap21', 2)
createCard('main', 'aplimited', 'card_ap22', 2)
createCard('main', 'aplimited', 'card_ap23', 2)
createCard('main', 'aplimited', 'card_ap24', 2)
createCard('main', 'aplimited', 'card_ap25', 2)
createCard('main', 'aplimited', 'card_ap26', 2)
createCard('main', 'aplimited', 'card_ap27', 2)
createCard('main', 'aplimited', 'card_ap28', 2)
createCard('main', 'aplimited', 'card_ap29', 2)
createCard('main', 'aplimited', 'card_ap30', 2)
createCard('main', 'aplimited', 'card_ap31', 2)
createCard('main', 'aplimited', 'card_ap32', 2)
createCard('main', 'aplimited', 'card_ap33', 2)
createCard('main', 'aplimited', 'card_ap34', 2)

createCard('main', 'aptotal', 'card_ap35', 2)
createCard('main', 'aptotal', 'card_ap36', 2)
createCard('main', 'aptotal', 'card_ap37', 2)
createCard('main', 'aptotal', 'card_ap38', 2)
createCard('main', 'aptotal', 'card_ap39', 2)
createCard('main', 'aptotal', 'card_ap40', 2)
createCard('main', 'aptotal', 'card_ap41', 2)
createCard('main', 'aptotal', 'card_ap42', 2)
createCard('main', 'aptotal', 'card_ap43', 2)
createCard('main', 'aptotal', 'card_ap44', 2)
createCard('main', 'aptotal', 'card_ap45', 2)
createCard('main', 'aptotal', 'card_ap46', 2)
createCard('main', 'aptotal', 'card_ap47', 2)
createCard('main', 'aptotal', 'card_ap48', 2)
createCard('main', 'aptotal', 'card_ap49', 2)
createCard('main', 'aptotal', 'card_ap50', 2)
createCard('main', 'aptotal', 'card_ap51', 2)
createCard('main', 'aptotal', 'card_ap52', 2)
createCard('main', 'aptotal', 'card_ap53', 2)
createCard('main', 'aptotal', 'card_ap54', 2)
createCard('main', 'aptotal', 'card_ap55', 2)


--]]
	
