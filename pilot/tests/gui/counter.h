#include <iostream>
#include <vector>
#include <string>
#include <map>


#include <QtWidgets>





class Counter
{
	
	public:
	
		class QtCounter : public QLabel
		{
			public:
				Counter *owner;
				QtCounter(Counter *owner, QWidget *parent);
			protected:
				virtual void mousePressEvent (QMouseEvent * e);
				virtual void paintEvent(QPaintEvent *e);
			private:
				void do_activate (QAction *action);
		};
	
		std::string name;			// unique string id of this counter 
		
		struct State
		{
			int x;
			int y;
			bool moved;				// only true if trait		
			int degrees;			// only not zero if trait
			std::string name;		// name of current (flipped) image	
		};
		
		
		Counter(const char *name, int x, int y);
		
		~Counter();
		
		
		
		Counter::QtCounter *counter;
		
		QImage baseBuffer;
	
		
		int width;		
		int height;	
		
		static void deleteAll();
		
		static Counter* findObj(const char *name);
		
		void setImage();
		
		void setMovedMask(const char *mask, bool moved);
		
		int incrementDegrees(); 
		
		void rotateImage();
		
		State state;
		
		static std::map<std::string, std::string> resources;
							     					      
							      
		static float alpha;
		
		static int mask_x;
		static int mask_y;
			
		
			
	private:
	
		
		const int id = _id++;	// id of this counter		
		
		static int _id;			// id of latest counter	
		
		static std::map<int, Counter *> objects;
		
};
 
