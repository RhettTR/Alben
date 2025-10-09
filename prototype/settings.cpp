#include <QtWidgets>


#include "counter.h"
#include "frame.h"
#include "overlay.h"
#include "scale.h"
#include "window.h"

#include "settings.h"


extern QWidget *container;
extern Scale *scaled;



std::string Settings::playerSide = "German";

Settings::OwnershipRights Settings::myOwnershipRights;
	
	 
	 



Settings::Settings(QWidget *parent) : QDialog(parent)
{
	this->setWindowTitle("Settings");
	this->resize(500,300);
	this->move(750, 650);
	
	
	
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
    
    
    QLabel *label2 = new QLabel("The restrictions your opponent has with counters owned by you:");
    layout2->addWidget(label2);
    layout2->addSpacing(10);
    
    QCheckBox *check1 = new QCheckBox("Hidden counters are invisible.", this);
    check1->setChecked(myOwnershipRights.Hidden);
    check1->setEnabled(false);
    layout2->addWidget(check1);
    
    QCheckBox *check2 = new QCheckBox("Concealed counters can not be seen.", this);
    check2->setChecked(myOwnershipRights.NotSeen);
    check2->setEnabled(false);
    layout2->addWidget(check2);
    
    QCheckBox *check3 = new QCheckBox("Concealed counters can not be revealed.", this);
    check3->setChecked(myOwnershipRights.NotRevealed);
    check3->setEnabled(false);
    layout2->addWidget(check3);
    
    QCheckBox *check4 = new QCheckBox("No right-click context menu.", this);
    check4->setChecked(myOwnershipRights.NoMenu);
    QObject::connect(check4, &QCheckBox::clicked, [=]()->void{ myOwnershipRights.NoMenu = check4->isChecked(); });
    layout2->addWidget(check4);
    
    QCheckBox *check5 = new QCheckBox("Counters can not be deleted.", this);
    check5->setChecked(myOwnershipRights.NoDelete);
    check5->setEnabled(false);
    layout2->addWidget(check5);
    
    QCheckBox *check6 = new QCheckBox("Counters can not be flipped.", this);
    check6->setChecked(myOwnershipRights.NoFlip);
    check6->setEnabled(false);
    layout2->addWidget(check6);
    
    QCheckBox *check7 = new QCheckBox("Counters can be moved or rotated.", this);
    check7->setChecked(myOwnershipRights.NoMove);
    check7->setEnabled(false);
    layout2->addWidget(check7);
    
    QCheckBox *check8 = new QCheckBox("A stack can not be opened.", this);
    check8->setChecked(myOwnershipRights.NoOpen);
    check8->setEnabled(false);
    layout2->addWidget(check8);
    
    QCheckBox *check9= new QCheckBox("A stack can not be inspected (no hoover window).", this);
    check9->setChecked(myOwnershipRights.NoHoover);
    check9->setEnabled(false);
    layout2->addWidget(check9);
    
    layout2->addStretch();
    
    
    QWidget *tab2 = new QWidget();
    tab2->setLayout(layout2);
	
	(void)tabs->addTab(tab2, "Ownership");
	
	
	
	
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
	
	
	Counter::setGUI();
	

	Window::getInstance("main")->frame->repaint();
	// force redraw of mask layer
	Overlay::overlay->clearMask();
	
}



void Settings::facingOption(bool checked)
{
	
	CentralFrame::facingMatters = checked;
		
	Counter::setGUI();
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
		Counter::setGUI();
	});
	
}
