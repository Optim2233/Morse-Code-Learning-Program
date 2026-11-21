#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <unordered_map>
#include <string>
#include <sstream>
#include <cctype>
#include <iostream>


// Morse code maps
const std::unordered_map<char, std::string> morseMap = {
    {'A', ".-"},   {'B', "-..."}, {'C', "-.-."}, {'D', "-.."},  {'E', "."},
    {'F', "..-."}, {'G', "--."},  {'H', "...."}, {'I', ".."},   {'J', ".---"},
    {'K', "-.-"},  {'L', ".-.."}, {'M', "--"},   {'N', "-."},   {'O', "---"},
    {'P', ".--."}, {'Q', "--.-"}, {'R', ".-."},  {'S', "..."},  {'T', "-"},
    {'U', "..-"},  {'V', "...-"}, {'W', ".--"},  {'X', "-..-"}, {'Y', "-.--"},
    {'Z', "--.."},
    {'0', "-----"},{'1', ".----"},{'2', "..---"},{'3', "...--"},{'4', "....-"},
    {'5', "....."},{'6', "-...."},{'7', "--..."},{'8', "---.."},{'9', "----."},
    {' ', " "}  // space between words
};

std::unordered_map<std::string, char> buildReverseMap() {
    std::unordered_map<std::string, char> rev;
    for (auto& pair : morseMap) {
        rev[pair.second] = pair.first;
    }
    return rev;
}
const std::unordered_map<std::string, char> reverseMorseMap = buildReverseMap();

// Simple word wrap for sf::Text
std::string wrapText(const std::string& str, const sf::Font& font, unsigned int charSize, float maxWidth) {
    std::string wrapped;
    std::string word;
    float spaceWidth = sf::Text(" ", font, charSize).getLocalBounds().width;
    float lineWidth = 0;

    for (size_t i = 0; i <= str.size(); ++i) {
        if (i == str.size() || str[i] == ' ') {
            if (!word.empty()) {
                sf::Text tempText(word, font, charSize);
                float wordWidth = tempText.getLocalBounds().width;
                if (lineWidth + wordWidth > maxWidth) {
                    wrapped += '\n';
                    lineWidth = 0;
                }
                wrapped += word;
                lineWidth += wordWidth;
                word.clear();
            }
            if (i < str.size()) {
                wrapped += ' ';
                lineWidth += spaceWidth;
            }
        } else {
            word += str[i];
        }
    }
    return wrapped;
}

enum class Screen { MainMenu, Translation };

int main() {
    sf::RenderWindow window(sf::VideoMode(750, 550), "Morse Code Learning Program", sf::Style::Default);
    window.setVerticalSyncEnabled(true);

    sf::Font font;
    if (!font.loadFromFile("assets/arial.ttf")) {
        std::cout << "Failed to load font!\n";
        return 1;
    }

    Screen currentScreen = Screen::MainMenu;

    // Main menu buttons
    const int buttonWidth = 200;
    const int buttonHeight = 50;
    const int buttonSpacing = 20;
    const int startY = 150;
    sf::RectangleShape mainButtons[3];
    sf::Text mainLabels[3];
    std::string mainButtonTexts[3] = { "Translation", "Lessons", "Coming Soon" };

    for (int i = 0; i < 3; ++i) {
        mainButtons[i].setSize(sf::Vector2f(buttonWidth, buttonHeight));
        mainButtons[i].setFillColor(sf::Color(100, 149, 237)); // cornflower blue
        mainLabels[i].setFont(font);
        mainLabels[i].setString(mainButtonTexts[i]);
        mainLabels[i].setCharacterSize(20);
        mainLabels[i].setFillColor(sf::Color::White);
        // Center text origin for later positioning
        sf::FloatRect textBounds = mainLabels[i].getLocalBounds();
        mainLabels[i].setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
    }

    // Translation screen stuff

    enum class TranslationMode { TextToMorse, MorseToText };
    TranslationMode mode = TranslationMode::TextToMorse;

    std::string userInput;
    std::string output;

    sf::RectangleShape backButton(sf::Vector2f(100, 30));
    sf::RectangleShape modeButton(sf::Vector2f(280, 40));

    backButton.setFillColor(sf::Color(128, 128, 128));
    modeButton.setFillColor(sf::Color(70, 130, 180));

    sf::Text backBtnLabel("Back", font, 16);
    backBtnLabel.setFillColor(sf::Color::White);

    sf::Text modeBtnLabel("", font, 20);
    modeBtnLabel.setFillColor(sf::Color::White);

    sf::Text inputText("", font, 24);
    inputText.setFillColor(sf::Color::White);

    sf::Text outputText("", font, 24);
    outputText.setFillColor(sf::Color::Green);

    sf::Text instructionText("", font, 16);
    instructionText.setFillColor(sf::Color(200, 200, 200));

    sf::RectangleShape inputBox, outputBox;
    inputBox.setFillColor(sf::Color(50, 50, 50));
    outputBox.setFillColor(sf::Color(40, 40, 40));

    sf::RectangleShape cursorRect;
    cursorRect.setFillColor(sf::Color::White);
    cursorRect.setSize(sf::Vector2f(2.f, 24.f));  // 2px wide, same height as input text

    sf::Clock blinkClock;
    bool cursorVisible = true;

    auto updateOutput = [&]() {
        output.clear();
        if (mode == TranslationMode::TextToMorse) {
            for (char c : userInput) {
                char upper = std::toupper(static_cast<unsigned char>(c));
                auto it = morseMap.find(upper);
                if (it != morseMap.end()) {
                    output += it->second;
                    if (upper != ' ') output += "/ ";  // add slash after each letter except space
                    else output += "  ";
                }
            }
        } else {
            size_t i = 0;
            std::string result;
            while (i < userInput.size()) {
                if (userInput[i] == ' ') {
                    if (i + 1 < userInput.size() && userInput[i + 1] == ' ') {
                        result += ' ';
                        i += 2;
                    } else {
                        i++;
                    }
                } else {
                    std::string morseLetter;
                    while (i < userInput.size() && userInput[i] != ' ') {
                        morseLetter += userInput[i];
                        i++;
                    }
                    auto it = reverseMorseMap.find(morseLetter);
                    if (it != reverseMorseMap.end()) {
                        result += it->second;
                    } else {
                        result += '?';
                    }
                }
            }
            output = result;
        }
    };

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }

            if (currentScreen == Screen::MainMenu) {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                    for (int i = 0; i < 3; ++i) {
                        if (mainButtons[i].getGlobalBounds().contains(mousePos)) {
                            std::cout << mainButtonTexts[i] << " button clicked!\n";
                            if (mainButtonTexts[i] == "Translation") {
                                currentScreen = Screen::Translation;
                                userInput.clear();
                                output.clear();
                                mode = TranslationMode::TextToMorse;
                            }
                            else {
                                // For now: just print message. You can add Lessons and Coming Soon screens later.
                            }
                        }
                    }
                }
            } else if (currentScreen == Screen::Translation) {
                if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

                    if (backButton.getGlobalBounds().contains(mousePos)) {
                        currentScreen = Screen::MainMenu;
                        userInput.clear();
                        output.clear();
                    }

                    if (modeButton.getGlobalBounds().contains(mousePos)) {
                        mode = (mode == TranslationMode::TextToMorse) ? TranslationMode::MorseToText : TranslationMode::TextToMorse;
                        userInput.clear();
                        output.clear();
                    }
                }

                if (event.type == sf::Event::TextEntered) {
                    char typed = static_cast<char>(event.text.unicode);
                    if (typed == 8 && !userInput.empty()) { // Backspace
                        userInput.pop_back();
                        updateOutput();
                    } else if (typed >= 32 && typed < 127) {
                        if (mode == TranslationMode::TextToMorse) {
                            if (std::isalnum(typed) || typed == ' ') {
                                userInput += typed;
                                updateOutput();
                            }
                        } else {
                            if (typed == '.' || typed == '-' || typed == ' ') {
                                userInput += typed;
                                updateOutput();
                            }
                        }
                    }
                }
            }
        }

        sf::Vector2u winSize = window.getSize();

        if (currentScreen == Screen::MainMenu) {
            // Position main buttons centered horizontally with spacing
            float totalHeight = 3 * buttonHeight + 2 * buttonSpacing;
            float startYCentered = (winSize.y - totalHeight) / 2.f;
            for (int i = 0; i < 3; ++i) {
                mainButtons[i].setPosition((winSize.x - buttonWidth) / 2.f, startYCentered + i * (buttonHeight + buttonSpacing));
                mainLabels[i].setPosition(mainButtons[i].getPosition().x + buttonWidth / 2.f,
                                          mainButtons[i].getPosition().y + buttonHeight / 2.f);
            }

            window.clear(sf::Color(30, 30, 30));
            for (int i = 0; i < 3; ++i) {
                window.draw(mainButtons[i]);
                window.draw(mainLabels[i]);
            }
        } else if (currentScreen == Screen::Translation) {
            backButton.setPosition(winSize.x - backButton.getSize().x - 20, 20);
            backBtnLabel.setPosition(backButton.getPosition().x + 30, backButton.getPosition().y + 5);

            modeButton.setPosition(20, 80);
            std::string modeText = (mode == TranslationMode::TextToMorse) ? "Text to Morse" : "Morse to Text";
            modeBtnLabel.setString("Switch Mode (" + modeText + ")");
            modeBtnLabel.setPosition(modeButton.getPosition().x + 10, modeButton.getPosition().y + 5);

            float margin = 20.f;
            inputBox.setPosition(margin, 140);
            inputBox.setSize(sf::Vector2f(winSize.x - 2 * margin, 80));

            outputBox.setPosition(margin, inputBox.getPosition().y + inputBox.getSize().y + 20);
            outputBox.setSize(sf::Vector2f(winSize.x - 2 * margin, 150));

            std::string wrappedInput = wrapText(userInput, font, inputText.getCharacterSize(), inputBox.getSize().x - 20);
            // Add blinking cursor to input display:
            std::string displayInput = wrappedInput;
            float cursorBlinkInterval = 0.5f; // seconds
            if (blinkClock.getElapsedTime().asSeconds() > cursorBlinkInterval) {
                cursorVisible = !cursorVisible;
                blinkClock.restart();
            }
            if (cursorVisible) {
                displayInput += '|';
            }
            inputText.setString("Input:\n" + displayInput);
            inputText.setPosition(inputBox.getPosition().x + 10, inputBox.getPosition().y + 10);

            std::string wrappedOutput = wrapText(output, font, outputText.getCharacterSize(), outputBox.getSize().x - 20);
            outputText.setString("Output:\n" + wrappedOutput);
            outputText.setPosition(outputBox.getPosition().x + 10, outputBox.getPosition().y + 10);

            instructionText.setString(
                (mode == TranslationMode::TextToMorse) ?
                "Type letters/digits and spaces to see Morse code." :
                "Type Morse code using '.', '-', spaces (single=letters, double=words)."
            );
            instructionText.setPosition(margin, outputBox.getPosition().y + outputBox.getSize().y + 10);

            window.clear(sf::Color(30, 30, 30));

            window.draw(backButton);
            window.draw(backBtnLabel);

            window.draw(modeButton);
            window.draw(modeBtnLabel);

            window.draw(inputBox);
            window.draw(inputText);

            window.draw(outputBox);
            window.draw(outputText);

            window.draw(instructionText);
        }

        window.display();
    }

    return 0;
}
