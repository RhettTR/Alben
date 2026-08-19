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
			std::string entryfield;
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
		static bool findTrait(const char *id, const char *trait, const char *field, std::string &value);
		static Counter::Table getDecks();
		static Counter::Table getDeck(const char *id);
		static Turn getTurn();
		static void setTurn(Turn turn);
		static void noAction(bool value);
		static void ifAction();
		static void updateId();
		static void updateTopZorder();
		static void updateBottomZorder();
		static Counter::Table findAllSides();
		static void doAction(const char *window, const char *id, const char *trait, const char *field, const char *name, const char *actionId = nullptr);
		static void doEvent(const char *eventName, const char *id, const char *trait, const char *key, int value);
		static void doLog(const char *key, const char *text);
		static bool doTest(const char *tag, const char *trait, const char *id);
		static void doCreate(const char *fromId, const char *trait);
		static void doDelete(const char *id);
		static void reportText(const char *type, const char *text);
		static void reportMove(int id, const char *name, const char *fromtag, const char *totag, int fromX, int fromY, int toX, int toY);
		static bool beforeDrag(const char *id);
		static bool afterDrag(const char *window, const char *id, int dx, int dy, int &x, int &y, int &cx, int &cy);
		static void dropped(const char *tag, const char *id, int x, int y);
		static void moved(const char *name, const char *toid, int x, int y);
		static void deleted(const char *id);
		static void flipped(const char *id, const char *image);
		static void handlers(const char *id, const char *func);
		static bool selectable(const char *id);
		static void feed(int x, int y);
		static bool isFeeding();
		static void stopFeed();
		static bool menu(const char *id, const char *name, const char *marker);
		static bool menucounter(int id);
		static void pointoffset(const char *tag, int cx, int cy, float &xoff, float &yoff, int &xopen, int &yopen, int &shown);
		static void refresh();
		static void undo();
		static void redo();
		static void copyCounter(std::string fromId, const char *toId, int zorder, std::string window, int x, int y, int cx, int cy);
		static void moveCounter(const char *window, const char *id, int zorder, int x, int y, int cx, int cy);
		static void copyCard(const char *fromId, const char *toId, const char *oldtoId, int zorder, int x, int y);
		static int  loadCounter(Counter::Table table);
		static void updateCounter(Counter::Table table);
		static void ask();
		static void confirm();
		static void synchronize();
		static void done();
		static void stagedone();
		static void loadDeck(Counter::Table table);
		static void loadLog(Counter::Table table);
		static void setResourceKey(const char *id);
		static void updatePos(const char *window, const char *id, int zorder, int x, int y, int cx, int cy);
		static void updateMoved(const char *id, bool moved);
		static void saveStage();
		static void deleteAll();
		static void resetState();
		static void resetBase();
		static void getRange(int &savedPointer, int &stagePointer);
		static void logReset();
		static void logStep(bool oneStep);
		static void logAbort();
		static QString mapPlace(const char *window, int cx, int cy);
		
		
		
		Luau();
		
		
		
		void compileScript();
		static void callbackScript(std::string script);
		static void error(int no, int count, ...);
		
		void startVM();
		void closeVM();
		
		
};
