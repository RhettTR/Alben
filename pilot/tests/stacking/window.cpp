#include "window.h" 


#define COLUMNS	3
#define SPACE	30	



	

Window::Window(QWidget *parent = nullptr) : QMainWindow(parent, Qt::Window | Qt::WindowMinimizeButtonHint)
{	
	
	this->reset();
	
	this->setWindowTitle("Counters");
	
	QPalette palette = this->palette();
	palette.setColor(QPalette::Window, Qt::white);
	this->setPalette(palette);	
	setAutoFillBackground(true);
	
    this->move(100, 100);
    this->resize(336, 500);
	
	this->show();
}


void Window::reset()
{
	this->columns = 0;
	this->rows = 0;
	this->width = 0;
	this->height = 0;
}


void Window::setPieceWidth(int width)
{
	
	this->width = width;
	
}


void Window::setPieceHeight(int height)
{
	
	this->height = height;
			
}


int Window::allocateX()
{
	
	if (columns == COLUMNS)
	{
		columns = 0;
		rows++;
	}
	
	return SPACE + ((columns++ % COLUMNS) * (SPACE + width));
	
}


int Window::allocateY()
{
	
	return SPACE + (rows * (SPACE + height));
	
}

