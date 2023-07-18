#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#if PLATFORM == WINDOWS
#include <conio.h>
#include <iostream>
#include <process.h>
#include <windows.h>
#endif


#include "books.h"
#include "cHTMLSystem.h"
#include "cGuild.h"
#include "CJSMapping.h"
#include "CJSEngine.h"
#include "classes.h"
#include "cEffects.h"
#include "cMagic.h"
#include "combat.h"
#include "commands.h"
#include "CPacketSend.h"
#include "cRaces.h"
#include "cScript.h"
#include "cServerDefinitions.h"
#include "cSpawnRegion.h"
#include "cThreadQueue.h"
#include "cVersionClass.h"
#include "cWeather.hpp"
#include "Dictionary.h"
#include "EventTimer.hpp"
#include "gump.h"
#include "jail.h"
#include "magic.h"
#include "movement.h"
#include "msgboard.h"
#include "ObjectFactory.h"
#include "PageVector.h"
#include "PartySystem.h"
#include "teffect.h"
#include "regions.h"
#include "skills.h"
#include "speech.h"
#include "ssection.h"
#include "StringUtility.hpp"
#include "townregion.h"
#include "uox3.h"
#include "weight.h"
#include "wholist.h"
#include <SEFunctions.cpp>


// console color constants
const WORD black = 0x0000;
const WORD blue = 0x0001;
const WORD green = 0x0002;
const WORD cyan = 0x0003;
const WORD red = 0x0004;
const WORD magenta = 0x0005;
const WORD brown = 0x0006;
const WORD gray = 0x0007;
const WORD darkgray = 0x0008;
const WORD hiBlue = 0x0009;
const WORD hiGreen = 0x000A;
const WORD hiCyan = 0x000B;
const WORD hiRed = 0x000C;
const WORD hiMagenta = 0x000D;
const WORD yellow = 0x000E;
const WORD white = 0x000F;

WORD adminNameAttribute;
WORD adminTextAttribute;
WORD serverNameAttribute;
WORD serverTextAttribute;
WORD terminalNameAttribute;
WORD terminalTextAttribute;



template <typename T>
int indexOf(const std::vector<T>& array, const T& value) {
	auto it = std::find(array.begin(), array.end(), value);
	if (it != array.end()) {
		return std::distance(array.begin(), it);
	}
	else {
		return -1;  // Return -1 if value is not found
	}
}

std::string toLowerCase(const std::string& str) {
	std::string result;
	for (char c : str) {
		result += std::tolower(c);
	}
	return result;
}

std::vector<std::string> commandList = {
	"sysbroadcast",
	"test",
	"test",
	"reload",
};

std::vector<std::string> split(const std::string& inputText);

int parseCommand(std::string input) {
	// Move the cursor to the top-left corner of the console
	//COORD topLeft = { 0, 0 };
	// Get the handle to the console output
	//HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	//system("cls");
	//SetConsoleTextAttribute(hConsole, adminNameAttribute);
	//std::cout << "Xoot:>";
	if (input.length() == 0) { input = "nothing"; }
	// toLowerCase
	std::string lInput = toLowerCase(input);
	// Split()
	std::vector<std::string> words = split(lInput);
	// Find indexOf()
	int inCommand = indexOf(commandList, words[0]);

	// Capture local terminal commands, forward all other input to the server
	switch (inCommand)
	{
	case 0: // this help data
	case 1:
	case 2:
		break;
	case 3: // set text color 0-15
	
		// Check if the command has enough arguments
		if (words.size() < 2) {
			Console.Error("Invalid command. Not enough arguments.");
			break;
		}

		// Extract the relevant values from the command
		int reloadBatch = std::stoi(words[1]);

		switch (reloadBatch)
		{
		case 0:	// Reload regions
			std::cout << "Reload 0" << std::endl;
			UnloadRegions();
			LoadRegions();
			LoadTeleportLocations();
			break;
		case 1:	// Reload spawn regions
			std::cout << "Reload 1" << std::endl;
			UnloadSpawnRegions();
			LoadSpawnRegions();
			break;
		case 2:	// Reload Spells
			std::cout << "Reload 2" << std::endl;
			Magic->LoadScript();
			break;
		case 3: // Reload Commands
			std::cout << "Reload 3" << std::endl;
			Commands->Load();
			break;
		case 4:	// Reload DFNs
			std::cout << "Reload 4" << std::endl;
			FileLookup->Reload();
			LoadSkills();
			Skills->Load();
			break;
		case 5: // Reload JScripts
			std::cout << "Reload 5" << std::endl;
			messageLoop << MSG_RELOADJS;
			break;
		case 6: // Reload HTMLTemplates
			std::cout << "Reload 6" << std::endl;
			HTMLTemplates->Unload();
			HTMLTemplates->Load();
			break;
		case 7:	// Reload INI
			std::cout << "Reload 7" << std::endl;
			cwmWorldState->ServerData()->Load();
			break;
		case 8: // Reload Everything
			std::cout << "Reload 8" << std::endl;
			FileLookup->Reload();
			UnloadRegions();
			LoadRegions();
			UnloadSpawnRegions();
			LoadSpawnRegions();
			Magic->LoadScript();
			Commands->Load();
			LoadSkills();
			Skills->Load();
			messageLoop << MSG_RELOADJS;
			HTMLTemplates->Unload();
			HTMLTemplates->Load();
			cwmWorldState->ServerData()->Load();
			break;
		case 9: // Reload Accounts
			std::cout << "Reload 9" << std::endl;
			Accounts->Load();
			break;
		case 10: // Reload Dictionaries
			std::cout << "Reload 10" << std::endl;
			Dictionary->LoadDictionaries();
			break;
		default:
			break;
		}
	
	break;
	case 4: // exit command to close terminal
		break;
	default:
		break;
	}
}

std::vector<std::string> split(const std::string& inputText) {
	std::stringstream ss(inputText);
	std::string word;
	std::vector<std::string> words;

	while (ss >> word) {
		words.push_back(word);
	}

	return words;
}






