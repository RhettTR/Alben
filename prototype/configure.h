#ifndef CONFIGURE_H
#define CONFIGURE_H


#include "window.h"





class Configure
{
	
	public:
	
		class ChoicePage : public QLabel
		{
			public:
				ChoicePage(Configure *base, std::string tag);
				
				QScrollArea *scrollArea;
				
				
				static std::map<std::string, ChoicePage *> choicepages;
				
			protected:
			
				virtual void resizeEvent(QResizeEvent* event);	
									
		};
		
		
		Configure(QWidget *parent);
		~Configure();
		
		void addListImage(QListWidget* list, std::string resource);
		void itemClicked(QListWidgetItem *item);
		void createMapSelection();
		void setMapSelection();
		void createSideSelection(int row, std::string side);
		void resetSideSelection();
		void setSides();
		void init();
		void selected(QString side, QString player);
		void setNewGame(bool newgame);
		void load();
		void updateComboBoxes();
	
		Window *window;
		
		QComboBox *pageComboBox;
		QStackedWidget *stackedWidget;
		
		static QWidget *mapPage;
		static QLabel *mapWindow;
		static std::string mapResource;	
		QWidget *sidePage;
		QGridLayout *grid;
		
		
		static ChoicePage *getPage(const char *choicepage);
		
		
		typedef std::string Side;
		
		struct Entry
		{
			Side side;
			Settings::Urid urid;
			Settings::OwnershipRights ownershipRights;
			
		};
		
		static std::vector<Entry> urids;
	
		
	private:
		
		bool newGame;
};	


#endif // CONFIGURE_H
