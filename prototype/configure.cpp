#include "configure.h"
#include "luau.h"
#include "toolbar.h"
#include "io.h"
#include "frame.h"



extern Configure *config;
extern Settings *settings;
extern IO *io;



Configure::ChoicePage *getPage(const char *choicepage);
std::map<std::string, Configure::ChoicePage *> Configure::ChoicePage::choicepages;
std::vector<Configure::Entry> Configure::urids;

QWidget *Configure::mapPage;
QLabel *Configure::mapWindow;
std::string Configure::mapResource;	



Configure::ChoicePage::ChoicePage(Configure *base, std::string tag) : QLabel(((QWidget *)base->stackedWidget))
{
	this->scrollArea = new QScrollArea;
	this->scrollArea->setWidget(this);
	
	choicepages[tag] = this;
	
	this->setObjectName(tag);
	base->stackedWidget->addWidget(this);
	
	base->pageComboBox->addItem(QString::fromStdString(tag));
	
	choicepages[tag]->setLayout(new QVBoxLayout());
	
	
}


void Configure::ChoicePage::resizeEvent(QResizeEvent* event)
{
	 QImage image = io->getImage(mapResource);
	 QSize scaledSize(mapWindow->size()); 
	 QImage scaled = image.scaled( scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	 mapWindow->setPixmap(QPixmap(QPixmap::fromImage(scaled)));
}


Configure::ChoicePage *Configure::getPage(const char *tag)
{
	std::string name = std::string(tag);
		
	for ( auto obj = Configure::ChoicePage::choicepages.begin(); obj != Configure::ChoicePage::choicepages.end(); ++obj  )
	
		if (obj->first == name)
			return obj->second;

	name = "'" + name + "'";
	
	Luau::error(40, 1, name.c_str()); 
	std::exit(1);
	
}



Configure::Configure(QWidget *parent)
{
	
	window = new Window(parent, "Configure", "config", "", false, 600, 800);


	pageComboBox = new QComboBox((QWidget *)window);
	pageComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	
	pageComboBox->setFixedHeight(26);
	pageComboBox->setFixedWidth(window->width() - 
		window->contentsMargins().right() - window->contentsMargins().left());
			
	stackedWidget = new QStackedWidget((QWidget *)window);
	
	
	mapPage = new ChoicePage(this, "Map");
	sidePage = new ChoicePage(this, "Sides");
		
	
	
	QObject::connect(pageComboBox, &QComboBox::activated,
			stackedWidget, &QStackedWidget::setCurrentIndex);
			
			
	window->centralWidget()->layout()->addWidget(pageComboBox);
	window->centralWidget()->layout()->addWidget(stackedWidget);


	
	window->setOptions("Sans Serif", 10, "Normal", "#000000");
	
	
	
	
	
	
	//(void)new Window::Text(sidePage, window, "text1", 140, 50, 250, 50);
	//((Window::Text *)window->widgets["text1"])->set("Your user key (256-bit prime) :");
	
	
	

}


Configure::~Configure()
{
	window->~Window();
}



void Configure::addListImage(QListWidget* list, std::string resource)
{
	QImage image = io->getImage(resource);
	

	QSize scaledSize(200, 200);

	QImage scaled = image.scaled( scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	
	QLabel *label = new QLabel();
    label->setPixmap(QPixmap(QPixmap::fromImage(scaled)));
    
    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(label->sizeHint());
    
    QVariant v;
	v.setValue(QString::fromStdString(resource));				
	item->setData(0, v);
	
					 
    list->addItem(item);    
    list->setItemWidget(item, label);
}


void Configure::itemClicked(QListWidgetItem *item)
{
	 QVariant v = item->data(0);
	 QString retrieved = qvariant_cast<QString>(v);
	 
	 mapResource = retrieved.toStdString();
	 
	 QImage image = io->getImage(mapResource);
	 QSize scaledSize(mapWindow->size());
	 
	 QImage scaled = image.scaled( scaledSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	 mapWindow->setPixmap(QPixmap(QPixmap::fromImage(scaled)));
	 
}


void Configure::createMapSelection()
{
	
	QHBoxLayout *total = new QHBoxLayout();
	
	
	QVBoxLayout *layout = new QVBoxLayout();
	
	mapWindow = new QLabel();
	
	mapWindow->setStyleSheet("border: 1px solid black;");
	
	mapWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	
	
	layout->addWidget(mapWindow);
	total->addLayout(layout);
	
	
	layout = new QVBoxLayout();
	
	QListWidget* list = new QListWidget();
	list->setFixedSize(200, 400);
	list->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
	
	addListImage(list, "map_classic_vassal");
    addListImage(list, "map_deluxe_vassal");
    
	mapResource = "map_classic_vassal";
    
	QObject::connect(list, &QListWidget::itemClicked,
					 list, [=]()->void{ itemClicked(list->currentItem()); });
	
	layout->addWidget(list);
	layout->addStretch();
	
	QPushButton *button = new QPushButton("Use this map");
	button->setMaximumWidth(100);
    QObject::connect(button, &QPushButton::released, [=]()->void{ setMapSelection(); });
	layout->addWidget(button);
	
	QPushButton *button1 = new QPushButton("Load setup");
	button1->setMaximumWidth(100);
    QObject::connect(button1, &QPushButton::released, [=]()->void{ io->loadSetUp("setup.gsav"); setSides(); });
	layout->addWidget(button1);
	
	
	
	
	
	total->addLayout(layout);

		
	((QVBoxLayout *)mapPage->layout())->addLayout(total);
	
}


void Configure::setMapSelection()
{
	Window::getInstance("main")->frame->setBackground(mapResource);
	Window::getInstance("main")->frame->update();
}


void Configure::createSideSelection(int row, std::string side)
{
					
	QLabel *label = new QLabel();
	label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	label->setText(QString::fromStdString(side));	
	grid->addWidget(label, row, 0);
	
	QComboBox *box = new QComboBox();
	box->setPlaceholderText("- select player -");
	for (auto const& [key, val] : Settings::players)	
		box->addItem(QString::fromStdString(key));
	QObject::connect(box, &QComboBox::activated, [=]()->void{ selected(label->text(), box->currentText()); });				 	
	grid->addWidget(box, row, 1);
	
	grid->addItem(new QSpacerItem(1, grid->rowMinimumHeight(row)), row, 2);			

}


void Configure::resetSideSelection()
{
	for (int n = 0; n < grid->rowCount(); n++)
	{
		QWidget* widget = grid->itemAtPosition(n, 1)->widget();				
		if (widget != nullptr)
		{	
			QComboBox *combo = dynamic_cast<QComboBox *>(widget);
			if (combo != nullptr)
				combo->setCurrentIndex(-1);		
		}		
	}
}


void Configure::setSides()
{
	
	if (Configure::urids.size() == 0)
	{
		// no game loaded, sides are determined by the module
		
		Counter::Table sides = Luau::findAllSides(); 
			
		for (auto obj = sides.begin(); obj != sides.end(); ++obj)
		{
			Configure::Entry entry; 					
			entry.side = std::get<std::string>(obj->first);
			if (entry.side != "none")
			{
				Configure::urids.push_back(entry);
				// NOTE: urid and rights are set when a side is selected
				
				// one "none" for every side
				entry.side = "none";
				// NOTE: ownership rights for "none" are always ignored
				Configure::urids.push_back(entry);
			}
			
		}	
	}
	
}


void Configure::init()
{
		
	// make map selection
	
	createMapSelection();
	
	
	// make side selection
	
	
	grid = new QGridLayout();
	
	grid->setColumnMinimumWidth(0, 100);
	grid->setColumnMinimumWidth(1, 300);
	grid->setColumnStretch(2, 1);
	
	
	setSides();
	
	
	int row = 0;
	
	for (Configure::Entry &entry : Configure::urids)
		if (entry.side != "none")
		{
			createSideSelection(row, entry.side);		
			row++;			
		}

	
	
	((QVBoxLayout *)sidePage->layout())->addLayout(grid);
	((QVBoxLayout *)sidePage->layout())->addStretch();
	
}


void Configure::selected(QString side, QString player)
{
		
	// connect side with player	
	for (Entry &entries : urids)	
		if (entries.side == side.toStdString())
		{
			strncpy(entries.urid, Settings::players[player.toStdString()], 2*Settings::USERKEYHEXS);
			// only set rights if this is your own side		
			if (Settings::mySide() == side.toStdString())
				entries.ownershipRights = Settings::myOwnershipRights;
		}
	
	
	// assign "none" urids; there is one "none" for each player
	
	int row = 0;
	
	for (Entry &entries : urids)
		if (entries.side == "none")
		{
			QWidget* widget = grid->itemAtPosition(row, 1)->widget();
			if (widget != nullptr)
			{	
				QComboBox *combo = dynamic_cast<QComboBox *>(widget);
				if (combo != nullptr)
				{
					QString cplayer = combo->currentText();
					strncpy(entries.urid, Settings::players[cplayer.toStdString()], 2*Settings::USERKEYHEXS);				
				}	
			}
			row++;
		}
		
		
	ToolBar::init();	

}


void Configure::setNewGame(bool newgame)
{
	
	if (newgame)
	{
		resetSideSelection();
		// delete urids
		/*for (Entry &entry : urids)
			std::fill_n(entry.urid, 2*Settings::USERKEYHEXS, '1');*/
		ToolBar::init();
	}
	
	
	config->sidePage->setDisabled(!newgame);
	this->newGame = newgame;	
}


void Configure::load()
{
	
	int n = 0;
	
	
	// set sides from loaded game
	
	for (const Configure::Entry &entry : Configure::urids) 
	{
		
		if (entry.side != "none")
		{
		
			Settings::Player player = "";
			
			for (const auto& [key, urid] : Settings::players)
				if (strncmp(urid, entry.urid, 2*Settings::USERKEYHEXS) == 0)
					player = key;
						
			if (player.empty())
				continue;
	
		
			QWidget *widget = grid->itemAtPosition(n, 1)->widget();
			
			if (widget != nullptr)
			{	
			
				QComboBox *combo = dynamic_cast<QComboBox *>(widget);
				
				if (combo != nullptr)
				{	
					int index = combo->findText(QString::fromStdString(player));
					combo->setCurrentIndex(index);										
				}
			}			
			
			n++;
		}
			
	}
	
	
	
	// set ownership rights

	for (const Configure::Entry &entry : Configure::urids)
		if (entry.side == Settings::mySide())
		{
			Settings::myOwnershipRights = entry.ownershipRights;
			settings->updateRightsBox();
		}
		
	
}


void Configure::updateComboBoxes()
{
	int n = 0;

	for (int n = 0; n < grid->rowCount(); n++)
	{
		QWidget* widget = grid->itemAtPosition(n, 1)->widget();
				
		if (widget != nullptr)
		{	
			QComboBox *combo = dynamic_cast<QComboBox *>(widget);
			
			if (combo != nullptr)
			{	
				combo->clear();
				
				for (auto const& [key, val] : Settings::players)	
					combo->addItem(QString::fromStdString(key));
			}
		}		
	}	
	
}
