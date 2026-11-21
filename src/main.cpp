#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <windows.h>
#include "content.hpp"
#include "morse.hpp"
#include "ui.hpp"

// Shared app state (declared extern in ui.hpp).
AppStates currentState = AppStates::MainMenu;
std::vector<PracticeStep> practiceSteps;
std::string practiceInput;
std::vector<sf::RectangleShape> sidebarPracticeButtons;
std::vector<sf::Text> sidebarPracticeLabels;

int main() {
    // All data paths are relative to the project root (this exe's parent folder's parent),
    // so switch there first — this makes the exe work no matter where it is launched from.
    char exePath[MAX_PATH];
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    std::filesystem::current_path(std::filesystem::path(exePath).parent_path().parent_path());

    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "After Getting Bitch Slapped", sf::State::Fullscreen); // this sets the gui screen
    window.setFramerateLimit(50); // limiting frame rate

    sf::Font font;
    if (!font.openFromFile("assets/arial.ttf")) { // uploading font, only arial - current patch : 1.2
        std::cout << "Failed to load the font.";
        return 1;
    }

    std::vector<LessonStep> lessonSteps; // lessonSteps[i].letter / .morse / .repeat
    std::vector<sf::RectangleShape> sidebarLessonButtons; // lesson mode sidebar buttons
    std::vector<sf::Text> sidebarLessonLabels;             // lesson mode sidebar labels

    int currentLessonIndex = 1;
    std::string lessonFilename = "bin/Lessons/Lesson" + std::to_string(currentLessonIndex) + ".txt";
    if (!loadLesson(lessonFilename, lessonSteps)) {
        std::cerr << "Failed to load the lesson : " << lessonFilename;
        return 1;
    }

    bool showHint = true; // whether the Morse hint is visible (toggled by the hint button)
    size_t currentStep = 0;
    int currentAttempt = 1;
    std::string lessonInput;

    // ---- Lesson sidebar buttons ----
    const int totalLessons = 18;
    const int lessonsPerCol = 9;
    const float buttonWidth = 80.f, buttonHeight = 50.f, spacing = 20.f;
    const sf::Vector2f sidebarStart(80.f, 100.f);

    for (int i = 0; i < totalLessons; ++i) {
        int col = i / lessonsPerCol;
        int row = i % lessonsPerCol;
        sf::RectangleShape btn(sf::Vector2f(buttonWidth, buttonHeight));
        btn.setFillColor(sf::Color(200, 200, 200));
        btn.setPosition({ sidebarStart.x + col * (buttonWidth + spacing), sidebarStart.y + row * (buttonHeight + spacing) });
        sidebarLessonButtons.push_back(btn);

        sf::Text label(font, std::to_string(i + 1), 18);
        label.setFillColor(sf::Color::Black);
        label.setPosition({ btn.getPosition().x + 20.f, btn.getPosition().y + 12.f });
        sidebarLessonLabels.push_back(label);
    }

    // ---- Main menu buttons ----
    sf::RectangleShape lessonButton(sf::Vector2f(400.f, 80.f));
    sf::Vector2u windowSize = window.getSize();
    sf::Vector2f objectSize = lessonButton.getSize();
    lessonButton.setPosition({ (windowSize.x - objectSize.x) / 2.f, (windowSize.y - objectSize.y) / 2.f });
    lessonButton.setFillColor(sf::Color::Black);

    sf::RectangleShape bigTextButton(sf::Vector2f(1290.f, 200.f));
    bigTextButton.setFillColor(sf::Color::White);
    bigTextButton.setPosition({ 500.f, 0.f });

    sf::Text bigText(font, " ", 100);
    bigText.setFillColor(sf::Color::Black);
    bigText.setOrigin({ bigText.getLocalBounds().position.x + bigText.getLocalBounds().size.x / 2.f,
                        bigText.getLocalBounds().position.y + bigText.getLocalBounds().size.y / 2.f });
    bigText.setPosition({ (bigTextButton.getPosition().x + bigTextButton.getSize().x / 2.f) - 10.f,
                          (bigTextButton.getPosition().y + bigTextButton.getSize().y / 2.f) + 23.f });

    sf::RectangleShape practiceButton(sf::Vector2f(400.f, 80.f));
    practiceButton.setFillColor(sf::Color::Black);
    practiceButton.setPosition({ (windowSize.x - objectSize.x) / 2.f, (windowSize.y - objectSize.y) / 2.f - 241.f });

    sf::Text practiceButtonText(font, "Practice", 24);
    practiceButtonText.setFillColor(sf::Color::Green);
    practiceButtonText.setOrigin({ practiceButtonText.getLocalBounds().position.x + practiceButtonText.getLocalBounds().size.x / 2.f,
                                   practiceButtonText.getLocalBounds().position.y + practiceButtonText.getLocalBounds().size.y / 2.f });
    practiceButtonText.setPosition({ practiceButton.getPosition().x + practiceButton.getSize().x / 2.f,
                                    practiceButton.getPosition().y + practiceButton.getSize().y / 2.f });

    // ---- Hint cover and its toggle ----
    sf::RectangleShape hintCover(sf::Vector2f(300.f, 55.f));
    hintCover.setFillColor(sf::Color::Black);
    hintCover.setPosition({ bigTextButton.getPosition().x + bigTextButton.getSize().x / 2.f - 85.f,
                            bigTextButton.getPosition().y + bigTextButton.getSize().y / 2.f + 140.f });

    sf::RectangleShape hintToggleButton(sf::Vector2f(50.f, 50.f));
    hintToggleButton.setFillColor(sf::Color::White);
    hintToggleButton.setPosition({ bigTextButton.getPosition().x + bigTextButton.getSize().x / 2.f + 300.f,
                                  bigTextButton.getPosition().y + bigTextButton.getSize().y / 2.f + 150.f });

    sf::Text hintToggleLabel(font, "ON", 12);
    hintToggleLabel.setFillColor(sf::Color::Blue);
    hintToggleLabel.setOrigin({ hintToggleLabel.getLocalBounds().position.x + hintToggleLabel.getLocalBounds().size.x / 2.f,
                                hintToggleLabel.getLocalBounds().position.y + hintToggleLabel.getLocalBounds().size.y / 2.f });
    hintToggleLabel.setPosition({ hintToggleButton.getPosition().x + hintToggleButton.getSize().x / 2.f,
                                 hintToggleButton.getPosition().y + hintToggleButton.getSize().y / 2.f });

    // ---- Lesson/practice screen texts ----
    sf::Text hintText(font, "The HINT", 40);
    hintText.setFillColor(sf::Color(255, 255, 255, 100));
    hintText.setOrigin({ hintText.getLocalBounds().position.x + hintText.getLocalBounds().size.x / 2.f,
                         hintText.getLocalBounds().position.y + hintText.getLocalBounds().size.y / 2.f });
    hintText.setPosition({ bigTextButton.getPosition().x + bigTextButton.getSize().x / 2.f,
                           bigTextButton.getPosition().y + bigTextButton.getSize().y / 2.f + 150.f });

    sf::Text inputText(font, "Some Input", 40);
    inputText.setFillColor(sf::Color::Green);
    inputText.setOrigin({ inputText.getLocalBounds().position.x + inputText.getLocalBounds().size.x / 2.f,
                          inputText.getLocalBounds().position.y + inputText.getLocalBounds().size.y / 2.f });
    inputText.setPosition({ bigTextButton.getPosition().x + bigTextButton.getSize().x / 2.f,
                            bigTextButton.getPosition().y + bigTextButton.getSize().y / 2.f + 300.f });

    sf::Text progressText(font, "Progress", 30);
    progressText.setFillColor(sf::Color::Yellow);
    progressText.setOrigin({ progressText.getLocalBounds().position.x + progressText.getLocalBounds().size.x / 2.f,
                             progressText.getLocalBounds().position.y + progressText.getLocalBounds().size.y / 2.f });
    progressText.setPosition({ bigTextButton.getPosition().x + bigTextButton.getSize().x / 2.f,
                               bigTextButton.getPosition().y + bigTextButton.getSize().y / 2.f + 225.f });

    sf::Text screenTitle(font, "", 30);
    screenTitle.setFillColor(sf::Color::White);
    screenTitle.setOrigin({ screenTitle.getLocalBounds().position.x + screenTitle.getLocalBounds().size.x / 2.f,
                            screenTitle.getLocalBounds().position.y + screenTitle.getLocalBounds().size.y / 2.f });
    screenTitle.setPosition({ 800.f, 20.f });

    sf::Text messageText(font, "Some Message", 30);
    messageText.setFillColor(sf::Color::Red);
    messageText.setOrigin({ messageText.getLocalBounds().position.x + messageText.getLocalBounds().size.x / 2.f,
                            messageText.getLocalBounds().position.y + messageText.getLocalBounds().size.y / 2.f });
    messageText.setPosition({ bigTextButton.getPosition().x + bigTextButton.getSize().x / 2.f + 375.f,
                              bigTextButton.getPosition().y + bigTextButton.getSize().y / 2.f + 600.f });

    sf::Text congratsText(font, "Congratulations!", 30);
    congratsText.setFillColor(sf::Color::Green);
    congratsText.setPosition({ 1200, 300 });

    sf::RectangleShape translationButton(sf::Vector2f(400.f, 80.f));
    translationButton.setFillColor(sf::Color::Black);
    translationButton.setPosition({ (windowSize.x - objectSize.x) / 2.f, (windowSize.y - objectSize.y) / 2.f - 120.f });

    sf::RectangleShape exitButton(sf::Vector2f(400.f, 80.f));
    exitButton.setPosition({ (windowSize.x - objectSize.x) / 2.f, (windowSize.y - objectSize.y) / 2.f + 120.f });
    exitButton.setFillColor(sf::Color::Black);

    // ---- Small side buttons ----
    sf::RectangleShape backButton(sf::Vector2f(80.f, 50.f));
    backButton.setFillColor(sf::Color::Green);
    backButton.setPosition({ 1825.f, 20.f });

    sf::RectangleShape clearButton(sf::Vector2f(80.f, 50.f));
    clearButton.setFillColor(sf::Color::Green);
    clearButton.setPosition({ backButton.getGlobalBounds().position.x,
                             backButton.getGlobalBounds().position.y + backButton.getGlobalBounds().size.y + 30.f });

    sf::RectangleShape switchButton(sf::Vector2f(80.f, 50.f));
    switchButton.setFillColor(sf::Color::White);
    switchButton.setPosition({ clearButton.getGlobalBounds().position.x,
                               clearButton.getGlobalBounds().position.y + clearButton.getGlobalBounds().size.y + 150.f });

    sf::RectangleShape nextLessonButton(sf::Vector2f(80.f, 50.f));
    nextLessonButton.setFillColor(sf::Color::Green);
    nextLessonButton.setPosition({ switchButton.getGlobalBounds().position.x,
                                   switchButton.getGlobalBounds().position.y + switchButton.getGlobalBounds().size.y + 150.f });

    sf::RectangleShape leftSidebar(sf::Vector2f(150.f, window.getSize().y));
    leftSidebar.setFillColor(sf::Color::White);
    leftSidebar.setPosition({ bigTextButton.getPosition().x, 0.f });

    sf::RectangleShape rightSidebar(sf::Vector2f(200.f, window.getSize().y));
    rightSidebar.setFillColor(sf::Color::White);
    rightSidebar.setPosition({ bigTextButton.getPosition().x + bigTextButton.getSize().x, 0.f });

    sf::RectangleShape sidebar(sf::Vector2f(500.f, window.getSize().y));
    sidebar.setFillColor(sf::Color(50, 50, 50));
    sidebar.setPosition({ 0.f, 0.f });

    // ---- Button labels ----
    sf::Text nextLessonLabel(font, "Next Lesson", 18);
    nextLessonLabel.setFillColor(sf::Color::Red);
    nextLessonLabel.setOrigin({ nextLessonLabel.getLocalBounds().position.x + nextLessonLabel.getLocalBounds().size.x / 2.f,
                                nextLessonLabel.getLocalBounds().position.y + nextLessonLabel.getLocalBounds().size.y / 2.f });
    nextLessonLabel.setPosition({ nextLessonButton.getPosition().x + nextLessonButton.getSize().x / 2.f,
                                  nextLessonButton.getPosition().y + nextLessonButton.getSize().y / 2.f });

    sf::Text switchLabel(font, "Switch", 18);
    switchLabel.setFillColor(sf::Color::Red);
    switchLabel.setOrigin({ switchLabel.getLocalBounds().position.x + switchLabel.getLocalBounds().size.x / 2.f,
                            switchLabel.getLocalBounds().position.y + switchLabel.getLocalBounds().size.y / 2.f });
    switchLabel.setPosition({ switchButton.getPosition().x + switchButton.getSize().x / 2.f,
                              switchButton.getPosition().y + switchButton.getSize().y / 2.f });

    sf::CircleShape morseKey(40.f); // 40.f radius for 80.f diameter
    morseKey.setFillColor(sf::Color::Red);

    sf::Text morseKeyLabel(font, "Morse Key", 15);
    morseKeyLabel.setFillColor(sf::Color::White);

    sf::Text clearLabel(font, "Clear", 18);
    clearLabel.setFillColor(sf::Color::Red);
    clearLabel.setOrigin({ clearLabel.getLocalBounds().position.x + clearLabel.getLocalBounds().size.x / 2.f,
                           clearLabel.getLocalBounds().position.y + clearLabel.getLocalBounds().size.y / 2.f });
    clearLabel.setPosition({ clearButton.getPosition().x + clearButton.getSize().x / 2.f,
                            clearButton.getPosition().y + clearButton.getSize().y / 2.f });

    sf::Text lessonButtonText(font, "LESSONS", 24);
    lessonButtonText.setFillColor(sf::Color::Green);
    lessonButtonText.setOrigin({ lessonButtonText.getLocalBounds().position.x + lessonButtonText.getLocalBounds().size.x / 2.f,
                                lessonButtonText.getLocalBounds().position.y + lessonButtonText.getLocalBounds().size.y / 2.f });
    lessonButtonText.setPosition({ lessonButton.getPosition().x + lessonButton.getSize().x / 2.f,
                                  lessonButton.getPosition().y + lessonButton.getSize().y / 2.f });

    sf::Text translationButtonText(font, "Translation", 24);
    translationButtonText.setFillColor(sf::Color::Green);
    translationButtonText.setOrigin({ translationButtonText.getLocalBounds().position.x + translationButtonText.getLocalBounds().size.x / 2.f,
                                      translationButtonText.getLocalBounds().position.y + translationButtonText.getLocalBounds().size.y / 2.f });
    translationButtonText.setPosition({ translationButton.getPosition().x + translationButton.getSize().x / 2.f,
                                        translationButton.getPosition().y + translationButton.getSize().y / 2.f });

    sf::Text exitText(font, "EXIT", 24);
    exitText.setFillColor(sf::Color::Green);
    exitText.setOrigin({ exitText.getLocalBounds().position.x + exitText.getLocalBounds().size.x / 2.f,
                         exitText.getLocalBounds().position.y + exitText.getLocalBounds().size.y / 2.f });
    exitText.setPosition({ exitButton.getPosition().x + exitButton.getSize().x / 2.f,
                           exitButton.getPosition().y + exitButton.getSize().y / 2.f });

    sf::Text backLabel(font, "Back", 18);
    backLabel.setFillColor(sf::Color::Red);
    backLabel.setOrigin({ backLabel.getLocalBounds().position.x + backLabel.getLocalBounds().size.x / 2.f,
                          backLabel.getLocalBounds().position.y + backLabel.getLocalBounds().size.y / 2.f });
    backLabel.setPosition({ backButton.getPosition().x + backButton.getSize().x / 2.f,
                            backButton.getPosition().y + backButton.getSize().y / 2.f });

    // ---- Translation mode widgets ----
    std::string typedInput;
    sf::Text typedInputText(font, " ", 24);
    typedInputText.setFillColor(sf::Color::White);
    typedInputText.setPosition({ 50.f, 50.f });

    std::string typedOutput;
    sf::Text typedOutputText(font, " ", 24);
    typedOutputText.setFillColor(sf::Color::Green);
    typedOutputText.setPosition({ 50.f, 550.f });

    sf::RectangleShape inputBox(sf::Vector2f(1000.f, 400.f));
    inputBox.setFillColor(sf::Color::Black);
    inputBox.setPosition({ 0.f, 0.f });

    sf::RectangleShape outputBox(sf::Vector2f(1000.f, 400.f));
    outputBox.setFillColor(sf::Color::Black);
    outputBox.setPosition({ 0.f, 500.f });

    // ---- Morse key press timing and sounds ----
    sf::Clock pressClock;
    bool isPressed = false;

    sf::SoundBuffer dotBuffer, dashBuffer;
    if (!dotBuffer.loadFromFile("assets/dot.wav") || !dashBuffer.loadFromFile("assets/dash.wav")) { // loading sounds
        std::cerr << "Error loading sound files." << std::endl;
        return 1;
    }

    sf::Sound dotSound(dotBuffer), dashSound(dashBuffer), spaceSound(dotBuffer);
    spaceSound.setPitch(0.5f);
    spaceSound.setVolume(50);

    std::cout << "Practice Mode - currentStep: " << currentStep << "/" << practiceSteps.size() << std::endl;

    while (window.isOpen()) { // Main application loop, runs until the window is closed
        while (const std::optional event = window.pollEvent()) { // Poll for all window events (keyboard, mouse, etc.)
            if (event->is<sf::Event::Closed>()) { // If the user closes the window
                window.close(); // Close the window and exit the loop
            }

            // Handle mouse button press events
            if (const auto* mousePressed = event->getIf<sf::Event::MouseButtonPressed>(); mousePressed && mousePressed->button == sf::Mouse::Button::Left) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window)); // Get mouse position in window coordinates

                // Main menu button logic
                if (currentState == AppStates::MainMenu) {
                    if (lessonButton.getGlobalBounds().contains(mousePos)) { // Clicked "Lessons" button
                        currentState = AppStates::LessonMode; // Switch to lesson mode
                        currentStep = 0; // Reset lesson step
                        currentAttempt = 1; // Reset attempt count
                        lessonInput.clear(); // Clear lesson input
                        messageText.setString(""); // Clear feedback message
                    } else if (translationButton.getGlobalBounds().contains(mousePos)) { // Clicked "Translation" button
                        currentState = AppStates::Morse2Text; // Switch to Morse2Text mode
                    } else if (exitButton.getGlobalBounds().contains(mousePos)) { // Clicked "Exit" button
                        window.close(); // Close the window
                    } else if (practiceButton.getGlobalBounds().contains(mousePos)) { // Clicked "Practice" button
                        currentState = AppStates::Practice; // Switch to practice mode
                        currentStep = 0; // Reset practice step
                        currentAttempt = 1; // Reset attempt count
                        lessonInput.clear(); // Clear lesson input
                        messageText.setString(""); // Clear feedback message

                        // Load the first practice file
                        if (!loadPractice("bin/practice/practice1.txt", practiceSteps)) {
                            std::cerr << "Failed to load the Practice file.bin/practice/practice1.txt" << std::endl;
                        }

                        // Setup sidebar buttons and labels for practice mode
                        sidebarPracticeButtons.clear();
                        sidebarPracticeLabels.clear();
                        const int totalPracticeFiles = 40;
                        const int practicePerCol = 10;
                        for (int i = 0; i < totalPracticeFiles; ++i) {
                            int col = i / practicePerCol;
                            int row = i % practicePerCol;
                            sf::RectangleShape btn(sf::Vector2f(80.f, 40.f));
                            btn.setFillColor(sf::Color(200, 200, 255));
                            btn.setPosition({ sidebarStart.x + col * (buttonWidth + spacing),
                                              sidebarStart.y + row * (buttonHeight + spacing) - 40.f });
                            sidebarPracticeButtons.push_back(btn);

                            sf::Text label(font, "P" + std::to_string(i + 1), 16);
                            label.setFillColor(sf::Color::Black);
                            label.setPosition({ btn.getPosition().x + 40.f, btn.getPosition().y + 5.f });
                            sidebarPracticeLabels.push_back(label);
                        }
                    }
                }
                // Handle clicks in other modes (Translation, Lesson, Practice)
                else if (currentState == AppStates::Text2Morse ||
                         currentState == AppStates::Morse2Text ||
                         currentState == AppStates::LessonMode ||
                         currentState == AppStates::Practice) {

                    // Switch between Text2Morse and Morse2Text modes
                    if (currentState == AppStates::Text2Morse || currentState == AppStates::Morse2Text) {
                        if (switchButton.getGlobalBounds().contains(mousePos)) {
                            currentState = (currentState == AppStates::Text2Morse) ? AppStates::Morse2Text : AppStates::Text2Morse;
                        }
                    }

                    // Handle "Back" button click
                    if (backButton.getGlobalBounds().contains(mousePos)) {
                        currentState = AppStates::MainMenu; // Return to main menu
                    }
                    // Handle "Clear" button click
                    else if (clearButton.getGlobalBounds().contains(mousePos)) {
                        typedInput.clear();
                        typedOutput.clear();
                        lessonInput.clear();
                    }
                    // Handle "Next Lesson" button click
                    else if (nextLessonButton.getGlobalBounds().contains(mousePos)) {
                        if (loadNextLesson(lessonSteps, currentLessonIndex, lessonFilename)) {
                            currentStep = 0;
                            currentAttempt = 1;
                            lessonInput.clear();
                            messageText.setString("");
                        } else {
                            messageText.setString("No more lessons.");
                            currentLessonIndex--;
                        }
                    }
                    // Handle lesson sidebar and hint cover toggling in lesson mode
                    else if (currentState == AppStates::LessonMode) {
                        if (hintToggleButton.getGlobalBounds().contains(mousePos)) {
                            showHint = !showHint;
                        }
                        for (size_t i = 0; i < sidebarLessonButtons.size(); i++) {
                            if (sidebarLessonButtons[i].getGlobalBounds().contains(mousePos)) {
                                currentLessonIndex = static_cast<int>(i) + 1;
                                lessonFilename = "bin/Lessons/Lesson" + std::to_string(currentLessonIndex) + ".txt";
                                if (!loadLesson(lessonFilename, lessonSteps)) {
                                    std::cerr << "Failed to load the Lesson : " << lessonFilename << std::endl;
                                } else {
                                    currentStep = 0;
                                    currentAttempt = 1;
                                    lessonInput.clear();
                                    messageText.setString("");
                                }
                            }
                        }
                    }
                    // Handle practice sidebar and hint cover toggling in practice mode
                    else if (currentState == AppStates::Practice) {
                        if (hintToggleButton.getGlobalBounds().contains(mousePos)) {
                            showHint = !showHint;
                        }
                        for (size_t i = 0; i < sidebarPracticeButtons.size(); ++i) {
                            if (sidebarPracticeButtons[i].getGlobalBounds().contains(mousePos)) {
                                currentStep = 0;
                                currentAttempt = 1;
                                practiceInput.clear();
                                messageText.setString("");
                                std::string filename = "bin/practice/practice" + std::to_string(i + 1) + ".txt";
                                if (!loadPractice(filename, practiceSteps)) {
                                    std::cerr << "Failed to load practice file: " << filename << std::endl;
                                }
                            }
                        }
                    }
                }

                // Handle morseKey press (for Morse code input)
                if (morseKey.getGlobalBounds().contains(mousePos)) {
                    (void)pressClock.restart(); // Start timing the press
                    isPressed = true; // Mark as pressed
                }
            }

            // Handle morseKey release (determine dot/dash/space)
            if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>(); mouseReleased && (mouseReleased->button == sf::Mouse::Button::Left || mouseReleased->button == sf::Mouse::Button::Right) && isPressed) {
                float duration = pressClock.getElapsedTime().asSeconds(); // Get press duration
                char inputChar = (duration < 0.2f) ? '.' : (duration < 0.5f) ? '-' : ' '; // Short press = dot, medium = dash, long = space

                // Add Morse input to the correct variable based on current state
                if (currentState == AppStates::LessonMode && currentStep < lessonSteps.size()) {
                    lessonInput += inputChar;
                } else if (currentState == AppStates::Practice && currentStep < practiceSteps.size()) {
                    practiceInput += inputChar;
                } else {
                    typedInput += inputChar;
                }

                // Play corresponding sound
                if (inputChar == '.') dotSound.play();
                else if (inputChar == '-') dashSound.play();
                else { spaceSound.play(); }

                isPressed = false; // Reset pressed state
            }

            // Handle keyboard text input events
            if (const auto* textEntered = event->getIf<sf::Event::TextEntered>()) {
                char typed = static_cast<char>(textEntered->unicode); // Convert Unicode to ASCII

                // Lesson mode input handling
                if (currentState == AppStates::LessonMode && currentStep < lessonSteps.size()) {
                    if (typed == 8 && !lessonInput.empty()) { // Backspace
                        lessonInput.pop_back();
                    } else if (typed == '.' || typed == '-') { // Dot or dash
                        lessonInput += typed;
                    } else if (typed == '\r' || typed == '\n') { // Enter key to check answer
                        const LessonStep& step = lessonSteps[currentStep];
                        if (lessonInput == step.morse) {
                            if (currentAttempt >= step.repeat) {
                                messageText.setString("Correct!");
                                currentStep++;
                                currentAttempt = 1;
                            } else {
                                messageText.setString("Correct! Try again.");
                                currentAttempt++;
                            }
                        } else {
                            messageText.setString("Incorrect, try again.");
                        }
                        lessonInput.clear();
                    }
                } else {
                    // General input handling for translation modes
                    if (typed == 8 && !typedInput.empty()) { // Backspace
                        typedInput.pop_back();
                    } else if (typed >= 32 && typed <= 127) { // Printable ASCII
                        typedInput += typed;
                    }
                }

                // Practice mode input handling
                if (currentState == AppStates::Practice && currentStep < practiceSteps.size()) {
                    if (typed == 8 && !practiceInput.empty()) { // Backspace
                        practiceInput.pop_back();
                    } else if (typed == '.' || typed == '-') { // Dot or dash
                        practiceInput += typed;
                    } else if (typed == '\r' || typed == '\n') { // Enter key to check answer
                        const PracticeStep& step = practiceSteps[currentStep];
                        if (practiceInput == step.hintMorse) {
                            messageText.setString("Correct!");
                            currentStep++;
                            if (currentStep >= practiceSteps.size()) {
                                messageText.setString("Congratulations! Practice completed.");
                            }
                        } else {
                            messageText.setString("Incorrect, try again.");
                        }
                        practiceInput.clear();
                    }
                }
            }
        } // End of event polling loop

        // Update and redraw the UI based on current state and variables
        updateUI(window, currentState, typedInput, typedOutput, font, typedInputText, typedOutputText,
                 inputBox, outputBox, backButton, backLabel, clearButton, clearLabel, switchButton, switchLabel,
                 morseKey, morseKeyLabel, lessonButton, lessonButtonText, translationButton, translationButtonText,
                 practiceButton, practiceButtonText, exitButton, exitText, lessonSteps, currentStep, currentAttempt,
                 lessonInput, bigText, hintText, inputText, progressText, messageText, congratsText, bigTextButton,
                 nextLessonButton, nextLessonLabel, sidebarLessonButtons, sidebarLessonLabels, currentLessonIndex,
                 sidebar, leftSidebar, rightSidebar, hintCover, hintToggleButton, hintToggleLabel, showHint, screenTitle);
    } // End of main application loop
}
