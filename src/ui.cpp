#include "ui.hpp"
#include "morse.hpp"

std::string wrapText(const std::string& str, sf::Font& font, unsigned char charSize, float maxWidth) {
    std::string wrap, word;
    float lineWidth = 0;
    float spaceWidth = sf::Text(font, " ", charSize).getLocalBounds().size.x; // we need the width of a space in pixels, not its length

    for (size_t i = 0; i <= str.size(); ++i) {
        if (i == str.size() || str[i] == ' ') { // end of string, or end of a word
            sf::Text temp(font, word, charSize); // a temporary text object to measure the word
            float wordWidth = temp.getLocalBounds().size.x;

            if (lineWidth + spaceWidth > maxWidth) { // would exceed the maximum width
                wrap += '\n'; // so add a line break
                lineWidth = 0;
            }
            wrap += word; // add the word either way
            if (i < str.size()) {
                wrap += ' ';
                lineWidth += spaceWidth;
            }
            lineWidth += wordWidth;
            word.clear();
        } else {
            word += str[i]; // still inside a word; keep collecting characters
        }
    }
    if (!word.empty()) { // safety check for a trailing word without a space after it
        sf::Text temp(font, word, charSize);
        float wordWidth = temp.getLocalBounds().size.x;
        if (wordWidth + spaceWidth > maxWidth) {
            wrap += '\n';
        }
        wrap += word;
    }
    return wrap;
}

// UI drawing functions, kept separate for readability.

void drawMainMenu(sf::RenderWindow& window,
                  sf::RectangleShape& lessonButton, sf::Text& lessonButtonText,
                  sf::RectangleShape& translationButton, sf::Text& translationButtonText,
                  sf::RectangleShape& practiceButton, sf::Text& practiceButtonText,
                  sf::RectangleShape& exitButton, sf::Text& exitText) {
    window.clear(sf::Color::Blue);
    window.draw(practiceButton);
    window.draw(practiceButtonText);
    window.draw(lessonButton);
    window.draw(lessonButtonText);
    window.draw(translationButton);
    window.draw(translationButtonText);
    window.draw(exitButton);
    window.draw(exitText);
    window.display();
}

void drawTranslationScreen(sf::RenderWindow& window, sf::Font& font,
                           const std::string& input, const std::string& output,
                           sf::Text& inputText, sf::Text& outputText,
                           sf::RectangleShape& inputBox, sf::RectangleShape& outputBox,
                           sf::RectangleShape& backButton, sf::Text& backLabel,
                           sf::RectangleShape& clearButton, sf::Text& clearLabel,
                           sf::RectangleShape& switchButton, sf::Text& switchLabel,
                           sf::CircleShape& morseKey, sf::Text& morseKeyLabel,
                           sf::Text& screenTitle, bool morseInputMode) {
    std::string wrappedOutput = wrapText(output, font, 24, 1000);
    std::string wrappedInput = wrapText(input, font, 24, 800);
    inputText.setString("Input: " + wrappedInput);
    outputText.setString("Output: " + wrappedOutput);

    window.clear(sf::Color::Black);
    window.draw(inputBox);
    window.draw(inputText);
    window.draw(outputBox);
    window.draw(outputText);
    window.draw(backButton);
    window.draw(backLabel);
    window.draw(clearButton);
    window.draw(clearLabel);
    if (morseInputMode) {
        morseKey.setPosition({ clearButton.getGlobalBounds().position.x, clearButton.getGlobalBounds().position.y + clearButton.getGlobalBounds().size.y + 30.f });
        morseKeyLabel.setOrigin({ morseKeyLabel.getLocalBounds().position.x + morseKeyLabel.getLocalBounds().size.x / 2.f, morseKeyLabel.getLocalBounds().position.y + morseKeyLabel.getLocalBounds().size.y / 2.f });
        morseKeyLabel.setPosition({ morseKey.getPosition().x + morseKey.getRadius(), morseKey.getPosition().y + morseKey.getRadius() });
        window.draw(morseKey);
        window.draw(morseKeyLabel);
    }
    window.draw(switchButton);
    window.draw(switchLabel);
    screenTitle.setString(morseInputMode ? "Morse Code to Text" : "Text to Morse Code");
    window.draw(screenTitle);
    window.display();
}

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
                sf::Text& hintToggleLabel, bool showHint) {
    window.clear(sf::Color::Black);

    if (currentStep >= lessonSteps.size()) { // all steps done: show the congratulations screen
        bigText.setString("");
        hintText.setString("");
        inputText.setString("");
        progressText.setString("");
        window.draw(sidebar);
        window.draw(leftSidebar);
        window.draw(rightSidebar);
        window.draw(congratsText);
        window.draw(backButton);
        window.draw(backLabel);
        window.draw(nextLessonButton);
        window.draw(nextLessonLabel);
    } else {
        const LessonStep& step = lessonSteps[currentStep];
        bigText.setString(std::string(1, step.letter));
        hintText.setString(step.morse);
        inputText.setString("Your input _ " + lessonInput);
        progressText.setString("Attempt " + std::to_string(currentAttempt) + " of " + std::to_string(step.repeat));
        window.draw(sidebar);
        window.draw(leftSidebar);
        window.draw(rightSidebar);
        window.draw(backButton);
        window.draw(backLabel);
        window.draw(bigTextButton);
        window.draw(bigText);
        if (showHint) {
            window.draw(hintText);
        }
        window.draw(progressText);
        window.draw(inputText);
        window.draw(messageText);
        window.draw(hintToggleButton);
        if (!showHint) {
            hintToggleLabel.setString("ON!!");
        } else {
            hintToggleLabel.setString("OFF!");
        }
        window.draw(hintToggleLabel);

        morseKey.setPosition({ bigTextButton.getPosition().x + bigTextButton.getSize().x / 2.f - morseKey.getRadius(),
                               bigTextButton.getPosition().y + bigTextButton.getSize().y / 2.f - morseKey.getRadius() + 450.f });
        morseKeyLabel.setOrigin({ morseKeyLabel.getLocalBounds().position.x + morseKeyLabel.getLocalBounds().size.x / 2.f,
                                  morseKeyLabel.getLocalBounds().position.y + morseKeyLabel.getLocalBounds().size.y / 2.f });
        morseKeyLabel.setPosition({ morseKey.getPosition().x + morseKey.getRadius(), morseKey.getPosition().y + morseKey.getRadius() });
        window.draw(morseKey);
        window.draw(morseKeyLabel);
    }

    for (auto& btn : sidebarLessonButtons)
        btn.setFillColor(sf::Color(200, 200, 200));

    if (currentLessonIndex - 1 < sidebarLessonButtons.size())
        sidebarLessonButtons[currentLessonIndex - 1].setFillColor(sf::Color::Yellow); // highlight the active lesson

    for (const auto& btn : sidebarLessonButtons)
        window.draw(btn);
    for (const auto& label : sidebarLessonLabels)
        window.draw(label);

    window.display();
}

void drawPractice(sf::RenderWindow& window, const std::vector<PracticeStep>& practiceSteps,
                  size_t currentStep, const std::string& practiceInput,
                  sf::Text& bigText, sf::Text& hintText, sf::Text& inputText,
                  sf::Text& messageText, sf::Text& congratsText,
                  sf::RectangleShape& bigTextButton, sf::CircleShape& morseKey, sf::Text& morseKeyLabel,
                  sf::RectangleShape& backButton, sf::Text& backLabel,
                  sf::RectangleShape& sidebar, sf::RectangleShape& leftSidebar,
                  sf::RectangleShape& rightSidebar, sf::RectangleShape& hintCover,
                  sf::RectangleShape& hintToggleButton, sf::Text& hintToggleLabel, bool showHint) {
    window.clear(sf::Color::Black);

    if (currentStep >= practiceSteps.size()) { // all steps done: show the congratulations screen
        bigText.setString("");
        hintText.setString("");
        inputText.setString("");
        window.draw(sidebar);
        window.draw(leftSidebar);
        window.draw(rightSidebar);
        window.draw(congratsText);
        window.draw(backButton);
        window.draw(backLabel);
    } else {
        const PracticeStep& step = practiceSteps[currentStep];
        bigText.setString(step.text);       // Full word/sentence
        hintText.setString(step.hintMorse); // Faint hint
        inputText.setString("Your input _ " + practiceInput);
        window.draw(sidebar);
        window.draw(leftSidebar);
        window.draw(rightSidebar);
        window.draw(backButton);
        window.draw(backLabel);
        window.draw(bigTextButton);

        window.draw(bigText);
        window.draw(hintText);
        window.draw(inputText);

        morseKey.setPosition({ bigTextButton.getPosition().x + bigTextButton.getSize().x / 2.f - morseKey.getRadius(),
                               bigTextButton.getPosition().y + bigTextButton.getSize().y / 2.f - morseKey.getRadius() + 450.f });
        morseKeyLabel.setOrigin({ morseKeyLabel.getLocalBounds().position.x + morseKeyLabel.getLocalBounds().size.x / 2.f,
                                  morseKeyLabel.getLocalBounds().position.y + morseKeyLabel.getLocalBounds().size.y / 2.f });
        morseKeyLabel.setPosition({ morseKey.getPosition().x + morseKey.getRadius(), morseKey.getPosition().y + morseKey.getRadius() });

        window.draw(morseKey);
        window.draw(morseKeyLabel);
        window.draw(messageText);
        window.draw(hintToggleButton);
        window.draw(hintToggleLabel);
        if (showHint == false) {
            window.draw(hintCover);
            hintToggleLabel.setString("ON!!");
        } else {
            hintToggleLabel.setString("OFF!");
        }
    }

    if (currentState == AppStates::Practice) {
        for (auto& btn : sidebarPracticeButtons) window.draw(btn);
        for (auto& label : sidebarPracticeLabels) window.draw(label);
    }

    window.display();
}

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
              sf::Text& hintToggleLabel, bool showHint, sf::Text& screenTitle) {
    switch (state) {
        case AppStates::MainMenu:
            drawMainMenu(window, lessonButton, lessonButtonText, translationButton, translationButtonText,
                         practiceButton, practiceButtonText, exitButton, exitText);
            break;
        case AppStates::Text2Morse:
            typedOutput = textToMorse(typedInput);
            drawTranslationScreen(window, font, typedInput, typedOutput, typedInputText, typedOutputText,
                                  inputBox, outputBox, backButton, backLabel, clearButton, clearLabel,
                                  switchButton, switchLabel, morseKey, morseKeyLabel, screenTitle, false);
            break;
        case AppStates::Morse2Text:
            typedOutput = morseToText(typedInput);
            drawTranslationScreen(window, font, typedInput, typedOutput, typedInputText, typedOutputText,
                                  inputBox, outputBox, backButton, backLabel, clearButton, clearLabel,
                                  switchButton, switchLabel, morseKey, morseKeyLabel, screenTitle, true);
            break;
        case AppStates::LessonMode:
            drawLesson(window, lessonSteps, currentStep, currentAttempt, lessonInput, bigText, hintText,
                        inputText, progressText, messageText, congratsText, bigTextButton, morseKey, morseKeyLabel,
                        backButton, backLabel, nextLessonButton, nextLessonLabel, sidebarLessonButtons,
                        sidebarLessonLabels, currentLessonIndex, sidebar, leftSidebar, rightSidebar, hintCover,
                        hintToggleButton, hintToggleLabel, showHint);
            break;

        case AppStates::Practice:
            if (currentStep < practiceSteps.size()) {
                bigText.setString(practiceSteps[currentStep].text);
                hintText.setString(practiceSteps[currentStep].hintMorse);
                inputText.setString("Your input _ " + practiceInput);
                progressText.setString("Step " + std::to_string(currentStep + 1) + " / " + std::to_string(practiceSteps.size()));
            } else {
                bigText.setString("");
                hintText.setString("");
                inputText.setString("");
                progressText.setString("");
            }
            drawPractice(window, practiceSteps, currentStep, practiceInput,
                         bigText, hintText, inputText, messageText, congratsText, bigTextButton,
                         morseKey, morseKeyLabel, backButton, backLabel,
                         sidebar, leftSidebar, rightSidebar, hintCover, hintToggleButton, hintToggleLabel, showHint);
            break;
    }
}
