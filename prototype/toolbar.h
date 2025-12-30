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
				ButtonAction(const char *id, std::string resourceName, QString buttonText, const QString toolTip,
							 std::function<void(void)>, const std::string);
				
		};
		
		class ToolButton : public QToolButton
		{
			public:
				std::string id;
				ToolButton(const char *id, std::string resourceName, const QString toolTip,
						   std::function<void(void)>, const std::string);
				
		};
		
		class Label : public QLabel
		{
			public:
				std::string id;		
				Label(std::string resourceName, const QString toolTip, int w, int h, const QString css);	
		};
		
		class MapSizeComboBox : public QComboBox
		{
			public:
				MapSizeComboBox(ToolBar *parent);
				ToolBar *parent;
				int findPlace(int forNewIndex);
				void editingFinished();
				void textActivated(QString text);
				static void insert(int zoom);
				static void setDefault(int zoom);				
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
	
		ToolBar(std::string tag, const QString title, QScrollArea *scrollArea = nullptr);
		~ToolBar();
		static ToolBar *getInstance(const char *instance);
		ToolButton *getToolButton(const char *id);
		void reset();
		std::string tag;
		bool toolbarPinned;
		void addImageButton(const char *id, std::string resourceName, QString buttonText, const QString toolTip, std::function<void(void)>, std::string = "");
		void setImageButton(std::string id, std::string resourceName, QString buttonText);
		void setLabelImage(std::string id, std::string resourceName, QString buttonText);
		void addLabel(std::string id, std::string str, const QString toolTip, int w, int h, const QString css);
		void setLabel(std::string id, QString text);
		void addseparator();
		void addSizeComboBox();
		
			
		void zoomIn();
		void zoomOut();
		
		void wheelIn(QPoint point);
		void wheelOut(QPoint point);
		
		void enabled(std::string id, bool value);
		
		static MapSizeComboBox *sizeBox;

		void zoom(float fraction);
		
		void showMenu(const char *menuid);
		void do_activate (QAction *action);
		
		
	private:
	
		QScrollArea *scrollArea;
		
		void zoomCoordinates(QPoint point, float newFraction);
		void zoomMiddle(float newFraction);
		void zoomFraction(QPoint point, float amount);
		void zoomIndex(int inc);
	
		static std::map<std::string, ToolBar *> instances;
		std::map<std::string, ToolButton *> buttons;
		
		
};


#endif // TOOLBAR_H
