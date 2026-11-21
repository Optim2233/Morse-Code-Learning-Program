#pragma once
#include <string>

// Translates plain text to Morse code, e.g. "SOS" -> "... --- ... "
std::string textToMorse(const std::string& input);

// Translates Morse code to plain text, e.g. "... --- ..." -> "SOS"
std::string morseToText(const std::string& morseInput);
