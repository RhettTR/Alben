#include <QtWidgets>


#include "counter.h"
#include "frame.h"
#include "overlay.h"
#include "scale.h"

#include "settings.h"


extern QWidget *container;
extern CentralFrame *mapFrame;
extern Scale *scaled;




Settings::Settings(QWidget *parent) : QDialog(parent)
{
	this->setWindowTitle("Settings");
	this->resize(500,300);
	this->move(750, 650);
	QVBoxLayout *layout = new QVBoxLayout(this);
	
	
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
	
}


void Settings::offsetOption(bool checked)
{
	
	Counter::haveOffset = !checked;
	
	mapFrame->repaint();
	Overlay::overlay->setMasks();	
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
	
	scaled->resourceScaleRotate(CentralFrame::backgroundID);
	
	
	Counter::setGUI();
	
	Overlay::overlay->setMasks();
	mapFrame->repaint();
	
}



void Settings::facingOption(bool checked)
{
	
	CentralFrame::facingMatters = checked;
		
	Counter::setGUI();
	Overlay::overlay->setMasks();
	mapFrame->repaint();
		
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
