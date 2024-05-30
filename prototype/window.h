#include <stack>
#include <any>

#include <QtWidgets>

		
		
class Window : public QMainWindow
{

	public:
	
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
		
		
		Window(QWidget *parent);
		

		Pane* getParent();
		
		
		static void root(int level);
		static void tabs(int level);
		static void tab(int level, std::string text);
		static void listBox(int level);
		static void listItem(int level, std::string text);
		static void comboBox(int level);
		static void comboItem(int level, std::string text);
		
		
		static QList<Pane*> paneList;
		
		
		void reset();
		
		
	protected:
	
		virtual void resizeEvent(QResizeEvent* event);
		
		
	private:
				
		static std::stack<std::any> panes;
		
		
};
