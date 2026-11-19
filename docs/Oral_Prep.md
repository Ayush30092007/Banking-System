# Oral Presentation Prep – answers using THIS project

**Q1 Class vs object?** A class is a blueprint; an object is an instance. `SavingsAccount` is a class; `bank.openAccount("Savings", ...)` creates one via `createAccount()` in `Account.cpp`.

**Q2 Encapsulation?** `Account` keeps `balance_` and `pin_` **private**. If `balance_` were public, any code could set it to any value and skip `deposit()`/`withdraw()` – no validation, no history record. Outside code must use `deposit()`, `withdraw()`, `getBalance()`.

**Q3 Constructor?** `Account(int accNo, Customer holder, string pin, double balance = 0.0)` initialises members with an initializer list. Memory for the object is allocated, then the constructor runs; the `vector<Transaction>` starts empty. `SavingsAccount` reuses it with `using Account::Account;`.

**Q4 Inheritance?** `SavingsAccount` and `CurrentAccount` inherit from `Account`. A savings account *is an* account (has number, holder, balance, deposit/withdraw) – genuine "is-a", with only the rules differing.

**Q5 Polymorphism?** `Bank::applyMonthEnd()` loops over `vector<unique_ptr<Account>>` and calls `a->applyMonthEnd()` → `monthEndAdjustment()`. Savings returns interest; Current returns an overdraft charge. Also `withdraw()` calls virtual `availableToWithdraw()`.

**Q6 Overloading vs overriding?** Overriding: `getType()` redefined in each derived class (same signature, runtime decision). Overloading: `Bank::findAccount(int)` has a `const` and a non-const version; also `Customer`'s getters. (Operator overloading: `operator<<`, `operator<`.)

**Q7 Abstraction vs encapsulation?** `Account` is abstract (pure virtual `getType`, `availableToWithdraw`, `monthEndAdjustment`, `policySummary`), so nobody can create a plain `Account` – callers only know the interface. Encapsulation = hiding data with private; abstraction = hiding *how* behind a common interface.

**Q8 Composition/association?** `Account` *has-a* `Customer` and a `vector<Transaction>` (composition – they live and die with the account). `Bank` manages many `Account`s (aggregation). `Menu` uses `Bank` (association). A Customer is not "a kind of" Account, so inheritance would be wrong.

**Q9 STL?** `vector<unique_ptr<Account>>` – dynamic size, fast iteration, and pointers are needed to store different derived types polymorphically. Also `std::sort`, `find_if`, `any_of`, `accumulate`, `max_element` with lambdas.

**Q10 Add a new class?** To add `FixedDepositAccount`: create `FixedDepositAccount.h/.cpp` deriving from `Account` and override the 4 pure virtuals; add one `if` line in `createAccount()` (Account.cpp) and one menu option in `Menu::createAccountUI()`. `Bank`, `FileManager` and the customer menu need **no change** – that is the benefit of polymorphism.

## Bonus features – where they are
| Feature | Location |
|---|---|
| File persistence | `FileManager.cpp` |
| Exception handling | `BankException.h`, try/catch in `Menu.cpp`, `main.cpp` |
| Smart pointers | `unique_ptr<Account>` in `Bank.h`, `make_unique` in `Account.cpp` |
| Templates | `Util::readNumber<T>`, `Util::fixed<T>` in `Utils.h` |
| STL algorithms + lambdas | `Bank.cpp` (sort, find_if, any_of, accumulate, count_if, max_element) |
| Operator overloading | `operator<`, `operator<<` in `Account`, `Transaction` |
| Design pattern | Factory – `createAccount()` |
| constexpr / const correctness / namespace / move | `SavingsAccount.h`, const getters, `namespace Util`, `std::move` in constructors |
