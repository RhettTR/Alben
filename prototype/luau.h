#include <string>
#include <vector>

#include "counter.h"


class Luau
{

	public:
	
		struct Popupentry
		{
			std::string entryname;
			std::string entrytrait;
			std::string entryid;
			std::string entryaction;
			std::string entryactions;
			bool entrytest;
		};
		
		struct Turn
		{
			int turn;
			int phase;
		};
		
		typedef struct std::vector<Popupentry> PopupEntries;
	
		static PopupEntries getTraits(const char *tag, const char *id);
		static Counter::Table getTraits(const char *window, int id);
		static Counter::Table getTrait(const char *window, const char *id);
		static Counter::Table getDecks();
		static Counter::Table getDeck(const char *id);
		static Turn getTurn();
		static void setTurn(Turn turn);
		static void doAction(const char *window, const char *id, const char *trait, const char *name);
		static void doEvent(const char *eventName, const char *id, const char *trait, const char *key, int value);
		static bool doTest(const char *tag, const char *trait, const char *id);
		static void doCreate(const char *fromId, const char *trait);
		static void doDelete(const char *id);
		static bool beforeDrag(const char *id);
		static bool afterDrag(const char *window, const char *id, int dx, int dy, int &x, int &y);
		static void dropped(const char *name, const char *fromid, const char *toid, int x, int y);
		static void moved(const char *name, const char *toid, int x, int y);
		static void deleted(const char *id);
		static void flipped(const char *id, const char *image);
		static void handlers(const char *id, const char *func);
		static bool selectable(const char *id);
		static bool menu(const char *id, const char *name, const char *marker);
		static bool menucounter(int id);
		static void pointoffset(const char *tag, int x, int y, int dx, int dy, float &xoff, float &yoff, int &open, int &shown);
		static void refresh();
		static void undo();
		static void redo();
		static void copyCounter(const char *fromId, const char *toId, int zorder, const char *window, int x, int y);
		static void copyCard(const char *fromId, const char *toId, const char *oldtoId, int zorder, int x, int y);
		static int  loadCounter(Counter::Table table);
		static void loadDeck(Counter::Table table);
		static void loadLog(Counter::Table table);
		static void setResourceKey(const char *id);
		static void updatePos(const char *window, const char *id, int x, int y);
		static void updateMoved(const char *id, bool moved);
		static void updateSide(const char *side);
		static void saveStage();
		static void deleteAll();
		static void resetState();
		static void getRange(int &savedPointer, int &stagePointer);
		static void logReset();
		static void logStep(bool oneStep);
		static void logAbort();
		static QString mapPlace(const char *window, int dx, int dy, int x, int y);
		
		
		
		Luau();
		
		
		
		void compileScript();
		static void callbackScript(std::string script);
		
		void startVM();
		void closeVM();
		
		
};
