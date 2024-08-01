#include "frame.h"
#include "window.h"
#include "overlay.h"





extern CentralFrame *repositoryFrame;




std::stack<std::any> Window::panes;


QList<Window::Pane*> Window::paneList;

		




Window::Pane::Pane(QWidget *parent) : QLabel(parent)
{
	
	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, Qt::white);
	this->setPalette(palette);	
	setAutoFillBackground(true);
	
	setContentsMargins(0, 0, 0, 0);
	
}
	
	
void Window::Pane::resizeEvent(QResizeEvent* event)
{
   QLabel::resizeEvent(event);
}



Window::ListBox::ListBox(QWidget *parent) : QListWidget(parent)
{
	QObject::connect(this, &QListWidget::itemClicked, 
					 this, [=]()->void{ itemClicked((ListItem *)this->selectedItems().at(0)); });
					 
	QObject::connect(this, &QListWidget::itemSelectionChanged, 
					 this, [=]()->void{ itemSelectionChanged(); });
}


void Window::ListBox::itemClicked(ListItem *item)
{	
	for (int i = 0; i < item->listWidget()->count(); ++i)
	{
		ListItem* lt = (ListItem *)item->listWidget()->item(i);
		lt->pane->setVisible(false);
	}
	
	item->pane->setVisible(true);	
}


void Window::ListBox::itemSelectionChanged()
{	
	
	ListItem *item = (ListItem *)this->selectedItems().at(0);
	
	for (int i = 0; i < item->listWidget()->count(); ++i)
	{
		ListItem* lt = (ListItem *)item->listWidget()->item(i);
		lt->pane->setVisible(false);
	}
	
	item->pane->setVisible(true);	
}



Window::ListItem::ListItem(const QString &text, ListBox *parent, Pane *paneParent, int type) : 
										QListWidgetItem(text, (QListWidget *)parent, type)
{
	Pane *pane = new Pane(paneParent);
	
	pane->setVisible(false);
	paneParent->layout()->addWidget(pane);
	
	
	this->pane = pane;
    
    parent->addItem(this);
    
    parent->setCurrentItem(parent->item(0));
    ((ListItem *)parent->currentItem())->pane->setVisible(true);
 

}



Window::ComboBox::ComboBox(QWidget *parent) : QComboBox(parent)
{
	QObject::connect(this, &QComboBox::activated, 
					 this, [=]()->void{ activated(this->currentIndex()); });
}

void Window::ComboBox::activated(int index)
{	
	
	for (int i = 0; i < this->count(); ++i)
	{
		QVariant variant = this->itemData(i);
		Pane *pane = variant.value<Pane*>();
		
		pane->setVisible(false);
	}
	
	
	
	QVariant variant = this->itemData(index);
	
	
	if ( strcmp(variant.typeName(), "Window::Pane*") == 0 )
	{
		Pane *pane = variant.value<Pane*>();		
		pane->setVisible(true); 
	}
		
}


Window::Window(QWidget *parent) : QMainWindow(parent, Qt::Window | Qt::WindowMinimizeButtonHint)
{	
	
	this->reset();
	
	this->setWindowTitle("Counters");
	
	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, Qt::white);
	this->setPalette(palette);	
	setAutoFillBackground(true);
	
    this->move(100, 100);
    this->resize(400, 250); 
    repositoryFrame->resize(400, 250);  
    
	
	this->show();
}



void Window::resizeEvent(QResizeEvent* event)
{
	QMainWindow::resizeEvent(event);
	 
	QLayoutItem *item = repositoryFrame->layout()->itemAt(0);
	
	if (item != nullptr)
	{	
		QWidget *widget = item->widget();	
		widget->resize(event->size());
	}		
}
	
	


		
Window::Pane* Window::getParent()
{

	// return current top pane
	

    Pane **pane = std::any_cast<Pane*>(&panes.top());  
  
	assert( pane != nullptr );
	
	
	
	if ((*pane)->layout() == nullptr)
		(*pane)->setLayout(new Overlay::FlowLayout(*pane));


	return *pane;
	
}



int lastLevel = 0;



void Window::root(int level)
{	
	
	Pane *pane = new Pane(repositoryFrame);
	
	paneList.append(pane);
	
	
	repositoryFrame->layout()->addWidget(pane);
	
	pane->resize(repositoryFrame->size());
	
	
	lastLevel = level;
	
	
	panes.push(pane);
	
}




void Window::tabs(int level)
{
	
	QTabWidget *w;
	
	
	
	Pane **parent = (Pane **)std::any_cast<Pane*>(&panes.top());
	
	assert( parent != nullptr );
	
	
	
	(*parent)->setLayout(new QVBoxLayout((*parent)));	
	(*parent)->layout()->setContentsMargins(QMargins());
	(*parent)->layout()->setSpacing(0);	
	
	
	
	
	w = new QTabWidget(*parent);
	
	
	(*parent)->layout()->addWidget(w);	
	w->resize((*parent)->size());
	
	
	lastLevel = level;
	
    panes.push(w);
  
}


void Window::tab(int level, std::string text)
{
	
	if (level == lastLevel)
		panes.pop();
	
		
	if (level < lastLevel) 			
		for (int i = 0; i <= lastLevel - level; i++)
			panes.pop();
	
	
	Pane **parent1 = std::any_cast<Pane*>(&panes.top());		
	QTabWidget **parent2 = std::any_cast<QTabWidget*>(&panes.top());
	
	assert( parent1 != nullptr || parent2 != nullptr );
	
	
	
	
	if (parent1 != nullptr)
	{
		
	}
	
	else
	
	{
		
		Pane *pane = new Pane(*parent2); 
		
		paneList.append(pane); 
    
		// IMPORTANT: you can not presume anything about what follows a tab;
		// therefore you can not set its layout here
		

		(void)(*parent2)->addTab(pane, QString::fromStdString(text));
		
	   
		pane->resize( ((QWidget*)(*parent2)->parent())->size() );
		
		panes.push(pane);
		
	}
	
	
		
	lastLevel = level;
    

}



void Window::listBox(int level)
{
	
    
	Pane **parent = (Pane **)std::any_cast<Pane*>(&panes.top());
		
	assert( parent != nullptr );
	
	
	
	(*parent)->setLayout(new QVBoxLayout((*parent)));	
	(*parent)->layout()->setContentsMargins(QMargins());
	(*parent)->layout()->setSpacing(0);		
	
   

	
	
	QSplitter *splitter = new QSplitter(Qt::Horizontal, *parent);
	
	splitter->setStyleSheet("QSplitter::handle{ background:gainsboro }");
	splitter->setHandleWidth(3);
	
		
	(*parent)->layout()->addWidget(splitter);
	
	
	
	
     
	ListBox *listWidget = new ListBox(splitter);
	
	
	
	listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	
	
	Pane *pane = new Pane(splitter);
	
	paneList.append(pane);
	
	pane->setLayout(new QVBoxLayout());
	pane->layout()->setContentsMargins(QMargins());
	
    
	splitter->addWidget(pane);
	splitter->addWidget(listWidget);

	QList<int> sizes;
    sizes.append(0.8 * splitter->sizeHint().width());
    sizes.append(0.2 * splitter->sizeHint().width());
    splitter->setSizes(sizes);
    
    
    lastLevel = level;
    
	panes.push(splitter);

	
}


void Window::listItem(int level, std::string text)
{

	if (level == lastLevel)
		panes.pop();
	
		
	if (level < lastLevel) 			
		for (int i = 0; i <= lastLevel - level; i++)
			panes.pop();
	
	
	Pane **parent1 = std::any_cast<Pane*>(&panes.top());		
	QSplitter **parent2 = std::any_cast<QSplitter*>(&panes.top());
	
	assert( parent1 != nullptr || parent2 != nullptr );
	
	
	
	if (parent1 != nullptr)
	{
		
	}
	
	else
	
	{
			
		ListItem *item = new ListItem(QString::fromStdString(text), 
									 (ListBox *)(*parent2)->widget(1), 
									 (Pane *)(*parent2)->widget(0));
    
		panes.push(item->pane);
	}


    lastLevel = level;
    
    
}


	
void Window::comboBox(int level)
{
	
	
	Pane **parent = (Pane **)std::any_cast<Pane*>(&panes.top());
	
	
	assert( parent != nullptr );
	
	
	(*parent)->setLayout(new QVBoxLayout((*parent)));	
	(*parent)->layout()->setContentsMargins(QMargins());
	(*parent)->layout()->setSpacing(0);	
			
   
	
	
	
	Pane *pane = new Pane(*parent);
	
	paneList.append(pane);
	
	pane->setLayout(new QVBoxLayout(pane));
	pane->layout()->setContentsMargins(QMargins());
	pane->layout()->setSpacing(0);
	
	pane->resize( (*parent)->size() );
	
	
	(*parent)->layout()->addWidget(pane);

	
	
	ComboBox *c = new ComboBox(pane);

	
	pane->layout()->addWidget(c);	
	
	
	
	lastLevel = level;
	
    
	panes.push(c);
	
}



void Window::comboItem(int level, std::string text)
{
	
	if (level == lastLevel)
		panes.pop();
	
		
	if (level < lastLevel) 			
		for (int i = 0; i <= lastLevel - level; i++)
			panes.pop();
	
	
	Pane **parent1 = std::any_cast<Pane*>(&panes.top());		
	ComboBox **parent2 = std::any_cast<ComboBox*>(&panes.top());
	
	
	assert( parent1 != nullptr || parent2 != nullptr );
	
	
	
	
	if (parent1 != nullptr)
	{
		
	}
	
	else
	{
	
		(*parent2)->addItem(QString::fromStdString(text)); 
	
		Pane *pane = new Pane((Pane *)(*parent2)->parent());
		
		paneList.append(pane);
		
		pane->setVisible(false);
		
		(*parent2)->setItemData((*parent2)->count() - 1, QVariant::fromValue(pane), Qt::UserRole);
	
		((Pane *)(*parent2)->parent())->layout()->addWidget(pane);
		
		(*parent2)->activated(0);
		
		
		
		panes.push(pane);
			
	}


	lastLevel = level;
	

}
	
	
void Window::reset()
{
	
	//  the code here is not finished !!!
	
	
	QMutableListIterator<Pane*> p(paneList);
	
	if (p.hasNext())
		delete p.next();
	
	
	paneList.clear();
	paneList.squeeze();

    
}
