#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <ctime>
#include <algorithm>

using namespace std;

// Structure for each Question
struct Question {
    int question_id;
    string question_text;
    map<char, string> answer_options; // Stores options as (option: text)
    char correct_answer;
    int category_id;
    string category_name;
};

// Structure to represent each player's session
struct PlayerSession {
    int session_id;
    time_t timestamp;
    int score;
    map<int, char> attempted_answers; // Stores (question_id: chosen_answer)
};

// Structure for each Player
struct Player {
    string name;
    int total_score;
    vector<PlayerSession> sessions; // List of all sessions for the player
};

// QuizGame class to manage questions, players, and leaderboard
class QuizGame {
private:
    vector<Question> questions;
    vector<Player> leaderboard;
    int sessionCounter = 0;

    // Function to add questions
    void initializeQuestions() {
        questions = {
            {1, "What is the capital of France?", {{'a', "London"}, {'b', "Paris"}, {'c', "Berlin"}, {'d', "Madrid"}}, 'b', 1, "Geography"},
            {2, "What is 2 + 2?", {{'a', "3"}, {'b', "4"}, {'c', "5"}, {'d', "6"}}, 'b', 2, "Math"},
            {3, "What is the largest planet?", {{'a', "Earth"}, {'b', "Mars"}, {'c', "Jupiter"}, {'d', "Saturn"}}, 'c', 1, "Astronomy"},
            {4, "Who wrote 'Romeo and Juliet'?", {{'a', "William Shakespeare"}, {'b', "Charles Dickens"}, {'c', "Mark Twain"}, {'d', "Jane Austen"}}, 'a', 3, "Literature"},
            {5, "What is the chemical symbol for water?", {{'a', "H2O"}, {'b', "CO2"}, {'c', "O2"}, {'d', "NaCl"}}, 'a', 2, "Science"}
        };
    }

    // Function to display leaderboard
    void displayLeaderboard() {
        cout << "\n--- Leaderboard ---\n";
        for (size_t i = 0; i < leaderboard.size(); i++) {
            cout << i + 1 << ". " << leaderboard[i].name << " - " << leaderboard[i].total_score << " points\n";
        }
        cout << endl;
    }

    // Sort leaderboard in descending order of total_score
    void updateLeaderboard() {
        sort(leaderboard.begin(), leaderboard.end(), [](const Player& a, const Player& b) {
            return a.total_score > b.total_score;
        });
        if (leaderboard.size() > 10) {
            leaderboard.resize(10); // Keep top 10 players
        }
    }

public:
    QuizGame() {
        initializeQuestions();
    }

    // Function to start a new session for a player
    void startQuiz(const string& playerName) {
        int score = 0;
        map<int, char> answers;

        cout << "\n--- Starting Quiz for " << playerName << " ---\n";

        for (const auto& question : questions) {
            cout << question.question_text << "\n";
            for (const auto& option : question.answer_options) {
                cout << option.first << ") " << option.second << "\n";
            }
            char answer;
            cout << "Answer: ";
            cin >> answer;
            answers[question.question_id] = answer;

            if (answer == question.correct_answer) {
                score += 10;
            }
        }

        PlayerSession session = {sessionCounter++, time(0), score, answers};

        // Check if player already exists
        auto it = find_if(leaderboard.begin(), leaderboard.end(), [&](const Player& p) {
            return p.name == playerName;
        });

        if (it != leaderboard.end()) {
            it->total_score += score;
            it->sessions.push_back(session);
        } else {
            Player newPlayer = {playerName, score, {session}};
            leaderboard.push_back(newPlayer);
        }

        updateLeaderboard();
        displayLeaderboard();
    }

    // Display session details for a player
    void displaySessionDetails(const string& playerName) {
        auto it = find_if(leaderboard.begin(), leaderboard.end(), [&](const Player& p) {
            return p.name == playerName;
        });

        if (it != leaderboard.end()) {
            cout << "\n--- Sessions for " << it->name << " ---\n";
            for (const auto& session : it->sessions) {
                cout << "Session ID: " << session.session_id << ", Date: " << ctime(&session.timestamp)
                     << ", Score: " << session.score << "\n";
                cout << "Attempted Answers:\n";
                for (const auto& answer : session.attempted_answers) {
                    cout << "Q" << answer.first << ": " << answer.second << "\n";
                }
                cout << endl;
            }
        } else {
            cout << "Player not found.\n";
        }
    }

    // Function to edit a question, only accessible to quiz conductor
    void editQuestion() {
        int questionId;
        cout << "Enter the question ID to edit: ";
        cin >> questionId;

        auto it = find_if(questions.begin(), questions.end(), [&](const Question& q) {
            return q.question_id == questionId;
        });

        if (it != questions.end()) {
            cout << "Editing question: " << it->question_text << "\n";

            cout << "Enter new question text: ";
            cin.ignore();
            getline(cin, it->question_text);

            for (auto& option : it->answer_options) {
                cout << "Current option " << option.first << ": " << option.second << "\n";
                cout << "Enter new text for this option: ";
                getline(cin, option.second);
            }

            cout << "Enter the new correct answer (a, b, c, d): ";
            cin >> it->correct_answer;

            cout << "Question updated successfully!\n";
        } else {
            cout << "Question with ID " << questionId << " not found.\n";
        }
    }
};

int main() {
    QuizGame game;
    char choice;
    int role;

    cout << "--- Welcome to the Quiz Game! ---\n";
    cout << "Choose your role:\n";
    cout << "1. Quiz Conductor\n";
    cout << "2. Player\n";
    cout << "Enter your choice: ";
    cin >> role;

    if (role == 1) {
        string code;
        cout << "Enter the access code for Quiz Conductor: ";
        cin >> code;

        if (code != "LM10") {
            cout << "Invalid access code. Exiting program.\n";
            return 0;
        }

        do {
            cout << "\nChoose an option:\n";
            cout << "1. Start Quiz\n";
            cout << "2. Edit a Question\n";
            cout << "3. Exit\n";
            cout << "Enter your choice: ";
            cin >> choice;

            if (choice == '1') {
                string playerName;
                cout << "Enter your name: ";
                cin >> playerName;
                game.startQuiz(playerName);

                cout << "Do you want to view session details? (y/n): ";
                cin >> choice;
                if (choice == 'y' || choice == 'Y') {
                    game.displaySessionDetails(playerName);
                }
            } else if (choice == '2') {
                game.editQuestion();
            } else if (choice == '3') {
                break;
            } else {
                cout << "Invalid choice. Please try again.\n";
            }
        } while (true);
    } else if (role == 2) {
        do {
            cout << "\nChoose an option:\n";
            cout << "1. Start Quiz\n";
            cout << "2. Exit\n";
            cout << "Enter your choice: ";
            cin >> choice;

            if (choice == '1') {
                string playerName;
                cout << "Enter your name: ";
                cin >> playerName;
                game.startQuiz(playerName);

                cout << "Do you want to view session details? (y/n): ";
                cin >> choice;
                if (choice == 'y' || choice == 'Y') {
                    game.displaySessionDetails(playerName);
                }
            } else if (choice == '2') {
                break;
            } else {
                cout << "Invalid choice. Please try again.\n";
            }
        } while (true);
    } else {
        cout << "Invalid role. Please restart and enter either '1' for Quiz Conductor or '2' for Player.\n";
    }

    cout << "\nThank you for using the Quiz Game!\n";
    return 0;
}