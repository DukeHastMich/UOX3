#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>
#include <algorithm>

// Define the color variables
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
	"help","?","man",
	"color",
};

std::vector<std::string> split(const std::string& inputText);

int main() {
	LPCSTR consoleName = "UOX Admin Shell - [UOX-ASH]";
	SetConsoleTitleA(consoleName);
	// Create a named pipe client to connect to the main console application
	HANDLE hPipe = nullptr;
	std::string input;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	adminNameAttribute = hiBlue;
	adminTextAttribute = white;
	serverNameAttribute = hiRed;
	serverTextAttribute = white;
	terminalNameAttribute = hiGreen;
	terminalTextAttribute = white;


	while (true) {
		if (hPipe == INVALID_HANDLE_VALUE || hPipe == 0) {
			std::cout << "Failed to connect to the named pipe. Error code: " << GetLastError() << std::endl;
			std::cout << "Retrying in 3\r";
			Sleep(600);
			std::cout << "Retrying in 2\r";
			Sleep(600);
			std::cout << "Retrying in 1\r";
			Sleep(600);

			// Move the cursor to the top-left corner of the console
			COORD topLeft = { 0, 0 };
			// Get the handle to the console output
			HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
			hPipe = CreateFileA(
				"\\\\.\\pipe\\MyPipe",  // Pipe name
				GENERIC_READ | GENERIC_WRITE, // Read and write access
				0,                       // No sharing
				nullptr,                 // Default security attributes
				OPEN_EXISTING,           // Open an existing pipe
				0,                       // Default attributes
				nullptr                  // No template file
			);
			std::cout << "Retrying connection.\r";
			Sleep(1200);
			system("cls");
			//SetConsoleCursorPosition(hConsole, topLeft);
		}
		else {
			// Prompt for input in the second console window
			SetConsoleTextAttribute(hConsole, adminNameAttribute);
			std::cout << "Xoot:>";
			SetConsoleTextAttribute(hConsole, adminTextAttribute);
			std::getline(std::cin, input);
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
				SetConsoleTextAttribute(hConsole, terminalNameAttribute);
				std::cout << "\n^^ - term.\n";
				SetConsoleTextAttribute(hConsole, terminalTextAttribute);
				std::cout << "Help:\n";
				std::cout << "-O   Local Terminal Commands and Options:\n";
				std::cout << "\n-o Help/?/man for this information.\n";
				std::cout << "\n-o color u n n n where n is (1-15) to select foreground and background colors.\n";
				std::cout << " |(u) refers to text originating from the admin (0), the server (1), or the terminal (2)\n";
				std::cout << " |The first n is the name color if applicable, the second n is the users text, the third n is their background color\n";
				std::cout << " |Colors:\n"
					<< " |0: Black\n"
					<< " |1: Blue\n"
					<< " |2: Green\n"
					<< " |3: Cyan\n"
					<< " |4: Red\n"
					<< " |5: Magenta\n"
					<< " |6: Yellow\n"
					<< " |7: White\n"
					<< " |8: Gray\n"
					<< " |9: Light Blue\n"
					<< " |10: Light Green\n"
					<< " |11: Light Cyan\n"
					<< " |12: Light Red\n"
					<< " |13: Light Magenta\n"
					<< " |14: Light Yellow\n"
					<< " |15: Bright White\n";

				std::cout << "\n-o Exit. Closes the terminal.\n";
				std::cout << "\n-O   UOX Server Commands:\n";

				break;
			case 3: // set text color 0-15
			{
				// Check if the command has enough arguments
				if (words.size() < 5) {
					std::cout << "Invalid command. Not enough arguments." << std::endl;
					break;
				}

				// Extract the relevant values from the command
				int userType = std::stoi(words[1]);
				int nameColor = std::stoi(words[2]);
				int textColor = std::stoi(words[3]);
				int bgColor = std::stoi(words[4]);




				// Determine the appropriate color variables based on the user type
				if (userType == 0) { // Admin
					adminNameAttribute = 0;
					adminTextAttribute = 0;
					adminNameAttribute |= nameColor;
					adminTextAttribute |= textColor;
					adminNameAttribute |= (bgColor << 4);
					adminTextAttribute |= (bgColor << 4);
				}
				else if (userType == 1) { // Server
					serverNameAttribute = 0;
					serverTextAttribute = 0;
					serverNameAttribute |= nameColor;
					serverTextAttribute |= textColor;
					serverNameAttribute |= (bgColor << 4);
					serverTextAttribute |= (bgColor << 4);
				}
				else if (userType == 2) { // Terminal
					terminalNameAttribute = 0;
					terminalTextAttribute = 0;
					terminalNameAttribute |= nameColor;
					terminalTextAttribute |= textColor;
					terminalNameAttribute |= (bgColor << 4);
					terminalTextAttribute |= (bgColor << 4);
				}
				else {
					std::cout << "Invalid user type." << std::endl;
					break;
				}

				// Validate the provided color values
				if (nameColor < 0 || nameColor > 15 || textColor < 0 || textColor > 15 || bgColor < 0 || bgColor > 15) {
					std::cout << "Invalid color values. Colors should be between 0 and 15." << std::endl;
					break;
				}
				break;
			}
			break;
			case 4: // exit command to close terminal
				break;
			default:
				// not a local command, forward to server
				DWORD bytesWritten;
				if (WriteFile(hPipe, lInput.c_str(), static_cast<DWORD>(lInput.length()), &bytesWritten, nullptr)) {
				}
				else {
					std::cout << "Failed to send message to the main console application. Error code: " << GetLastError() << std::endl;
				}

				// and then try to look for replies
				const int bufferSize = 256;
				char buffer[bufferSize];
				DWORD bytesRead;
				if (ReadFile(hPipe, buffer, bufferSize - 1, &bytesRead, nullptr)) {
					buffer[bytesRead] = '\0';
					SetConsoleTextAttribute(hConsole, serverNameAttribute);
					std::cout << "Server reply: ";
					SetConsoleTextAttribute(hConsole, serverTextAttribute);
					std::cout << buffer << std::endl;
				}
				break;
			}
		}
	}

	CloseHandle(hPipe);

	return 0;
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



