#ifndef WINDOW_H
#define WINDOW_H


#include <QtWidgets>
#include <map>
#include <stack>
#include <any>


#include "counter.h"


class CentralFrame;


		
		
class Window : public QMainWindow
{
	
	public:
		
		
		class Label;
		class CheckBox;
		
		
		class PlainTextEdit : public QPlainTextEdit
		{
			public:
				PlainTextEdit(QString css, QWidget *parent);
		};
		
		class LineEdit : public QLineEdit
		{
			public:
				LineEdit(QPlainTextEdit *logbox, QString css, QWidget *parent);
				QPlainTextEdit *logbox;
			protected:	
				void keyPressEvent(QKeyEvent *e);
		};
		
		
		class Pane : public QLabel
		{
			public:
				Pane(QWidget *parent = nullptr);
			protected:
				virtual void resizeEvent(QResizeEvent* e);
		};
		
		
		class ListItem;
		
		class ListBox : public QListWidget
		{
			public:
				ListBox(QWidget *parent);				
			private:
				void itemClicked(ListItem *item);
				void itemSelectionChanged();
		};
		
		class ListItem : public QListWidgetItem
		{
			public:
				ListItem(const QString &text, ListBox *parent, Pane *paneParent, int type = Type);
				Pane *pane;
			
		};
		
		class ComboBox : public QComboBox
		{
			public:
				ComboBox(QWidget *parent = nullptr);
				void activated(int index);
				
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
				CentralFrame *frame;	
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
		
		
		
		
		
		Window(QWidget *parent, QString title, std::string tag, std::string type);
		~Window();
		
		
		
		
		typedef std::map<std::string, QWidget *> Widgets;
		
		Widgets widgets;	
		
		
		void addAWidget(std::string key,  QWidget *value);
		static Window *instance;
		QWidget *container;
		void setWidgets(int height = 0);
		bool zooming;
		
		
		
		static Window *getInstance(const char *instance);
		static void rollDie();
		void deleteWidgets();
		void showWindow();
		void setSingleRowed(int x, int y, int w, int h);
		
		//
		std::string tag;
		
		
		QScrollArea *scrollArea;	
		
		
		CentralFrame *frame;
		
		// repository
		
		Pane* getParent();
		
		void visibility(bool value);
		
		static std::string rootTag;	
		
		void root(int level);
		void tabs(int level);
		void tab(int level, std::string text);
		void listBox(int level);
		void listItem(int level, std::string text);
		void comboBox(int level);
		void comboItem(int level, std::string text);
		void imageItem(std::string imageID);
		
		
		QList<Pane*> paneList;
		void reset();
		
		
		// log / chat window
		
		static PlainTextEdit *textbox;
		static LineEdit *edit;
		
		
	protected:
		
		virtual void resizeEvent(QResizeEvent* event);	
		
			
		
	
	private:
	
		static std::map<std::string, Window *> instances;
		std::stack<std::any> panes;
		
		
};


#endif // WINDOW_H
