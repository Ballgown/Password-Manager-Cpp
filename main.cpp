#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

struct Cred {
    string site;
    string username;
    string password;
};

// XOR Encrypt/Decrypt
string xorEncryptDecrypt(string data, char key = 'K') {
    for (char &c : data) {
        c = c ^ key;
    }
    return data;
}

// Load vault + master password
void Loadvault(vector<Cred> &vault, string &masterPassword) {
    ifstream file("vault.txt");
    cout << "loading vault...\n";
    if (!file) {
        cout << "No previous file found, starting fresh\n";
        return;
    }

    // Read master password (first line)
    getline(file, masterPassword);
    masterPassword = xorEncryptDecrypt(masterPassword);

    // Read credentials
    string line;
    while (getline(file, line)) {
        Cred c;
        size_t pos1 = line.find('|');
        size_t pos2 = line.find('|', pos1 + 1);

        if (pos1 == string::npos || pos2 == string::npos) continue;

        c.site = line.substr(0, pos1);
        c.username = line.substr(pos1 + 1, pos2 - pos1 - 1);
        c.password = xorEncryptDecrypt(line.substr(pos2 + 1));

        vault.push_back(c);
    }
    file.close();
    cout << "Vault loaded successfully.\n";
}

// Save single credential to file (append)
void SaveToFile(const Cred& c) {
    ofstream file("vault.txt", ios::app);
    if (file) {
        string encPassword = xorEncryptDecrypt(c.password);
        file << c.site << "|" << c.username << "|" << encPassword << "\n";
        file.close();
    } else {
        cout << "Failed to write to file.\n";
    }
}

// Save entire vault + master password (for delete)
void SaveVault(const vector<Cred>& vault, const string& masterPassword) {
    ofstream file("vault.txt");
    if (file) {
        file << xorEncryptDecrypt(masterPassword) << "\n";
        for (const auto& c : vault) {
            string encPassword = xorEncryptDecrypt(c.password);
            file << c.site << "|" << c.username << "|" << encPassword << "\n";
        }
        file.close();
    } else {
        cout << "Failed to write to file.\n";
    }
}

void AddCredentials(vector<Cred> &vault) {
    Cred c;
    cout << "What username you would like to have?\n";
    cin >> ws;
    getline(cin, c.username);
    cout << "Pls tell me the site name for that username:\n";
    cin >> ws;
    getline(cin, c.site);
    cout << "Now, finally, set the password:\n";
    cin >> c.password;

    vault.push_back(c);
    SaveToFile(c);
    cout << "Credential added & saved.\n";
}

void ViewCredentials(const vector<Cred>& vault) {
    if (vault.empty()) {
        cout << "Oops, seems like there are no previous entries ;)";
        return;
    }

    cout << "Stored credentials are:\n";
    for (const auto &c : vault) {
        cout << "\nSite:" << c.site << "\nUsername:" << c.username << "\nPassword:" << c.password << endl;
    }
}

void SearchBySite(const vector<Cred>& vault) {
    cout << "Enter the site name to search:";
    string site_name;
    cin >> ws;
    getline(cin, site_name);
    if (vault.empty()) {
        cout << "Oops, seems like there are no previous entries ;)";
        return;
    }

    int i = 0;
    for (const auto &c : vault) {
        if (c.site == site_name) {
            cout << "Site found!\n";
            cout << "Username:" << c.username << endl << "Password:" << c.password << endl;
            i++;
        }
    }
    if (i == 0) {
        cout << "No matches for such site found!";
    }
}

void DeleteCredential(vector<Cred>& vault, const string& masterPassword) {
    cout << "Enter the site name to delete:";
    string site_name;
    cin >> ws;
    getline(cin, site_name);

    bool found = false;
    for (auto it = vault.begin(); it != vault.end(); ++it) {
        if (it->site == site_name) {
            vault.erase(it);
            found = true;
            break;
        }
    }

    if (found) {
        SaveVault(vault, masterPassword);
        cout << "Credential deleted and vault updated.\n";
    } else {
        cout << "No such site found.\n";
    }
}

int main() {
    vector<Cred> vault;
    string masterPassword;
    Loadvault(vault, masterPassword);

    if (masterPassword.empty()) {
        cout << "Set a master password for your vault:\n";
        cin >> masterPassword;
        SaveVault(vault, masterPassword); // save master immediately
        cout << "Master password set!\n";
    } else {
        string input;
        cout << "Enter master password to unlock vault: ";
        cin >> input;
        if (input != masterPassword) {
            cout << "Wrong master password! Exiting.\n";
            return 0;
        }
        cout << "Vault unlocked!\n";
    }

    int choice;
    cout << "Welcome to C++ password manager! \n";
    do {
        cout << "\nMenu:\n";
        cout << "1. Add New Credential\n";
        cout << "2. View All Credentials\n";
        cout << "3. Search by Site\n";
        cout << "4. Delete Credential\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice) {
            case 1: AddCredentials(vault); break;
            case 2: ViewCredentials(vault); break;
            case 3: SearchBySite(vault); break;
            case 4: DeleteCredential(vault, masterPassword); break;
            case 5: cout << "Exiting successfully! Byee, have a nice day !!\n"; break;
            default: cout << "Wrong or invalid option selected!\n"; break;
        }
    } while (choice != 5);

    return 0;
}
