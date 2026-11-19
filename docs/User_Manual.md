# User Manual

## 1. System requirements
- Any OS with a C++17 compiler (g++ 7+ / clang / MSVC). Terminal / command prompt.

## 2. Compile and run
```
make
./bank                 (Windows: bank.exe)
./bank other_file.txt  (optional: use a different data file)
```
Data is saved automatically to `data/accounts.txt` after every change and loaded on start-up.

## 3. Main menu
1. Admin Login (password: `admin123`)  2. Customer Login (account number + PIN)  3. Exit

## 4. Admin menu (CRUD)
| Option | Function | CRUD |
|---|---|---|
| 1 | Create account (type, name, IC, phone, PIN, opening deposit ≥ RM 10) | Create |
| 2 | View all accounts, sorted by name or balance | Read |
| 3 | Search by account number or name keyword | Read |
| 4 | Update name/phone or reset PIN | Update |
| 5 | Delete (close) account – balance is paid out; overdrawn accounts cannot be closed | Delete |
| 6 | Account report (asks account number; shows details and full history) | Read |
| 7 | Logout | – |

## 5. Customer menu
Account details · Deposit · Withdraw · Transfer · Transaction history · Change PIN · Logout.

## 6. Account types
- **Savings** – 2.5% p.a. interest; must keep RM 10 minimum balance.
- **Current** – no interest; overdraft up to RM 1,000 (12% p.a. charge at month-end).

## 7. Input requirements
- Menu: whole number shown. Amounts: positive numbers. PIN: exactly 4 digits.
- IC: 12 digits (dashes allowed). Phone: 9–11 digits. Text fields: not empty, no `|` character.

## 8. Error messages
| Message | Meaning |
|---|---|
| Invalid input. Please enter a number between X and Y | Wrong menu/number entry – re-enter |
| Account N was not found | Account does not exist |
| This IC number already has an account of that type | Duplicate prevented |
| Insufficient funds. You can withdraw up to RM X | Exceeds the account's rule |
| Amount must be greater than zero | Zero amount entered |
| Cannot transfer to the same account | Sender = recipient |
| Cannot close an overdrawn account | Customer owes money |
| Too many failed attempts | 3 wrong passwords/PINs |
| Data file is corrupted at line N | Bad data file; valid records before it are kept |
| Cannot write to data file | Folder missing or no permission (create `data/`) |

## 9. Advanced features (bonus)
Automatic file persistence, month-end interest processing, sorting/searching with STL algorithms,
and custom exceptions – all are used through the normal menus above.
