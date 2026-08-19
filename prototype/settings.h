#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtWidgets>


class Settings : public QDialog
{
	public:
	
		Settings(QWidget *parent);
		
		void init();
		
		QLineEdit *uridbox;
		QLineEdit *userkeybox;
		QLineEdit *nickbox;
		
				
		static constexpr int USERKEYBITS = 512;
		static constexpr int USERKEYHEXS = USERKEYBITS / 4;
		
		
		typedef std::string Player;
		typedef char Urid[2*Settings::USERKEYHEXS];
		
		static std::map<Player, Urid> players; 
		
		
		
		static Urid *myUrid();
		static Urid *sideUrid(std::string side);
		static std::string mySide();
		static bool isMySide(std::string side);
		static std::string myNick();
		static bool isOwner(std::string id);
		static bool isSide(std::string side);
		
		void updateRightsBox();
		std::string streamRightsBox();
		void setRightsBox(std::string str);
		
		
		
		struct OwnershipRights 
		{
			unsigned int Hidden : 1;
			unsigned int NotSeen : 1;
			unsigned int NotRevealed : 1;
			unsigned int NoMenu : 1;
			unsigned int NoDelete : 1;
			unsigned int NoFlip : 1;
			unsigned int NoMove : 1;
			unsigned int NoOpen : 1;
			unsigned int NoHoover : 1;
		};
		
		
		static OwnershipRights myOwnershipRights;



		
	private:
		void offsetOption(bool checked);
		void openOption(bool checked);
		void activated(QComboBox *rotation);
		void facingOption(bool checked);
		void colorSelection(QLabel *box);
		void loadKey();
		void setKeyFile();
		void setUridFile();
		void createUserKey();
		void addUrid();
		void createUrid();
		void readUrids();
		void writeUrids();
		void removeUrid();
		
		static char userKey[USERKEYHEXS];
		static bool userKeyExists;
		QListWidget *uridlist;
		static constexpr int RIGHTS = 9;
		QCheckBox *rights[RIGHTS];
		void createRightsBox(int n, QString text, bool enabled);
		
		
		
};


#endif // SETTINGS_H
