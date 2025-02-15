#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

struct PasswordEntry {
    string website;
    string username;
    string password;
};

class PasswordManager {
private:
    vector<PasswordEntry> entries;
    string filename = "vault.txt";
    string masterKey;

    string xorEncryptDecrypt(const string& input) {
        string output = input;
        for (size_t i = 0; i < input.size(); ++i) {
            output[i] = input[i] ^ masterKey[i % masterKey.size()];
        }
        return output;
    }

public:
    static string getHiddenInput() {
        string password;
        char c;
        while ((c = getchar()) != '\n') {
            password += c;
            cout << '*';
        }
        cout << endl;
        return password;
    }

    void setMasterPassword(const string& key) {
        masterKey = key;
        while (masterKey.size() < 32) masterKey += key;
    }

    void addEntry(const string& website, const string& username, const string& password) {
        entries.push_back({
            xorEncryptDecrypt(website),
            xorEncryptDecrypt(username),
            xorEncryptDecrypt(password)
            });
    }

    void saveToFile() {
        ofstream file(filename);
        for (const auto& entry : entries) {
            file << entry.website << "\t"
                << entry.username << "\t"
                << entry.password << "\n";
        }
    }

    void loadFromFile() {
        entries.clear();
        ifstream file(filename);
        string line;
        while (getline(file, line)) {
            size_t pos1 = line.find('\t');
            size_t pos2 = line.find('\t', pos1 + 1);

            entries.push_back({
                line.substr(0, pos1),
                line.substr(pos1 + 1, pos2 - pos1 - 1),
                line.substr(pos2 + 1)
                });
        }
    }

    void listEntries() {
        for (const auto& entry : entries) {
            cout << "Website: " << xorEncryptDecrypt(entry.website) << "\n"
                << "Username: " << xorEncryptDecrypt(entry.username) << "\n"
                << "Password: " << xorEncryptDecrypt(entry.password) << "\n"
                << "----------------\n";
        }
    }
};

void displayMenu() {
    cout << "\nPassword Manager\n";
    cout << "1. Add New Password\n";
    cout << "2. View All Passwords\n";
    cout << "3. Exit\n";
    cout << "Enter your choice: ";
}

int main() {
    PasswordManager pm;
    string masterPassword;

    cout << "Set master password: ";
    pm.setMasterPassword(PasswordManager::getHiddenInput());
    pm.loadFromFile();

    int choice;
    do {
        displayMenu();
        cin >> choice;
        cin.ignore(); // Clear input buffer

        switch (choice) {
        case 1: {
            string website, username, password;
            cout << "Enter website: ";
            getline(cin, website);
            cout << "Enter username: ";
            getline(cin, username);
            cout << "Enter password: ";
            password = PasswordManager::getHiddenInput();

            pm.addEntry(website, username, password);
            pm.saveToFile();
            cout << "Password saved!\n";
            break;
        }
        case 2: {
            cout << "\nStored Passwords:\n";
            pm.listEntries();
            break;
        }
        case 3: {
            cout << "Exiting...\n";
            break;
        }
        default: {
            cout << "Invalid choice. Try again.\n";
        }
        }
    } while (choice != 3);

    return 0;
}