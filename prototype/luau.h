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
		static void doCreate(const char *fromId, const char *trait);
		static bool beforeDrag(const char *id);
		static bool afterDrag(const char *id, int &x, int &y);
		static void handlers(const char *id, const char *func);
		static void undo();
		static void redo();
		static void copyCounter(const char *fromId, const char *toId, int zorder, int x, int y);
		static void updatePos(const char *id, int x, int y);
		static void updateMoved(const char *id, bool moved);
		
		
		Luau();
		
		
		
		void compileScript();
		void startVM();
		void closeVM();
		
		
};
