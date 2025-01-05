Wallet Generator Program v2 🚀

## Overview 🌟

This program is a cryptocurrency wallet generator written in C++. It creates wallets by generating mnemonic phrases, private keys, and public keys, calculates wallet addresses, and logs the wallet data if the address is found in a SQLite database. The program runs continuously in a loop, generating and logging wallets.

---

## Features ✨

1. **Mnemonic Phrase Generation** 📝:
   - Generates 12-word mnemonic phrases using a provided word list file (`bip.txt`).

2. **Cryptographic Key Generation** 🔑:
   - Generates private keys using a Keccak256 hash of the mnemonic.
   - Creates elliptic curve public keys using the `secp256k1` curve.

3. **Wallet Address Calculation** 🏦:
   - Derives wallet addresses from the public key using a Keccak256 hash.

4. **SQLite Database Check** 🔍:
   - Checks if the generated address exists in a SQLite database (`database.db`).

5. **Logging** 📜:
   - Logs the mnemonic phrase, wallet address, and private key to a file (`log.txt`) if the address is found in the database.

6. **Error Handling** ⚠️:
   - Handles errors related to file I/O, SQLite operations, and OpenSSL key generation.

---

## Requirements 📦

### Software 💻
- **C++17 or later**
- **Libraries**:
  - [SQLite3](https://www.sqlite.org) 🛢️
  - [OpenSSL](https://www.openssl.org) 🔐

### Files 📂
- **Word List File**:
  - A text file named `bip.txt` containing words for mnemonic phrase generation, one word per line.
- **Database**:
  - A SQLite database file named `database.db` with a table `addresses` containing wallet addresses.

---

## Installation 🛠️

### Prerequisites 🧰
1. Install the required libraries:
   - **Linux**: 
     ```bash
     sudo apt-get install libsqlite3-dev libssl-dev
     ```
   - **macOS**: 
     ```bash
     brew install sqlite openssl
     ```
   - **Windows**:
     Install libraries using vcpkg or download precompiled binaries from official sources.

2. Ensure the `bip.txt` file is in the same directory as the program.

### Build Instructions 🏗️
1. Clone or download the source code.
2. Compile the program:
   ```bash
   g++ -o wallet_generator wallet_generator.cpp -lsqlite3 -lssl -lcrypto
   ```

---

## Usage 🚀

1. Ensure the `database.db` and `bip.txt` files are present in the working directory.
2. Run the program:
   ```bash
   ./wallet_generator
   ```
3. The program will continuously generate wallets, check the database for matches, and log matched wallets to `log.txt`. 🗂️

---

## Program Structure 🧩

### Functions 🔧
1. **`save_to_file`**: Logs wallet data to a file.
2. **`read_words_from_file`**: Reads words from `bip.txt` for mnemonic generation.
3. **`generate_mnemonic`**: Creates a 12-word mnemonic phrase.
4. **`keccak256_hash`**: Computes a Keccak256 hash.
5. **`check_address_in_db`**: Queries the SQLite database to check for an address.
6. **`generate_wallet`**: Orchestrates the wallet creation and logging process.

### Main Loop 🔄
The program runs an infinite loop, calling the `generate_wallet` function to generate and process wallets continuously.

---

## File Formats 📄

### `bip.txt` 📘
- Contains a list of words, one per line.
- Example:
  ```
  apple
  banana
  cherry
  ...
  ```

### `log.txt` 🗒️
- Logs wallet details in the following format:
  ```
  Seed: <mnemonic phrase>
  Address: <wallet address>
  Private: <private key>
  -----------------------------
  ```

---

## Customization 🎨

1. **Database Path**:
   - Change the `db_path` variable to use a different SQLite database.

2. **Log File Path**:
   - Update the `log_path` variable to specify a different log file.

---

## Known Issues 🐛

1. **Infinite Loop** 🔁:
   - The program runs indefinitely. Modify the `main` function to control execution flow if necessary.

2. **Error Handling** ⚠️:
   - Minimal error handling for database schema or corrupted files. Ensure the input files are well-formed.

3. **Performance** ⏱️:
   - Random generation and database queries may be slow for large word lists or databases.
  
---

## Contributing 🤝

Contributions, bug reports, and feature requests are welcome! Fork the repository and submit a pull request. 💡
