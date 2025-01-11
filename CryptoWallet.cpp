#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <thread>
#include <atomic>
#include <sstream>
#include <ctime>
#include <sqlite3.h>
#include <openssl/sha.h>

using namespace std;

atomic<int> wallet_count(0);

class MnemonicGenerator {
public:
    explicit MnemonicGenerator(const string& filepath) {
        ifstream file(filepath);
        if (!file) {
            throw runtime_error("Word list file not found!");
        }

        string word;
        while (file >> word) {
            word_list.push_back(word);
        }
    }

    string generate_mnemonic() const {
        if (word_list.empty()) {
            throw runtime_error("Word list is empty!");
        }

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

private:
    vector<string> word_list;
};

class Wallet {
public:
    explicit Wallet(const string& mnemonic) {
        this->mnemonic = mnemonic;
        this->private_key = sha256_hash(mnemonic);
        this->address = sha256_hash(private_key);
    }

    const string& get_mnemonic() const { return mnemonic; }
    const string& get_private_key() const { return private_key; }
    const string& get_address() const { return address; }

private:
    string mnemonic;
    string private_key;
    string address;

    static string sha256_hash(const string& input) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);

        stringstream hex_stream;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            hex_stream << hash[i];
        }
        return hex_stream.str();
    }
};

class Database {
public:
    explicit Database(const string& db_path) {
        if (sqlite3_open(db_path.c_str(), &db) != SQLITE_OK) {
            throw runtime_error("Failed to open database: " + string(sqlite3_errmsg(db)));
        }
    }

    ~Database() {
        sqlite3_close(db);
    }

    bool check_address_in_db(const string& address) {
        sqlite3_stmt* stmt;
        const char* sql = "SELECT 1 FROM addresses WHERE address = ?";

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            cerr << "Failed to query database: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        sqlite3_bind_blob(stmt, 1, address.data(), address.size(), SQLITE_STATIC);

        int rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);

        return rc == SQLITE_ROW;
    }

private:
    sqlite3* db;
};

class Logger {
public:
    explicit Logger(const string& log_path) : log_path(log_path) {}

    void save(const Wallet& wallet) {
        ifstream infile(log_path);
        string existing_content;
        if (infile) {
            stringstream buffer;
            buffer << infile.rdbuf();
            existing_content = buffer.str();
        }
        infile.close();

        string new_entry = "Seed: " + wallet.get_mnemonic() + "\n"
                           "Address: " + wallet.get_address() + "\n"
                           "Private: " + wallet.get_private_key() + "\n"
                           "-----------------------------\n";

        ofstream outfile(log_path);
        outfile << existing_content << new_entry;
        outfile.close();
    }

private:
    string log_path;
};

class WalletGenerator {
public:
    WalletGenerator(const string& db_path, const string& log_path, const string& word_file_path)
        : database(db_path), logger(log_path), mnemonic_generator(word_file_path) {}

    void generate_wallet() {
        Wallet wallet(mnemonic_generator.generate_mnemonic());
        wallet_count++;

        if (database.check_address_in_db(wallet.get_address())) {
            logger.save(wallet);
            cout << endl << "Found wallet!" << endl;
            cout << "Seed: " << wallet.get_mnemonic() << endl;
            cout << "Address: " << wallet.get_address() << endl;
            cout << "Private: " << wallet.get_private_key() << endl;
            cout << "-----------------------------" << endl;
        }
    }

    void generate_wallets_in_parallel(int num_threads) {
        vector<thread> threads;

        for (int i = 0; i < num_threads; ++i) {
            threads.push_back(thread([this]() {
                while (true) {
                    generate_wallet();
                }
            }));
        }

        for (auto& th : threads) {
            th.join();
        }
    }

private:
    Database database;
    Logger logger;
    MnemonicGenerator mnemonic_generator;
};

class Application {
public:
    Application(const string& db_path, const string& log_path, const string& word_file_path)
        : generator(db_path, log_path, word_file_path) {}

    void run() {
        int num_threads = thread::hardware_concurrency();
        time_t start_time = time(0);

        thread status_thread(update_terminal_status, num_threads, start_time);
        status_thread.detach();

        generator.generate_wallets_in_parallel(num_threads);
    }

private:
    WalletGenerator generator;

    static void update_terminal_status(int num_threads, time_t start_time) {
        while (true) {
            time_t now = time(0);
            double duration = difftime(now, start_time);

            int hours = duration / 3600;
            int minutes = (duration - (hours * 3600)) / 60;
            int seconds = (duration - (hours * 3600) - (minutes * 60));

            string time_str = to_string(hours) + ":" + (minutes < 10 ? "0" : "") + to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + to_string(seconds);

            cout << "\rGenerated wallets: " << wallet_count.load()
                 << " | Threads in use: " << num_threads
                 << " | Elapsed time: " << time_str << flush;

            this_thread::sleep_for(chrono::seconds(1));
        }
    }
};

int main() {
    try {
        Application app("./database.db", "./log.txt", "bip.txt");
        app.run();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }

}
