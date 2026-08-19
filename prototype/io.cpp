#include <iostream>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <QtSvg>


#include "io.h"
#include "scale.h"
#include "luau.h"
#include "configure.h"
#include "settings.h"
#include "toolbar.h"
#include "overlay.h"



extern IO *io;
extern ToolBar *mainToolBar;
extern Configure *config;
extern Settings *settings;


using namespace std;
using namespace std::filesystem;


std::fstream IO::fs;

 
bool IO::stepping;
bool IO::recording;
IO::ConnectDialog *IO::netDialog;
IO::Net *IO::server;

int IO::intType;
int IO::stringType;
int IO::boolType;
int IO::doubleType;
int IO::tableType;



IO::ConnectDialog::ConnectDialog(QWidget *parent) : QDialog(parent)
{
	
	this->setWindowTitle("Connect to peer");
	this->setSizeGripEnabled(false);
	
	this->setStyleSheet("QLineEdit {background-color : white;} \
					     QLabel {background-color : transparent; color : white} \
					     QPlainTextEdit {border: 2px solid gray; border-style: inset; \
							 font: normal normal normal 15px/1.4 Arial; color: grey;}");

	
	QGridLayout *gridLayout = new QGridLayout(this);

	
	gridLayout->setContentsMargins(QMargins(20,10,20,10));
	
	
	
	
	QLabel *portlabel = new QLabel(this);
	portlabel->setText("Port:");
	
	port = new QLineEdit(this);
	port->setMaximumWidth(45);
	port->setText("6067");
	
	QPushButton *listen = new QPushButton(this);
	listen->setText("Listen");
	QObject::connect(listen, &QPushButton::clicked, [=]() { IO::server->listen(); });
	
	QPushButton *sync = new QPushButton(this);
	sync->setText("Synchronize");
	QObject::connect(sync, &QPushButton::clicked, [=]() { IO::server->confirm(); }); 
	
	
	QLabel *iplabel = new QLabel(this);
	iplabel->setText("IP:");
	
	ip = new QLineEdit(this);
	ip->setText("127.0.0.1");
	ip->setMinimumWidth(125);
	
	QLabel *prt = new QLabel(this);
	prt->setText("Port:");
	
	portc = new QLineEdit(this);
	portc->setMaximumWidth(45);
	portc->setText("6067");
	
	QPushButton *connect = new QPushButton(this);
	connect->setText("Connect");
	QObject::connect(connect, &QPushButton::clicked, [=]() { IO::server->connect(); });
	
	QPushButton *disconnect = new QPushButton(this);
	disconnect->setText("Disconnect");
	QObject::connect(disconnect, &QPushButton::clicked, [=]() { IO::server->disconnect(); });  
	
	messages = new QPlainTextEdit(this);
	messages->setReadOnly(true);
	messages->setMinimumHeight(100);
	
	
	gridLayout->addWidget(portlabel, 0, 2, Qt::AlignRight);
	gridLayout->addWidget(port, 0, 3, Qt::AlignLeft);
	gridLayout->addWidget(listen, 0, 4, Qt::AlignLeft);
	gridLayout->addWidget(sync, 0, 5, Qt::AlignLeft);
	
	gridLayout->addWidget(iplabel, 1, 0, Qt::AlignRight);
	gridLayout->addWidget(ip, 1, 1, Qt::AlignLeft);
	gridLayout->addWidget(prt, 1, 2, Qt::AlignRight);
	gridLayout->addWidget(portc, 1, 3, Qt::AlignLeft);
	gridLayout->addWidget(connect, 1, 4, Qt::AlignLeft);
	gridLayout->addWidget(disconnect, 1, 5, Qt::AlignLeft);
	
	gridLayout->addWidget(messages, 2, 0, 1, -1);
	
	
	this->setFixedSize(450, 250);
}


IO::ConnectDialog::~ConnectDialog()
{
}


void IO::ConnectDialog::paintEvent (QPaintEvent *event)
{
	QPainter painter(this);
	
	QImage image = io->getImage("__background");
	painter.drawImage(0,0,image); 
}








IO::Net::Net(QObject *parent) : QObject(parent)
{
	server = new QTcpServer(this);
	socket = new QTcpSocket(this);
	
	QObject::connect(socket, &QTcpSocket::connected, this, [=]() { onConnected(); });
	QObject::connect(socket, &QTcpSocket::errorOccurred, this, [=]() { onError(); });
	QObject::connect(socket, &QTcpSocket::bytesWritten, this, &IO::Net::onWritten);
	QObject::connect(socket, &QTcpSocket::readyRead, this, [=]() { onReadyRead(); });
	
	

	buffsize = 8*1024;
    
	// sets internal socket buffersize	
    socket->setReadBufferSize(buffsize);
    
    	

		 
}

IO::Net::~Net()
{
	delete server;
	delete socket;
}



void IO::Net::outTable(Counter::Table t)
{
	for (auto obj = t.begin(); obj != t.end(); ++obj)
	{
		std::visit(
			Overload{
				[] (int k) { printf("%d=", k);  },
				[] (std::string k) { printf("\"%s\"=", k.c_str()); }
			},
			obj->first
		);
		std::visit(
			Overload{
				[] (double k) { printf("%f\n", k); },
				[] (bool k) { (k ? printf("true\n") : printf("false\n")); },				
				[] (std::string k) { printf("\"%s\"\n", k.c_str()); },
				[this] (Counter::Table k) { outTable(k); }
			},
			obj->second
		);	
	}
}



bool IO::Net::checkPort(QString portString)
{
	if (!portString.isEmpty())
	{
		bool ok;
		int port = portString.toInt(&ok);
		
		if (!ok)
			Luau::error(23, 0);
		else 
		
		// crude port test, use a non-well-known, non-assigned port 
		// https://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.xhtml?&page=98
		// the default port 6067 is at time of writing unassigned
		
		if (port < 1024 || port > 65535)
			Luau::error(24, 0);			
		else
			return true;
	}
	else
		Luau::error(22, 0);
		
	return false;	
}


void IO::Net::listen()
{	
	
	if (socket->state() == QAbstractSocket::UnconnectedState)
	{
		if (checkPort(netDialog->port->text()))
		{
			if (!server->isListening())
			{
				int port = netDialog->port->text().toInt();
				
				bool ok = server->listen(QHostAddress::Any, port);
				
				if (!ok)
				{
					netDialog->messages->appendPlainText("Error when trying to listen to port " + QString::number(port));
					return;
				}
				
				netDialog->messages->appendPlainText("Listening on port " + QString::number(server->serverPort()) + "...");
				
				QObject::connect(server, &QTcpServer::newConnection, this, [this]() { this->onConnection(); });
			}
			else
				netDialog->messages->appendPlainText("Server is already listening");
				//Luau::error(26, 0);
		}
	}
	else
		netDialog->messages->appendPlainText("Already connected");
}


void IO::Net::confirm()
{
	if (socket->state() != QAbstractSocket::UnconnectedState)
		Luau::confirm();
	else
		netDialog->messages->appendPlainText("There is no connection");
}


void IO::Net::sync()
{
	if (socket->state() != QAbstractSocket::UnconnectedState)
		Luau::synchronize();
	else
		netDialog->messages->appendPlainText("There is no connection");
}


void IO::Net::onConnection()
{
    if (server->hasPendingConnections())
    {
        delete this->socket;
        this->socket = server->nextPendingConnection();
        //this->socket->open(QIODeviceBase::ReadWrite);
        
        QObject::connect(socket, &QTcpSocket::stateChanged, this, [=]() { onStateChanged(); });
        QObject::connect(socket, &QTcpSocket::readyRead, this, [=]() { onReadyRead(); });
        QObject::connect(socket, &QTcpSocket::errorOccurred, this, [=]() { onError(); });
		QObject::connect(socket, &QTcpSocket::bytesWritten, this, &IO::Net::onWritten);
        

        QHostAddress address = QHostAddress(socket->peerAddress());
        
        bool ok;
        quint32 ip4 = address.toIPv4Address(&ok);
        if (ok)
			netDialog->messages->appendPlainText("Connected to " + QHostAddress(ip4).toString());
		else
			netDialog->messages->appendPlainText("Connected to " + address.toString());
			
		mainToolBar->setImageButton("connection", "__connected", "");
		
		// as server ask opponent to sync with your map
		Luau::ask();
    }
}


void IO::Net::connect()
{	
	if (socket->state() == QAbstractSocket::UnconnectedState)
	{
		if (checkPort(netDialog->portc->text()))
		{
			QHostAddress address = QHostAddress(netDialog->ip->text());
			if (address.isNull())
			{
				Luau::error(25, 1, netDialog->ip->text().toStdString().c_str());
				return;
			}
			
			socket->connectToHost(address, netDialog->portc->text().toUShort());
		
		}
	}
	else
		netDialog->messages->appendPlainText("Already connected");
		//Luau::error(27, 0);		
}


void IO::Net::disconnect()
{	
	if (socket->state() != QAbstractSocket::UnconnectedState)
	{
		socket->disconnectFromHost();
		netDialog->messages->appendPlainText("Disconnected from " + netDialog->ip->text());
		
		mainToolBar->setImageButton("connection", "__disconnected", "");
	}
	else
	
		netDialog->messages->appendPlainText("There is no connection");
		//Luau::error(28, 0);
}



// QDataStream serialization of std::strings
 
QDataStream& operator <<(QDataStream& out, const std::string &in) 
{
	QByteArray raw;
	raw.setRawData(in.data(), in.size());
	out << raw;
	return out;
}

QDataStream& operator >>(QDataStream &in, std::string &out)
{
	char* data;
	in >> data;
	if (data)
		out = data;		
	delete[] data;
	return in;
}



void IO::Net::writeTable(Counter::Table table)
{
	
	for (auto obj = table.begin(); obj != table.end(); ++obj)
	{
		std::visit(
			Overload{
				[this] (int k) { *pstream << IO::intType; *pstream << k; },
				[this] (std::string k) { *pstream << IO::stringType; *pstream << k; }
			},
			obj->first
		);
		std::visit(
			Overload{
				[this] (double k) { *pstream << IO::doubleType; *pstream << k; },
				[this] (bool k) { *pstream << IO::boolType; *pstream << k; },				
				[this] (std::string k) { *pstream << IO::stringType; *pstream << k; },
				[this] (Counter::Table k) { *pstream << IO::tableType; *pstream << (qint32)k.size(); writeTable(k); }
			},
			obj->second
		);	
	}
	
}


void IO::Net::writeData(Counter::Table table, qint32 tableSize, qint32 type)
{
	
    if (socket->state() == QAbstractSocket::ConnectedState)
    {			
		QDataStream stream(socket);	
		
		this->pstream = &stream;
			
		
		*pstream << tableSize;
		*pstream << type;
		*pstream << (qint32)table.size();

		
		writeTable(table);
			
    }
    
}


bool IO::Net::connected()
{
	return socket->state() == QAbstractSocket::ConnectedState;
}


Counter::Table IO::Net::readTable(qint32 keys)
{
	
	
	Counter::Leftside left;
	Counter::Rightside right;
	Counter::Table table;
	
	
	
	
	for (qint32 i = 0; i < keys; i++)
	{
		
		
		if (pstream->atEnd())	
			return table;
			
			
		int type;	
		*pstream >> type;  
		
			
		if (type == IO::stringType)
		{
			std::string key;
			*pstream >> key;
			left = key;
		}
		else
			if (type == IO::intType)
			{
				int key;
				*pstream >> key;			
				left = key;
			}
		else
			netDialog->messages->appendPlainText("Error: reading key " + QString::number(type));
		
		
		
		// value
		
		
		*pstream >> type;
		
		
		if (type == IO::boolType)
		{
			bool value;
			*pstream >> value;	
			right = value;
		}
		else
			if (type == IO::stringType)
			{
				std::string key;
				*pstream >> key;
				right = key;
			}
			else
				if (type == IO::doubleType)				
				{
					double value;
					*pstream >> value;
					right = value;
				}
				else
					if (type == IO::tableType)
					{
						qint32 s;
						*pstream >> s;						
						right = readTable(s);						
					}
					else
						netDialog->messages->appendPlainText("Error: reading value");


		
		table[left] = right;
	
	}
	
	
	return table;
	
	
}        



void IO::Net::onReadyRead()
{  

	QDataStream stream(socket);
		
	this->pstream = &stream;
	
	
	
	// table size is first 4 bytes (qint32)
	// type (1=update 2=sync 3=stage sync) is last 4 bytes (qint32)
	// table keys last 4 bytes
	
	
	
	while (socket->bytesAvailable() > 0)	
	{
	
	
		char data[3 * sizeof(qint32)];
		
		qint64 result = socket->peek((char *)data, 3 * sizeof(qint32));
		
		

		if (result < 12) 
		{
			bool res = this->socket->waitForReadyRead(5000);
			if (res == false)
			{
				Luau::error(35, 1, 5000);
				return; 
			}
		}
		
		
		
		qint32 tableSize = qFromBigEndian<quint32>(&data[0]);
		qint32 type = qFromBigEndian<quint32>(&data[4]);
		qint32 keys = qFromBigEndian<quint32>(&data[8]);;
		
		
		
		

		if (type == 1)
		{
			
			stream >> tableSize;	

			stream >> type;
				
			stream >> keys;

			Counter::Table table = readTable(keys);			
			Luau::updateCounter(table);	
			
		}
			
		
		if (type == 2)
		{
			stream >> tableSize;	
			
			stream >> type;
			
			stream >> keys;
		
			Counter::Table table = readTable(keys);
			Luau::loadCounter(table);
			
			Luau::done();
		}
			
			
		if (type == 3)
		{
			stream >> tableSize;	
			
			stream >> type;
			
			stream >> keys;
			
	

			Counter::Table table = readTable(keys);
			Luau::updateCounter(table);
		
			Luau::stagedone();
		}
	
	}
		
		
}



void IO::Net::onStateChanged()
{

    QHostAddress address = QHostAddress(socket->peerAddress());
    
    if (socket->state() == QAbstractSocket::UnconnectedState)
	{	
  
		bool ok;
		quint32 ip4 = address.toIPv4Address(&ok);
		if (ok)
			netDialog->messages->appendPlainText(QHostAddress(ip4).toString() + " disconnected");
		else
			netDialog->messages->appendPlainText(address.toString() + " disconnected");
		
		mainToolBar->setImageButton("connection", "__disconnected", "");	
		
	}	
	
}


void IO::Net::onConnected()
{
    netDialog->messages->appendPlainText("Connected to " + netDialog->ip->text());
    mainToolBar->setImageButton("connection", "__connected", "");
}

void IO::Net::onError()
{
	netDialog->messages->appendPlainText(socket->errorString());
	mainToolBar->setImageButton("connection", "__disconnected", "");
}

void IO::Net::onWritten(qint64 bytes)
{
	//netDialog->messages->appendPlainText("Successfully wrote " + QString::number(bytes) + " bytes");
}







	
void IO::closeGame()
{
	Luau::deleteAll();
	Luau::resetState();
	Luau::resetBase();
	Luau::logReset();
	Window::getInstance("main")->frame->closeAllOpenStacks();
	
	ToolBar::getInstance("main")->enabled("__forward", false);
	ToolBar::getInstance("main")->enabled("__end", false);
	
}
	
	
IO::LoadGame::LoadGame(std::string fileName)
{	
	
	bool logfile = false;
	
	Configure::urids.clear();
	
	
	try
	{
		fs.exceptions(std::ios_base::badbit);
				
		fs.open(fileName, ios::binary | ios::in);
	
		if (fs.is_open()) 
		{
			
			
				
			// read file build table
			while (fs)
			{	
				
				// read key
				std::string key;
							
				if (!std::getline(fs, key, '\0'))
					break;
					
				if (key == "turn")
				{
					// read turn
					Luau::Turn turn;
					 					
					fs.read(reinterpret_cast<char*>(&turn.turn), sizeof turn.turn); 					
					fs.read(reinterpret_cast<char*>(&turn.phase), sizeof turn.phase);
					
					Luau::setTurn(turn);
				}
				
				
				if (key == "side")
				{
		
					Configure::Entry entry; 
					
					std::getline(fs, entry.side, '\0');
			
					fs.read(reinterpret_cast<char*>(&entry.urid), sizeof entry.urid);
					
					fs.read(reinterpret_cast<char*>(&entry.ownershipRights), sizeof entry.ownershipRights);
							
					Configure::urids.push_back(entry);
					
				}	
				
					
				if (key == "counter")
				{	
										
					// read number of keys in this table
					std::size_t size; 
					
					fs.read(reinterpret_cast<char*>(&size), sizeof size);					
					
					if (size > 0)
					{	
					
						// read table
						Counter::Table table = loadTable(size);
						
						(void)Luau::loadCounter(table);
							
					}
						
				}
				
				
				// read log (if any)
				if (key == "log")				
				{
					
					// this is a log file
					logfile = true;
					
					
					std::size_t size;
					fs.read(reinterpret_cast<char*>(&size), sizeof size);
					
					
					if (size > 0)
					{	
					
						// read table
						Counter::Table table = loadTable(size);
					
						Luau::loadLog(table);
						
					}
				}
				
				
			}
				
			
			
	
			
			
			if (logfile)
			{
				ToolBar::getInstance("main")->enabled("__forward", true);
				ToolBar::getInstance("main")->enabled("__end", true);
				ToolBar::getInstance("main")->enabled("__abort", true);
				Counter::setDisabled(true);
				IO::stepping = true;				
			}
				
				
				
			// note: close stream is done in the destructor
			
			
		}
			
	}
	catch (const ifstream::failure& e)
	{
		std::cout << e.what() << std::endl;
	}

}




Counter::Table IO::LoadGame::loadTable(std::size_t keys)
{
	
	
	Counter::Leftside left;
	Counter::Rightside right;
	Counter::Table table;
	
	
	
	
	for (std::size_t i = 0; i < keys; i++)
	{
		// key
		
		char type;
	
		if (!fs.read(reinterpret_cast<char*>(&type), 1))
			return table;
		
			
		if ((int)type == IO::stringType)
		{
			std::string key;
			std::getline(fs, key, '\0');
			left = key;
		}
		else
			if ((int)type == IO::intType)
			{
				int key;
				fs.read(reinterpret_cast<char*>(&key), sizeof(key));			
				left = key;
			}
			
		
		
		
		// value
		
		fs.read(reinterpret_cast<char*>(&type), 1);
		
		
		if ((int)type == IO::boolType)
		{
			char value;
			fs.read(reinterpret_cast<char*>(&value), 1);
			if ((bool)value)
				right = true; 	
			else
				right = false; 
		}
		else
			if ((int)type == IO::stringType)
			{
				std::string value;
				std::getline(fs, value, '\0');
				right = value;
			}
			else
				if ((int)type == IO::doubleType)				
				{
					double value;
					fs.read(reinterpret_cast<char*>(&value), sizeof value);
					right = value;
				}
				else
					if ((int)type == IO::tableType)
					{
						std::size_t s;
						fs.read(reinterpret_cast<char*>(&s), sizeof s);	
						right = loadTable(s);
					}


		
		table[left] = right;
	
	}
	
	
	return table;
	
	
}




QString activeDirectory = QDir::homePath() + "/Documents";


void IO::loadGame()
{
	
	// no undo
	
	QString fileName = QFileDialog::getOpenFileName(nullptr, "Open Game",
													activeDirectory,
													"Load Files (*.gsav *.glog);;All Files(*.*)");
	
	
	// test if fileName is a gametop saved file
	
	if (!fileName.isNull())
	{
		closeGame();
		
		// horrible bug, must be here
		Luau::resetBase();			
	
		LoadGame *load = new LoadGame(fileName.toStdString());	
		delete load;
		
		Counter::resetId();
		Counter::resetZorder();
		
		Counter::setGUI("all");
		
		
		config->load(); 
		
		
		ToolBar::init();
		
	}
	
	
}


void IO::loadSetUp(string filename)
{
	QFileInfo fileInfo(QString::fromStdString(filename));

	QString base = fileInfo.baseName();
	

	
	string completeFileName = "./setups/" + base.toStdString() + ".gsav";


	// horrible bug, must be here
	Luau::resetBase();	
	
	
	LoadGame *load = new LoadGame(completeFileName);	
	delete load;
	
	Counter::resetId();
	Counter::resetZorder();
	
	Counter::setGUI("all");
	
	ToolBar::init();
	
}


QString IO::loadHtml(string filename)
{
	QFileInfo fileInfo(QString::fromStdString(filename));

	QString base = fileInfo.baseName();
	

	
	string completeFileName = "./docs/" + base.toStdString() + ".html";
	
	
	QString text;
	
	
	
	try
	{
		fs.exceptions(std::ios_base::badbit);
				
		fs.open(completeFileName, ios::in);
	
		if (fs.is_open()) 
		{
				

			while (fs)
			{	
				
				// read a line of html
				std::string line;
							
				if (!std::getline(fs, line, '\0'))
					break;
					
			
				text.append(QString::fromStdString(line));
			
			}
			
			fs.close();
			
		}
			
	}
	catch (const ifstream::failure& e)
	{
		std::cout << e.what() << std::endl;
	}


	
	return text;	
	

	
}




IO::IO()		
{
	
	load_resources("./__images");
	load_resources("./images");
	
	IO::stepping = false;
	IO::recording = false;
	

	
	IO::intType = 1;
	IO::stringType = 2;
	IO::boolType = 3;
	IO::doubleType = 4;
	IO::tableType = 5;
							
	
		
	
}



QImage& IO::getImage(string str)
{	
	try
    {
        _resources.at(str);
    }
    catch (const std::out_of_range &e)
    {
        Luau::error(7, 1, str.c_str()); 
        std::exit(1);
    }
    
    
	return _resources[str].image;
}


QSize IO::getSize(string str)
{
	try
    {
        _resources.at(str);
    }
    catch (const std::out_of_range &e)
    {
        Luau::error(7, 1, str.c_str()); 
        std::exit(1);
    }
	
	
	return _resources[str].size;
}


bool IO::isResource(string str)
{
	if (_resources.find(str) == _resources.end()) 
		return false;
		
	return true;	
}



//QSvgRenderer
//QSvgWidget

void IO::load_resources(string directory)
{  
	
	try
	{
		
		for (recursive_directory_iterator i(directory), end; i != end; ++i)
		{		 
			if (!is_directory(i->path()))
			{
				string str = i->path().parent_path().string();
				
				// convert back-slashes to fore-slashes
				std::replace(str.begin(), str.end(), '\\', '/');	
				
				str = str + "/" + i->path().stem().string();
				
				string base = directory + "/";
				string substr = str.substr(base.length());
				printf("loaded %s\n", substr.c_str());
				
				string filename = string(i->path().relative_path().generic_string());
	
				
						
				
				if (i->path().extension() == ".svg")
				{
		
					
					QSvgRenderer renderer(QString::fromStdString(filename));
					
					
					
					QImage image(renderer.defaultSize(), QImage::Format_ARGB32);
						
					image.fill(Qt::transparent);
							
				   
					QPainterPath path;
					path.moveTo(23.0, 23.0);
					path.arcTo(0.0, 0.0, 46.0, 46.0, 90.0, 90.0);
					path.lineTo(0.0, 479.0);
					path.arcTo(0.0, 479.0, 46.0, 46.0, 180.0, 90.0);
					path.lineTo(329.0, 525.0);
					path.arcTo(329.0, 479.0, 46.0, 46.0, -90.0, 90.0);
					path.lineTo(375.0, 23.0);
					path.arcTo(329.0, 0.0, 46.0, 46.0, 0.0, 90.0);
					path.lineTo(23.0, 0.0);

				
					
					QPainter painter(&image);
					painter.setClipPath(path);
					painter.setRenderHint(QPainter::Antialiasing);
					painter.setRenderHint(QPainter::TextAntialiasing);
					painter.setRenderHint(QPainter::SmoothPixmapTransform);
					renderer.render(&painter);
					
					
					_resources[substr].size = image.size();
					_resources[substr].image = image;
					 
				}
				else
				{

					QImageReader reader(QString::fromStdString(filename));
					QImage image = QImage(reader.size(), QImage::Format_ARGB32_Premultiplied);
					image.load(QString::fromStdString(filename));
									
					_resources[substr].size = image.size();
					_resources[substr].image = image;
					
				}
		
				
			}
		}
		
	}
	catch (filesystem_error &e) 
	{
		std::cout << e.code() << '\n';
		std::cout << e.what() << '\n';
	}
	
			
}


void IO::addPadding(string resourceId, QColor color, int left, int top, int right, int bottom)
{
	QImage resource = io->getImage(resourceId);

	QImage padded = 
		QImage(io->getSize(resourceId) + QSize(left + right, top + bottom), QImage::Format_ARGB32_Premultiplied);
		
	padded.fill(color);
	
	QPainter *paint = new QPainter(&padded);
	paint->drawImage(QPoint(left, top), resource);
	delete paint;
	
	// memory leak ? (no it's the heap)
	io->_resources[resourceId].image = padded;
	
	io->_resources[resourceId].size = padded.size();
	
}


void IO::addMask(string resourceId, QColor color, int left, int top, int width, int height)
{
	QImage resource = io->getImage(resourceId);

	QImage mask = 
		QImage(QSize(width, height), QImage::Format_ARGB32_Premultiplied);
		
	mask.fill(color);
	
	QPainter *paint = new QPainter(&resource);
	paint->drawImage(QPoint(left, top), mask);
	delete paint;
	
	io->_resources[resourceId].image = resource;
	
	io->_resources[resourceId].size = resource.size();
	
}


void IO::changeImage(std::string fromImage, std::string toImage)
{
	
	QImage resource = io->getImage(toImage);
	

	io->_resources[fromImage].image = resource;
	
	io->_resources[fromImage].size = resource.size();
	
}


bool IO::connected()
{
	return server->connected();
}





void IO::reset()
{
	
	load_resources("./__images");
	load_resources("./images");
	
}


void IO::transfer_resource_keys()
{
	for (auto resource = _resources.begin(); resource != _resources.end(); ++resource)
		Luau::setResourceKey(resource->first.c_str());
}



void IO::saveTable(Counter::Table table)
{
	for (auto obj = table.begin(); obj != table.end(); ++obj)
	{
		std::visit(
			Overload{
				[] (int k) { fs.write(reinterpret_cast<const char*>(&intType), 1);
							 fs.write(reinterpret_cast<const char*>(&k), sizeof k); },
				[] (std::string k) { fs.write(reinterpret_cast<const char*>(&stringType), 1);
								     fs.write(k.c_str(), k.size() + 1); }
			},
			obj->first
		);
		std::visit(
			Overload{
				[] (double k) { fs.write(reinterpret_cast<const char*>(&doubleType), 1);
								fs.write(reinterpret_cast<const char*>(&k), sizeof k); },
				[] (bool k) { fs.write(reinterpret_cast<const char*>(&boolType), 1);
							  fs.write(reinterpret_cast<const char*>(&k), 1); },				
				[] (std::string k) { fs.write(reinterpret_cast<const char*>(&stringType), 1);
									 fs.write(k.c_str(), k.size() + 1); },
				[&] (Counter::Table k) { fs.write(reinterpret_cast<const char*>(&tableType), 1);
									     std::size_t s = k.size();
										 fs.write(reinterpret_cast<const char*>(&s), sizeof s);
										 saveTable(k); }
			},
			obj->second
		);	
	}
}




QString IO::saveGame(QString saveAs, QString saveTo, QString suffix)
{
	
	QFileDialog dialog = QFileDialog(nullptr, saveAs,
									 activeDirectory,
									 saveTo);
										 
	dialog.setDefaultSuffix(suffix);
	dialog.setAcceptMode(QFileDialog::AcceptSave);
										 
	QStringList fileNames;
	
	if (dialog.exec() == QFileDialog::Accepted)
	{
	
		fileNames = dialog.selectedFiles();
		
		if (fileNames.count() == 0)
			return "";
		
		
		QFileInfo fileInfo(fileNames[0]);
		
		QDir dir = fileInfo.absoluteDir();
		
		activeDirectory = dir.absolutePath();
		
		
		
		
		fs.open(fileNames[0].toStdString(), ios::binary | ios::out | ios::trunc);
		
		
	
		
		// save turn and phase
		
		std::string key = "turn";
		fs.write(key.c_str(), key.size() + 1);
		
		Luau::Turn turn = Luau::getTurn();
		
		fs.write(reinterpret_cast<const char*>(&turn.turn), sizeof turn.turn);
		fs.write(reinterpret_cast<const char*>(&turn.phase), sizeof turn.phase);
	     
		
		
		// save sides
		
		
		
		// (these fields must later be encrypted)		
		for (const Configure::Entry &entry : Configure::urids) 
		{
		
			key = "side";
			fs.write(key.c_str(), key.size() + 1);
		
			
			fs.write(entry.side.c_str(), entry.side.size() + 1);
			fs.write(reinterpret_cast<const char*>(&entry.urid), sizeof entry.urid);
			
			Settings::OwnershipRights r;
			
			if (entry.side == Settings::mySide())
				r = Settings::myOwnershipRights;			
			else
				r = entry.ownershipRights;
			
			fs.write(reinterpret_cast<const char*>(&r), sizeof r);
		}
		
		
		
		
		// save counters
		
		
		for (auto obj = Counter::counters.begin(); obj != Counter::counters.end(); ++obj)
		{
			
			decltype(obj->second) counter = obj->second;
			
			
			std::string key = "counter";
			fs.write(key.c_str(), key.size() + 1);
		
			
			Counter::Table table = Luau::getTraits("", counter->id);
			
			if (!table.empty())
			{
			
				// save top level table size
				std::size_t s = table.size();
				fs.write(reinterpret_cast<const char*>(&s), sizeof s);
			
				saveTable(table);
			}						
				
		}
		
		
		
		fs.close();
		
		return fileNames[0];
		
	}
	
	
	return nullptr;										
			
}




void IO::saveLog(QString file)
{
	
	if (file.isNull() || file.isEmpty())
		return;
		
	// append
	fs.open(file.toStdString(), ios::binary | ios::out | ios::app);
	
	
	// get log range
	int savedPointer = 0, stagePointer = 0;
	
	Luau::getRange(savedPointer, stagePointer);
	
	
	
	for (int i = savedPointer; i < stagePointer; i++)
	{
		
		std::string key = "log";
		fs.write(key.c_str(), key.size() + 1);
		
		
		Counter::Table table = Luau::getTraits("State", i+1);
	
		// save top level table size
		std::size_t s = table.size();
		fs.write(reinterpret_cast<const char*>(&s), sizeof s);
		
		
		saveTable(table);
	}
	
	fs.close();
	
}



void IO::loadConfig()
{
	
	// make default config path and file name
	
	QDir home = QDir(QDir::home());
	
	QString name = home.absolutePath() + "/.gametop/config.txt";
	QString normalizedName = QDir::cleanPath(name);
	QString configFile = QDir::toNativeSeparators(normalizedName);
	
	
	try
	{
		fs.exceptions(std::ios_base::badbit);
				
		fs.open(configFile.toStdString(), ios::in);
	
		if (!fs.is_open()) 
		{
			// no config file exists; make one
			
			std::filesystem::path path{configFile.toStdString()};
			std::filesystem::create_directories(path.parent_path());
			
			saveConfig();
			
			return;
		}
		
		
		// a config file exists
		
		std::string input;
			
		while (fs)
		{				
			if (std::getline(fs, input, '\n'))
			{
				
				if (input.rfind("NICK=", 0) == 0)
					settings->nickbox->setText(QString::fromStdString(input.substr(strlen("NICK="))));
					
				if (input.rfind("USERKEYFILE=", 0) == 0)
					settings->userkeybox->setText(QString::fromStdString(input.substr(strlen("USERKEYFILE="))));
					
				if (input.rfind("URIDFILE=", 0) == 0)
					settings->uridbox->setText(QString::fromStdString(input.substr(strlen("URIDFILE="))));
					
				if (input.rfind("OWNERSHIPRIGHTS=", 0) == 0)
					settings->setRightsBox(input.substr(strlen("OWNERSHIPRIGHTS=")));	
				
			}
		}	
		
		fs.close();
		
		
			
	}
	catch (const ifstream::failure& e)
	{
		std::cout << e.what() << std::endl;
	}
	
}


void IO::saveConfig()
{
	
	QDir home = QDir(QDir::home());
	
	QString name = home.absolutePath() + "/.gametop/config.txt";
	QString normalizedName = QDir::cleanPath(name);
	QString configFile = QDir::toNativeSeparators(normalizedName);
	
	std::ofstream fileStream(configFile.toStdString());
	
	
	if (fileStream.is_open())
	{
		fileStream << "NICK=" <<  settings->nickbox->text().toStdString() << "\n";
		fileStream << "USERKEYFILE=" << settings->userkeybox->text().toStdString() << "\n";
		fileStream << "URIDFILE=" << settings->uridbox->text().toStdString() << "\n";
		fileStream << "OWNERSHIPRIGHTS=" << settings->streamRightsBox() << "\n";
		fileStream.close();
	}
	
}
