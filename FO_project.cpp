#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>

using namespace std;

const int MAX_RECORDS = 100;
const int ID_SIZE = 3;
const int NAME_SIZE = 10;
const int MARK_SIZE = 3;

struct Student {
    char id[ID_SIZE + 1];
    char name[NAME_SIZE + 1];
    int mark1;
    int mark2;
};

void getData(Student& student) {
    cout << "Enter student ID (3 characters): ";
    cin >> student.id;
    cout << "Enter student name (10 characters): ";
    cin >> student.name;
    cout << "Enter 1st term mark (3 digits): ";
    cin >> student.mark1;
    cout << "Enter 2nd term mark (3 digits): ";
    cin >> student.mark2;
}

void displayData(Student& student) {
    cout << student.id << "*" << student.name << "*"
        << setw(MARK_SIZE) << setfill('0') << student.mark1
        << setw(MARK_SIZE) << setfill('0') << student.mark2 << endl;
}

void packFixedLength(fstream& file, Student& student) {
    file.write(student.id, ID_SIZE);
    file.write(student.name, NAME_SIZE);
    file.write(reinterpret_cast<char*>(&student.mark1), sizeof(int));
    file.write(reinterpret_cast<char*>(&student.mark2), sizeof(int));
}

bool unpackFixedLength(fstream& file, Student& student) {
    file.read(student.id, ID_SIZE);
    file.read(student.name, NAME_SIZE);
    file.read(reinterpret_cast<char*>(&student.mark1), sizeof(int));
    file.read(reinterpret_cast<char*>(&student.mark2), sizeof(int));
    return !file.fail();
}

void packDelimiter(fstream& file, Student& student) {
    file << student.id << " " << student.name << " "
        << student.mark1 << " " << student.mark2 << endl;
}

bool unpackDelimiter(fstream& file, Student& student) {
    file >> student.id >> student.name >> student.mark1 >> student.mark2;
    return !file.fail();
}
void error(int errorCode) {
    switch (errorCode) {
    case 1:
        cout << "Error: could not open file." << endl;
        break;
    case 2:
        cout << "Error: invalid option." << endl;
        break;
    default:
        cout << "Unknown error." << endl;
    }
}

void search() {
    fstream file;
    file.open("data.txt", ios::in);
    if (!file) {
        error(1);
        return;
    }
    char id[ID_SIZE + 1];
    cout << "Enter student ID to search for: ";
    cin >> id;
    Student student;
    bool found = false;
    while (unpackFixedLength(file, student)) {
        if (strcmp(student.id, id) == 0) {
            displayData(student);
            found = true;
            break;
        }
    }
    if (!found) {
        cout << "Student with ID " << id << " not found." << endl;
    }
    file.close();
}

void modify(Student& student) {
    cout << "Enter new data for the student:" << endl;
    getData(student);
}

void deleteRecord(Student* students, int c, int index) {
    for (int i = index; i < c - 1; i++) {
        students[i] = students[i + 1];
    }
}

void deleteRecordMenu(Student* students, int& count) {
    char id[ID_SIZE + 1];
    cout << "Enter student ID to delete: ";
    cin >> id;
    int index = -1;
    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].id, id) == 0) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        cout << "Student with ID " << id << " not found." << endl;
    }
    else {
        deleteRecord(students, count, index);
        count--;
        cout << "Student with ID " << id << " has been deleted." << endl;
    }
}


int main() {
    int option;
    do {
        cout << "Menu:" << endl;
        cout << "1. Insert a record" << endl;
        cout << "2. Display all records" << endl;
        cout << "3. Search for a record" << endl;
        cout << "4. Modify a record" << endl;
        cout << "5. Delete a record" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter option: ";
        cin >> option;
        switch (option) {
        case 1: {
            fstream file;
            file.open("data.txt", ios::out | ios::app);
            if (!file) {
                error(1);
                break;
            }
            Student student;
            getData(student);
            packDelimiter(file, student);
            file.close();
            break;
        }
        case 2: {
            fstream file;
            file.open("data.txt", ios::in);
            if (!file) {
                error(1);
                break;
            }
            Student student;
            while (unpackDelimiter(file, student)) {
                displayData(student);
            }
            file.close();
            break;
        }
        case 3:
            search();
            break;
        case 4: {
            fstream file;
            file.open("data.txt", ios::in | ios::out);
            if (!file) {
                error(1);
                break;
            }
            char id[ID_SIZE + 1];
            cout << "Enter student ID to modify: ";
            cin >> id;
            Student student;
            bool found = false;
            int position;
            while (unpackDelimiter(file, student)) {
                position = file.tellg();
                if (strcmp(student.id, id) == 0) {
                    displayData(student);
                    modify(student);
                    found = true;
                    file.seekp(position - NAME_SIZE - MARK_SIZE * 2 - 2);
                    packDelimiter(file, student);
                    break;
                }
            }
            if (!found) {
                cout << "Student with ID " << id << " not found." << endl;
            }
            file.close();
            break;
        }
        case 5: {
            Student students[MAX_RECORDS];
            int count = 0;
            fstream file;
            file.open("data.txt");
            if (!file) {
                error(1);
                break;
            }
            Student student;
            while (unpackDelimiter(file, student)) {
                students[count] = student;
                count++;
            }
            file.close();
            deleteRecordMenu(students, count);
            file.open("data.txt", ios::out | ios::trunc);
            if (!file) {
                error(1);
                break;
            }
            for (int i = 0; i < count; i++) {
                packDelimiter(file, students[i]);
            }
            file.close();
            break;
        }
        case 6:
            cout << "Goodbye!" << endl;
            break;
        default:
            error(1);
        }
    } while (option != 6);
    return 0;
}

