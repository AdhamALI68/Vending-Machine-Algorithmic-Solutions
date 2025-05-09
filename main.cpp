#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>
#include <algorithm>
#include <climits>
using namespace std;


void loadStock(vector<pair<string, int>>& products, vector<int>& stock)
{
    ifstream stockFile("Stock.txt");
    if (!stockFile)
    {
        cerr << "Error: Unable to open stock file!" << endl;
        exit(1);
    }

    products.clear();
    stock.clear();
    string name;
    int price, count;

    while (stockFile >> name >> price >> count)
    {
        products.push_back({name, price});
        stock.push_back(count);
    }

    stockFile.close();
}

void saveStock(const vector<pair<string, int>>& products, const vector<int>& stock)
{
    ofstream stockFile("Stock.txt");
    if (!stockFile)
    {
        cerr << "Error: Unable to write to stock file!" << endl;
        exit(1);
    }

    for (size_t i = 0; i < products.size(); i++)
    {
        stockFile << products[i].first << " " << products[i].second << " " << stock[i] << endl;
    }

    stockFile.close();
}


void loadCurrency(map<int, int>& currency)
{
    ifstream currencyFile("Currency.txt");
    if (!currencyFile) {
        cerr << "Error: Unable to open currency file!" << endl;
        exit(1);
    }

    currency.clear();
    int denom, count;

    while (currencyFile >> denom >> count)
    {
        currency[denom] = count;
    }

    currencyFile.close();
}


void saveCurrency(const map<int, int>& currency)
{
    ofstream currencyFile("Currency.txt");
    if (!currencyFile) {
        cerr << "Error: Unable to write to currency file!" << endl;
        exit(1);
    }

    for (auto it = currency.begin(); it != currency.end(); ++it) {
        currencyFile << it->first << " " << it->second << endl;
    }

    currencyFile.close();
}

void displayProducts(vector<pair<string, int>>& products, vector<int>& stock)
{
    cout << "Available products:" << endl;
    for (int i = 0; i < products.size(); i++)
    {
        if (stock[i] > 0)
        {
            cout << i + 1 << ". " << products[i].first << " - " << products[i].second
                 << " EGP (Stock: " << stock[i] << ")" << endl;
        }
    }
    cout << "Enter 0 to exit." << endl;
}

int selectProduct(vector<pair<string, int>>& products, vector<int>& stock)
{
    cout << "Select a product by number: ";
    int choice;
    cin >> choice;
    if (choice == 0)
    {
        return -1;
    }
    if (choice < 1 || choice > products.size() || stock[choice - 1] == 0)
    {
        cout << "Invalid choice or out of stock. Try again." << endl;
        return selectProduct(products, stock);
    }
    return choice - 1;
}

void addMoneyToCurrency(map<int, int>& currency, int moneyInserted)
{
    cout << "Please enter the denominations for " << moneyInserted << " EGP:" << endl;
    while (moneyInserted > 0) {
        cout << "Enter a denomination: ";
        int denom;
        cin >> denom;
        if (currency.find(denom) == currency.end() || denom > moneyInserted) {
            cout << "Invalid denomination. Try again." << endl;
            continue;
        }
        currency[denom]++;
        moneyInserted -= denom;
    }
}

bool provideChange_Greedy(map<int, int>& currency, int change)
{
    map<int, int> tempCurrency = currency;
    map<int, int> changeGiven;

    for (auto it = tempCurrency.rbegin(); it != tempCurrency.rend(); ++it)
    {
        int denom = it->first;
        int needed = min(change / denom, it->second);
        change -= needed * denom;
        tempCurrency[denom] -= needed;
        changeGiven[denom] += needed;
    }

    if (change > 0)
    {
        cout << "Sorry, exact change is not possible. Remaining: " << change << " EGP." << endl;
        currency = tempCurrency;
        return false;
    }

    cout << "Change given: ";
    for (auto it = changeGiven.begin(); it != changeGiven.end(); ++it)
    {
        if (it->second > 0)
        {
            cout << it->second << " x " << it->first << " EGP, ";
            currency[it->first] -= it->second;
        }
    }
    cout << endl;

    return true;
}

bool provideChange_DP(map<int, int>& currency, int change) {
    // Extract denominations and their counts into vectors
    vector<int> coins;
    vector<int> counts;
    for (auto it = currency.begin(); it != currency.end(); ++it) {
        coins.push_back(it->first);
        counts.push_back(it->second);
    }

    int n = coins.size();
    vector<vector<int>> dp(n + 1, vector<int>(change + 1, 0));
    vector<vector<int>> coinUsage(n + 1, vector<int>(change + 1, 0)); // Tracks how many of each coin are used

    // Base case: there's one way to make 0 change (use no coins)
    dp[0][0] = 1;

    for (int i = 1; i <= n; i++) {
        for (int j = 0; j <= change; j++) {
            dp[i][j] = dp[i - 1][j]; // Without using the current coin
            coinUsage[i][j] = 0;

            if (j >= coins[i - 1] && dp[i][j - coins[i - 1]] > 0 && coinUsage[i][j - coins[i - 1]] < counts[i - 1]) {
                dp[i][j] += dp[i][j - coins[i - 1]];
                coinUsage[i][j] = coinUsage[i][j - coins[i - 1]] + 1;
            }
        }
    }

    // Check if exact change is possible
    if (dp[n][change] == 0) {
        cout << "Sorry, exact change is not possible. Remaining: " << change << " EGP." << endl;
        return false;
    }

    // Backtrack to determine the coins used
    int remaining = change;
    cout << "Change given: ";
    for (int i = n; i > 0 && remaining > 0; i--) {
        while (remaining >= coins[i - 1] && coinUsage[i][remaining] > 0) {
            cout << coins[i - 1] << " EGP ";
            currency[coins[i - 1]]--; // Deduct the coin from the currency map
            remaining -= coins[i - 1];
            coinUsage[i][remaining]--;
        }
    }
    cout << endl;

    return true;
}

bool provideChange_Brute(map<int, int>& currency, int change)
{
    vector<int> denominations;
    for (auto it = currency.begin(); it != currency.end(); ++it)
    {
        if (it->second > 0)
        {
            denominations.push_back(it->first);
        }
    }

    sort(denominations.begin(), denominations.end());

    vector<int> bestCombination;
    int minCoins = INT_MAX;

    int totalCombinations = 1 << denominations.size();

    for (int mask = 0; mask < totalCombinations; mask++) {
        map<int, int> tempCurrency = currency;
        vector<int> tempCombination;
        int currentChange = change;

        for (int i = 0; i < denominations.size(); i++)
        {
            if (mask & (1 << i))
            {
                int denom = denominations[i];
                while (currentChange >= denom && tempCurrency[denom] > 0)
                {
                    currentChange -= denom;
                    tempCurrency[denom]--;
                    tempCombination.push_back(denom);
                }
            }
        }

        if (currentChange == 0 && tempCombination.size() < minCoins) {
            bestCombination = tempCombination;
            minCoins = tempCombination.size();
        }
    }

    if (bestCombination.empty()) {
        cout << "Sorry, exact change is not possible. Remaining: " << change << " EGP." << endl;
        return false;
    }

    for (int i = 0; i < bestCombination.size(); i++) {
        currency[bestCombination[i]]--;
    }

    cout << "Change given: ";
    for (int i = 0; i < bestCombination.size(); i++) {
        cout << bestCombination[i] << " EGP ";
    }
    cout << endl;

    return true;
}

void displayCurrency(const map<int, int>& currency)
{
    cout << "Remaining currency in the vending machine:" << endl;
    for (auto it = currency.begin(); it != currency.end(); ++it)
    {
        cout << it->second << " x " << it->first << " EGP " << endl;
    }
    cout << endl;
}

void displayStock(vector<pair<string, int>>& products, vector<int>& stock)
{
    cout << "Remaining stock of products:" << endl;
    for (int i = 0; i < products.size(); i++)
    {
        cout << products[i].first << ": " << stock[i] << endl;
    }
    cout << endl;
}

int main()
{
    vector<pair<string, int>> products;
    vector<int> stock;
    map<int, int> currency;

    loadStock(products, stock);
    loadCurrency(currency);

    int remainingMoney = 0;

    while (true)
    {
        displayProducts(products, stock);
        int productIndex = selectProduct(products, stock);

        if (productIndex == -1)
        {
            break;
        }

        cout << "Selected product: " << products[productIndex].first
             << " - Price: " << products[productIndex].second << " EGP" << endl;

        int productPrice = products[productIndex].second;

        if (remainingMoney >= productPrice)
        {
            remainingMoney -= productPrice;
            stock[productIndex]--;
            cout << "Remaining balance used to buy the item. Remaining balance: "
                 << remainingMoney << " EGP." << endl;
        }
        else
        {
            int amountNeeded = productPrice - remainingMoney;
            cout << "Insert " << amountNeeded << " EGP: ";
            int moneyInserted;
            cin >> moneyInserted;

            if (moneyInserted < amountNeeded)
            {
                cout << "Insufficient amount. Transaction canceled." << endl;
                continue;
            }

            addMoneyToCurrency(currency, moneyInserted);

            int change = moneyInserted - amountNeeded;
            remainingMoney = change;
            stock[productIndex]--;

            if (remainingMoney > 0)
            {
                cout << "Remaining balance: " << remainingMoney << " EGP." << endl;
            }
        }

        displayStock(products, stock);
        displayCurrency(currency);

        cout << "Do you want to:\n"
             << "1. Buy another item\n"
             << "2. Exit and take remaining money" << endl
             << "Choice: ";
        int choice;
        cin >> choice;

        if (choice == 2)
        {
            if (remainingMoney > 0)
            {
                cout << "Returning remaining money: " << remainingMoney << " EGP." << endl;
                //provideChange_Greedy(currency, remainingMoney);
                provideChange_DP(currency, remainingMoney);
                //provideChange_Brute(currency, remainingMoney);
            }
            break;
        }
    }

    cout << "Thank you for using the vending machine!" << endl;

    saveStock(products, stock);
    saveCurrency(currency);

    return 0;
}
