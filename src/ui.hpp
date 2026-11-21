#pragma once
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "content.hpp"

enum class AppStates { MainMenu, LessonMode, Morse2Text, Text2Morse, Practice };

// Shared app state, defined in main.cpp.
extern AppStates currentState;
extern std::vector<PracticeStep> practiceSteps;
extern std::string practiceInput;
extern std::vector<sf::RectangleShape> sidebarPracticeButtons; // practice mode sidebar buttons
extern std::vector<sf::Text> sidebarPracticeLabels;            // practice mode sidebar labels

// Adds line breaks once a string would pass a set maximum pixel width.
std::string wrapText(const std::string& str, sf::Font& font, unsigned char charSize, float maxWidth);

void drawMainMenu(sf::RenderWindow& window,
                  sf::RectangleShape& lessonButton, sf::Text& lessonButtonText,
                  sf::RectangleShape& translationButton, sf::Text& translationButtonText,
                  sf::RectangleShape& practiceButton, sf::Text& practiceButtonText,
                  sf::RectangleShape& exitButton, sf::Text& exitText);

// Shared by both translation modes; morseInputMode adds the clickable Morse key
// and swaps the screen title (was two near-identical functions before).
void drawTranslationScreen(sf::RenderWindow& window, sf::Font& font,
                           const std::string& input, const std::string& output,
                           sf::Text& inputText, sf::Text& outputText,
                           sf::RectangleShape& inputBox, sf::RectangleShape& outputBox,
                           sf::RectangleShape& backButton, sf::Text& backLabel,
                           sf::RectangleShape& clearButton, sf::Text& clearLabel,
                           sf::RectangleShape& switchButton, sf::Text& switchLabel,
                           sf::CircleShape& morseKey, sf::Text& morseKeyLabel,
                           sf::Text& screenTitle, bool morseInputMode);

void drawLesson(sf::RenderWindow& window, const std::vector<LessonStep>& lessonSteps,
                size_t currentStep, int currentAttempt, const std::string& lessonInput,
                sf::Text& bigText, sf::Text& hintText, sf::Text& inputText,
                sf::Text& progressText, sf::Text& messageText, sf::Text& congratsText,
                sf::RectangleShape& bigTextButton, sf::CircleShape& morseKey, sf::Text& morseKeyLabel,
                sf::RectangleShape& backButton, sf::Text& backLabel,
                sf::RectangleShape& nextLessonButton, sf::Text& nextLessonLabel,
                std::vector<sf::RectangleShape>& sidebarLessonButtons,
                const std::vector<sf::Text>& sidebarLessonLabels,
                int currentLessonIndex, sf::RectangleShape& sidebar,
                sf::RectangleShape& leftSidebar, sf::RectangleShape& rightSidebar,
                sf::RectangleShape& hintCover, sf::RectangleShape& hintToggleButton,
                sf::Text& hintToggleLabel, bool showHint);

void drawPractice(sf::RenderWindow& window, const std::vector<PracticeStep>& practiceSteps,
                  size_t currentStep, const std::string& practiceInput,
                  sf::Text& bigText, sf::Text& hintText, sf::Text& inputText,
                  sf::Text& messageText, sf::Text& congratsText,
                  sf::RectangleShape& bigTextButton, sf::CircleShape& morseKey, sf::Text& morseKeyLabel,
                  sf::RectangleShape& backButton, sf::Text& backLabel,
                  sf::RectangleShape& sidebar, sf::RectangleShape& leftSidebar,
                  sf::RectangleShape& rightSidebar, sf::RectangleShape& hintCover,
                  sf::RectangleShape& hintToggleButton, sf::Text& hintToggleLabel, bool showHint);

// Updates the output strings for the active mode and redraws the whole UI.
void updateUI(sf::RenderWindow& window, AppStates state,
              std::string& typedInput, std::string& typedOutput,
              sf::Font& font, sf::Text& typedInputText, sf::Text& typedOutputText,
              sf::RectangleShape& inputBox, sf::RectangleShape& outputBox,
              sf::RectangleShape& backButton, sf::Text& backLabel,
              sf::RectangleShape& clearButton, sf::Text& clearLabel,
              sf::RectangleShape& switchButton, sf::Text& switchLabel,
              sf::CircleShape& morseKey, sf::Text& morseKeyLabel,
              sf::RectangleShape& lessonButton, sf::Text& lessonButtonText,
              sf::RectangleShape& translationButton, sf::Text& translationButtonText,
              sf::RectangleShape& practiceButton, sf::Text& practiceButtonText,
              sf::RectangleShape& exitButton, sf::Text& exitText,
              const std::vector<LessonStep>& lessonSteps, size_t currentStep, int currentAttempt,
              const std::string& lessonInput,
              sf::Text& bigText, sf::Text& hintText, sf::Text& inputText,
              sf::Text& progressText, sf::Text& messageText, sf::Text& congratsText,
              sf::RectangleShape& bigTextButton,
              sf::RectangleShape& nextLessonButton, sf::Text& nextLessonLabel,
              std::vector<sf::RectangleShape>& sidebarLessonButtons,
              const std::vector<sf::Text>& sidebarLessonLabels, int currentLessonIndex,
              sf::RectangleShape& sidebar, sf::RectangleShape& leftSidebar, sf::RectangleShape& rightSidebar,
              sf::RectangleShape& hintCover, sf::RectangleShape& hintToggleButton,
              sf::Text& hintToggleLabel, bool showHint, sf::Text& screenTitle);
