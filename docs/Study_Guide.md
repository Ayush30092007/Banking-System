# Banking System – Study Guide for Your Oral Presentation

## 1. The project in 30 seconds (say this first)

"It is a console banking system with two roles. An **admin** creates, searches, updates and closes accounts and runs month-end interest. A **customer** logs in with account number and PIN to deposit, withdraw, transfer and view history. There are two account types, **Savings** and **Current**. They share one abstract base class `Account`, but follow different rules. All data is saved to a text file, so it survives a restart."

## 2. The 10 classes and what each one is responsible for

| Class | Job (one sentence) | Key idea |
|---|---|---|
| `Account` (abstract) | Holds number, holder, PIN, balance, history; does deposit/withdraw | Abstraction, encapsulation |
| `SavingsAccount` | Earns 2.5% p.a., must keep RM 10 minimum | Inheritance, override |
| `CurrentAccount` | Overdraft up to RM 1000, 12% p.a. charge on overdraft | Inheritance, override |
| `Customer` | Name, IC, phone | Encapsulation (private + getters/setters) |
| `Transaction` | One history line (type, amount, balance after, note, time) | Composition, `operator<<` |
| `Bank` | Owns all accounts; add / find / update / delete / transfer / report | Smart pointers, STL, lambdas |
| `FileManager` | ONLY reads/writes the data file | Single Responsibility |
| `Menu` | ONLY console UI; talks to `Bank` through its public interface | Separation of UI and logic |
| `BankException` + 4 subclasses | Custom errors: InvalidAmount, InsufficientFunds, AccountNotFound, DuplicateAccount | Exception hierarchy |
| `Util` (namespace) | Input validation, formatting, templates | Templates, namespace |

### Relationships (draw this on paper if asked)

```
Menu ──uses──> Bank ──owns (unique_ptr, 0..*)──> Account ◄── SavingsAccount
                │                                   │  ◄── CurrentAccount
                └──uses──> FileManager              ├── has-a Customer      (composition)
                                                    └── has many Transaction (composition)
```

- **Inheritance (is-a):** Savings and Current are accounts.
- **Composition (has-a, owns lifetime):** Account owns its Customer and Transactions. Delete the account and they are gone. `Bank` also owns its accounts through `unique_ptr`, so that is composition too (exclusive ownership). Your `Oral_Prep.md` calls it "aggregation"; if asked, say "ownership through `unique_ptr`, which is composition".
- **Association:** `Menu` holds a reference to `Bank` but does not own it.

## 3. What happens when you run it (trace one deposit)

1. `main.cpp` creates `Bank bank(file)`. The constructor calls `FileManager::load()`, which fills the vector from the text file.
2. `Menu menu(bank); menu.run();` starts the main loop.
3. Customer picks **Deposit** → `Menu::customerMenu` reads the amount with `Util::readNumber<double>` (validated, loops until valid).
4. `bank_.deposit(accNo, amt)` → `getAccount()` (throws `AccountNotFoundException` if missing) → `Account::deposit()`.
5. `Account::deposit` checks `amount > 0` (else `InvalidAmountException`), adds to `balance_`, calls private `record()` which appends a `Transaction`.
6. `Bank::deposit` then calls `save()` → `FileManager::save()` rewrites the file.
7. Any `BankException` is caught in `Menu` and printed as `Error: ...`, so the program never crashes on bad input.

## 4. Polymorphism: the most important thing to be able to show

**Where:** `Bank::applyMonthEnd()` in `Bank.cpp`:
```cpp
for (auto& a : accounts_) total += a->applyMonthEnd();
```
`a` is a `unique_ptr<Account>`, but at runtime the object is a Savings or Current account. `Account::applyMonthEnd()` calls the **virtual** `monthEndAdjustment()`:
- Savings returns `balance * 2.5% / 12` (positive = interest)
- Current returns `balance * 12% / 12` only when balance is negative (negative = charge)

Same for `withdraw()`: it calls virtual `availableToWithdraw()`. Savings returns `balance - 10`; Current returns `balance + 1000`. So the one `withdraw()` code enforces two different rules.

**Why `virtual ~Account() = default;` matters:** `Bank` deletes derived objects through a base pointer. Without a virtual destructor that is undefined behaviour.

**Abstract:** `Account` has 4 pure virtual functions (`= 0`), so `Account a;` will not compile. It prevents anyone creating a meaningless "generic" account.

## 5. Where each required concept lives (for "show me in your code")

| Concept | File / function |
|---|---|
| Class vs object | `class SavingsAccount` (`SavingsAccount.h`); object made in `createAccount()` (`Account.cpp`) |
| Encapsulation | `Account` private `balance_`, `pin_`; only `deposit()/withdraw()` change the balance |
| Constructor | `Account(int accNo, Customer holder, string pin, double balance = 0.0)`; derived classes reuse it with `using Account::Account;` |
| Abstraction | pure virtuals in `Account.h` |
| Inheritance | `SavingsAccount`, `CurrentAccount` : `public Account` |
| Polymorphism | `Bank::applyMonthEnd`, `Account::withdraw` |
| Overloading | `Bank::findAccount(int)` has const and non-const versions; `Account::getHolder()` also has both. Operator overloading: `operator<` and `operator<<` |
| Overriding | `getType()`, `availableToWithdraw()`, etc. marked `override` |
| Composition | `Account` has `Customer holder_` and `vector<Transaction> history_` |
| STL | `vector<unique_ptr<Account>>` in `Bank.h`; `std::sort`, `find_if`, `any_of`, `accumulate`, `count_if`, `max_element` in `Bank.cpp` |
| Templates | `Util::readNumber<T>`, `Util::fixed<T>` in `Utils.h` |
| Lambdas | `[accNo](const auto& a){...}` in `Bank.cpp` |
| Smart pointers | `unique_ptr`, `make_unique` |
| Exceptions | `BankException.h`; try/catch in `Menu.cpp` and `main.cpp` |
| File handling | `FileManager.cpp` (`ifstream` / `ofstream`) |
| Design pattern | Factory: `createAccount()` |
| constexpr | `INTEREST_RATE`, `OVERDRAFT_LIMIT`, etc. |

## 6. Question #10 ("add a new class") – practise this one out loud

Add `FixedDepositAccount`:
1. Create `include/FixedDepositAccount.h` and `src/FixedDepositAccount.cpp`, deriving from `Account`, overriding the 4 pure virtuals (e.g. `availableToWithdraw()` returns 0 before maturity).
2. Add one `if (type == "FixedDeposit")` line in `createAccount()` in `Account.cpp`.
3. Add one option in `Menu::createAccountUI()`.

`Bank`, `FileManager` and the customer menu do **not** change. That is the payoff of polymorphism. The Makefile picks up new `src/*.cpp` automatically.

## 7. Things a sharp lecturer might probe (be honest, don't bluff)

- **Money stored as `double`.** Real banks use integer cents because binary floating point cannot represent 0.1 exactly. You round interest to 2 decimals with `std::round`, and compare with `1e-9` tolerance, but integer cents would be safer. Good "Future Improvements" answer.
- **PINs are saved in plain text** in `data/accounts.txt`, and the admin password is hard-coded (`admin123`). Real systems hash PINs. Mention this as a limitation.
- **Corrupted file behaviour.** If loading fails midway, the bank keeps the records loaded so far and warns you. But the next save would overwrite the file with only those records. A fix would be to back up the file or refuse to save after a failed load.
- **Transfer is not atomic across a crash.** Withdraw then deposit then save; fine for a console project since a failure between steps is very unlikely, but a real bank would use transactions.
- **Why `Customer` is passed by value then `std::move`d:** the constructor needs its own copy anyway, so taking by value and moving avoids a second copy when the caller passes a temporary.
- **Why `std::vector<std::unique_ptr<Account>>` and not `vector<Account>`:** `Account` is abstract and you would get object slicing; you need pointers to hold different derived types in one container. `unique_ptr` gives automatic cleanup with no `delete`.
- **`using Account::Account;`:** inherits the base constructors so derived classes don't repeat them (C++11).
- **`const` correctness:** `getBalance() const`, `verifyPin() const`, `const Account* findAccount(int) const` — they promise not to modify the object, and let the compiler enforce it.

## 8. Suggested 5-minute demo order (for the video and the live demo)

1. Run `./bank`; show the empty-bank message.
2. Admin login → create a Savings account (Ali, RM 500) and a Current account (Siti, RM 100).
3. Show **invalid input handling**: type letters at a menu, a 3-digit IC, a 5-digit PIN.
4. View all accounts sorted by balance; search "ali".
5. Run month-end → Ali's balance becomes 501.04 (polymorphism in action).
6. Customer login as Siti: withdraw RM 1000 → works (overdraft), withdraw more → refused. Then show the same on Ali's Savings account: can't go below RM 10.
7. Transfer, then show transaction history.
8. Exit, restart, show "Loaded 2 account(s)" (file persistence).

## 9. Remaining items on the submission checklist

Source code, report, UML, user manual, and 27 test cases are all present. Still to do on your side: the **YouTube demo video (under 10 minutes)** and the **peer evaluation of your partner's project**.

## 10. Build and run

```
make            # builds ./bank  (g++ -std=c++17)
./bank          # run
bash tests/run_tests.sh   # 27 automated tests
```
On Windows with MinGW: `g++ -std=c++17 -Iinclude main.cpp src/*.cpp -o bank.exe` then `bank.exe`.
