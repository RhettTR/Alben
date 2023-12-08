#include <string>
#include <vector>

class Luau
{

	public:
	
		struct Popupentry
		{
			std::string entryname;
			const char *entryaction;
		};
		
		typedef struct std::vector<Popupentry> PopupEntries;
	
		static PopupEntries getTraits(const char *window, const char *id);
		static void doAction(const char *window, const char *id, const char *name);
		static void doEvent(const char *eventName, const char *id, const char *trait, const char *key, int value);
		static void doCreate(const char *fromId, const char *trait, const char *toId, int degrees, int x, int y);
		static void undo();
		static void redo();
		static void copyCounter(const char *fromId, const char *toId, int degrees, int x, int y);
		
		
		Luau();
		
		
		
		void compileScript();
		void startVM();
		void closeVM();
	
};
