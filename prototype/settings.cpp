#include <QtWidgets>


#include "counter.h"
#include "frame.h"
#include "overlay.h"
#include "scale.h"
#include "window.h"
#include "luau.h"
#include "configure.h"

#include "settings.h"


// prime for userkey
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/core_names.h>
#include <openssl/params.h>
#include <openssl/ec.h>


extern QWidget *container;
extern Scale *scaled;
extern Configure *config;
extern Settings *settings;
extern IO *io;






char Settings::userKey[Settings::USERKEYHEXS];
bool Settings::userKeyExists = false;
std::map<Settings::Player, Settings::Urid>  Settings::players; 
Settings::OwnershipRights Settings::myOwnershipRights  = {0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b1, 0b0, 0b0};
	
	 
	 



Settings::Settings(QWidget *parent) : QDialog(parent)
{
	this->setWindowTitle("Settings");
	this->resize(500,400);
	this->move(700, 500);
	
	
	
	QTabWidget *tabs = new QTabWidget(this);
	tabs->resize(this->size());
	
	
	QVBoxLayout *layout = new QVBoxLayout();
	
	QCheckBox *offset = new QCheckBox("Offset-&less stacks", this);
	offset->setChecked(!Counter::haveOffset);
	QObject::connect( offset, &QAbstractButton::clicked, [=]()->void{ offsetOption(offset->isChecked()); });
    layout->addWidget(offset);
    
    QCheckBox *open = new QCheckBox("&Open stack with window", this);
	open->setChecked(!CentralFrame::openStackoffset);
	QObject::connect( open, &QAbstractButton::clicked, [=]()->void{ openOption(open->isChecked()); });
    layout->addWidget(open);
    
    QHBoxLayout *sizeLayout = new QHBoxLayout();
    QComboBox *rotation = new QComboBox();
    rotation->setStyleSheet("QListView::item {height:20px;}");
    rotation->setMaximumWidth(60);
	rotation->addItem("-90°", -90);
	rotation->addItem("0°", 0);
	rotation->addItem("90°", 90);
	rotation->addItem("180°", 180);
	int index = rotation->findData(Scale::rotation);
    rotation->setCurrentIndex(index);
    QObject::connect( rotation, &QComboBox::activated, [=]()->void{ activated(rotation); });
    sizeLayout->addWidget(rotation);
    QLabel *label = new QLabel();
	label->setText("The rotation of the map");
	sizeLayout->addWidget(label);
    sizeLayout->addStretch();
    layout->addLayout(sizeLayout);
    
    QCheckBox *check = new QCheckBox("Facing &matters", this);
	check->setChecked(CentralFrame::facingMatters);
	QObject::connect( check, &QAbstractButton::clicked, [=]()->void{ facingOption(check->isChecked()); });
    layout->addWidget(check);
    
    sizeLayout = new QHBoxLayout();
    QLabel *box = new QLabel();
    box->setFixedHeight(20);
    box->setFixedWidth(40);
    box->setStyleSheet("border: 1px solid black");
    QPixmap pixmap(box->size());
    pixmap.fill(QColor::fromString(Counter::selectionColor));
	box->setPixmap(pixmap);
	box->show();
	sizeLayout->addWidget(box);
    label = new QLabel("Color for selected units");
    sizeLayout->addWidget(label);
    QPushButton *button = new QPushButton("Select color");
    QObject::connect(button, &QPushButton::released, [=]()->void{ colorSelection(box); });
	sizeLayout->addWidget(button);
	sizeLayout->addStretch();
    layout->addLayout(sizeLayout);
    
    layout->addStretch();
    
    QWidget *tab1 = new QWidget();
    tab1->setLayout(layout); 
    
    (void)tabs->addTab(tab1, "General");
    
    
    
    QVBoxLayout *layout2 = new QVBoxLayout();
    
  
    sizeLayout = new QHBoxLayout();
	label = new QLabel("Your nick: ");
	sizeLayout->addWidget(label);
	nickbox = new QLineEdit();	
	QObject::connect(nickbox, &QLineEdit::editingFinished, [=]()->void{ io->saveConfig(); });
    sizeLayout->addWidget(nickbox);
    sizeLayout->addStretch();
    layout2->addLayout(sizeLayout);
    layout2->addStretch();
	
	QWidget *tab2 = new QWidget();
    tab2->setLayout(layout2);
    
    (void)tabs->addTab(tab2, "User");
    
    
    
    
    QVBoxLayout *layout3 = new QVBoxLayout();
    
    
    QLabel *label2 = new QLabel("The restrictions your opponent has with counters owned by you:");
    layout3->addWidget(label2);
    layout3->addSpacing(10);
    
    createRightsBox(0, "Hidden counters are invisible.", false);
    layout3->addWidget(rights[0]);
    createRightsBox(1, "Concealed counters can not be seen.", false);
    layout3->addWidget(rights[1]);
    createRightsBox(2, "Concealed counters can not be revealed.", false);
    layout3->addWidget(rights[2]);
	createRightsBox(3, "No right-click context menu.", true);
	QObject::connect(rights[3], &QCheckBox::clicked, [=]()->void{ myOwnershipRights.NoMenu = rights[3]->isChecked(); });
    layout3->addWidget(rights[3]);
    createRightsBox(4, "Counters can not be deleted.", false);
    layout3->addWidget(rights[4]);
	createRightsBox(5, "Counters can not be flipped.", false);
    layout3->addWidget(rights[5]);
    createRightsBox(6, "Counters can not be moved or rotated.", false);
    layout3->addWidget(rights[6]);
	createRightsBox(7, "A stack can not be opened.", false);
    layout3->addWidget(rights[7]);
	createRightsBox(8, "A stack can not be inspected (no hoover window).", false);
    layout3->addWidget(rights[8]);
    layout3->addStretch(); 
    sizeLayout = new QHBoxLayout();
    button = new QPushButton("Make current rights default");
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    QObject::connect(button, &QPushButton::released, [=]()->void{ io->saveConfig(); });
	sizeLayout->addWidget(button);
	sizeLayout->addStretch(); 
	layout3->addLayout(sizeLayout);
    
    updateRightsBox();
    
    QWidget *tab3 = new QWidget();
    tab3->setLayout(layout3);
	
	(void)tabs->addTab(tab3, "Ownership");
	
	
	
	
	
	QVBoxLayout *layout4 = new QVBoxLayout();
	
	
	sizeLayout = new QHBoxLayout();
	button = new QPushButton("Create a User Key");
    QObject::connect(button, &QPushButton::released, [=]()->void{ createUserKey(); });
    sizeLayout->addWidget(button);
    QLabel *redlabel = new QLabel();
    redlabel->setTextFormat(Qt::RichText);  
    redlabel->setText("  The User Key file will be set to read-only. <font color=red>It is advisable<br>" 
					  "  to create only one User Key and never delete it.</font>");
	//QPalette palette;
	//palette.setColor(QPalette::WindowText, Qt::red);
	//redlabel->setPalette(palette);				   
    sizeLayout->addWidget(redlabel);
    sizeLayout->addStretch(); 
    layout4->addLayout(sizeLayout);
    
    
    sizeLayout = new QHBoxLayout();
    button = new QPushButton("Create a Urid");
    QObject::connect(button, &QPushButton::released, [=]()->void{ createUrid(); });
	sizeLayout->addWidget(button);
	label = new QLabel("    Make your own Unified Resource Identifier (Urid). It is\n"
					   "    best to create only one Urid and never delete it.");
	sizeLayout->addWidget(label);
	sizeLayout->addStretch(); 
	layout4->addLayout(sizeLayout);
	
	layout4->addSpacing(10);
	
	
	sizeLayout = new QHBoxLayout();
	label = new QLabel("File name for User Key:");
    sizeLayout->addWidget(label);
    layout4->addLayout(sizeLayout);
    
	sizeLayout = new QHBoxLayout();
	userkeybox = new QLineEdit();
	QObject::connect(userkeybox, &QLineEdit::textChanged, [=]()->void{ io->saveConfig(); });
	userkeybox->setReadOnly(true);
	userkeybox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizeLayout->addWidget(userkeybox); 
       
	button = new QPushButton("Select file");
	button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    QObject::connect(button, &QPushButton::released, [=]()->void{ setKeyFile(); });
	sizeLayout->addWidget(button);
    layout4->addLayout(sizeLayout);
    
    
    
    sizeLayout = new QHBoxLayout();
	label = new QLabel("File name for Urids:");
    sizeLayout->addWidget(label);
    layout4->addLayout(sizeLayout);
    
	sizeLayout = new QHBoxLayout();
	uridbox = new QLineEdit();
	QObject::connect(uridbox, &QLineEdit::textChanged, [=]()->void{ io->saveConfig(); });
	uridbox->setReadOnly(true);
	uridbox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    sizeLayout->addWidget(uridbox); 
       
	button = new QPushButton("Select file");
	button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    QObject::connect(button, &QPushButton::released, [=]()->void{ setUridFile(); });
	sizeLayout->addWidget(button);
    layout4->addLayout(sizeLayout);
    
    
    
    
    
    
    layout4->addSpacing(15);
    
    sizeLayout = new QHBoxLayout();
	label = new QLabel("List of Urids (including your own) by description:");
    sizeLayout->addWidget(label);
    layout4->addLayout(sizeLayout);
    
    
    
    sizeLayout = new QHBoxLayout();
    uridlist = new QListWidget();
    uridlist->setFrameShape(QFrame::Panel);
	uridlist->setFrameShadow(QFrame::Sunken);
	uridlist->setLineWidth(1);
    uridlist->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    
   
    sizeLayout->addWidget(uridlist);
    layout4->addLayout(sizeLayout);
    
    
    
    sizeLayout = new QHBoxLayout();
    button = new QPushButton("Add Urid");
	button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    QObject::connect(button, &QPushButton::released, [=]()->void{ addUrid(); });
	sizeLayout->addWidget(button);
	button = new QPushButton("Remove selected Urid");
	button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    QObject::connect(button, &QPushButton::released, [=]()->void{ removeUrid(); });
	sizeLayout->addWidget(button);
	
	sizeLayout->addStretch();
    layout4->addLayout(sizeLayout);
  
	
	
	
	QWidget *tab4 = new QWidget();
    tab4->setLayout(layout4);
	
	(void)tabs->addTab(tab4, "Keys");
	
	
}


void Settings::createRightsBox(int n, QString text, bool enabled)
{
	rights[n] = new QCheckBox(text, this);
	rights[n]->setEnabled(enabled);
}


void Settings::updateRightsBox()
{
	rights[0]->setChecked(myOwnershipRights.Hidden);
	rights[1]->setChecked(myOwnershipRights.NotSeen);
	rights[2]->setChecked(myOwnershipRights.NotRevealed);
	rights[3]->setChecked(myOwnershipRights.NoMenu);
	rights[4]->setChecked(myOwnershipRights.NoDelete);
	rights[5]->setChecked(myOwnershipRights.NoFlip);
	rights[6]->setChecked(myOwnershipRights.NoMove);
	rights[7]->setChecked(myOwnershipRights.NoOpen);
	rights[8]->setChecked(myOwnershipRights.NoHoover);
}


std::string Settings::streamRightsBox()
{
	std::string result("");
	
	for (int i = 0; i < RIGHTS; i++)
		if (rights[i]->isChecked())
			result += '1';
		else
			result += '0';
			
	return result;
}


void Settings::setRightsBox(std::string str)
{
	for (int i = 0; i < RIGHTS; i++)
		rights[i]->setChecked(str.at(i) == '1');
}


void Settings::offsetOption(bool checked)
{
	
	Counter::haveOffset = !checked;
	
	Window::getInstance("main")->frame->repaint();
	//Overlay::overlay->setMasks();	
	Overlay::overlay->update();	
	
}


void Settings::openOption(bool checked)
{
	
	CentralFrame::openStackoffset = !checked;
	
}


void Settings::activated(QComboBox *rotation)
{
	
	QVariant variant = rotation->currentData();
	int degrees = variant.value<int>();
	Scale::rotation = degrees;
	
	scaled->resourceScaleRotate("main", Window::getInstance("main")->frame->backgroundID);
	
	
	Counter::setGUI("main");
	

	Window::getInstance("main")->frame->repaint();
	// force redraw of mask layer
	Overlay::overlay->clearMask();
	
}



void Settings::facingOption(bool checked)
{
	
	CentralFrame::facingMatters = checked;
		
	Counter::setGUI("main");
	Window::getInstance("main")->frame->repaint();
		
}


void Settings::colorSelection(QLabel *box)
{
	
	QColorDialog *dialog = new QColorDialog();
	dialog->setOptions(QColorDialog::DontUseNativeDialog);
	dialog->setCurrentColor(QColor::fromString(Counter::selectionColor));
	dialog->open();
	QObject::connect(dialog, &QDialog::accepted, [=](){
		QColor color = dialog->selectedColor();
		QString string = color.name(QColor::HexRgb);
		Counter::selectionColor = string;
		QPixmap pixmap(box->size());
		pixmap.fill(QColor::fromString(Counter::selectionColor));
		box->setPixmap(pixmap);
		Counter::setGUI("main");
	});
	
}


void Settings::loadKey()
{
	
	Settings::userKeyExists = false;
	
		
	if (!userkeybox->text().isEmpty())
	{
		
		std::fstream fs(userkeybox->text().toStdString(), fs.binary | fs.in);

		if (!fs.is_open())
		{ 
			//Luau::error(42, 1, userkeybox->text().toStdString().c_str());
			// bug; lua global "error" not yet defined
			printf("Failed to open file %s\n", userkeybox->text().toStdString().c_str());
			return;
		}

		char s[USERKEYHEXS + 1];
		
		fs.read(s, USERKEYHEXS + 1);
			
		fs.close();	
		
		
		strncpy(Settings::userKey, s, USERKEYHEXS);
		
		Settings::userKeyExists = true;	
		
		
	}
	
}


void Settings::setKeyFile()
{
												
	QString fileName = QFileDialog::getOpenFileName(this, "Select User Key",
													QDir::homePath(),
													"GameTop Userkey File *.gkey (*.gkey);;All Files(*.*)");																								
						
	if (!fileName.isEmpty())												
		userkeybox->setText(fileName);
	
	loadKey();
	
	ToolBar::init();
	
}


void Settings::setUridFile()
{
												
	QString fileName = QFileDialog::getOpenFileName(this, "Urid File",
													QDir::homePath(),
													"GameTop Urid File *.guid (*.guid);;All Files(*.*)");																																				
						
	if (!fileName.isEmpty())
	{												
		uridbox->setText(fileName);
		readUrids();
	}
	
}


void Settings::addUrid()
{
	
	QString uridstring = 
		Window::textInput("Add Urid", "Paste the urid string (256 hexadecimal characters) here:", "");
	
	
	if (uridstring.isEmpty())
		return;
	
	//check right length
	if (uridstring.length() != 2*Settings::USERKEYHEXS)
		return;
	
	// chech if hex	
	QRegularExpression hexMatcher("^[0-9A-F]{256}$", QRegularExpression::CaseInsensitiveOption);
	QRegularExpressionMatch match = hexMatcher.match(uridstring);

	if (!match.hasMatch())
		return;
		
		
		
	QString description = 
		Window::textInput("Add a Urid", "Give a description of the Urid so that it can be recognized in the list of Urids:", "");	
	
	if (description.isEmpty())
		return;
	
	if (description == "none")
		return;	
	
	QListWidgetItem *item = new QListWidgetItem(description);
	
	uridlist->addItem(item);
	
	
	
	std::string urid = uridstring.toStdString();
	
	char arr[2*Settings::USERKEYHEXS + 1];

    strcpy(arr, urid.c_str());
    
	strncpy(Settings::players[description.toStdString()], &arr[0], 2*Settings::USERKEYHEXS);
	

	
	
	writeUrids();
	
}


void Settings::removeUrid()
{
	
	QListWidgetItem *selected = uridlist->currentItem();

	// nothing selected
	if (selected == nullptr)
		return;

    
	QString player = selected->text();
	
	delete selected;
	
		
	
	for (auto it = players.begin(); it != players.end(); )
    {
        if (it->first == player)
            it = players.erase(it);
        else
            ++it;
    }
	
	writeUrids(); 	     
	
}






extern "C" {
	
	
	char *createPrime()
	{
	
		int bits = Settings::USERKEYBITS;
		

		BIGNUM *p = BN_new();

		BN_generate_prime_ex(p, bits, 1, NULL, NULL, NULL);


		int rtn = BN_check_prime(p, NULL, NULL);

		if (rtn != 1) 
		{
			Luau::error(44, 0);
			return NULL;
		}

		
		BN_CTX *ctx = BN_CTX_new();

		BIGNUM *p_one = BN_new();

		BIGNUM *p_two = BN_new();
		BIGNUM *p2 = BN_new();

		BN_dec2bn(&p_one,"1");
		BN_dec2bn(&p_two,"2");

		BN_sub(p2, p, p_one);
		BN_div(p2, NULL, p2, p_two, ctx);


		rtn = BN_check_prime(p2, NULL,NULL);

		if (rtn != 1)
		{ 
			Luau::error(43, 0);
			return NULL;
		}
		
		// BN_free(BIGNUM *a);
		return BN_bn2hex(p);
	}
	
	char *createurid(char *userkey)
	{
		
		BIGNUM *key = NULL;
		int result = BN_hex2bn(&key, userkey);
		
		if (result != Settings::USERKEYHEXS)
			return NULL;
			
		
		BN_CTX *ctx= BN_CTX_new();
		
		BIGNUM *factor = BN_new();
		BN_generate_prime_ex(factor, Settings::USERKEYBITS, 1, NULL, NULL, NULL);
		
		BIGNUM *urid = BN_new();
		
		result = BN_mul(urid, key, factor, ctx);
		
		if (result == 0)
			return NULL;
			

		return BN_bn2hex(urid);
	}
	
	bool divides(char *urid, char *key)
	{
		BIGNUM *rem = BN_new();
		BN_CTX *ctx= BN_CTX_new();
		
		BIGNUM *u = BN_new();
		int res = BN_hex2bn(&u, urid);
		
		if (res != 2*Settings::USERKEYHEXS)
			return 0;
			
		BIGNUM *k = BN_new();
		res = BN_hex2bn(&k, key);
		
		if (res != Settings::USERKEYHEXS)
			return 0;
		
		
		res = BN_mod(rem, u, k, ctx);
	
		if (res == 0)
			return 0;

		int num = atoi(BN_bn2dec(rem));
		
		return num == 0; 
		
	}
		
		
	 
		
	
}	// extern "C"





void Settings::createUserKey()
{
	
	char s[USERKEYHEXS + 1];
	
	strcpy(s, createPrime());
	
	
	
	QString fileName;
	
	
	
	fileName = QFileDialog::getSaveFileName(this, "Save User Key",
											QDir::homePath(),
											"GameTop Userkey File *.gkey (*.gkey);;All Files(*.*)");
											
	if (!fileName.isEmpty())
	{									
		QFileInfo fileInfo(fileName);
		fileName = fileInfo.absoluteFilePath() + ".gkey";										
	}

	
	
													
	if (!fileName.isEmpty())	
	{
	
		std::fstream fs(fileName.toStdString(), fs.binary | fs.out);

		if (!fs.is_open())
		{
			Luau::error(42, 1, fileName.toStdString().c_str());
			return;
		}
		
		fs.write(s, strlen(s) + 1);
		
		fs.close();
		
		
		// set user key file to read-only; may not work on Windows (lol)
		
		std::filesystem::permissions(
			fileName.toStdString(),
			std::filesystem::perms::owner_write | std::filesystem::perms::group_all | std::filesystem::perms::others_all,
			std::filesystem::perm_options::remove
		);
		
		userkeybox->setText(fileName);
		
		strncpy(Settings::userKey, s, USERKEYHEXS);
		
		Settings::userKeyExists = true;
		
		
		QMessageBox confirmation(this);
		confirmation.setWindowTitle("Create User Key");
		confirmation.setText("A user key has been created.");
		confirmation.exec();
		
		
	}
	
}


void Settings::createUrid()
{
	
	if (!Settings::userKeyExists)
	{
		Luau::error(45, 0);
		return;
	}
	
	
	// presume memory of t is freed after this function exits
	
	char *t; 
	
	t = createurid(&Settings::userKey[0]);
	
	
	if (t == NULL)
		return;
	
	
	QString description = 
		Window::textInput("Urid Description", "Give a description of the Urid so that it can be recognized in the list of Urids:", "");
		
	if (description.isEmpty())
		return;
	
	if (description == "none")
		return;
	
	
	QListWidgetItem *item = new QListWidgetItem(description);
	
	uridlist->addItem(item);
		
	strncpy(Settings::players[description.toStdString()], t, 2*Settings::USERKEYHEXS);
	
	
	
	
	writeUrids();
	
}


void Settings::readUrids()
{
	
	if (uridbox->text().isEmpty())
		return;
		
	
	std::fstream fs(uridbox->text().toStdString(), fs.binary | fs.in);

	if (!fs.is_open())
	{
		Luau::error(42, 1, uridbox->text().toStdString().c_str());
		return;
	}
	
	
	uridlist->clear();
	Settings::players.clear();
	/*
	for (int i = uridlist->count() - 1; i >= 0; --i) 
	{
        QListWidgetItem* item = uridlist->takeItem(i); 
        delete item; 
    }
    */
	
	

	while (fs)
	{
		
		std::string desc;

		std::getline(fs, desc, '\0');
		
		if (desc.size() == 0)
			return;
		
		
		QListWidgetItem *item = new QListWidgetItem(QString::fromStdString(desc));
		
		uridlist->addItem(item);
		
		
	
		
		char urid[2*USERKEYHEXS];
		
		fs.read(urid, 2*USERKEYHEXS);
		
		strncpy(Settings::players[desc], &urid[0], 2*Settings::USERKEYHEXS);

	
	}
	
	fs.close();
	

	config->updateComboBoxes();
	
	
}


void Settings::writeUrids()
{
	
	QString fileName;
	
	
	if (!uridbox->text().isEmpty())
		fileName = uridbox->text();
	else
	{
		fileName = QFileDialog::getSaveFileName(this, "Save Urid",
												QDir::homePath(),
												"GameTop Urid File *.guid (*.guid);;All Files(*.*)",
												nullptr,
												QFileDialog::DontConfirmOverwrite);
		if (!fileName.isEmpty())
		{									
			QFileInfo fileInfo(fileName);
			fileName = fileInfo.absoluteFilePath() + ".guid";										
		}
	}
	
													
	if (!fileName.isEmpty())	
	{
	
		std::fstream fs(fileName.toStdString(), fs.binary | fs.out);

		if (!fs.is_open())
		{
			Luau::error(42, 1, fileName.toStdString().c_str());
			return;
		}
		
		
		for (auto & [player, urid] : Settings::players)
		{
			fs.write(player.c_str(), player.size() + 1);			
			fs.write(urid, 2*Settings::USERKEYHEXS);			
		}
		
		fs.close();
		
		
		// update comboboxes
		config->updateComboBoxes();
		
	}
	
}




void Settings::init()
{
	
	readUrids();
	
	// load userkey
	loadKey();
	
}




Settings::Urid *Settings::myUrid()
{
	
	if (Settings::userKeyExists)
		for (auto & [player, urid] : players)
		{	
			bool result = divides(&urid[0], Settings::userKey);
			
			if (result)
				return &urid;	
		}
	
	return nullptr;
	
}


Settings::Urid *Settings::sideUrid(std::string side)
{
	
	if (Settings::userKeyExists)
		for (auto & urids : Configure::urids)
		{	
			if (urids.side == side)
				return &urids.urid;
		}
	
	return nullptr;
	
}


std::string Settings::mySide()
{
	
	for (auto urids : Configure::urids)
	{	
		if (urids.side != "none")
		{
			bool result = divides(&urids.urid[0], Settings::userKey);
		
			if (result)
				return urids.side;
		}
	}
	
	return "none";
	
}


bool Settings::isMySide(std::string side)
{
	
	// test if widget has a side and if side is mine
	
	if (side.empty())
		// no side is always my side
		return true;
	
	const Urid *myurid= myUrid();
	const Urid *sideurid = sideUrid(side);
	
	if (myurid == nullptr)
		// error
		return false;
	
	if (sideurid == nullptr)
		// no urid for side defined, side is mine 
		return true;
	
	if (strncmp((char *)sideurid, (char *)myurid, 2*Settings::USERKEYHEXS) == 0)
		return true;
		
	
	return false;
	
}


std::string Settings::myNick()
{
	
	if (!settings->nickbox->text().isEmpty())
		return settings->nickbox->text().toStdString();
	
	return "none";
	
}


bool Settings::isOwner(std::string id)
{
	
	
	Counter *counter = Counter::repository[id];
	

	
	std::string side = "none";
	
	for (auto urid : Configure::urids)
		if (urid.side == counter->side)
			side = urid.side;
	
	bool found = false;
	
	if (Settings::userKeyExists)
		for (auto urids : Configure::urids)
			if (urids.side == side)
			{	
			
				bool result = divides(&urids.urid[0], Settings::userKey);
				
				if (result)
					found = true;
					
			}
		
	
	return found;
	
}



bool Settings::isSide(std::string side)
{
	
	bool is = true;
	
	
	if (side.empty() or side == "none")
		return is;
	
	
	if (Settings::userKeyExists)
		for (auto urids : Configure::urids)
			if (urids.side == side)
			{		
				bool result = divides(&urids.urid[0], Settings::userKey);

				if (!result)
					return false;		
			}
		
	
	return is;
	
}
