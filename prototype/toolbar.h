#ifndef TOOLBAR_H
#define TOOLBAR_H


#include <functional>

#include <QtWidgets>




class ToolBar : public QToolBar
{
	
	public:
	
		class ButtonAction : public QAction
		{
			public:
				std::string id;		
				ButtonAction(std::string resourceName, const QString toolTip, std::function<void(void)>);	
		};
		
		class MapSizeComboBox : public QComboBox
		{
			public:
				MapSizeComboBox(ToolBar *parent);
				ToolBar *parent;
				int findPlace(int forNewIndex);
				void editingFinished();
				void textActivated(QString text);				
			protected:
				virtual void focusInEvent(QFocusEvent *e) override;
				virtual void focusOutEvent(QFocusEvent *e) override;
				virtual bool eventFilter(QObject *obj, QEvent *e) override;	
				
		};
		
		class MapSizeAction : public QWidgetAction 
		{
			public:
				MapSizeAction(QObject *parent);
		};
	
		ToolBar(QWidget *parent, QScrollArea *scrollArea);
		~ToolBar();
		void addImageButton(std::string resourceName, const QString toolTip, std::function<void(void)>);
		void addSizeComboBox();
			
		void zoomIn();
		void zoomOut();
		
		void wheelIn(QPoint point);
		void wheelOut(QPoint point);
		
		void enable(std::string id);
		void disable(std::string id);
		
		static MapSizeComboBox *sizeBox;
	
		
	private:
	
		QScrollArea *scrollArea;
		
		void zoom(float fraction);
		void zoomCoordinates(QPoint point, float newFraction);
		void zoomMiddle(float newFraction);
		void zoomFraction(QPoint point, float amount);
		void zoomIndex(int inc);
};


#endif // TOOLBAR_H
