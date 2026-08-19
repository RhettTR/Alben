#ifndef IO_H
#define IO_H


#include <fstream>
#include <string>
#include <map>
#include <QtWidgets>
#include <QtNetwork>

#include "counter.h"



class IO
{
	
	public:	
	
	
		// net
		class ConnectDialog : public QDialog
		{
			
			public:				
				ConnectDialog(QWidget *parent);
				~ConnectDialog();
				
				QLineEdit *port;
				QLineEdit *ip;
				QLineEdit *portc;
				QPlainTextEdit *messages;
				
			protected:	
				virtual void paintEvent (QPaintEvent *event);	
		};
		
		
		
		class Net : public QObject
		{
			public:
				Net(QObject *parent = nullptr);
				~Net();
				void listen();
				void confirm();
				void sync();
				void connect();
				void disconnect();
				void writeData(Counter::Table table, qint32 tableSize, qint32 type);
				bool connected();
				void outTable(Counter::Table t);
				
			private:
				QTcpServer *server;
				QTcpSocket *socket;
				qint32 buffsize;
				QDataStream *pstream;
				bool checkPort(QString port);
				void writeTable(Counter::Table table);
				Counter::Table readTable(qint32 keys);
				void onConnection();
				void onStateChanged();
				void onReadyRead();
				void onConnected();
				void onError();
				static void onWritten(qint64 bytes);
							
		};
		
	

        
        static bool stepping;
        static bool recording;
        static ConnectDialog *netDialog;
        static Net *server;
        
        
        typedef std::variant<int, std::string, bool, double, Counter::Table> Variant;
        
	
		class LoadGame 
		{
			private:
				Counter::Table loadTable(std::size_t keys);
			public:			
				LoadGame(std::string fileName);				
				virtual ~LoadGame() 
				{
					fs.close();
				}	
		};
			
							      
		IO();
		
		void load_resources(std::string directory);
		static void addPadding(std::string resourceId, QColor color, int left, int top, int right, int bottom);
		static void addMask(std::string resourceId, QColor color, int left, int top, int width, int height);
		static void changeImage(std::string fromImage, std::string toImage);
		static bool connected();
		void reset();
		void transfer_resource_keys();

		
		struct Resource
		{	
			QImage image;
			QSize  size;	
		}; 
		
		void closeGame();
		
		// getters for resources
		QImage& getImage(std::string str);
		QSize getSize(std::string str);
		bool isResource(std::string str);
		
		
		void saveTable(Counter::Table table);
		QString saveGame(QString saveAs, QString saveTo, QString suffix);
		void saveLog(QString file);
		void loadGame();
		void loadSetUp(std::string filename);
		QString loadHtml(std::string filename);
		
		void loadConfig();
		static void saveConfig();
		
		static int intType;
		static int stringType;
		static int boolType;
		static int doubleType;
		static int tableType;	
		
		
		
		// define State stack
		typedef std::string Leftside;
		typedef Counter::Rightside Rightside;

		class Stage : public std::map<Leftside, Rightside> {};
		class Stack : public std::map<int, Stage> {};
		

		
		
	protected:
			
		static std::fstream fs;	
		
	private:
		
		std::map<std::string, Resource> _resources;
		
		
		
};


#endif // IO_H
