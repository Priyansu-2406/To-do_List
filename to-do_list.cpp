#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <stack>
#include <stdexcept>

using namespace std;

struct Task {
    string description;
    string category;
    int priority;
    string dueDate;

    bool isOverdue() const {
        time_t now = time(0);
        tm *ltm = localtime(&now);
        ostringstream todayStream;
        todayStream << 1900 + ltm->tm_year << "-"
                    << setfill('0') << setw(2) << 1 + ltm->tm_mon << "-"
                    << setfill('0') << setw(2) << ltm->tm_mday;
        return dueDate < todayStream.str();  // Check if the due date is in the past
    }
};

vector<Task> tasks;
vector<string> categories = {"Work", "Personal", "Study", "Shopping", "Other"};
stack<pair<Task, string>> undoStack;  // Stack to track last actions (task, action type)

void loadTasks(const string &filename) {
    ifstream file(filename);
    if (!file) return;

    string line;
    while (getline(file, line)) {
        istringstream ss(line);
        Task task;
        getline(ss, task.description, '|');
        getline(ss, task.category, '|');
        ss >> task.priority;
        ss.ignore();
        getline(ss, task.dueDate);
        tasks.push_back(task);
    }
}

void saveTasks(const string &filename) {
    ofstream file(filename);
    for (size_t i = 0; i < tasks.size(); ++i) {
        const auto &task = tasks[i];
        file << task.description << "|" << task.category << "|" 
             << task.priority << "|" << task.dueDate << endl;
    }
}

void addTask() {
    Task task;
    cout << "Enter task description: ";
    cin.ignore();
    getline(cin, task.description);

    cout << "Choose a category: " << endl;
    for (size_t i = 0; i < categories.size(); ++i) {
        cout << i + 1 << ". " << categories[i] << endl;
    }
    int categoryChoice;
    cin >> categoryChoice;
    if (categoryChoice > 0 && categoryChoice <= categories.size()) {
        task.category = categories[categoryChoice - 1];
    } else {
        cout << "Invalid choice, defaulting to 'Other'" << endl;
        task.category = "Other";
    }

    cout << "Enter priority (1-5): ";
    cin >> task.priority;
    cin.ignore();

    cout << "Enter due date (YYYY-MM-DD) or leave empty if not applicable: ";
    getline(cin, task.dueDate);

    tasks.push_back(task);
    undoStack.push({task, "add"});  // Push the added task and action type onto the stack for undo
}

void removeTask() {
    int index;
    cout << "Enter task index to remove: ";
    cin >> index;
    if (index > 0 && index <= tasks.size()) {
        Task removedTask = tasks[index - 1];
        tasks.erase(tasks.begin() + index - 1);
        undoStack.push({removedTask, "remove"});  // Push the removed task and action type onto the stack for undo
    } else {
        cout << "Invalid index!" << endl;
    }
}

void remindPastDueTasks() {
    cout << "Past Due Tasks:" << endl;
    bool hasOverdue = false;
    for (size_t i = 0; i < tasks.size(); ++i) {
        const Task &task = tasks[i];
        if (task.isOverdue()) {
            cout << " - " << task.description << " (Due: " << task.dueDate << ")" << endl;
            hasOverdue = true;
        }
    }
    if (!hasOverdue) {
        cout << "No past due tasks." << endl;
    }
}

void remindUserOfPastDueTasks() {
    cout << "Checking for past due tasks..." << endl;
    remindPastDueTasks();  // Call the existing function
}

void sortTasksByPriority() {
    sort(tasks.begin(), tasks.end(), [](const Task &a, const Task &b) {
        return a.priority < b.priority;  // Sort in ascending order
    });
}

void listTasks() {
    sortTasksByPriority();  // Sort tasks by priority before listing
    for (size_t i = 0; i < tasks.size(); ++i) {
        const auto &task = tasks[i];
        cout << "Task " << i + 1 << ":" << endl;
        cout << "Description: " << task.description << endl;
        cout << "Category: " << task.category << endl;
        cout << "Priority: " << task.priority << endl;
        if (!task.dueDate.empty()) {
            cout << "Due Date: " << task.dueDate << endl;
        } else {
            cout << "Due Date: None" << endl;
        }
        cout << "---------------------------" << endl;
    }
}

void undoLastAction() {
    if (undoStack.empty()) {
        cout << "No actions to undo." << endl;
        return;
    }
    
    auto [lastTask, action] = undoStack.top();
    undoStack.pop();

    if (action == "add") {
        // Undo addition by removing the last added task
        tasks.pop_back();
    } else if (action == "remove") {
        // Undo removal by re-adding the task
        tasks.push_back(lastTask);
    }
    
    cout << "Last action undone: " << lastTask.description << endl;
}

void displayMenu() {
    cout << "1. Add Task" << endl;
    cout << "2. List Tasks by Priority" << endl;
    cout << "3. Remove Task" << endl;
    cout << "4. Undo Last Action" << endl;
    cout << "5. Check Past Due Tasks" << endl;
    cout << "6. Exit" << endl;
    cout << "Choose an option: ";
}

int main() {
    const string filename = "tasks.txt";
    loadTasks(filename);

    int choice;
    do {
        displayMenu();
        cin >> choice;
        switch (choice) {
            case 1:
                addTask();
                break;
            case 2:
                listTasks();
                break;
            case 3:
                removeTask();
                break;
            case 4:
                undoLastAction();  // Undo last action
                break;
            case 5:
                remindUserOfPastDueTasks();  // Check for past due tasks
                break;
            case 6:
                saveTasks(filename);
                break;
            default:
                cout << "Invalid choice!" << endl;
        }
    } while (choice != 6);

    return 0;
}
