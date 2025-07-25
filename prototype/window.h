#ifndef WINDOW_H
#define WINDOW_H


#include <QtWidgets>
#include <map>


#include "counter.h"


		
		
class Window : public QMainWindow
{
	
	public:
		
		
		class Label;
		class CheckBox;
		
		
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
				
				Token(Window *window, int id, Counter::Table *table);
				virtual ~Token();
				void showRightClickMenu();
				void do_activate (QAction *action);
				
				Window *window; 
				
				int margin;	// the area around the counter for rendering masks
				
				void setImage(std::string image);
				
				State state;
				
				int baseWidth;
				int baseHeight;
				int counterId;
				std::string name;
				
				Counter::Table *overlays;
				
				void set(std::string text);	
						
			protected:	
						
				virtual void mousePressEvent(QMouseEvent *event);			
				
		};
		
		
		class Frame : public QFrame
		{
			public:
							
				Frame(Window *parent, std::string background);
				~Frame();
							
				Window *window;
				QPixmap pixmap;
		
				int height;
				
				
			protected:	
				virtual void mousePressEvent(QMouseEvent *event);
				virtual void dragEnterEvent(QDragEnterEvent *event);
				virtual void dropEvent(QDropEvent *event);
				virtual void paintEvent (QPaintEvent *event);
				
			private:
				QImage backgroundImage;		
						
		};
		
		
		class Label : public QLabel
		{
			public:
				Label(std::string tag, Window *parent, int x, int y, int w, int h, std::string resourecName, QString styleSheet);
					
				int x;
				int y;
				int w;
				int h;
				std::string tag;			
				void setText(std::string text);	
				std::string get();
				QImage backgroundImage;
				Window *parent;
				
			protected:	
				virtual void mousePressEvent(QMouseEvent *event);	
		};
		
		
		class CheckBox : public QCheckBox
		{
			public:
				CheckBox(std::string id, Window *parent, int x, int y, int w, int h, QString text, std::string luaScript, QString styleSheet);
					
				int x;
				int y;	
				bool get();
				Frame *frame;	
		};
		
		
		class PushButton : public QPushButton
		{
			public:
				PushButton(std::string widget, QImage image, QString text, int x, int y, std::string luaScript,
								Window *parent = nullptr);
						
				int x;
				int y;				
				QImage image;
				Window *parent;				
		};
		
		
		
		
		
		Window(QWidget *parent, QString title, std::string tag);
		~Window();
		
		typedef std::map<std::string, QWidget *> Widgets;
		typedef std::map<int, Token *> Tokens;
		
		Widgets widgets;	
		Tokens tokens;
		
		
		void addAWidget(std::string key,  QWidget *value);
		void addAToken(int key,  Token *value);
		static Window *instance;
		void removeAToken(int key);
		void createToken(int id, Counter::Table *state);
		void setWidgets(int height = 0);
		
		float factor;  // the scale factor applied to the window
		
		
		static Window *getInstance(const char *instance);
		void showWindow();
		void setSingleRowed(int x, int y, int w, int h);
		
		
		std::string tag;
		QScrollArea *scrollArea;	
		
		
		QFrame *frame;
		
		
		
	protected:
		
		virtual void resizeEvent(QResizeEvent* event);
		
	
	private:
	
		static std::map<std::string, Window *> instances;
		
		
};


#endif // WINDOW_H
