#ifndef __CCONSOLE__
#define __CCONSOLE__

// In essence, CConsole is our nice generic class for outputting display
// As long as the interface is kept the same, we can display whatever we wish to
// We store a set of coordinates being the window corner and size, for systems with windowing support, which describes the window we are in
#include <utility>

//===================================================================================================
constexpr auto MAX_CONSOLE_BUFF = 512;

const UI08 CNORMAL	= 0;
const UI08 CBLUE	= 1;
const UI08 CRED		= 2;
const UI08 CGREEN	= 3;
const UI08 CYELLOW	= 4;
const UI08 CBWHITE	= 5;

//===================================================================================================
// CConsole
//===================================================================================================
class CConsole
{
public:
	CConsole();
	~CConsole();
	auto initialize() ->void;

	auto Flush() ->void {
		std::cout.flush();
	}
	// streaming overloads, we take pointers and references
	
	auto operator<<(const char *output) ->CConsole& ;
	auto operator<<(const std::int8_t *output) ->CConsole& ;
	auto operator<<(const std::uint8_t *output) ->CConsole& ;

	auto operator<<(const std::uint8_t &output) ->CConsole& ;
	auto operator<<(const std::int8_t &output) ->CConsole& ;
	auto operator<<(const std::uint16_t &output) ->CConsole& ;
	auto operator<<(const std::int16_t &output) ->CConsole& ;
	auto operator<<(const std::uint32_t &output) ->CConsole& ;
	auto operator<<(const std::int32_t &output) ->CConsole& ;
	auto operator<<(const std::uint64_t &output) ->CConsole& ;
	auto operator<<(const std::int64_t &output) ->CConsole& ;
	auto operator<<(const float &output) ->CConsole& ;
	auto operator<<(const double &output) ->CConsole& ;
	auto operator<<(const long double &output) ->CConsole& ;

	auto operator<<(CBaseObject *output) ->CConsole&;
	auto operator<<(const CBaseObject *output) ->CConsole&;
	auto operator<<(const CBaseObject &output) ->CConsole&;

	auto operator<<( const std::string &outPut ) ->CConsole&;
	
	auto operator<<(CEndL& myObj) -> CConsole&;
	
	
	auto print(const std::string& toPrint) ->void;
	auto log(const std::string& msg, const std::string& filename) ->void;
	auto log(const std::string& msg) ->void;
	auto error(const std::string& msg) ->void;
	auto warning(const std::string& msg ) ->void;
	
	auto CurrentMode() const ->std::uint8_t ;
	auto CurrentMode(std::uint8_t value) ->void;
	
	auto PrintSectionBegin() ->void;
	
	auto TurnYellow() ->void;
	auto TurnRed() ->void;
	auto TurnGreen() ->void;
	auto TurnBlue() ->void;
	auto TurnNormal() ->void;
	auto TurnBrightWhite() ->void;
	auto PrintDone() ->void;
	auto PrintFailed() ->void;
	auto PrintPassed() ->void;
	
	auto ClearScreen() ->void;
	auto Start(const std::string& temp) ->void;
	auto PrintBasedOnVal(bool value) ->void;
	auto MoveTo(std::int32_t x, std::int32_t y = -1 ) ->void;//y=-1 will move on the current line
	
	auto LogEcho() const ->bool;
	auto LogEcho(bool value) ->void;
	auto PrintSpecial(std::uint8_t color, const std::string& msg ) ->void;
	auto Poll() ->void;
	
	auto RegisterKey(std::int32_t key, std::string cmdName, std::uint16_t scriptID ) ->void;
	auto RegisterFunc(const std::string &key, const std::string &cmdName, UI16 scriptID ) ->void;
	auto SetKeyStatus(std::int32_t key, bool isEnabled ) ->void;
	auto SetFuncStatus(const std::string &key, bool isEnabled ) ->void;
	auto Registration() ->void;
	
private:
	auto reset() ->void;
	auto windowSize() ->std::pair<int,int>;
	auto clearScreen() ->void;
	auto setTitle(const std::string& title) ->void;
	auto sendCMD(const std::string& cmd) ->CConsole&;
	
	auto PrintStartOfLine() ->void;
	auto StartOfLineCheck() ->void;
	auto cl_getch() ->std::int32_t;
	auto Process(std::int32_t c) ->void;
	auto DisplaySettings() ->void;

	struct JSConsoleEntry {
		UI16		scriptID;
		bool		isEnabled;
		std::string cmdName;
		JSConsoleEntry() : scriptID( 0 ), isEnabled( true ), cmdName( "" ) {
		}
		JSConsoleEntry( UI16 id, const std::string &cName ) : scriptID( id ), isEnabled( true ), cmdName( cName ) {
		}
	};
	
	typedef std::map< std::string, JSConsoleEntry >				JSCONSOLEFUNCMAP;
	typedef std::map< std::string, JSConsoleEntry >::iterator	JSCONSOLEFUNCMAP_ITERATOR;
	typedef std::map< SI32, JSConsoleEntry >					JSCONSOLEKEYMAP;
	typedef std::map< SI32, JSConsoleEntry >::iterator			JSCONSOLEKEYMAP_ITERATOR;
	
	JSCONSOLEKEYMAP		JSKeyHandler;
	JSCONSOLEFUNCMAP	JSConsoleFunctions;
	UI16 height, width;	// for differing windows
	UI16 curLeft, curTop;
	std::bitset< 16 >	filterSettings;
	UI08 currentMode;
	UI08 previousColour;
	bool logEcho;
	bool is_initialized ;
	
	static const std::string ESC ;
	static const std::string CSI ;
	static const std::string BEL ;
	//  Command sequences
	static const std::string ATTRIBUTE ;  // find/replace on # with attribute number
	static const std::string MOVE	 ; // we are going to find/replace on ROW/COL
	static const std::string HORIZMOVE ;
	static const std::string VERTMOVE ;
	static const std::string OFFCURSOR ;
	static const std::string ONCURSOR ;
	
	static const std::string ONGRAPHIC ;
	static const std::string OFFGRAPHIC ;
	
	static const std::string SETTITLE ;
	
	static const std::string CLEAR ;
};

//===================================================================================================
// CEndL
//===================================================================================================
//===================================================================================================
class CEndL {
public:
	void Action( CConsole& test ) {
		test << "\n"; test.Flush();
	}
	
};
//===================================================================================================
extern CConsole Console;
extern CEndL myendl;

#endif

