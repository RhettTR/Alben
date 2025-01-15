#ifndef SETTINGS_H
#define SETTINGS_H



class Settings : public QDialog
{
	public:
		Settings(QWidget *parent);
	
		
	private:
		void offsetOption(bool checked);
		void openOption(bool checked);
		void activated(QComboBox *rotation);
		void facingOption(bool checked);
		void colorSelection(QLabel *box);

};


#endif // SETTINGS_H
