#include <QtWidgets>

class CentralFrame : public QFrame
{
	
	public:
		
		std::string name;
	
		CentralFrame(QWidget *parent, std::string name);
		
		void mousePressEvent(QMouseEvent *event);
		void dragEnterEvent(QDragEnterEvent *event);
		void dropEvent(QDropEvent *event);
	
};
