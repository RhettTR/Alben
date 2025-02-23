#ifndef SETTINGS_H
#define SETTINGS_H



class Settings : public QDialog
{
	public:
	
		Settings(QWidget *parent);
		
		static std::string playerSide;
		
		
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


};


#endif // SETTINGS_H
