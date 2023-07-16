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
	
		static PopupEntries getTraits(const char *name);
		static void doAction(const char *actionName);
		static void doEvent(const char *eventName, const char *id, const char *trait, const char *key, int value);
		static void undo();
		static void redo();
		
		
		Luau();
		
		
		
		void compileScript();
		void startVM();
		void closeVM();
	
};
