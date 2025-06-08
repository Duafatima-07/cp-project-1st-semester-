#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <iomanip>
using namespace std;

// ========== CONFIGURATION ==========
const map<int, string> CATEGORIES = {
    {1, "Pizza"}, {2, "Burger"}, {3, "Pasta"}, {4, "Salad"}
};

const map<string, map<string, int>> PRICES = {
    {"Pizza", {{"Small", 10}, {"Medium", 12}, {"Large", 15}}},
    {"Burger", {{"Small", 8}, {"Medium", 10}, {"Large", 12}}},
    {"Pasta", {{"Small", 12}, {"Medium", 14}, {"Large", 16}}},
    {"Salad", {{"Small", 7}, {"Medium", 9}, {"Large", 11}}}
};

const int DELIVERY_TIME = 30; // minutes

// ========== DATA STRUCTURES ==========
struct OrderItem {
    string category;
    string size;
    int quantity;
    int price;
};

// ========== VOICE SYSTEM ==========
void speak(const string& message) {
    cout << "\033[1;35mZunaira:\033[0m " << message << endl;
    string command = "espeak -v en+f3 \"" + message + "\"";

    system(command.c_str());
    this_thread::sleep_for(chrono::milliseconds(300));
}

// ========== DISPLAY FUNCTIONS ==========
void showWelcome() {
    system("cls");
    cout << "\033[1;33m=== SPEEDY BITES ORDER SYSTEM ===\033[0m\n" << endl;
    speak("Hi there! Welcome to Speedy Bites! I'm Zunaira, your order assistant. Are you feeling hungry today?");
}

void showMenu() {
    cout << "\n\033[1;36m====== TODAY'S MENU ======\033[0m" << endl;
    for (const auto& item : CATEGORIES) {
        cout << item.first << ". " << left << setw(10) << item.second 
             << " - Small $" << PRICES.at(item.second).at("Small")
             << ", Medium $" << PRICES.at(item.second).at("Medium")
             << ", Large $" << PRICES.at(item.second).at("Large") << endl;
    }
    cout << "\033[1;36m==========================\033[0m" << endl;
}

// ========== INPUT FUNCTIONS ==========
string getCustomerResponse(const string& prompt) {
    if (!prompt.empty()) speak(prompt);
    cout << "\nYou: ";
    string input;
    getline(cin, input);
    return input;
}

string getSize(const string& category) {
    while(true) {
        string input = getCustomerResponse("What size would you like? 1 for Small, 2 for Medium, or 3 for Large?");
        if(input == "1") return "Small";
        if(input == "2") return "Medium";
        if(input == "3") return "Large";
        speak("Please choose 1, 2 or 3");
    }
}

int getQuantity(const string& item) {
    while(true) {
        string input = getCustomerResponse("How many " + item + " would you like?");
        try {
            int qty = stoi(input);
            if(qty >= 1) return qty;
        } catch(...) {}
        speak("Please enter a number");
    }
}

string getDeliveryAddress() {
    return getCustomerResponse("Great! Where should we deliver your order?");
}

string getPaymentMethod() {
    while(true) {
        string input = getCustomerResponse("Would you like to pay cash on delivery or use online payment?");
        for (auto& c : input) c = tolower(c);
        if(input.find("cash") != string::npos) return "Cash on Delivery";
        if(input.find("online") != string::npos) return "Online Payment";
        speak("Please say 'cash' or 'online'");
    }
}

// ========== ORDER PROCESSING ==========
void processOrder() {
    vector<OrderItem> order;
    
    while(true) {
        showMenu();
        string input = getCustomerResponse("Let me know the number of the item you'd like to order!");
        
        if(input == "done") {
            if(!order.empty()) break;
            speak("Would you like to see the menu again?");
            continue;
        }

        try {
            int choice = stoi(input);
            if(CATEGORIES.count(choice)) {
                string category = CATEGORIES.at(choice);
                string size = getSize(category);
                int qty = getQuantity(size + " " + category);
                int price = PRICES.at(category).at(size);
                
                order.push_back({category, size, qty, price});
                speak("Perfect! I've added " + to_string(qty) + " " + size + " " + category + " to your order");
                speak("Want to add anything else? You can say a number from the menu or say 'done'");
            } else {
                speak("We have items 1 to 4 available");
            }
        } catch(...) {
            speak("Please enter a number from the menu");
        }
    }

    if(!order.empty()) {
        int total = 0;
        cout << "\n\033[1;32m====== ORDER SUMMARY ======\033[0m" << endl;
        for(auto& item : order) {
            int itemTotal = item.price * item.quantity;
            cout << item.quantity << "x " << item.size << " " << item.category 
                 << " - $" << itemTotal << endl;
            total += itemTotal;
        }
        cout << "\033[1;32m===========================\033[0m" << endl;
        cout << right << setw(25) << "TOTAL: $" << total << endl;
        
        string confirm = getCustomerResponse("Here's your order summary. Is everything correct?");
        
        for (auto& c : confirm) c = tolower(c);
        if(confirm.find("yes") != string::npos || confirm.find("correct") != string::npos) {
            string address = getDeliveryAddress();
            string payment = getPaymentMethod();
            
            speak("Got it! " + address + ". Your food will be hot and ready at your door in about " 
                 + to_string(DELIVERY_TIME) + " minutes");
            
            if(payment == "Cash on Delivery") {
                speak("We'll collect $" + to_string(total) + " in cash when we deliver");
            } else {
                speak("We'll send you a payment link shortly");
            }
            
            speak("Thanks for ordering from Speedy Bites, and enjoy your meal! "
                 "If you're ever hungry again, you know who to call!");
        } else {
            speak("Let's correct your order. What would you like to change?");
        }
    }
}

int main() {
    showWelcome();
    
    string hungry = getCustomerResponse("");
    for (auto& c : hungry) c = tolower(c);
    
    if(hungry.find("yes") != string::npos || hungry.find("yeah") != string::npos) {
        processOrder();
    } else {
        speak("No problem! Come back when you're hungry. Have a great day!");
    }

    return 0;
}
