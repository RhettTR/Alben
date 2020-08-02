package.cpath = "./?.so"

local QtCore = require 'qtcore'
local QtGui = require 'qtgui'
local QtWidgets = require 'qtwidgets'


local app = QtWidgets.QApplication.new(0, {})
app.setApplicationName('qtLua .. Scale and Rotate')


local window = QtWidgets.QMainWindow.new()

window:setMinimumWidth(640)
window:setMinimumHeight(400) 
                  

                  

image = QtGui.QImage("./sailboat.png")


inIcon = QtGui.QIcon("./zoom_in.png")
outIcon = QtGui.QIcon("./zoom_out.png")
rotateIcon = QtGui.QIcon("./rotate.png")

local buttonIn = QtWidgets.QPushButton(inIcon, " ", window)
local buttonOut = QtWidgets.QPushButton(outIcon, " ", window)
local buttonRotate = QtWidgets.QPushButton(rotateIcon, " ", window)

buttonIn:setFixedSize(80, 20)
buttonOut:setFixedSize(80, 20)
buttonRotate:setFixedSize(80, 20)



local buttonBox = QtWidgets.QWidget(window)
layout = QtWidgets.QHBoxLayout()
buttonBox:setLayout(layout)
layout:addWidget(buttonIn, 1)
layout:addWidget(buttonOut, 1)
layout:addWidget(buttonRotate, 1)

buttonBox:setMinimumWidth(window:width())
buttonBox:setMinimumHeight(25)






local frame = QtWidgets.QFrame.new(window)
frameWidth = window:width()
frameHeight = window:height() - 30
frame:setMinimumWidth(frameWidth)
frame:setMinimumHeight(frameHeight)
frame:setFrameRect(QtCore.QRect(0, 30, frameWidth, frameHeight))



-- rawset(_G, 'SIGNAL', function(s) return '2' .. s end)
-- rawset(_G, 'SLOT', function (s) return '1' .. s end)

--frame:__addslot('handler()', function(event) 
    --local pos = event:pos()
    --print(event:type())
    --print(pos:x().." "..pos:y())
--    print('******')
--end)
--frame.connect(buttonIn, SIGNAL 'clicked()', frame, SLOT 'handler()')


local scales = { 0.2, 0.3, 0.44, 0.67, 1.0, 1.5 }
local scaleIndex = 5
local rotationIncrement = 60
local rotation = 0


function frame:mousePressEvent(event)
    local pos = event:pos()
    print(event:type())
    print(pos:x().." "..pos:y())
    if (buttonIn:geometry():contains(pos)) then
        if (scaleIndex < #scales) then
            scaleIndex = scaleIndex + 1
        end
    end
    if (buttonOut:geometry():contains(pos)) then
        if (scaleIndex > 1) then
            scaleIndex = scaleIndex - 1
        end
    end
    if (buttonRotate:geometry():contains(pos)) then
        if (rotation + rotationIncrement <= 180) then
            rotation = rotation + rotationIncrement
        else
            rotation = (rotation + rotationIncrement - 180) - 180
        end
    end
    frame:repaint()
end



function round(float)
    return math.floor(float)
end


function frame:paintEvent(event)
    local painter = QtGui.QPainter(frame)
    painter:begin(frame)
    painter:setClipRect(frame:frameRect())
    transform = QtGui.QTransform()
    transform:scale(scales[scaleIndex], scales[scaleIndex])
    transform:rotate(rotation)
    local newImage = image:transformed(transform, QtCore.TransformationMode.SmoothTransformation)
    local w = newImage:width()
    local h = newImage:height()
    local x = round((frameWidth - w)/2)
    local y = round((frameHeight - h)/2)
    painter:drawImage(QtCore.QRect(x, y, w, h), newImage)
    painter['end'](painter)
end
                 




window.show(window)

app.exec()
