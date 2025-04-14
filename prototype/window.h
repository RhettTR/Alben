#include <QtWidgets>
#include <map>

#include "counter.h"

		
		
class Window : public QMainWindow
{
	
	public:
		
		class Token;
		class Label;
		class CheckBox;
		
		class Frame : public QFrame
		{
			public:
				struct Coordinate 
				{ 
					int x; 
					int y; 
				};
				typedef typename std::vector<Coordinate> Coordiantes;
							
				Frame(Window *parent, std::string background);
	
				void snaptoGrid (int x, int y, Token *dragged, Coordinate &coordinate);
				void addtoGrid(int x, int y);
				
				std::map<std::string, Label *> labels;
				std::map<std::string, CheckBox *> checkboxes;
				std::map<int, Token *> tokens;
				
			protected:	
				virtual void mousePressEvent(QMouseEvent *event);
				virtual void dragEnterEvent(QDragEnterEvent *event);
				virtual void dropEvent(QDropEvent *event);
				virtual void paintEvent (QPaintEvent *event);
				
			private:
				QImage backgroundImage;	
				
		};
		
		class Token : public QLabel
		{
			public:
			
				struct State				
				{
					int id;
					int x;					
					int y;										
					std::string image;			// name of current (flipped) image					
				};
				
				Token(int counterId, Counter::Table *table);
				~Token(); 
				
				int margin;	// the area around the counter for rendering masks
				
				void setImage(std::string name);
				
				State state;
				
				int baseWidth;
				int baseHeight;
				int counterId;
				
				Counter::Table *overlays;
				
				void set(std::string text);
				
				
		};
		
		
		class Label : public QLabel
		{
			public:
				Label(std::string id, Frame *parent, int x, int y, int w, int h, QString styleSheet);
							
				void set(std::string text);	
				std::string get();	
		};
		
		
		class CheckBox : public QCheckBox
		{
			public:
				CheckBox(std::string id, Window *parent, int x, int y, int w, int h, QString text, std::string luaScript, QString styleSheet);
					
				bool get();	
		};
		
		
		
		Window(QWidget *parent, QString title, std::string name, std::string background);
		
		std::string name;
		
		static Frame *frame;
		
		static Window *getInstance(const char *instance);
		
	
	private:
	
		static std::map<std::string, Window *> instances;
		
};
