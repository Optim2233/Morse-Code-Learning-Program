#include "content.hpp"
#include <fstream>
#include <iostream>
#include <cctype>
#include <filesystem>

int countContentFiles(const std::string& dir, const std::string& prefix) {
    int count = 0;
    std::error_code ec;
    for (const auto& entry : std::filesystem::directory_iterator(dir, ec)) {
        const std::string& name = entry.path().filename().string();
        if (name.compare(0, prefix.size(), prefix) == 0 && name.size() >= 4 && name.compare(name.size() - 4, 4, ".txt") == 0)
            ++count;
    }
    return count;
}

bool loadLesson(const std::string& filename, std::vector<LessonStep>& steps) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open Lesson file: " << filename << std::endl; // std::cerr is used for error feedbacks instead of regular std::cout
        return false;
    }
    steps.clear(); // Clear leftovers from the previous lessons
    std::string letterstr, morse; // temporary variables
    int repeat;
    while (file >> letterstr >> morse >> repeat) { // access from file: three variables per line, e.g. A .- 5. Token-based
        if (letterstr.size() != 1) continue; // the letter must be a single character; skip the line otherwise
        char upperchar = static_cast<char>(std::toupper(letterstr[0])); // fail-safe in case a lesson file contains e.g. "aa .- 6"
        steps.push_back(LessonStep{ upperchar, morse, repeat });
    }
    std::cout << "LessonSteps loaded: " << steps.size() << std::endl;
    return true;
}

bool loadPractice(const std::string& filename, std::vector<PracticeStep>& practiceSteps) {
    practiceSteps.clear(); // Clear leftovers from the previous practices

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open practice file: " << filename << std::endl;
        return false;
    }

    std::string line;
    PracticeStep step; // temporary struct instead of temporary variables
    int lineCount = 0;

    while (std::getline(file, line)) { // Line-based, unlike the token-based lesson files
        std::cout << "Read line " << lineCount << ": '" << line << "'" << std::endl;
        if (line.empty()) {
            std::cout << "Skipping empty line" << std::endl;
            continue;
        } else if (lineCount % 2 == 0) { // even line: the plain text
            step.text = line; // e.g. "HELLO"
        } else { // odd line: the Morse hint
            step.hintMorse = line; // e.g. ".... . .-.. .-.. ---"
            practiceSteps.push_back(step); // a text/morse pair is complete; store it
            std::cout << "Added step: text='" << step.text << "' hint='" << step.hintMorse << "'" << std::endl;
        }
        lineCount++;
    }

    file.close();

    std::cout << "Total practice steps loaded: " << practiceSteps.size() << std::endl;
    return !practiceSteps.empty(); // practice content comes in pairs; a half-read file counts as a failure
}

bool loadNextLesson(std::vector<LessonStep>& lessonSteps, int& currentLessonIndex, std::string& lessonFilename) {
    currentLessonIndex++;
    lessonFilename = "bin/lessons/Lesson" + std::to_string(currentLessonIndex) + ".txt";
    if (!loadLesson(lessonFilename, lessonSteps)) {
        std::cerr << "Failed to load the lesson :" << lessonFilename << std::endl;
        return false;
    }
    return true;
}
