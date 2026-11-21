#include<SFML/Graphics.hpp>
#include<iostream>
#include<string>
#include<sstream>
#include<fstream>
#include<vector>
#include<cctype>
#include<unordered_map>
#include<SFML/Audio.hpp>
    enum class AppStates { Mainmenu, LessonButton , Morse2Text, Text2Morse, Practice};
    AppStates currentstate = AppStates::Mainmenu;
    struct LessonStep 
    {
        char letter;
        std::string morse;
        int repeat;
    };

    struct PracticeStep 
    {
        std::string text;
        std::string hintmorse;
    }; 
        std::string PracticeInput = "";
        std::vector<PracticeStep> PracticeSteps;//Each vectors has two sub variables, PracticeSteps[0].text,PracticeSteps[0].hintmorse
        std::vector<LessonStep> LessonSteps;// Each vectors has three sub variables ,  LessonSteps[0].letter, LessonSteps[0].morse, LessonSteps[0].repeat
        std::vector<sf::RectangleShape> sidebarPracticeButtons;//for storing practice mode btns
        std::vector<sf::Text> sidebarPracticeLabels;//for storing practice labels
        std::vector<sf::RectangleShape> sidebarLessonbuttons;//for storing lessonmode btns
        std::vector<sf::Text> sidebarlabels;//for storing lesson labels, (Inconsistent Naming Logic: Will be fixed in later patches, Note: If your are seeeing this, then it proably isn't still fixed yet)

    bool LoadLesson(const std::string& filename, std::vector<LessonStep>& steps)//Important Logic that will be used in lesson files loading.
        {
            std::ifstream file(filename);
            if(!file.is_open()){
                std::cerr<<"Failed to open Lesson file: "<<filename<<std::endl;//std::cerr is used for error feedbacks instead of regular std::cout
                return false;
            }
            steps.clear();//Clear leftovers from the previous lessons
            std::string letterstr, morse;//temporary variables
            int repeat; 
            while(file >> letterstr >> morse >> repeat){//access from file: three variables One line E.g. A .- 5. Note: Token-based
                if(letterstr.size() != 1) continue;//check if the letterstr(A) is a single letter,if not, skip the entire line. This is important because the letterstr is converted to char in the later logic
                char upperchar = static_cast<char>(std::toupper(letterstr[0]));//Fail safe in case someone made a mistake in the file content like e.g. AA .- 6, Offers limited protection against incorrect file format
                LessonStep step { upperchar, morse, repeat }; // Stores the temporary variables in the struct. 
                steps.push_back(step);//Stores the struct in the vector, vector offers dynamic array. Best to use vectors here, instead of arrays
            } 
            std::cout << "LessonSteps loaded: " << LessonSteps.size() << std::endl;
            return true;//The lesson loading is completed successfully
        }

    bool loadPractice(const std::string& filename, std::vector<PracticeStep>& practiceSteps) {// Same logic as LoadLesson asides from file reading differences due to different file format
    practiceSteps.clear();//Clear leftovers from the previous practices

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open practice file: " << filename << std::endl;//std::cerr is used for error feedbacks instead of regular std::cout
        return false;
    }

    std::string line;
    PracticeStep step;//temporary struct instead of temporary variables
    int lineCount = 0;

    while (std::getline(file, line)) {//notice that the file reading method is different due to the different file format. Notee: Line-based
        std::cout << "Read line " << lineCount << ": '" << line << "'" << std::endl;  // e.g. Practice file format : HELLO
        if (line.empty())                                                             //                             .... . .-.. .-.. ---
        {
            std::cout << "Skipping empty line" << std::endl;   
            continue;
        }

        else if (lineCount % 2 == 0)//check if the line is even child, if so, it is the big text. 
        {
            step.text = line;//e.g. line has data "Hello"
        }else //check if the line is odd child, if so it is the hintmorse
        {
            step.hintmorse = line;//e.g. line has data ".... . .-.. .-.. ---"
            practiceSteps.push_back(step);//each pair of practice code ends in odd child, if odd child is read, the pair of text and morsetext is completed and pushed to vector
            std::cout << "Added step: text='" << step.text << "' hint='" << step.hintmorse << "'" << std::endl;
        }
        lineCount++;
    }

    file.close();

    std::cout << "Total practice steps loaded: " << practiceSteps.size() << std::endl;
    return !practiceSteps.empty();//This is intentional as practice format comes in pair and more prone to errors or corruptions. practiceSteps.empty() return true even if only one out of pair is empty
}
//Main logic of Translation Mode
    const std::unordered_map<char , std::string> MorseMap = {//Map links one variable to another, If we know the first variable, we can access the second variable
        {'A', ".-"},   {'B', "-..."}, {'C', "-.-."}, {'D', "-.."},//If we have A, we can access A's morse code, .-
        {'E', "."},    {'F', "..-."}, {'G', "--."},  {'H', "...."},
        {'I', ".."},   {'J', ".---"}, {'K', "-.-"},  {'L', ".-.."},
        {'M', "--"},   {'N', "-."},   {'O', "---"},  {'P', ".--."},
        {'Q', "--.-"}, {'R', ".-."},  {'S', "..."},  {'T', "-"},
        {'U', "..-"},  {'V', "...-"}, {'W', ".--"},  {'X', "-..-"},
        {'Y', "-.--"}, {'Z', "--.."}, {'0', "-----"}, {'1', ".----"},
        {'2', "..---"},{'3', "...--"}, {'4', "....-"}, {'5', "....."},
        {'6', "-...."},{'7', "--..."}, {'8', "---.."}, {'9', "----."},
        {' ', "/"}, {'.' , ".-.-.-"}
        };

    std::string Translation2Morse(const std::string& input){//This function makes use of the map
        std::string output;
        for(char c : input){//input is a string (e.g. roman) checks for every char in the string
            char upper = std::toupper(static_cast<unsigned char>(c));//converts every char into captial, explicit type conversion
            auto it = MorseMap.find(upper);// here we find the corresponding second variable for every char. Note: auto is used. find don't return what u think they would, what auto is subsituting here is "std::unordered_map<char, std::string>::const_iterator"
            if(it != MorseMap.end()){//if "it" is found
                output += it->second ;//the output is the corresponding variable. Note: -> is called member access operator like '.' but for pointers
                output += " ";//adds a space
            }else{output+="?";}//if char is not in the map
        }
        return output;//give corresponding variable
    }

    std::string MorseToText(const std::string & MorseInput){
        std::string output, token;
        std::unordered_map<std::string , char > reverseMap;// make a map that has first variable as string and second as char

        for(const auto& pair : MorseMap){ //most important and confusing logic here. for every pair(e.g. A,.-) in morsemap
        reverseMap[pair.second] = pair.first; //reverseMap[.-] = A
        }

        std::istringstream stream(MorseInput ); //splits the string by spaces

        while(stream >> token){//token stores each space separated substring
            auto it = reverseMap.find(token); // same logic
            if(it != reverseMap.end()){
                output += it->second;
            }else{
                output += '?'; // Unknown Morse code
            }
        }
        return output;
    }

    std::string wraptext(std::string& str, sf::Font& font, unsigned char charsize , float maxwidth){// basically, it adds a line break once the string passes a set maximum limit
        std::string wrap, word;
        float linewidth = 0;
        float spacewidth =sf::Text(" ",font,charsize).getLocalBounds().width;//we need the with of space on the sfml gui(pixel). Not the length. thus, getLocalBounds().width is used

        for(size_t i = 0; i <= str.size(); ++i){//size_t is used here. Oversimplified explanation is int is used for small looping. size_t is used for larger looping
            if(i == str.size() || str[i]== ' '){//checks for end of string(input) or end with space
                sf::Text temp(word,font,charsize);//declare a temporary text object
                float wordwidth = temp.getLocalBounds().width;//gets the width of that object

                if(linewidth + spacewidth > maxwidth){  //checks whether it exceeds the set max limit
                    wrap += '\n';//if so, add line break or rather add new line
                    linewidth = 0;//and set the linewidth to 0
                }
                wrap += word;//whether it exceeds or not, add the word into the wrap
                if(i < str.size()){//if the i is not at the end of the input
                    wrap += ' ';//adds a space after adding word to the wrap
                    linewidth += spacewidth;//add spacewidth to the linewidth
                }
                linewidth += wordwidth;//add wordwidth to the linewidth
                word.clear();//clears the word 
            } else{
            word+= str[i];//if not the end of input or end with space, add normally
        }
        }
        if(!word.empty()){//This condition is for safety check. Note: This if-condition is redundant and will be removed on future patches  
            sf::Text temp(word,font, charsize);
            float wordwidth = temp.getLocalBounds().width;
            if(wordwidth + spacewidth > maxwidth){
                wrap += '\n';
            }
            wrap += word;
        }
    return wrap;
    }

    int currentlessonindex = 1 ;
    bool loadNextLesson(std::vector<LessonStep>& LessonSteps, int& currentlessonindex, std::string& Lessonfilename) {//this function is exclusive to nextlessonbutton 
    currentlessonindex++;
    Lessonfilename = "bin/Lessons/Lesson" + std::to_string(currentlessonindex) + ".txt";
    if (!LoadLesson(Lessonfilename, LessonSteps)) {
        std::cerr << "Failed to load the lesson :" << Lessonfilename << std::endl;
        return false;
    }
    return true;
}

//UI Drawing Functions. Functions for better management and easier readabiltiy
void drawMainMenu(sf::RenderWindow& window, sf::RectangleShape& LessonButton, sf::Text& LessonButtonText,
                  sf::RectangleShape& TranslationButton, sf::Text& TranslationText,
                  sf::RectangleShape& PracticeButton, sf::Text& PracticeText,
                  sf::RectangleShape& exitbutton, sf::Text& exitText) {
    window.clear(sf::Color::Blue);
    window.draw(PracticeButton);
    window.draw(PracticeText);
    window.draw(LessonButton);
    window.draw(LessonButtonText);
    window.draw(TranslationButton);
    window.draw(TranslationText);
    window.draw(exitbutton);
    window.draw(exitText);
    window.display();
}

void drawText2Morse(sf::RenderWindow& window, sf::Font& font, const std::string& Input, const std::string& Output,
                    sf::Text& InputText, sf::Text& OutputText, sf::RectangleShape& InputBox, sf::RectangleShape& OutputBox,
                    sf::RectangleShape& Backbutton, sf::Text& BackText, sf::RectangleShape& Clearbutton, sf::Text& ClearText,
                    sf::RectangleShape& SwitchButton, sf::Text& SwitchText,sf::Text& TranslationHead) {
    std::string wrapout = wraptext(const_cast<std::string&>(Output), font, 24, 1000);
    std::string wrapin = wraptext(const_cast<std::string&>(Input), font, 24, 800);
    InputText.setString("Input: " + wrapin);
    OutputText.setString("Output: " + wrapout);

    window.clear(sf::Color::Black);
    window.draw(InputBox);
    window.draw(InputText);
    window.draw(OutputBox);
    window.draw(OutputText);
    window.draw(Backbutton);
    window.draw(BackText);
    window.draw(Clearbutton);
    window.draw(ClearText);
    window.draw(SwitchButton);
    window.draw(SwitchText);
    TranslationHead.setString("Text to Morse Code");
    window.draw(TranslationHead);
    window.display();
}

void drawMorse2Text(sf::RenderWindow& window, sf::Font& font, const std::string& Input, const std::string& Output,
                    sf::Text& InputText, sf::Text& OutputText, sf::RectangleShape& InputBox, sf::RectangleShape& OutputBox,
                    sf::RectangleShape& Backbutton, sf::Text& BackText, sf::RectangleShape& Clearbutton, sf::Text& ClearText,
                    sf::CircleShape& MorseKey, sf::Text& MorseKeytext,
                    sf::RectangleShape& SwitchButton, sf::Text& SwitchText,sf::Text& TranslationHead) {
    std::string wrapout = wraptext(const_cast<std::string&>(Output), font, 24, 1000);
    std::string wrapin = wraptext(const_cast<std::string&>(Input), font, 24, 800);
    InputText.setString("Input: " + wrapin);
    OutputText.setString("Output: " + wrapout);

    window.clear(sf::Color::Black);
    window.draw(InputBox);
    window.draw(InputText);
    window.draw(OutputBox);
    window.draw(OutputText);
    window.draw(Backbutton);
    window.draw(BackText);
    window.draw(Clearbutton);
    window.draw(ClearText);
    MorseKey.setPosition(Clearbutton.getGlobalBounds().left, Clearbutton.getGlobalBounds().top + Clearbutton.getGlobalBounds().height + 30.f);
    MorseKeytext.setOrigin(
        MorseKeytext.getLocalBounds().left + MorseKeytext.getLocalBounds().width / 2.f,
        MorseKeytext.getLocalBounds().top + MorseKeytext.getLocalBounds().height / 2.f);
    MorseKeytext.setPosition(
        MorseKey.getPosition().x + MorseKey.getRadius(),
        MorseKey.getPosition().y + MorseKey.getRadius());
    window.draw(MorseKey);
    window.draw(MorseKeytext);
    window.draw(SwitchButton);
    window.draw(SwitchText);
    TranslationHead.setString("Morse Code to Text");
    window.draw(TranslationHead);
    window.display();
}

void drawLesson(sf::RenderWindow& window, std::vector<LessonStep>& LessonSteps, size_t currentStep, int currentAttempt, const std::string& LessonInput,
                sf::Text& BigText, sf::Text& hintText, sf::Text& inputText, sf::Text& progressText, sf::Text& messageText,
                sf::Text& Congrats, sf::RectangleShape& BigTextButton, sf::CircleShape& MorseKey, sf::Text& MorseKeytext,
                sf::RectangleShape& Backbutton, sf::Text& BackText,
                sf::RectangleShape& NextLessonButton, sf::Text& NextLessonText,
                std::vector<sf::RectangleShape>& sidebarLessonbuttons,
                std::vector<sf::Text>& sidebarlabels,
                int currentlessonindex,sf::RectangleShape& sidebar,sf::RectangleShape& Lsidebar, sf::RectangleShape& Rsidebar,
                sf::RectangleShape& hintcover,sf::RectangleShape& coveracti, sf::Text& coveractswi, bool& show) {
    window.clear(sf::Color::Black);
    
    if (currentStep >= LessonSteps.size()) {
        BigText.setString("");
        hintText.setString("");
        inputText.setString("");
        progressText.setString("");
        window.draw(sidebar);
        window.draw(Lsidebar);
        window.draw(Rsidebar);
        window.draw(Congrats);
        window.draw(Backbutton);
        window.draw(BackText);
        window.draw(NextLessonButton);
        window.draw(NextLessonText);
    } else {
        const LessonStep& step = LessonSteps[currentStep];
        BigText.setString(std::string(1, step.letter));
        hintText.setString(step.morse);
        inputText.setString("Your input _ " + LessonInput);
        progressText.setString("Attempt " + std::to_string(currentAttempt) + " of " + std::to_string(step.repeat));
        window.draw(sidebar);
        window.draw(Lsidebar);
        window.draw(Rsidebar);
        window.draw(Backbutton);
        window.draw(BackText);
        window.draw(BigTextButton);
        window.draw(BigText);
        if(show) {
            window.draw(hintText);
        }
        window.draw(progressText);
        window.draw(inputText);
        window.draw(messageText);
        window.draw(coveracti);
        if(!show){
            coveractswi.setString("ON!!");
        } else {
            coveractswi.setString("OFF!");
        }
        window.draw(coveractswi);

        MorseKey.setPosition(
            BigTextButton.getPosition().x + BigTextButton.getSize().x / 2.f - MorseKey.getRadius(),
            BigTextButton.getPosition().y + BigTextButton.getSize().y / 2.f - MorseKey.getRadius() + 450.f
        );
        MorseKeytext.setOrigin(
            MorseKeytext.getLocalBounds().left + MorseKeytext.getLocalBounds().width / 2.f,
            MorseKeytext.getLocalBounds().top + MorseKeytext.getLocalBounds().height / 2.f
        );
        MorseKeytext.setPosition(
            MorseKey.getPosition().x + MorseKey.getRadius(),
            MorseKey.getPosition().y + MorseKey.getRadius()
        );
        window.draw(MorseKey);
        window.draw(MorseKeytext);
    }

    for (auto& btn : sidebarLessonbuttons)
        btn.setFillColor(sf::Color(200, 200, 200));

    if (currentlessonindex - 1 < sidebarLessonbuttons.size())
        sidebarLessonbuttons[currentlessonindex - 1].setFillColor(sf::Color::Yellow);

    for (const auto& btn : sidebarLessonbuttons)
        window.draw(btn);
    for (const auto& labels : sidebarlabels)
        window.draw(labels);
       

    window.display();
}
void drawPractice(sf::RenderWindow& window, std::vector<PracticeStep>& PracticeSteps, size_t currentStep, const std::string& PracticeInput,
                  sf::Text& BigText, sf::Text& hintText, sf::Text& inputText, sf::Text& messageText,
                  sf::Text& Congrats, sf::RectangleShape& BigTextButton, sf::CircleShape& MorseKey, sf::Text& MorseKeytext,
                  sf::RectangleShape& Backbutton, sf::Text& BackText,
                  sf::RectangleShape& sidebar, sf::RectangleShape& Lsidebar, sf::RectangleShape& Rsidebar,sf::RectangleShape& hintcover,
                  sf::RectangleShape& coveracti, sf::Text& coveractswi, bool& show) {
    
    window.clear(sf::Color::Black);

    if (currentStep >= PracticeSteps.size()) {
        BigText.setString("");
        hintText.setString("");
        inputText.setString("");
        window.draw(sidebar);
        window.draw(Lsidebar);
        window.draw(Rsidebar);
        window.draw(Congrats);
        window.draw(Backbutton);
        window.draw(BackText);
    } else {
        const PracticeStep& step = PracticeSteps[currentStep];
        BigText.setString(step.text);          // Full word/sentence
        hintText.setString(step.hintmorse);    // Faint hint
        inputText.setString("Your input _ " + PracticeInput);
        window.draw(sidebar);
        window.draw(Lsidebar);
        window.draw(Rsidebar);
        window.draw(Backbutton);
        window.draw(BackText);
        window.draw(BigTextButton);
        
        window.draw(BigText);
        window.draw(hintText);
        window.draw(inputText);
        

        MorseKey.setPosition(BigTextButton.getPosition().x + BigTextButton.getSize().x / 2.f - MorseKey.getRadius(),
                             BigTextButton.getPosition().y + BigTextButton.getSize().y / 2.f - MorseKey.getRadius() + 450.f);
        MorseKeytext.setOrigin(
            MorseKeytext.getLocalBounds().left + MorseKeytext.getLocalBounds().width / 2.f,
            MorseKeytext.getLocalBounds().top + MorseKeytext.getLocalBounds().height / 2.f);
        MorseKeytext.setPosition(
            MorseKey.getPosition().x + MorseKey.getRadius(),
            MorseKey.getPosition().y + MorseKey.getRadius());

        window.draw(MorseKey);
        window.draw(MorseKeytext);
        window.draw(messageText);
        window.draw(coveracti);
        window.draw(coveractswi);
        if(show == false){
            window.draw(hintcover);
            coveractswi.setString("ON!!");
        } else {
            coveractswi.setString("OFF!");
        }

    }
    if (currentstate == AppStates::Practice) {
    for (auto& btn1 : sidebarPracticeButtons) window.draw(btn1);
    for (auto& labelP : sidebarPracticeLabels) window.draw(labelP);//Note there are inconsistencies in the drawPractice compared to drawLesson. Fixes are unlikely
}
   
    window.display();
}

 

void updateUI(sf::RenderWindow& window, AppStates currentstate, std::string& Input, std::string& Output,
              sf::Font& font, sf::Text& InputText, sf::Text& OutputText, sf::RectangleShape& InputBox,
              sf::RectangleShape& OutputBox, sf::RectangleShape& Backbutton, sf::Text& BackText,
              sf::RectangleShape& Clearbutton, sf::Text& ClearText, sf::RectangleShape& SwitchButton,
              sf::Text& SwitchText, sf::CircleShape& MorseKey, sf::Text& MorseKeytext,
              sf::RectangleShape& LessonButton, sf::Text& LessonButtonText, sf::RectangleShape& TranslationButton,
              sf::Text& TranslationText,
              sf::RectangleShape& PracticeButton, sf::Text& PracticeButtonText, sf::RectangleShape& exitbutton, sf::Text& exitText,
              std::vector<LessonStep>& LessonSteps, size_t currentStep, int currentAttempt,
              const std::string& LessonInput, sf::Text& BigText, sf::Text& hintText, sf::Text& inputText,
              sf::Text& progressText, sf::Text& messageText, sf::Text& Congrats, sf::RectangleShape& BigTextButton,
              sf::RectangleShape& NextLessonButton, sf::Text& NextLessonText,
              std::vector<sf::RectangleShape>& sidebarLessonbuttons,
              std::vector<sf::Text>& sidebarlabels, int currentlessonindex, sf::RectangleShape& sidebar, sf::RectangleShape& Lsidebar , sf::RectangleShape& Rsidebar,sf::RectangleShape& hintcover,
              sf::RectangleShape& coveracti, sf::Text& coveractswi,bool& show,sf::Text& TranslationHead) {
    switch (currentstate) {
        case AppStates::Mainmenu:
            drawMainMenu(window, LessonButton, LessonButtonText, TranslationButton, TranslationText,PracticeButton, PracticeButtonText, exitbutton, exitText);
            break;
        case AppStates::Text2Morse:
            Output = Translation2Morse(Input);
            drawText2Morse(window, font, Input, Output, InputText, OutputText, InputBox, OutputBox,
                           Backbutton, BackText, Clearbutton, ClearText, SwitchButton, SwitchText,TranslationHead);
            break;
        case AppStates::Morse2Text:
            Output = MorseToText(Input);
            drawMorse2Text(window, font, Input, Output, InputText, OutputText, InputBox, OutputBox,
                           Backbutton, BackText, Clearbutton, ClearText, MorseKey, MorseKeytext,
                           SwitchButton, SwitchText,TranslationHead);
            break;
        case AppStates::LessonButton:
            drawLesson(window, LessonSteps, currentStep, currentAttempt, LessonInput, BigText, hintText, inputText,
                       progressText, messageText, Congrats, BigTextButton, MorseKey, MorseKeytext, Backbutton, BackText,
                       NextLessonButton, NextLessonText, sidebarLessonbuttons, sidebarlabels, currentlessonindex,sidebar,Lsidebar,Rsidebar,hintcover,coveracti,coveractswi,show);
            break;

        case AppStates::Practice:
            if(currentStep < PracticeSteps.size()){
                BigText.setString(PracticeSteps[currentStep].text);
                hintText.setString(PracticeSteps[currentStep].hintmorse);
                inputText.setString("Your input _ " + PracticeInput);
                progressText.setString("Step " + std::to_string(currentStep + 1) + " / " + std::to_string(PracticeSteps.size() ));
            }else{
                BigText.setString("");
                hintText.setString("");
                inputText.setString("");
                progressText.setString(""); 
            }
            drawPractice(window, PracticeSteps, currentStep, PracticeInput,
                 BigText, hintText, inputText, messageText, Congrats, BigTextButton,
                  MorseKey, MorseKeytext,  Backbutton, BackText, 
                  sidebar, Lsidebar, Rsidebar, hintcover,coveracti,coveractswi,show);
                  break;
    }
}
  
    int main(){
        sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "After Getting Bitch Slapped", sf::Style::Fullscreen);//this sets the size of the gui screen
        window.setFramerateLimit(50);//limiting frame rate
        sf::Font font;
        if(!font.loadFromFile("assets/arial.ttf")){//uploading font, only arial - current patch : 1.2
            std::cout<<"Failed to load the font.";
            return 1;
        }
        
        std::string Lessonfilename = "bin/Lessons/Lesson" + std::to_string(currentlessonindex) +".txt";//currentlessonindex is at 1
        if(!LoadLesson(Lessonfilename, LessonSteps)){
            std::cerr <<"Failed to load the lesson : "<<Lessonfilename;
            return 1;
        }
        bool show = true;//for coveracti
        size_t currentStep = 0;
        int currentAttempt = 1;
        std::string LessonInput;

        int totallessons = 18;
        int lessonperCol = 9;
        float buttonwidth = 80.f , buttonheight = 50.f, spacing = 20.f;

    std::cout << "Practice Mode - currentStep: " << currentStep << "/" << PracticeSteps.size() << std::endl;
    if(currentStep < PracticeSteps.size())
    {
    std::cout << "Current practice text: " << PracticeSteps[currentStep].text << std::endl;
    }

        sf::Vector2f sidebarstart(80.f, 100.f);

        for(int i = 0; i < totallessons ; ++i){
            int col = i / lessonperCol;
            int row = i % lessonperCol;
            sf::RectangleShape btn(sf::Vector2f(buttonwidth, buttonheight));
            btn.setFillColor(sf::Color(200,200,200));
            btn.setPosition(
                sidebarstart.x + col * (buttonwidth +spacing),
                sidebarstart.y + row * (buttonheight + spacing)
            );
            sidebarLessonbuttons.push_back(btn);

            sf::Text labels(std::to_string(i +1), font, 18);
            labels.setFillColor(sf::Color::Black);
            labels.setPosition(
                btn.getPosition().x + 20.f , btn.getPosition().y + 12.f
            );
            sidebarlabels.push_back(labels);
        }

        sf::RectangleShape LessonButton (sf::Vector2f (400.f, 80.f));
        sf::Vector2u windowSize = window.getSize();
        sf::Vector2f objectSize = LessonButton.getSize();
        LessonButton.setPosition(
        (windowSize.x - objectSize.x) / 2.f,
        (windowSize.y - objectSize.y) / 2.f);
        LessonButton.setFillColor(sf::Color::Black);

        sf::RectangleShape BigTextButton (sf::Vector2f(1290.f,200.f));
        BigTextButton.setFillColor(sf::Color::White);
        BigTextButton.setPosition(500.f,0.f);

        sf::Text BigText(" ", font, 100);
        BigText.setFillColor(sf::Color::Black);
        BigText.setOrigin(
            BigText.getLocalBounds().left + BigText.getLocalBounds().width / 2.f,
            BigText.getLocalBounds().top + BigText.getLocalBounds().height / 2.f
        );
        BigText.setPosition(
            (BigTextButton.getPosition().x + BigTextButton.getSize(). x / 2.f) - 10.f,
            (BigTextButton.getPosition().y + BigTextButton.getSize().y / 2.f ) + 23.f
        );

        sf::RectangleShape L2Practice(sf::Vector2f(80.f, 50.f));
        L2Practice.setFillColor(sf::Color::Green);
        L2Practice.setPosition(10.f,150.f);

        sf::Text L2PText("To Practice", font, 18);
        L2PText.setFillColor(sf::Color::Red);
        L2PText.setOrigin(
            L2PText.getLocalBounds().left + L2PText.getLocalBounds().width / 2.f,
            L2PText.getLocalBounds().top + L2PText.getLocalBounds().height / 2.f
        );
        L2PText.setPosition(
            L2Practice.getPosition().x + L2Practice.getSize().x / 2.f,
            L2Practice.getPosition().y + L2Practice.getSize().y / 2.f
        );

        sf::RectangleShape PracticeButton(sf::Vector2f(400.f, 80.f));
        PracticeButton.setFillColor(sf::Color::Black);
        PracticeButton.setPosition(
            (windowSize.x - objectSize.x ) / 2.f,
            (windowSize.y - objectSize.y) / 2.f - 241.f
        );

        
        sf::Text PracticeButtonText("Practice", font, 24);
        PracticeButtonText.setFillColor(sf::Color::Green);
        PracticeButtonText.setOrigin(
        PracticeButtonText.getLocalBounds().left + 
        PracticeButtonText.getLocalBounds().width / 2.f,
        PracticeButtonText.getLocalBounds().top + 
        PracticeButtonText.getLocalBounds().height / 2.f
        );
        PracticeButtonText.setPosition(
        PracticeButton.getPosition().x + 
        PracticeButton.getSize().x / 2.f,
        PracticeButton.getPosition().y + 
        PracticeButton.getSize().y / 2.f
        );
        sf::RectangleShape hintcover(sf::Vector2f(300.f,55.f));
        hintcover.setFillColor(sf::Color::Black);
        hintcover.setPosition(
            BigTextButton.getPosition().x + BigTextButton.getSize().x / 2.f - 85.f,
            BigTextButton.getPosition().y + BigTextButton.getSize().y / 2.f + 140.f
        );
        sf::RectangleShape coveracti(sf::Vector2f(50.f,50.f));
        coveracti.setFillColor(sf::Color::White);
        coveracti.setPosition(
             BigTextButton.getPosition().x + BigTextButton.getSize().x / 2.f + 300.f,
            BigTextButton.getPosition().y + BigTextButton.getSize().y / 2.f + 150.f
        );
        
        sf::Text coveractswi("ON", font , 12);
        coveractswi.setFillColor(sf::Color::Blue);
        coveractswi.setOrigin(
           coveractswi.getLocalBounds().left + coveractswi.getLocalBounds().width / 2.f,
            coveractswi.getLocalBounds().top + coveractswi.getLocalBounds().height / 2.f
        );
        coveractswi.setPosition(
                coveracti.getPosition().x + coveracti.getSize().x / 2.f ,
                coveracti.getPosition().y + coveracti.getSize().y / 2.f 
            );

        sf::Text hintText("The HINT", font, 40);
        hintText.setFillColor(sf::Color(255, 255, 255, 100));
        hintText.setOrigin(
            hintText.getLocalBounds().left + hintText.getLocalBounds().width / 2.f,
            hintText.getLocalBounds().top + hintText.getLocalBounds().height / 2.f
        );
        hintText.setPosition(
                BigTextButton.getPosition().x + BigTextButton.getSize().x / 2.f ,
                BigTextButton.getPosition().y + BigTextButton.getSize().y / 2.f + 150.f
            );

        sf::Text inputText("Some Input", font, 40);
        inputText.setFillColor(sf::Color::Green);
        inputText.setOrigin(
            inputText.getLocalBounds().left + inputText.getLocalBounds().width / 2.f,
            inputText.getLocalBounds().top + inputText.getLocalBounds().height / 2.f
        );
        inputText.setPosition(
            BigTextButton.getPosition().x + BigTextButton.getSize().x / 2.f,
            BigTextButton.getPosition().y + BigTextButton.getSize().y / 2.f + 300.f
        );

        sf::Text progressText("Progress", font, 30);
        progressText.setFillColor(sf::Color::Yellow);
        progressText.setOrigin(
            progressText.getLocalBounds().left + progressText.getLocalBounds().width / 2.f,
            progressText.getLocalBounds().top + progressText.getLocalBounds().height / 2.f
        );
        progressText.setPosition(
            BigTextButton.getPosition().x + BigTextButton.getSize().x / 2.f,
            BigTextButton.getPosition().y + BigTextButton.getSize().y / 2.f + 225.f
        );

        sf::Text TranslationHead("",font,30);
        TranslationHead.setFillColor(sf::Color::White);
        TranslationHead.setOrigin(
            TranslationHead.getLocalBounds().left + TranslationHead.getLocalBounds().width / 2.f,
            TranslationHead.getLocalBounds().top + TranslationHead.getLocalBounds().height / 2.f
        );
        TranslationHead.setPosition(800.f, 20.f);

        sf::Text messageText("Some Message", font, 30);
        messageText.setFillColor(sf::Color::Red);
        messageText.setOrigin(
            messageText.getLocalBounds().left + messageText.getLocalBounds().width / 2.f,
            messageText.getLocalBounds().top + messageText.getLocalBounds().height / 2.f
        );
        messageText.setPosition(
            BigTextButton.getPosition().x + BigTextButton.getSize().x / 2.f + 375.f,
            BigTextButton.getPosition().y + BigTextButton.getSize().y / 2.f + 600.f
        );

        sf::Text Congrats ("Congratulations!", font, 30);
        Congrats.setFillColor(sf::Color::Green);
        Congrats.setPosition(1200, 300);

       
        
        sf::RectangleShape TranslationButton (sf::Vector2f(400.f,80.f));
        TranslationButton.setFillColor(sf::Color::Black);
        TranslationButton.setPosition(
            ( (windowSize.x - objectSize.x) /2.f),
            ( (windowSize.y - objectSize.y) /2.f) - 120.f
        );
        

        sf::RectangleShape exitbutton (sf::Vector2f(400.f,80.f));
        exitbutton.setPosition(((windowSize.x - objectSize.x) / 2.f),
        ((windowSize.y - objectSize.y) / 2.f) + 120.f);
        exitbutton.setFillColor(sf::Color::Black);

        sf::RectangleShape Backbutton(sf::Vector2f(80.f,50.f));
        Backbutton.setFillColor(sf::Color::Green);
        Backbutton.setPosition(1825.f, 20.f);

        sf::RectangleShape Clearbutton (sf::Vector2f(80.f,50.f));
        Clearbutton.setFillColor(sf::Color::Green);
        Clearbutton.setPosition(
            Backbutton.getGlobalBounds().left , Backbutton.getGlobalBounds().top + Backbutton.getGlobalBounds().height + 30.f
        );

        sf::RectangleShape SwitchButton (sf::Vector2f(80.f,50.f));
        SwitchButton.setFillColor(sf::Color::White);
        SwitchButton.setPosition(
            Clearbutton.getGlobalBounds().left,
            Clearbutton.getGlobalBounds().top + Clearbutton.getGlobalBounds().height + 150.f
        );
        
        sf::RectangleShape NextLessonButton (sf::Vector2f(80.f,50.f));
        NextLessonButton.setFillColor(sf::Color::Green);
        NextLessonButton.setPosition(
            SwitchButton.getGlobalBounds().left,
            SwitchButton.getGlobalBounds().top + SwitchButton.getGlobalBounds().height + 150.f
        );


        sf::RectangleShape Lsidebar (sf::Vector2f(150.f, window.getSize().y));
        Lsidebar.setFillColor(sf::Color::White);
        Lsidebar.setPosition(BigTextButton.getPosition().x,0.f);

       sf::RectangleShape Rsidebar (sf::Vector2f(200.f, window.getSize().y));
        Rsidebar.setFillColor(sf::Color::White);
        Rsidebar.setPosition(BigTextButton.getPosition().x + BigTextButton.getSize(). x,0.f);

        sf::Text NextLessonText("Next Lesson", font, 18);
        NextLessonText.setFillColor(sf::Color::Red);
        NextLessonText.setOrigin(
            NextLessonText.getLocalBounds().left + NextLessonText.getLocalBounds().width / 2.f,
            NextLessonText.getLocalBounds().top + NextLessonText.getLocalBounds().height / 2.f
        );
        NextLessonText.setPosition(
            NextLessonButton.getPosition().x + NextLessonButton.getSize().x / 2.f,
            NextLessonButton.getPosition().y + NextLessonButton.getSize().y / 2.f
        );

        sf::Text SwitchText ("Switch", font, 18);
        SwitchText.setFillColor(sf::Color::Red);
        SwitchText.setOrigin(
            SwitchText.getLocalBounds().left + SwitchText.getLocalBounds().width / 2.f,
            SwitchText.getLocalBounds().top + SwitchText.getLocalBounds().height / 2.f
        );
        SwitchText.setPosition(
            SwitchButton.getPosition().x + SwitchButton.getSize().x / 2.f,
            SwitchButton.getPosition().y + SwitchButton.getSize().y / 2.f
        );

        sf::CircleShape MorseKey(40.f); // 40.f radius for 80.f diameter
        MorseKey.setFillColor(sf::Color::Red);
        
        sf::Text MorseKeytext("Morse Key", font, 15);
        MorseKeytext.setFillColor(sf::Color::White);
       
        sf::Text ClearText("Clear",font, 18);
        ClearText.setFillColor(sf::Color::Red);
        ClearText.setOrigin(
            ClearText.getLocalBounds().left + ClearText.getLocalBounds().width / 2.f,
            ClearText.getLocalBounds().top + ClearText.getLocalBounds().height / 2.f
        );
        ClearText.setPosition(
        Clearbutton.getPosition().x + Clearbutton.getSize().x / 2.f,
        Clearbutton.getPosition().y + Clearbutton.getSize().y / 2.f
        );

        sf::Text LessonButtonText ("LESSONS" , font, 24);
        LessonButtonText.setFillColor(sf::Color::Green);
        LessonButtonText.setOrigin(
        LessonButtonText.getLocalBounds().left + 
        LessonButtonText.getLocalBounds().width / 2.f,
        LessonButtonText.getLocalBounds().top + 
        LessonButtonText.getLocalBounds().height / 2.f
        );

        LessonButtonText.setPosition(
        LessonButton.getPosition().x + 
        LessonButton.getSize().x / 2.f,
        LessonButton.getPosition().y + 
        LessonButton.getSize().y / 2.f
        );

        sf::Text TranslationText ("Translation", font, 24);
        TranslationText.setFillColor(sf::Color::Green);
        TranslationText.setOrigin(
        TranslationText.getLocalBounds().left + TranslationText.getLocalBounds().width / 2.f,
        TranslationText.getLocalBounds().top + TranslationText.getLocalBounds().height / 2.f
        );
        TranslationText.setPosition(
        TranslationButton.getPosition().x + TranslationButton.getSize().x / 2.f,
        TranslationButton.getPosition().y + TranslationButton.getSize().y / 2.f
        );
        
        sf::Text exitText ("EXIT", font, 24);
        exitText.setFillColor(sf::Color::Green);
        exitText.setOrigin(
        exitText.getLocalBounds().left + exitText.getLocalBounds().width / 2.f,
        exitText.getLocalBounds().top + exitText.getLocalBounds().height / 2.f
        );  
        exitText.setPosition(
        exitbutton.getPosition().x + exitbutton.getSize().x / 2.f,
        exitbutton.getPosition().y + exitbutton.getSize().y /   2.f
        );
        
        
        sf::Text BackText ("Back", font, 18);
        BackText.setFillColor(sf::Color::Red);
        BackText.setOrigin(
            BackText.getLocalBounds().left + BackText.getLocalBounds().width / 2.f,
            BackText.getLocalBounds().top + BackText.getLocalBounds().height / 2.f          
        );
        BackText.setPosition(
            Backbutton.getPosition().x + Backbutton.getSize().x /2.f,
            Backbutton.getPosition().y + Backbutton.getSize().y / 2.f
        );


        std::string Input;
        sf::Text InputText(" ", font, 24);
        InputText.setFillColor(sf::Color::White);
        InputText.setPosition(50.f,50.f);

        std::string Output;
        sf::Text OutputText(" ", font, 24);
        OutputText.setFillColor(sf::Color::Green);
        OutputText.setPosition(50.f,550.f);


        sf::RectangleShape InputBox(sf::Vector2f(1000.f,400.f));
        InputBox.setFillColor(sf::Color::Black);
        InputBox.setPosition(0.f,0.f);

        sf::RectangleShape OutputBox(sf::Vector2f(1000.f,400.f));
        OutputBox.setFillColor(sf::Color::Black);
        OutputBox.setPosition(0.f,500.f);

        sf::RectangleShape sidebar(sf::Vector2f(500.f, window.getSize().y));
        sidebar.setFillColor(sf::Color(50,50,50));
        sidebar.setPosition(0.f,0.f);

        sf::Clock pressclock;
        bool isPressed = false;

        sf::SoundBuffer dotbuffer, dashbuffer,spacebuffer;
        sf::Sound dotSound, dashSound,spaceSound;
        if (!dotbuffer.loadFromFile("assets/dot.wav") || !dashbuffer.loadFromFile("assets/dash.wav")) {//loading sounds
            std::cerr << "Error loading sound files." << std::endl;
            return 1;
        }
        
        dotSound.setBuffer(dotbuffer);
        dashSound.setBuffer(dashbuffer);
        spaceSound = dotSound;
        spaceSound.setPitch(0.5f);
        spaceSound.setVolume(50);

        
        while (window.isOpen()) { // Main application loop, runs until the window is closed
    sf::Event event;
    while (window.pollEvent(event)) { // Poll for all window events (keyboard, mouse, etc.)
        if (event.type == sf::Event::Closed) { // If the user closes the window
            window.close(); // Close the window and exit the loop
        }

        // Handle mouse button press events
        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousepos = window.mapPixelToCoords(sf::Mouse::getPosition(window)); // Get mouse position in window coordinates

            // Main menu button logic
            if (currentstate == AppStates::Mainmenu) {
                if (LessonButton.getGlobalBounds().contains(mousepos)) { // Clicked "Lessons" button
                    currentstate = AppStates::LessonButton; // Switch to lesson mode
                    currentStep = 0; // Reset lesson step
                    currentAttempt = 1; // Reset attempt count
                    LessonInput.clear(); // Clear lesson input
                    messageText.setString(""); // Clear feedback message
                } else if (TranslationButton.getGlobalBounds().contains(mousepos)) { // Clicked "Translation" button
                    currentstate = AppStates::Morse2Text; // Switch to Morse2Text mode
                } else if (exitbutton.getGlobalBounds().contains(mousepos)) { // Clicked "Exit" button
                    window.close(); // Close the window
                } else if (PracticeButton.getGlobalBounds().contains(mousepos)) { // Clicked "Practice" button
                    currentstate = AppStates::Practice; // Switch to practice mode
                    currentStep = 0; // Reset practice step
                    currentAttempt = 1; // Reset attempt count
                    LessonInput.clear(); // Clear lesson input
                    messageText.setString(""); // Clear feedback message

                    // Load the first practice file
                    std::string practicefile = "bin/practice/practice1.txt";
                    if(!loadPractice(practicefile, PracticeSteps)){
                        std::cerr<<"Failed to load the Practice file."<<practicefile<<std::endl;
                    }

                    // Setup sidebar buttons and labels for practice mode
                    sidebarPracticeButtons.clear();
                    sidebarPracticeLabels.clear();
                    const int totalPracticeFiles = 40;
                    float yOffset = 50.f;
                    int practiceperCol = 10;
                    float buttonwidth = 80.f , buttonheight = 50.f, spacing = 20.f;
                    for (int i = 0; i < totalPracticeFiles; ++i) {
                        int col = i / practiceperCol;
                        int row = i % practiceperCol;
                        sf::RectangleShape btn1(sf::Vector2f(80.f, 40.f));
                        btn1.setFillColor(sf::Color(200, 200, 255));
                        btn1.setPosition(
                            sidebarstart.x + col * (buttonwidth + spacing),
                            sidebarstart.y + row * (buttonheight + spacing) - 40.f
                        );
                        sidebarPracticeButtons.push_back(btn1);

                        sf::Text labelP("P" + std::to_string(i + 1), font, 16);
                        labelP.setFillColor(sf::Color::Black);
                        labelP.setPosition(btn1.getPosition().x + 40.f, btn1.getPosition().y + 5.f);
                        sidebarPracticeLabels.push_back(labelP);

                        yOffset += 50.f;
                    }
                }
            }
            // Handle clicks in other modes (Translation, Morse2Text, Lesson, Practice)
            else if (currentstate == AppStates::Text2Morse ||
                     currentstate == AppStates::Morse2Text ||
                     currentstate == AppStates::LessonButton ||
                     currentstate == AppStates::Practice) {

                // Switch between Text2Morse and Morse2Text modes
                if(currentstate == AppStates::Text2Morse || currentstate == AppStates::Morse2Text) {
                    if(SwitchButton.getGlobalBounds().contains(mousepos)) {
                        currentstate = (currentstate == AppStates::Text2Morse) ? AppStates::Morse2Text : AppStates::Text2Morse;
                    }
                }

                // Handle "Back" button click
                if(Backbutton.getGlobalBounds().contains(mousepos)) {
                    currentstate = AppStates::Mainmenu; // Return to main menu
                }
                // Handle "Clear" button click
                else if (Clearbutton.getGlobalBounds().contains(mousepos)) {
                    Input.clear();
                    Output.clear();
                    LessonInput.clear();
                }
                // Handle "Next Lesson" button click
                else if (NextLessonButton.getGlobalBounds().contains(mousepos)) {
                    if(loadNextLesson(LessonSteps, currentlessonindex, Lessonfilename)) {
                        currentStep = 0;
                        currentAttempt = 1;
                        LessonInput.clear();
                        messageText.setString("");
                    } else {
                        messageText.setString("No more lessons.");
                        currentlessonindex--;
                    }
                }
                // Handle lesson sidebar and hint cover toggling in lesson mode
                else if(currentstate == AppStates::LessonButton) {
                    if(coveracti.getGlobalBounds().contains(mousepos)){
                        (show == true)? show = false : show = true;
                    }
                    for (size_t i = 0; i < sidebarLessonbuttons.size(); i++) {
                        if (sidebarLessonbuttons[i].getGlobalBounds().contains(mousepos)) {
                            currentlessonindex = i + 1;
                            Lessonfilename = "bin/Lessons/Lesson" + std::to_string(currentlessonindex) + ".txt";
                            if(!LoadLesson(Lessonfilename, LessonSteps)) {
                                std::cerr << "Failed to load the Lesson : " << Lessonfilename << std::endl;
                            } else {
                                currentStep = 0;
                                currentAttempt = 1;
                                LessonInput.clear();
                                messageText.setString("");
                            }
                        }
                    }
                }
                // Handle practice sidebar and hint cover toggling in practice mode
                else if (currentstate == AppStates::Practice) {
                    if(coveracti.getGlobalBounds().contains(mousepos)){
                        (show == true)? show = false : show = true;
                    }
                    for (size_t i = 0; i < sidebarPracticeButtons.size(); ++i) {
                        if (sidebarPracticeButtons[i].getGlobalBounds().contains(mousepos)) {
                            currentStep = 0;
                            currentAttempt = 1;
                            PracticeInput.clear();
                            messageText.setString("");
                            std::string filename = "bin/practice/practice" + std::to_string(i + 1) + ".txt";
                            if (!loadPractice(filename, PracticeSteps)) {
                                std::cerr << "Failed to load practice file: " << filename << std::endl;
                            }
                        }
                    }
                }
            }

            // Handle MorseKey button press (for Morse code input)
            if (MorseKey.getGlobalBounds().contains(mousepos)) {
                pressclock.restart(); // Start timing the press
                isPressed = true; // Mark as pressed
            }
        }

        // Handle MorseKey button release (determine dot/dash/space)
        if (event.type == sf::Event::MouseButtonReleased && (event.mouseButton.button == sf::Mouse::Left || event.mouseButton.button == sf::Mouse::Right) && isPressed) {
            float duration = pressclock.getElapsedTime().asSeconds(); // Get press duration
            char inputChar = (duration < 0.2f) ? '.' : (duration < 0.5f) ? '-' : ' '; // Short press = dot, medium = dash, long = space

            // Add Morse input to the correct variable based on current state
            if (currentstate == AppStates::LessonButton && currentStep < LessonSteps.size()) {
                LessonInput += inputChar;
            } else if (currentstate == AppStates::Practice && currentStep < PracticeSteps.size()) {
                PracticeInput += inputChar;
            } else {
                Input += inputChar;
            }

            // Play corresponding sound
            if (inputChar == '.') dotSound.play();
            else if (inputChar == '-') dashSound.play();
            else {spaceSound.play();}

            isPressed = false; // Reset pressed state
        }

        // Handle keyboard text input events
        if (event.type == sf::Event::TextEntered) {
            char typed = static_cast<char>(event.text.unicode); // Convert Unicode to ASCII

            // Lesson mode input handling
            if (currentstate == AppStates::LessonButton && currentStep < LessonSteps.size()) {
                if (typed == 8 && !LessonInput.empty()) { // Backspace
                    LessonInput.pop_back();
                } else if (typed == '.' || typed == '-') { // Dot or dash
                    LessonInput += typed;
                } else if (typed == '\r' || typed == '\n') { // Enter key to check answer
                    const LessonStep& step = LessonSteps[currentStep];
                    if (LessonInput == step.morse) {
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
                    LessonInput.clear();
                }
            } else {
                // General input handling for translation modes
                if (typed == 8 && !Input.empty()) { // Backspace
                    Input.pop_back();
                } else if (typed >= 32 && typed <= 127) { // Printable ASCII
                    Input += typed;
                }
            }

            // Practice mode input handling
            if (currentstate == AppStates::Practice && currentStep < PracticeSteps.size()) {
                if (typed == 8 && !PracticeInput.empty()) { // Backspace
                    PracticeInput.pop_back();
                } else if (typed == '.' || typed == '-') { // Dot or dash
                    PracticeInput += typed;
                } else if (typed == '\r' || typed == '\n') { // Enter key to check answer
                    const PracticeStep& step = PracticeSteps[currentStep];
                    if (PracticeInput == step.hintmorse) {
                        messageText.setString("Correct!");
                        currentStep++;
                        if (currentStep >= PracticeSteps.size()) {
                            messageText.setString("Congratulations! Practice completed.");
                        }
                    } else {
                        messageText.setString("Incorrect, try again.");
                    }
                    PracticeInput.clear();
                }
            }
        }
    } // End of event polling loop

    // Update and redraw the UI based on current state and variables
    updateUI(window, currentstate, Input, Output, font, InputText, OutputText, InputBox, OutputBox,
             Backbutton, BackText, Clearbutton, ClearText, SwitchButton, SwitchText,
             MorseKey, MorseKeytext, LessonButton, LessonButtonText, TranslationButton,
             TranslationText, PracticeButton, PracticeButtonText, exitbutton, exitText, LessonSteps, currentStep,
             currentAttempt, LessonInput, BigText, hintText, inputText,
             progressText, messageText, Congrats, BigTextButton,
             NextLessonButton, NextLessonText,
             sidebarLessonbuttons, sidebarlabels, currentlessonindex, sidebar, Lsidebar, Rsidebar, hintcover, coveracti, coveractswi, show, TranslationHead);
} } // End of main application loop
