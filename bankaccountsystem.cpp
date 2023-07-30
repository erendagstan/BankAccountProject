#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>

using namespace std;

class BankAccount {
private:
    string name;
    string username;
    string password;
    double money;

public:
    // Constructor
    BankAccount(const string& name, const string& username, const string& password, double money)
        : name(name), username(username), password(password), money(money) {}

    // Getter methods
    string getName() const {
        return name;
    }

    string getUsername() const {
        return username;
    }

    string getPassword() const {
        return password;
    }

    double getMoney() const {
        return money;
    }

    // Setter method
    void setMoney(double amount) {
        money = amount;
    }
};

bool authenticate(const string& username, const string& password, const vector<BankAccount>& accounts) {
    for (const auto& account : accounts) {
        if (account.getUsername() == username && account.getPassword() == password) {
            return true; // Authentication successful
        }
    }
    return false; // Authentication failed
}

void displayMenu() {
    
	cout<<"|*******************************|"<<endl;
	cout<<"|===========HULK BANK===========|"<<endl;
	cout<<"|*******************************|"<<endl;
	cout<<"|-------1.Account Details-------|"<<endl;
	cout<<"|-------2.Deposit Money---------|"<<endl;
	cout<<"|-------3.Withdraw Money--------|"<<endl;
	cout<<"|-------4.Transfer Money--------|"<<endl;
	cout<<"|-------5.EXIT------------------|"<<endl;
	cout<<"|*******************************|"<<endl;
}

void viewAccountDetails(const BankAccount& account) {
    cout << "Name: " << account.getName() << endl;
    cout << "Username: " << account.getUsername() << endl;
    cout << "Balance: $" << account.getMoney() << endl;
    cout << endl;
}

void depositMoney(BankAccount& account, double amount) {
    double currentBalance = account.getMoney();
    account.setMoney(currentBalance + amount);
    cout << "Deposit successful!" << endl;
    cout << "New balance: $" << account.getMoney() << endl;
    cout << endl;
}

void withdrawMoney(BankAccount& account, double amount) {
    double currentBalance = account.getMoney();
    if (amount <= currentBalance) {
        account.setMoney(currentBalance - amount);
        cout << "Withdrawal successful!" << endl;
        cout << "New balance: $" << account.getMoney() << endl;
    } else {
        cout << "Insufficient balance for withdrawal." << endl;
    }
    cout << endl;
}

bool transferMoney(BankAccount& sender, BankAccount& receiver, double amount) {
    double senderBalance = sender.getMoney();
    if (amount <= senderBalance) {
        sender.setMoney(senderBalance - amount);
        receiver.setMoney(receiver.getMoney() + amount);
        cout << "Transfer successful!" << endl;
        cout << "Amount transferred: $" << amount << endl;
        return true;
    } else {
        cout << "Insufficient balance for transfer." << endl;
        return false;
    }
}

void updateFile(const vector<BankAccount>& accounts, const string& filePath) {
    ofstream file(filePath);
    if (!file.is_open()) {
        cout << "Unable to open the file for writing." << endl;
        return;
    }

    for (const auto& account : accounts) {
        file << account.getName() << ", "
             << account.getUsername() << ", "
             << account.getPassword() << ", "
             << account.getMoney() << endl;
    }

    file.close();
    cout << "File updated successfully." << endl;
}

int main() {
    string filePath = "bank-accounts-c++.txt"; // Replace with your file path

    ifstream file(filePath);
    if (!file.is_open()) {
        cout << "Unable to open the file." << endl;
        return 1;
    }

    vector<BankAccount> accounts;

    string line;
    while (getline(file, line)) {
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        size_t pos3 = line.find(',', pos2 + 1);

        string name = line.substr(0, pos1);
        string username = line.substr(pos1 + 2, pos2 - pos1 - 2);
        string password = line.substr(pos2 + 2, pos3 - pos2 - 2);
        double money = stod(line.substr(pos3 + 2));

        BankAccount account(name, username, password, money);
        accounts.push_back(account);
    }

    file.close();

    string username, password;
    bool authenticated = false;
    int attempts = 0;
    const int maxAttempts = 5;
    
    while (!authenticated) {
        cout << "Enter username: ";
        cin >> username;
        cout << "Enter password: ";
        cin >> password;

        authenticated = authenticate(username, password, accounts);

        if (!authenticated) {
        	attempts++;
            cout << "Invalid username or password. Please try again." << endl;
            
            if(attempts==3){
            	cout<<"Warning: You have entered invalid username or password 3 times. Program will wait 10sec. If you try wrong 2 more times, the program will be closed automatically."<<endl;
            	//time can be added...
            	std::this_thread::sleep_for(std::chrono::seconds(10)); //10 sec wait
    			}
		 	if (attempts >= maxAttempts) {
                cout << "Too many invalid attempts. Exiting the program. Goodbye!" << endl;
                return 1;
            }
        }
    }

    cout << "Authentication successful!" << endl;

    BankAccount* currentUser = nullptr;
    for (auto& account : accounts) {
        if (account.getUsername() == username) {
            currentUser = &account;
            break;
        }
    }

    if (currentUser == nullptr) {
        cout << "User account not found." << endl;
        return 1;
    }

    int choice;
    do {
        displayMenu();
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                viewAccountDetails(*currentUser);
                break;
            case 2: {
                double amount;
                cout << "Enter the amount to deposit: $";
                cin >> amount;
                depositMoney(*currentUser, amount);
                updateFile(accounts, filePath);
                break;
            }
            case 3: {
                double amount;
                cout << "Enter the amount to withdraw: $";
                cin >> amount;
                withdrawMoney(*currentUser, amount);
                updateFile(accounts, filePath);
                break;
            }
            case 4: {
                string receiverUsername;
                double amount;
                cout << "Enter the receiver's username: ";
                cin >> receiverUsername;
                cout << "Enter the amount to transfer: $";
                cin >> amount;

                BankAccount* receiver = nullptr;
                for (auto& account : accounts) {
                    if (account.getUsername() == receiverUsername) {
                        receiver = &account;
                        break;
                    }
                }

                if (receiver != nullptr) {
                    if (transferMoney(*currentUser, *receiver, amount)) {
                        cout << "Transfer successful!" << endl;
                        updateFile(accounts, filePath);
                    } else {
                        cout << "Transfer failed." << endl;
                    }
                } else {
                    cout << "Receiver account not found." << endl;
                }
                break;
            }
            case 5:
                cout << "Exiting the program. Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
                break;
        }
    } while (choice != 5);

    return 0;
}
