#pragma once
#include <string>
#include <vector>

// One step of a lesson: a letter, its Morse code, and how many times to repeat it.
struct LessonStep {
    char letter;
    std::string morse;
    int repeat;
};

// One step of a practice: a word/sentence and its Morse code as a hint.
struct PracticeStep {
    std::string text;
    std::string hintMorse;
};

// Counts content files in dir whose name starts with prefix and ends with ".txt".
// Used so the sidebars always match what is actually on disk, not a hardcoded number.
int countContentFiles(const std::string& dir, const std::string& prefix);

// Lesson files are token-based, one step per line: e.g. "A .- 5"
bool loadLesson(const std::string& filename, std::vector<LessonStep>& steps);

// Practice files are line-based pairs: text line, then Morse hint line.
bool loadPractice(const std::string& filename, std::vector<PracticeStep>& practiceSteps);

// Advances to the next lesson file (bin/lessons/LessonN.txt); used by the "Next Lesson" button.
bool loadNextLesson(std::vector<LessonStep>& lessonSteps, int& currentLessonIndex, std::string& lessonFilename);
