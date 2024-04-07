#include <QtWidgets>
		
		
class Window : public QMainWindow
{

	public:
	
		Window(QWidget *parent);
		
		void reset();
		
		void setPieceWidth(int width);
		
		void setPieceHeight(int height);
		
		int allocateX();
		
		int allocateY();
		
	private:
	
		int columns, rows, width, height;
		
		
};
