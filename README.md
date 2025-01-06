Wallet Generator Program v2 🚀

# 🚀 Wallet Generator v2

This project is a multithreaded wallet generator written in C++ that checks generated wallets against a SQLite database containing binary-encoded addresses (BLOB). It uses the **SHA256** hashing algorithm and supports **mnemonic phrase generation**.

---

## 🛠 Features

- 🔑 **Generate Wallets**: Create wallets using random mnemonic phrases.
- 🗂 **Database Lookup**: Cross-check generated wallets against a SQLite database of binary-encoded addresses (BLOB).
- 🌍 **Multithreading**: Utilize multiple CPU threads for high performance.
- 📜 **Logging**: Save successfully matched wallets to a log file.
- ⏲ **Real-Time Status**: Display generation stats in the terminal.

---

## 🏗 Prerequisites

1. **C++17** or higher.
2. **SQLite3** development library.
3. **OpenSSL** for SHA256 hashing.
4. A word list file (`bip.txt`) containing the words for generating mnemonic phrases.
5. A SQLite database (`database.db`) with a table `addresses` and `BLOB` type for storing wallet addresses.

---

## 📦 How to Build and Run

### 🧰 1. Install Dependencies
Ensure you have the following installed on your system:
- SQLite3 development package (`libsqlite3-dev` on Ubuntu, `sqlite-devel` on Fedora).
- OpenSSL library (`libssl-dev` on Ubuntu, `openssl-devel` on Fedora).

### ⚙️ 2. Compile the Code
Run the following command in your terminal:
```bash
g++ -o CryptoWallet CryptoWallet.cpp -lssl -lcrypto -lsqlite3 -Wno-deprecated-declarations
```

### ▶️ 3. Run the Program
Run the compiled binary:
```bash
./wallet_generator
```

The program will:
1. Load words from the `bip.txt` file.
2. Continuously generate wallets and check if they exist in the `database.db` SQLite database.
3. Save found wallets to a `log.txt` file.

---

## 🗂 Project Structure

```
├── wallet_generator.cpp    # Main C++ source code
├── bip.txt                 # Word list file for generating mnemonic phrases
├── database.db             # SQLite database with binary-encoded addresses (BLOB)
├── log.txt                 # Output file for matched wallets
└── README.md               # Project documentation
```

---

## ⚡️ Usage Details

### 🔢 Multithreading
The number of threads defaults to the hardware concurrency on your machine. You can modify this by changing:
```cpp
int num_threads = thread::hardware_concurrency();
```

### 📝 Logging
Matched wallets will be logged in the following format:
```
Seed: <mnemonic phrase>
Address: <generated address>
Private: <private key>
-----------------------------
```

### 🗄 Database
- The SQLite database `database.db` must have a table named `addresses` with a column `address` of type `BLOB`.
- Example table creation SQL:
  ```sql
  CREATE TABLE addresses (
      address BLOB NOT NULL
  );
  ```

---

## ✨ Example Output

**Terminal Status:**
```
Generated wallets: 100000 | Threads in use: 8 | Elapsed time: 0:01:25
```

**Log File (`log.txt`):**
```
Seed: travel moon galaxy tiger forest orbit token velvet sun flame salmon lucky
Address: a3b1c5d6e7f9...
Private: d4e5f6a7b8c9...
-----------------------------
```

---

## 📚 Resources

- [SQLite Documentation](https://sqlite.org/docs.html)
- [OpenSSL Documentation](https://www.openssl.org/docs/)

---

## 🛡 License

This project is released under the **MIT License**. Feel free to use and modify it.

---

🎉 **Happy Wallet Generating!** 🎉

