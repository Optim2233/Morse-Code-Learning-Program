#include "morse.hpp"
#include <sstream>
#include <cctype>
#include <unordered_map>

// Map links one variable to another: if we know the letter, we can access its Morse code.
const std::unordered_map<char, std::string> MorseMap = {
    {'A', ".-"},   {'B', "-..."}, {'C', "-.-."}, {'D', "-.."},
    {'E', "."},    {'F', "..-."}, {'G', "--."},  {'H', "...."},
    {'I', ".."},   {'J', ".---"}, {'K', "-.-"},  {'L', ".-.."},
    {'M', "--"},   {'N', "-."},   {'O', "---"},  {'P', ".--."},
    {'Q', "--.-"}, {'R', ".-."},  {'S', "..."},  {'T', "-"},
    {'U', "..-"},  {'V', "...-"}, {'W', ".--"},  {'X', "-..-"},
    {'Y', "-.--"}, {'Z', "--.."}, {'0', "-----"}, {'1', ".----"},
    {'2', "..---"},{'3', "...--"}, {'4', "....-"}, {'5', "....."},
    {'6', "-...."},{'7', "--..."}, {'8', "---.."}, {'9', "----."},
    {' ', "/"},    {'.', ".-.-.-"}
};

std::string textToMorse(const std::string& input) {
    std::string output;
    for (char c : input) { // check every char in the input string
        char upper = std::toupper(static_cast<unsigned char>(c)); // convert to capital so the map lookup works
        auto it = MorseMap.find(upper); // find the Morse code for this char
        if (it != MorseMap.end()) {
            output += it->second;
            output += " ";
        } else {
            output += "?"; // char is not in the map
        }
    }
    return output;
}

std::string morseToText(const std::string& morseInput) {
    std::string output, token;
    std::unordered_map<std::string, char> reverseMap; // the Morse code table, inverted: "..." -> 'S'

    for (const auto& pair : MorseMap) {
        reverseMap[pair.second] = pair.first; // reverseMap["..."] = 'S'
    }

    std::istringstream stream(morseInput); // splits the string by spaces

    while (stream >> token) { // token stores each space-separated Morse letter
        auto it = reverseMap.find(token);
        if (it != reverseMap.end()) {
            output += it->second;
        } else {
            output += '?'; // Unknown Morse code
        }
    }
    return output;
}
