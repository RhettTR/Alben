package.cpath = "/home/test/projects/alben/build-lqt/lib/?.so"

local QtCore = require 'qtcore'
local QtGui = require 'qtgui'
local QtWidgets = require 'qtwidgets'


local app = QtWidgets.QApplication.new(0, {})


local window = QtWidgets.QMainWindow.new()

window:setMinimumWidth(640)
window:setMinimumHeight(400) 
                  
                   
local frame = QtWidgets.QFrame.new(window)

frame:setMinimumWidth(640)
frame:setMinimumHeight(400)
frame:setAcceptDrops(true)


function frame:dragEnterEvent(event)
    event:acceptProposedAction()
end

function frame:dropEvent(event)
    local mimeData = QtCore.QMimeData.new()
    mimeData = event:mimeData()
    local byteArray = QtCore.QByteArray.new(mimeData:data('application/x-alben-counter'))
    local x = byteArray:mid(0,2):toInt()
    local y = byteArray:mid(2,2):toInt()
    local offset = QtCore.QPoint.new(x, y)
    event:source():move(event:pos() - offset)
    event:accept()
end



map = QtGui.QImage("/home/test/projects/alben/scripts/Map.png")

piece1 = QtGui.QPixmap("/home/test/projects/alben/scripts/piece1.png")
piece2 = QtGui.QPixmap("/home/test/projects/alben/scripts/piece2.png")


Counter = { pixmap, frame }
Counter.__index = Counter


         
function Counter:create(x, y, image)
    local cnt = {}
    setmetatable(cnt, Counter)
    cnt.pixmap = image
    cnt.frame = QtWidgets.QFrame.new(frame)
    cnt.frame:setMinimumWidth(50)
    cnt.frame:setMaximumWidth(50)
    cnt.frame:setMinimumHeight(50)
    cnt.frame:setMaximumHeight(50)   
    cnt.frame:move(x, y)
    cnt.frame:setAcceptDrops(false)
    local leftDownHandler = function(self, mouseEvent)
        local drag = QtGui.QDrag(self)      
        drag:setPixmap(cnt.pixmap)
        local offset = mouseEvent:pos()
        drag:setHotSpot(offset)
        local byteArray = QtCore.QByteArray.new()
        local byteArrayY = QtCore.QByteArray.new()
        byteArray:setNum(offset:x())
        byteArrayY:setNum(offset:y())
        byteArray:append(byteArrayY)
        local mimeData = QtCore.QMimeData.new()
        mimeData:setData('application/x-alben-counter', byteArray)
        drag:setMimeData(mimeData)
        dropAction = drag:exec()
        return true
    end
    cnt.frame.mousePressEvent = leftDownHandler
    return cnt
end



counters = {}
table.insert(counters, Counter:create(14, 10, piece1))
table.insert(counters, Counter:create(196, 114, piece2))



function frame:mousePressEvent(event)
    local pos = event:pos()
    print(event:type())
    print(pos:x().." "..pos:y())
end



function frame:paintEvent(event)
    local painter = QtGui.QPainter(frame)
    painter:begin(frame)
    painter:drawImage(QtCore.QRect(0, 0, 640, 400), map)
    for i = 1, #counters do
        painter:drawPixmap(QtCore.QRect(counters[i].frame:x(), counters[i].frame:y(), 50, 50), counters[i].pixmap)
    end
    painter['end'](painter)
end
                 


frame:show()
window.show(window)


app.exec()
