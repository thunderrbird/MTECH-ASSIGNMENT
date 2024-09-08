#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <cmath>
#include <algorithm>
#include <unordered_set>

using namespace std;

// Function to process the tokens (either pad or truncate to 10 characters)
string tokenProcessor(const string &token) {
    string result = token;
    if (token.size() < 10) {
        // Pad with '*' if less than 10 characters
        result.append(10 - token.size(), '*');  
    } else if (token.size() > 10) {
        // Truncate to 10 characters if more
        result = token.substr(0, 10);  
    }
    return result;
}

// Function to extract tokens from the file, with given limitations
vector<string> extractTokens(const string &filename) {
    ifstream file(filename);
    vector<string> tokens;
    string token = "";
    char forbidden[] = {',', '.', ' '};

    if (file.is_open()) {
        char ch;
        while (file.get(ch)) {
            bool isforbiddenElement = (ch == forbidden[0] || ch == forbidden[1] || ch == forbidden[2]);

            if (isforbiddenElement) {
                if (!token.empty()) {
                    string processedToken = tokenProcessor(token);
                    tokens.push_back(processedToken);
                    token = "";
                }
            } else {
                token += ch;
            }
        }

        if (!token.empty()) {
            string processedToken = tokenProcessor(token);
            tokens.push_back(processedToken);
        }
    }

    return tokens;
}

// Convert decimal to radix-3 (ternary)
string decimalToTernary(int decimal) {
    if (decimal == 0) return "0";
    string ternary = "";
    while (decimal > 0) {
        ternary = to_string(decimal % 3) + ternary;
        decimal /= 3;
    }
    return ternary;
}

// Base class for common probe functionality
class DataStructure {
protected:
    int successfulSearchProbes = 0;
    int unsuccessfulSearchProbes = 0;
    int insertProbes = 0;
    int deleteProbes = 0;
    int successfulSearchCount = 0;
    int unsuccessfulSearchCount = 0;
    int insertCount = 0;
    int deleteCount = 0;

public:


    virtual void insert(const string& token) = 0;
    virtual bool search(const string& token) = 0;
    virtual void remove(const string& token) = 0;

    double averageProbes(int probes, int count) const {
        if (count == 0) {
            return 0;
        } else {
            return static_cast<double>(probes) / count;
        }
    }

    virtual void printProbes() {
        cout << "Average no. of probe for successful search: ";
        cout << averageProbes(successfulSearchProbes, successfulSearchCount) << endl;
        cout << "Average no. of probe for unsuccessful search: ";
        cout << averageProbes(unsuccessfulSearchProbes, unsuccessfulSearchCount) << endl;
        cout << "Average no. of probe for insertion: ";
        cout << averageProbes(insertProbes, insertCount) << endl;
        cout << "Average no. of probe for deletion: ";
        cout << averageProbes(deleteProbes, deleteCount) << endl;
    }
};

// Open Hashing (Unsorted Lists)
class OpenHashTableUnsorted : public DataStructure {
private:
    vector<list<string>> table;
    int size;

public:
    OpenHashTableUnsorted(int m) {
        size = m;
        table.resize(m);  // Assuming 'table' is a vector
    }

    int hash(int key) {
        double A = (sqrt(5) - 1) / 2;
        return static_cast<int>(size * (key * A - int(key * A)));
    }

    int generateKey(const string& token) {
        int sum = 0;
        for (char c : token) {
            sum += static_cast<int>(c);
        }
        return sum;
    }

    void insert(const string& token) override {
        int key = generateKey(token);
        int index = hash(key);
        table[index].push_back(token);
        insertProbes += table[index].size();
        insertCount++;
    }

    bool search(const string& token) override {
        int key = generateKey(token);
        int index = hash(key);
        int probes = 0;
        for (const auto& str : table[index]) {
            probes++;
            if (str == token) {
                successfulSearchProbes += probes;
                successfulSearchCount++;
                return true;
            }
        }
        unsuccessfulSearchProbes += probes;
        unsuccessfulSearchCount++;
        return false;
    }

    void remove(const string& token) override {
        int key = generateKey(token);
        int index = hash(key);
        table[index].remove(token);
        deleteProbes += table[index].size();
        deleteCount++;
    }
};

// Open Hashing (Sorted Lists)
class OpenHashTableSorted : public DataStructure {
private:
    vector<list<string>> table;
    int size;

public:
    OpenHashTableSorted(int m) {
        size = m;                  // Initialize size
        table.resize(m);           // Initialize table with m empty lists
    }

    int hash(int key) {
        double A = (sqrt(5) - 1) / 2;
        return static_cast<int>(size * (key * A - int(key * A)));
    }

    int generateKey(const string& token) {
        int sum = 0;
        for (char c : token) {
            sum += static_cast<int>(c);
        }
        return sum;
    }

    void insert(const string& token) override{
        int key = generateKey(token);                // Compute the hash key
        int index = hash(key);                       // Compute the index using the hash function

        list<string>& lst = table[index];            // Access the list at the computed index
        bool inserted = false;                      // Flag to check if the token was inserted

        // Traverse the list to find the correct position
        for (auto it = lst.begin(); it != lst.end(); ++it) {
            if (*it >= token) {
                lst.insert(it, token);              
                inserted = true;
                break;
            }
        }

        if (!inserted) {
            lst.push_back(token);                   
        }

        // Compute the number of probes
        int probes = 0;
        for (auto it = lst.begin(); it != lst.end(); ++it) {
            probes++;
            if (*it == token) {
                break;
            }
        }
        insertProbes += probes;
        insertCount++;
    }

    bool search(const string& token) override {
        int key = generateKey(token);
        int index = hash(key);
        int probes = 0;
        for (const auto& str : table[index]) {
            probes++;
            if (str == token) {
                successfulSearchProbes += probes;
                successfulSearchCount++;
                return true;
            }
        }
        unsuccessfulSearchProbes += probes;
        unsuccessfulSearchCount++;
        return false;
    }

    void remove(const string& token) override {
        int key = generateKey(token);
        int index = hash(key);
        table[index].remove(token);
        deleteProbes += table[index].size();
        deleteCount++;
    }
};

// Closed Hashing (Linear Probing)
class ClosedHashTable : public DataStructure {
private:
    vector<string> table;
    vector<bool> occupied; // To track which slots are occupied
    int size;

public:
    ClosedHashTable(int m) : size(m) {
        table.resize(m, "");  // Initialize table with empty strings
        occupied.resize(m, false); // Initialize occupied status
    }

    int hash(int key, int i = 0) {
        return (key + i) % size;
    }

    int generateKey(const string& token) {
        int sum = 0;
        for (char c : token) {
            sum += static_cast<int>(c);
        }
        return sum;
    }

    void insert(const string& token) {
        int key = generateKey(token);
        int i = 0;
        int index;
        while (occupied[index = hash(key, i)]) {
            i++;
        }
        table[index] = token;
        occupied[index] = true;
        insertProbes += i + 1;
        insertCount++;
    }

    bool search(const string& token) {
        int key = generateKey(token);
        int i = 0;
        int index;
        int probes = 0;
        while (occupied[index = hash(key, i)]) {
            probes++;
            if (table[index] == token) {
                successfulSearchProbes += probes;
                successfulSearchCount++;
                return true;
            }
            i++;
        }
        unsuccessfulSearchProbes += probes;
        unsuccessfulSearchCount++;
        return false;
    }

    void remove(const string& token) {
        int key = generateKey(token);
        int i = 0;
        int index;
        while (occupied[index = hash(key, i)]) {
            if (table[index] == token) {
                table[index] = "";  // Mark as empty
                occupied[index] = false;
                deleteProbes += i + 1;
                deleteCount++;
                return;
            }
            i++;
        }
    }
};


// Binary Search Tree
class BinarySearchTree : public DataStructure {
private:
    struct TreeNode {
        string token;
        TreeNode *left;
        TreeNode *right;

        // Constructor
        TreeNode(const string& str) {
            token = str;
            left = right = nullptr;  // Initialize left and right pointers
        }
    };

    TreeNode* root;

    TreeNode* insert(TreeNode* node, const string& token, int& probes)  {
        if (!node) {
            probes++;
            return new TreeNode(token);
        }
        probes++;
        if (token < node->token)
            node->left = insert(node->left, token, probes);
        else if (token > node->token)
            node->right = insert(node->right, token, probes);
        return node;
    }

    bool search(TreeNode* node, const string& token, int& probes)  {
        if (!node) return false;
        probes++;
        if (node->token == token) return true;
        if (token < node->token)
            return search(node->left, token, probes);
        else
            return search(node->right, token, probes);
    }

    TreeNode* remove(TreeNode* node, const string& token, int& probes) {
        if (!node) return nullptr;
        probes++;
        if (token < node->token) {
            node->left = remove(node->left, token, probes);
        } else if (token > node->token) {
            node->right = remove(node->right, token, probes);
        } else {
            if (!node->left) {
                TreeNode* temp = node->right;
                delete node;
                return temp;
            } else if (!node->right) {
                TreeNode* temp = node->left;
                delete node;
                return temp;
            }
            TreeNode* temp = findMin(node->right);
            node->token = temp->token;
            node->right = remove(node->right, temp->token, probes);
        }
        return node;
    }

    TreeNode* findMin(TreeNode* node) {
        while (node && node->left) {
            node = node->left;
        }
        return node;
    }

public:
    BinarySearchTree() {
        root = nullptr;  // Initialize root pointer
    }

    void insert(const string& token) {
        int probes = 0;
        root = insert(root, token, probes);
        insertProbes += probes;
        insertCount++;
    }

    bool search(const string& token) {
        int probes = 0;
        bool found = search(root, token, probes);
        if (found) {
            successfulSearchProbes += probes;
            successfulSearchCount++;
        } else {
            unsuccessfulSearchProbes += probes;
            unsuccessfulSearchCount++;
        }
        return found;
    }

    void remove(const string& token) {
        int probes = 0;
        root = remove(root, token, probes);
        deleteProbes += probes;
        deleteCount++;
    }
};

// Main function to handle the overall program logic
int main() {
    int m, n, I;
    cout << "Enter hash table size (m): ";
    cin >> m;
    cout << "Enter the number of insertions (n): ";
    cin >> n;

    vector<int> M;
    cout << "Enter methods to investigate (1: Open Hash Unsorted, 2: Open Hash Sorted, 3: Closed Hash, 4: BST), end with -1: ";
    int method;
    while (cin >> method && method != -1) {
        M.push_back(method);
    }

    cout << "Enter decimal number to be converted to radix-3 (I): ";
    cin >> I;
    string operations = decimalToTernary(I);

    string filename;
    cout << "Enter the filename containing tokens: ";
    cin >> filename;
    vector<string> tokens = extractTokens(filename);

    unordered_set<string> distinctTokens;  // Ensure distinct tokens are used

    OpenHashTableUnsorted* openUnsorted = nullptr;
    OpenHashTableSorted* openSorted = nullptr;
    ClosedHashTable* closedHash = nullptr;
    BinarySearchTree* bst = nullptr;

    // Initialize the required data structures
    for (int method : M) {
        if (method == 1) {
            openUnsorted = new OpenHashTableUnsorted(m);
        } else if (method == 2) {
            openSorted = new OpenHashTableSorted(m);
        } else if (method == 3) {
            closedHash = new ClosedHashTable(m);
        } else if (method == 4) {
            bst = new BinarySearchTree();
        }
    }

    // Insert the first n distinct tokens into the data structure
    for (const auto& token : tokens) {
        if (distinctTokens.size() >= n) break;
        if (distinctTokens.find(token) == distinctTokens.end()) {
            distinctTokens.insert(token);
            for (int method : M) {
                if (method == 1 && openUnsorted){
                    openUnsorted->insert(token);
                }
                if (method == 2 && openSorted) {
                    openSorted->insert(token);
                }
                if (method == 3 && closedHash){
                    closedHash->insert(token);
                }
                if (method == 4 && bst) {
                    bst->insert(token);
                }
            }
        }
    }

    // Perform operations on the rest of the tokens 
    for (size_t i = n; i < tokens.size(); ++i) {
        // Loop through the operations
        char op = operations[i % operations.size()];  
        const string& token = tokens[i];
        for (int method : M) {
            if (op == '0') {  
                // Search
                if (method == 1 && openUnsorted){
                    openUnsorted->search(token);
                }
                if (method == 2 && openSorted) {
                    openSorted->search(token);
                }
                if (method == 3 && closedHash) {
                    closedHash->search(token);
                }
                if (method == 4 && bst){
                    bst->search(token);
                }
            } else if (op == '1') { 
                // Insert
                if (method == 1 && openUnsorted){
                    openUnsorted->insert(token);
                }
                if (method == 2 && openSorted) {
                    openSorted->insert(token);
                }
                if (method == 3 && closedHash) {
                    closedHash->insert(token);
                }
                if (method == 4 && bst) {
                    bst->insert(token);
                }
            } else if (op == '2') { 
                // Delete
                if (method == 1 && openUnsorted){
                    openUnsorted->remove(token);
                }
                if (method == 2 && openSorted) {
                    openSorted->remove(token);
                }
                if (method == 3 && closedHash) {
                    closedHash->remove(token);
                }
                if (method == 4 && bst) {
                    bst->remove(token);
                }
            }
        }
    }

    // Print out the average probes for each method
    for (int method : M) {
        cout << "\nMethod " << method << " Probes: " << endl;
        if (method == 1 && openUnsorted) {
            openUnsorted->printProbes();
        }
        if (method == 2 && openSorted){
            openSorted->printProbes();
        }
        if (method == 3 && closedHash) {
            closedHash->printProbes();
        }
        if (method == 4 && bst){
            bst->printProbes();
        }
    }

    // Clean up allocated memory
    delete openUnsorted;
    delete openSorted;
    delete closedHash;
    delete bst;

    return 0;
}
