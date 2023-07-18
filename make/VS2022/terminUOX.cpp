#include <algorithm>
//#include <atomic>
//#include <chrono>
//#include <cstdlib>
//#include <filesystem>
//#include <iostream>
//#include <numeric>
//#include <optional>
//#include <random>
#include <sstream>
#include <string>
//#include <thread>
#include <vector>
//#if PLATFORM == WINDOWS
#include <conio.h>
#include <iostream>


//#include <process.h>
//#include <windows.h>
//#endif
//
//
#include <filesystem>
#include "uox3.h"
#include "cdice.h"
#include "SEFunctions.h"
#include "cGuild.h"
#include "combat.h"
#include "movement.h"
#include "townregion.h"
#include "cWeather.hpp"
#include "cRaces.h"
#include "skills.h"
#include "commands.h"
#include "cMagic.h"
#include "CJSMapping.h"
#include "cScript.h"
#include "cEffects.h"
#include "classes.h"
#include "regions.h"
#include "magic.h"
#include "ssection.h"
#include "cThreadQueue.h"
#include "cHTMLSystem.h"
#include "cServerDefinitions.h"
#include "cVersionClass.h"
#include "Dictionary.h"
#include "speech.h"
#include "gump.h"
#include "ObjectFactory.h"
#include "network.h"
#include "UOXJSClasses.h"
#include "UOXJSPropertySpecs.h"
#include "JSEncapsulate.h"
#include "CJSEngine.h"
#include "PartySystem.h"
#include "cSpawnRegion.h"
#include "CPacketSend.h"

// bulk script reload vars
void		LoadTeleportLocations(void);
void		LoadSpawnRegions(void);
void		LoadRegions(void);
void		UnloadRegions(void);
void		UnloadSpawnRegions(void);
void		LoadSkills(void);

// ill mannered verbiage, or simply forward declarations?
void SysBroadcast(const std::string& txt);

//
//
//// console color constants
//const WORD black = 0x0000;
//const WORD blue = 0x0001;
//const WORD green = 0x0002;
//const WORD cyan = 0x0003;
//const WORD red = 0x0004;
//const WORD magenta = 0x0005;
//const WORD brown = 0x0006;
//const WORD gray = 0x0007;
//const WORD darkgray = 0x0008;
//const WORD hiBlue = 0x0009;
//const WORD hiGreen = 0x000A;
//const WORD hiCyan = 0x000B;
//const WORD hiRed = 0x000C;
//const WORD hiMagenta = 0x000D;
//const WORD yellow = 0x000E;
//const WORD white = 0x000F;
//
//WORD adminNameAttribute;
//WORD adminTextAttribute;
//WORD serverNameAttribute;
//WORD serverTextAttribute;
//WORD terminalNameAttribute;
//WORD terminalTextAttribute;
//
//
//

template <typename T> // case sensitive
int indexOf(const std::vector<T>& array, const T& value) {
	auto it = std::find(array.begin(), array.end(), value);
	if (it != array.end()) {
		return std::distance(array.begin(), it);
	}
	else {
		return -1;  // Return -1 if value is not found
	}
}
/*
template <typename T>
int indexOf(const std::vector<T>& array, const T& value) {
	auto it = std::find_if(array.begin(), array.end(), [&](const T& elem) {
		return std::equal(elem.begin(), elem.end(), value.begin(), value.end(), [](char a, char b) {
			return std::tolower(a) == std::tolower(b);
			});
		});

	if (it != array.end()) {
		return std::distance(array.begin(), it);
	}
	else {
		return -1;  // Return -1 if value is not found
	}
}
*/
//
std::string toLowerCase(const std::string& str) {
	std::string result;
	for (char c : str) {
		result += std::tolower(c);
	}
	return result;
}

//std::vector<std::string> split(const std::string& inputText) {
//	std::stringstream ss(inputText + " :"); // the " :" is a safety word to prevent single element arrays from breaking the operation
//	std::string word;
//	std::vector<std::string> words;
//
//	while (ss >> word) {
//		words.push_back(word);
//	}
//
//	return words;
//}
std::vector<std::string> split(const std::string& inputText) {
	if (inputText.empty() || inputText.find_first_not_of(' ') == std::string::npos) {
		// Return an empty vector if the input string is empty or contains only whitespace
		return {};
	}

	std::stringstream ss(inputText);
	std::string word;
	std::vector<std::string> words;

	while (ss >> word) {
		words.push_back(word);
	}

	return words;
}

// command list
std::vector<std::string> commandList = {
	"null",
	"exit",
	"nothing",
	"reload",
	"shutdown",
	"broadcast"
};

// reload list
std::vector<std::string> commandList = {
	"regions",
	"spawns",
	"spells",
	"commands",
	"defines",
	"scripts",
	"html",
	"ini",
	"everything",
	"accounts",
	"dictionary",
	"",
	""
};


std::vector<std::string> split(const std::string& inputText);

SI32 parseCommandFunc(std::string input) {
	if (input.length() == 0) { input = "nothing"; }
	std::vector<std::string> words = split(input);
	std::string lInput = toLowerCase(words[0]);
	int inCommand = indexOf(commandList, lInput);
	std::string stringOut = words[0] + "|" + std::to_string(inCommand) + "\n";
	Console.Print( stringOut);
	std::string message = "";

	switch (inCommand)
	{
	case 0: // null
		break;

	case 1: // exit
		return inCommand;
		break;
	case 2: // nothing
		return inCommand;
		break;
	case 3: // reload
	{
		// Check if the command has enough arguments
		if (words.size() < 2) {
			Console.Error("Invalid command. Not enough arguments.");
			break;
		}

		int reloadBatch;
		try {
			reloadBatch = std::stoi(words[1]);
		}
		catch (const std::exception& e) {
			Console.Error("Invalid argument. The value must be an integer.\n");
			break;
		}

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
		return inCommand;
		break;
	}
	case 4: // shutdown server

		return inCommand;
		break;
	case 5: // broadcast
		// Check if the command has enough arguments
		if (words.size() < 2) {
			Console.Error("Invalid command. Not enough arguments.\n");
			break;
		}

		for (size_t i = 1; i < words.size(); ++i) { 
			message += words[i];
			if (i < words.size()) {
				message += " ";
			}
		}
		SysBroadcast(message);
		return inCommand;
		break;

	default:

		return inCommand;
		break;
	}
	return inCommand;
}
