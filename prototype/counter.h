#ifndef COUNTER_H
#define COUNTER_H


#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "frame.h"
#include "settings.h"

#include <QtWidgets>



class CentralFrame;
class StackFrame;
class StackOpen;


template <class... Fs> struct Overload : Fs... { using Fs::operator()...; };
template <class... Fs> Overload(Fs...) -> Overload<Fs...>;



class Counter
{
	
	public:
	
	
		class QtCounter : public QLabel
		{
			public:
				Counter *owner;				
				QtCounter(Counter *owner, QFrame *parent);
				~QtCounter();
				QTimer *timer;
				void showRightClickMenu(QtCounter *counter);
				
			protected:
				virtual void mousePressEvent (QMouseEvent *e);
				virtual void mouseMoveEvent (QMouseEvent *e);
				virtual void leaveEvent (QEvent *e);
				virtual void mouseDoubleClickEvent (QMouseEvent *e);
				
			private:
				void do_activate (QAction *action);
				QPoint popupPoint;
				void hooverAction();	
		};
			
		
		class SystemMask
		{
			public:
					
				SystemMask(const char *name, const char *mask, int x, int y);
				
				int x;
				int y;
				std::string name;
				std::string mask;		
		};
		
		
		
		
		
		int id;						// id of this counter		
		std::string name;			// unique string id of this counter
		
		
		
		
		
		class Table;
		
		typedef std::variant<int, std::string> Leftside;
		typedef std::variant<double, bool, std::string, Table> Rightside;

		class Table : public std::map<Leftside, Rightside> {};
		
		
		struct State				// the subset of fields needed to render the counter/card
		{
			int x;					
			int y;
			bool moved;				// only true if trait		
			int degrees;			// only not zero if trait
			std::string image;		// name of current (flipped) image
			int zorder;				// position in a stack
			Table *overlays;		// masks & labels if any
			float opacity;			// opacity 0.0 to 1.0 (full)
									
		};
		
		
		Counter(Table *state);
		Counter(int id, Table *state);
		
		~Counter();
		
		
		CentralFrame *parentFrame;
		
		Counter::QtCounter *counter;
		
		QImage baseBuffer;		// 100% buffer
		QImage scaledBuffer;		
	
		
		
		int width;		
		int height;	
		int margin;				// the area around the counter for rendering masks
		
		int scaledWidth;		
		int scaledHeight;	
		int scaledMargin;
		
		
		static void deleteAll();
		
		static int nextId();
		static int topZorder();
		static void resetId();
		static void resetZorder();
		static Counter* findObj(const char *name);
		static bool snaptoDefaultGrid (Counter *counter, int &x, int &y);
		static void toggleSelect(const char *name);
		static void clearMoved();
		static void setDisabled(bool disable);
		
		void setImage();
		void setPos(int x, int y);
				
		unsigned long long getOwnershipField();
		void setOwnershipField(unsigned long long field);
		
		Settings::OwnershipRights getRights();	
		void setRights(Settings::OwnershipRights rights);
		
		static void setGUI();
		
		int incrementDegrees(); 
		
		
		State state;
		Table table;
		Counter *source;		// stores the source if the counter has a copy
							     					      
							      
		static float alpha;
		static bool  haveOffset;
		static int   stackOffset;
		static QString selectionColor;
		
				
		
		bool selected;			// true if this counter is selected
		bool doesNotStack;		// true if stacking is prohibited
		bool disabled;			// true prevent selection, moving and right-click
		
		
				
		static std::map<int, Counter *> counters;
		
		static std::map<int, Counter *> repository;
		
		static std::map<std::string, Counter::SystemMask *> masks;
		
			
	private:
	
		static int _id;			// id of latest counter	
		
		static int _lastZorder;	// the number itself is not important, 
								// only its relative value to other counters in a stack
								
		unsigned long long _ownershipField;			// holds a 64-bit number
		Settings::OwnershipRights _ownershipRights;	// bitfield of rights					
		
		
};
 
#endif // COUNTER_H
