#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <fstream>
#include <algorithm>
#include <conio.h>
#include <iomanip>
#include <ctime>
#include <unordered_map>
#include <sstream>

using namespace std;

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define CYAN "\033[36m"
#define MAGENTA "\033[35m"
#define WHITE "\033[97m"

struct Question {
    string question;
    vector<string> options;
    char correctAnswer;
};

void clearScreen() {
    system("cls");
}

void displayOpeningPage() {
    clearScreen();
    cout << WHITE << "===============================================================\n" << RESET;
    cout << CYAN << setw(50) << " Welcome to the Quiz Application" << RESET << endl;
    cout << WHITE << "===============================================================\n" << RESET;
    cout << GREEN << "\nThis application helps you test your knowledge in various subjects,\n"
         << "like Discrete Math, Programming Fundamentals, English, ICT, and Islamiat." << RESET << endl;
    cout << YELLOW << "\nInstructions:\n" << RESET;
    cout << MAGENTA << "1. Log in with your CMS ID." << RESET << endl;
    cout << MAGENTA << "2. Select a subject, difficulty level, and quiz type." << RESET << endl;
    cout << MAGENTA << "3. Attempt the questions and view your report card at the end." << RESET << endl;
    cout << MAGENTA << "4. Your results will be saved for future reference." << RESET << endl;
    cout << WHITE << "\n===============================================================\n" << RESET;
    cout << CYAN << "Press Enter to continue..." << RESET << endl;
    cin.ignore();
}

void displayWelcomeScreen(const string& userName) {
    clearScreen();
    cout << WHITE << "===============================================================\n" << RESET;
    cout << CYAN << setw(50) << " Welcome, " << userName << "!" << RESET << endl;
    cout << WHITE << "===============================================================\n" << RESET;
    cout << GREEN << "\nWe are excited to have you here! Get ready to start your quiz journey." << RESET << endl;
    cout << WHITE << "\n===============================================================\n" << RESET;
    cout << CYAN << "Press Enter to proceed to the quiz setup..." << RESET << endl;
    cin.ignore();
}

unordered_map<string, string> loadCMSData(const string& filename) {
    unordered_map<string, string> cmsData;
    ifstream file(filename);
    if (!file) {
        cout << RED << "Error: Could not open CMS data file " << filename << RESET << endl;
        exit(1);
    }
    string cmsID, name;
    while (file >> cmsID) {
        getline(file, name);
        if (!name.empty() && name[0] == ' ') {
            name = name.substr(1);
        }
        cmsData[cmsID] = name;
    }
    file.close();
    return cmsData;
}

vector<Question> loadQuestions(const string& filename, const string& difficulty) {
    vector<Question> questions;
    ifstream file(filename);
    if (!file) {
        cout << RED << "Error: Could not open file " << filename << RESET << endl;
        exit(1);
    }
    string line;
    bool inDifficultySection = false;
    Question q;
    while (getline(file, line)) {
        if (line == "[EASY]" || line == "[MEDIUM]" || line == "[HARD]") {
            inDifficultySection = (line == difficulty);
            continue;
        }
        if (inDifficultySection) {
            if (!line.empty()) {
                if (q.question.empty()) {
                    q.question = line;
                } else if (q.options.size() < 4) {
                    q.options.push_back(line);
                } else if (q.options.size() == 4) {
                    q.correctAnswer = line[0];
                    questions.push_back(q);
                    q = Question();
                }
            }
        }
    }
    file.close();
    return questions;
}

void shuffleOptions(Question& q) {
    srand(time(0));
    string correctOption = q.options[q.correctAnswer - 'A'];
    random_shuffle(q.options.begin(), q.options.end());
    for (int i = 0; i < q.options.size(); i++) {
        if (q.options[i] == correctOption) {
            q.correctAnswer = 'A' + i;
            break;
        }
    }
}

bool askQuestion(Question& q, int& score, int& attempted, int timePerQuestion, int current, int total) {
    clearScreen();
    cout << CYAN << "\n================================================================\n" << RESET;
    cout << WHITE << "Question " << current << " of " << total << RESET << endl;
    cout << YELLOW << "Question:\n" << RESET;
    cout << WHITE << q.question << RESET << endl;
    cout << CYAN << "\nOptions:\n" << RESET;
    for (int i = 0; i < q.options.size(); i++) {
        cout << CYAN << char('A' + i) << ") " << GREEN << q.options[i] << RESET << endl;
    }
    cout << CYAN << "\n================================================================\n" << RESET;
    char userAnswer = '\0';
    bool answered = false;
    if (timePerQuestion <= 0) {
        cout << WHITE << "\nType your answer (A, B, C, D) or press 'Q' to skip: " << RESET;
        while (!answered) {
            if (_kbhit()) {
                userAnswer = _getch();
                userAnswer = toupper(userAnswer);
                if (userAnswer == 'Q') {
                    cout << RED << "\nQuestion skipped.\n" << RESET;
                    attempted++;
                    this_thread::sleep_for(chrono::seconds(2));
                    return false;
                } else if (userAnswer >= 'A' && userAnswer <= 'D') {
                    answered = true;
                    break;
                }
            }
        }
    } else {
        int timeLeft = timePerQuestion;
        while (timeLeft > 0) {
            clearScreen();
            cout << CYAN << "\n================================================================\n" << RESET;
            cout << WHITE << "Time Left: " << GREEN << setw(2) << right << timeLeft << " seconds\n" << RESET;
            cout << WHITE << "Question " << current << " of " << total << RESET << endl;
            cout << YELLOW << "Question:\n" << RESET;
            cout << WHITE << q.question << RESET << endl;
            cout << CYAN << "\nOptions:\n" << RESET;
            for (int i = 0; i < q.options.size(); i++) {
                cout << CYAN << char('A' + i) << ") " << GREEN << q.options[i] << RESET << endl;
            }
            cout << CYAN << "\n================================================================\n" << RESET;
            if (_kbhit()) {
                userAnswer = _getch();
                userAnswer = toupper(userAnswer);
                if (userAnswer >= 'A' && userAnswer <= 'D') {
                    answered = true;
                    break;
                }
            }
            this_thread::sleep_for(chrono::seconds(1));
            timeLeft--;
        }
        if (!answered) {
            cout << RED << "\nTime's up! Moving to next question.\n" << RESET;
            attempted++;
            this_thread::sleep_for(chrono::seconds(2));
            return false;
        }
    }
    if (answered) {
        attempted++;
        if (userAnswer == q.correctAnswer) {
            cout << GREEN << "\nCorrect!\n" << RESET;
            score++;
        } else {
            cout << RED << "\nIncorrect. The correct answer was: " << q.correctAnswer << RESET << endl;
        }
        this_thread::sleep_for(chrono::seconds(2));
    }
    return answered;
}


void generateReportCard(const string& userName, int totalQuestions, int attempted, int score) {
    clearScreen();
    double percentage = (double(score) / totalQuestions) * 100.0;
    stringstream report;
    report << CYAN << "================================================================\n" << RESET;
    report << CYAN << setw(45) << "Quiz Report Card" << RESET << endl;
    report << CYAN << "================================================================\n" << RESET;
    report << GREEN << "Name: " << WHITE << userName << RESET << endl;
    report << GREEN << "Total Questions: " << WHITE << totalQuestions << RESET << endl;
    report << GREEN << "Attempted: " << WHITE << attempted << RESET << endl;
    report << GREEN << "Correct Answers: " << WHITE << score << RESET << endl;
    report << GREEN << "Percentage: " << WHITE << fixed << setprecision(2) << percentage << "%" << RESET << endl;
    report << CYAN << "================================================================\n" << RESET;
    if (percentage >= 80) {
        report << GREEN << "Remarks: Excellent work! Keep it up!" << RESET << endl;
    } else if (percentage >= 50) {
        report << YELLOW << "Remarks: Good effort, but you can improve." << RESET << endl;
    } else {
        report << RED << "Remarks: You need to work harder." << RESET << endl;
    }
    report << CYAN << "================================================================\n" << RESET;
    cout << report.str();
    ofstream file("quiz_results.txt", ios::app);
    file << "Name: " << userName << endl;
    file << "Total Questions: " << totalQuestions << endl;
    file << "Attempted: " << attempted << endl;
    file << "Correct: " << score << endl;
    file << "Percentage: " << percentage << "%" << endl;
    file << "---------------------------------------------------------------" << endl;
    file.close();
}





int main() {
    displayOpeningPage();
    unordered_map<string, string> cmsData = loadCMSData("cms_data.txt");
    string cmsID, userName;
    while (true) {
        cout << WHITE << "Enter your CMS ID to login: " << RESET;
        cin >> cmsID;
        auto it = cmsData.find(cmsID);
        if (it != cmsData.end()) {
            userName = it->second;
            cin.ignore();
            displayWelcomeScreen(userName);
            break;
        } else {
            cout << RED << "Invalid CMS ID. Please try again.\n" << RESET;
        }
    }
    cout << YELLOW << "\nSelect a Subject:\n" << RESET;
    cout << GREEN << "1. Discrete Math\n2. Programming Fundamentals\n3. English\n4. ICT\n5. Islamiat\n" << RESET;
    int subjectChoice;
    cout << WHITE << "Enter choice (1-5): " << RESET;
    cin >> subjectChoice;
    string subjects[] = {"DiscreteMath.txt", "ProgrammingFundamentals.txt", "English.txt", "ICT.txt", "Islamiat.txt"};
    string filename = subjects[subjectChoice - 1];
    cout << YELLOW << "\nSelect Difficulty Level:\n" << RESET;
    cout << GREEN << "1. Easy\n2. Medium\n3. Hard\n" << RESET;
    int difficultyChoice;
    cout << WHITE << "Enter choice (1-3): " << RESET;
    cin >> difficultyChoice;
    string difficulties[] = {"[EASY]", "[MEDIUM]", "[HARD]"};
    string difficulty = difficulties[difficultyChoice - 1];
    int numQuestions;
    cout << YELLOW << "\nEnter the number of MCQs you want to attempt (1-40): " << RESET;
    cin >> numQuestions;
    if (numQuestions < 1 || numQuestions > 40) {
        cout << RED << "Invalid number of questions! Defaulting to 10.\n" << RESET;
        numQuestions = 10;
    }
    cout << YELLOW << "\nSelect the type of quiz:\n" << RESET;
    cout << GREEN << "1. Timeless Quiz (No time limit for each question)\n2. Timed Quiz (10 seconds per question)\n" << RESET;
    int quizType;
    cout << WHITE << "Enter choice (1-2): " << RESET;
    cin >> quizType;
    vector<Question> allQuestions = loadQuestions(filename, difficulty);
    random_shuffle(allQuestions.begin(), allQuestions.end());
    int score = 0, attempted = 0;
    int timePerQuestion = quizType == 2 ? 10 : -1;
    for (int i = 0; i < numQuestions && i < allQuestions.size(); i++) {
        shuffleOptions(allQuestions[i]);
        askQuestion(allQuestions[i], score, attempted, timePerQuestion, i + 1, numQuestions);
    }
    generateReportCard(userName, numQuestions, attempted, score);
    return 0;
}
