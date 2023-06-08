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
		static void doAction(const char *name);
		
		
		Luau();
		
		
		
		void compileScript();
		void startVM();
		void closeVM();
	
};
