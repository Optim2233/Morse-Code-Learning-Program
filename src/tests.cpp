// Unit tests for the pure logic: Morse translation and the two file parsers.
// Build and run (from the project root):
//   g++ -std=c++17 -o bin/run_tests.exe src/tests.cpp src/morse.cpp src/content.cpp && bin\run_tests.exe
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include "morse.hpp"
#include "content.hpp"

static void writeTempFile(const std::string& name, const std::string& content) {
    std::ofstream out(name);
    out << content;
}

int main() {
    // --- Morse translation ---
    assert(textToMorse("SOS") == "... --- ... ");
    assert(textToMorse("hello world") == ".... . .-.. .-.. --- / .-- --- .-. .-.. -.. ");
    assert(textToMorse("?") == "?"); // unknown char maps to '?'
    assert(morseToText("... --- ...") == "SOS");
    assert(morseToText("..--") == "?"); // unknown Morse token maps to '?'
    assert(morseToText(textToMorse("HELLO WORLD")) == "HELLO WORLD"); // round-trip

    // --- Lesson parser ---
    writeTempFile("test_lesson.txt", "A .- 2\nBADTOKEN .. 3\nb -... 1\n");
    std::vector<LessonStep> lessonSteps;
    assert(loadLesson("test_lesson.txt", lessonSteps));
    assert(lessonSteps.size() == 2);          // "BADTOKEN" skipped: letter must be one char
    assert(lessonSteps[0].letter == 'A' && lessonSteps[0].morse == ".-" && lessonSteps[0].repeat == 2);
    assert(lessonSteps[1].letter == 'B');     // lowercase 'b' is uppercased
    std::remove("test_lesson.txt");

    // --- Practice parser ---
    writeTempFile("test_practice.txt", "HELLO\n.... .\n\nWORLD\n.---\n"); // blank line must not shift pairing
    std::vector<PracticeStep> practiceSteps;
    assert(loadPractice("test_practice.txt", practiceSteps));
    assert(practiceSteps.size() == 2);
    assert(practiceSteps[0].text == "HELLO" && practiceSteps[0].hintMorse == ".... .");
    assert(practiceSteps[1].text == "WORLD" && practiceSteps[1].hintMorse == ".---");
    std::remove("test_practice.txt");

    // --- Practice parser: dangling text line without a Morse hint is dropped ---
    writeTempFile("test_practice.txt", "HELLO\n.... .\nEXTRA\n");
    assert(loadPractice("test_practice.txt", practiceSteps));
    assert(practiceSteps.size() == 1);
    std::remove("test_practice.txt");

    // --- Missing files fail cleanly ---
    assert(!loadLesson("test_missing_lesson.txt", lessonSteps));
    assert(!loadPractice("test_missing_practice.txt", practiceSteps));

    std::cout << "All tests passed." << std::endl;
    return 0;
}
