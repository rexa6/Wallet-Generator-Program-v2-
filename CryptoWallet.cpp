#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <iomanip>
#include <sqlite3.h>
#include <openssl/sha.h>
#include <thread>
#include <atomic>

using namespace std;

atomic<int> wallet_count(0);  

void save_to_file(const string& log_path, const string& phrase, const string& address, const string& private_key) {
    ofstream log_file(log_path, ios::app);
    if (log_file.is_open()) {
        log_file << "Seed: " << phrase << "\n"
                 << "Address: " << address << "\n"
                 << "Private: " << private_key << "\n"
                 << "-----------------------------\n";
    }
}

vector<string> read_words_from_file(const string& filepath) {
    vector<string> words;
    ifstream file(filepath);
    string word;
    while (file >> word) {
        words.push_back(word);
    }
    return words;
}

string generate_mnemonic(const vector<string>& word_list) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, word_list.size() - 1);

    string mnemonic;
    for (int i = 0; i < 12; ++i) {
        mnemonic += word_list[dis(gen)] + " ";
    }
    mnemonic.pop_back();
    return mnemonic;
}

string sha256_hash(const string& input) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);

    stringstream hex_stream;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        hex_stream << hex << setw(2) << setfill('0') << (int)hash[i];
    }
    return hex_stream.str();
}

bool check_address_in_db(sqlite3* db, const string& address) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT 1 FROM addresses WHERE address = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to query database: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, address.c_str(), -1, SQLITE_STATIC);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return rc == SQLITE_ROW;
}

void generate_wallet(const string& db_path, const string& log_path, const vector<string>& word_list) {
    sqlite3* db;
    if (sqlite3_open(db_path.c_str(), &db) != SQLITE_OK) {
        cerr << "Failed to open database: " << sqlite3_errmsg(db) << endl;
        return;
    }

    string mnemonic = generate_mnemonic(word_list);
    string private_key = sha256_hash(mnemonic);
    string address = sha256_hash(private_key);

    cout << "Seed: " << mnemonic << endl;
    cout << "Private: " << private_key << endl;
    cout << "Address: " << address << endl;
    cout << "-----------------------------" << endl;

    if (check_address_in_db(db, address)) {
        save_to_file(log_path, mnemonic, address, private_key);
    }

    sqlite3_close(db);

    wallet_count++; 
}

void generate_wallets_in_parallel(const string& db_path, const string& log_path, const vector<string>& word_list, int num_threads) {
    vector<thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.push_back(thread(generate_wallet, db_path, log_path, word_list));
    }

    for (auto& th : threads) {
        th.join();
    }
}

int main() {
    string db_path = "./database.db";
    string log_path = "./log.txt";

    vector<string> word_list = read_words_from_file("bip.txt");
    if (word_list.empty()) {
        cerr << "Word list not found or empty!" << endl;
        return 1;
    }

    int num_threads = thread::hardware_concurrency();  
    cout << "Using " << num_threads << " threads." << endl;

    while (true) {
        generate_wallets_in_parallel(db_path, log_path, word_list, num_threads);
    }

    return 0;
}
