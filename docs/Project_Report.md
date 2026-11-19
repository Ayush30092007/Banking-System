# Banking System – BAI22123 Object-Oriented Programming Project Report

**Name:** [YOUR NAME]  **Student ID:** [YOUR ID]  **Lecturer:** Dr. Ng Choon Ching  **Partner (peer evaluator):** [PARTNER NAME]
**Submission:** 20 November 2026   **Demo video:** [YOUTUBE LINK]

## 1. Introduction
This project is a console-based Banking System written in C++17. It lets bank staff (Admin) manage customer accounts and lets customers perform everyday banking operations. It is built using object-oriented design, with data saved to a text file.

## 2. Problem Statement
Small banks and credit unions that track accounts on paper or spreadsheets suffer from calculation errors, lost records and no consistent rules per account type. A structured program is needed that stores accounts, enforces account rules automatically and keeps a transaction history.

## 3. Objectives
1. Implement full CRUD (create, read, update, delete) for bank accounts.
2. Support deposit, withdraw and transfer with validation.
3. Model different account types (Savings, Current) with different rules using inheritance and polymorphism.
4. Persist data in a file so records survive after the program closes.
5. Handle invalid input and errors without crashing.
6. Apply encapsulation, abstraction, inheritance, polymorphism and composition.

## 4. System Requirements
**Functional:** admin login; create/view/search/update/delete accounts; customer login with PIN; deposit, withdraw, transfer; transaction history; change PIN; month-end interest; bank report; save/load.
**Non-functional:** runs in a terminal; response is instant; invalid input never terminates the program; data saved after every change.
**Software:** C++17 compiler (g++), any OS. **Target users:** bank staff and customers.

## 5. System Design
Layered design with single-responsibility classes:
- **Menu** (user interface) → **Bank** (business logic / CRUD) → **Account hierarchy** (rules) and **FileManager** (storage).
Flow: start → load file → main menu → Admin menu or Customer menu → every change is saved to `data/accounts.txt`.
Admin password is fixed (`admin123`) for demonstration.

## 6. UML Diagrams
- Use Case Diagram: `docs/UseCase.puml` (actors: Admin, Customer)
- Class Diagram: `docs/ClassDiagram.puml` (attributes, methods, visibility, inheritance, composition)

*(Render at https://www.plantuml.com/plantuml and paste the images here before submission.)*

## 7. Class Descriptions
| Class | Responsibility |
|---|---|
| `Account` (abstract) | Common data (number, holder, PIN, balance, history), deposit/withdraw, defines pure virtual rules |
| `SavingsAccount` | 2.5% p.a. interest; RM 10 minimum balance |
| `CurrentAccount` | Overdraft up to RM 1,000; 12% p.a. charge on overdraft |
| `Customer` | Holder's name, IC, phone |
| `Transaction` | One history entry (type, amount, balance after, note, time) |
| `Bank` | Stores accounts; CRUD, transfer, month-end, report |
| `FileManager` | Reads/writes the data file |
| `Menu` | Console menus and input prompts |
| `BankException` + 4 subclasses | Specific error types |
| `Util` namespace | Validated input, formatting helpers |

## 8. OOP Concepts Used
- **Classes/objects:** 10+ classes with clear responsibilities.
- **Encapsulation:** all data members private; access via getters and methods (e.g. balance changed only through `deposit`/`withdraw`).
- **Constructors:** member-initializer lists in `Account`, `Customer`, `Transaction`; `Bank(string file)` loads data.
- **Abstraction:** `Account` is abstract with 4 pure virtual functions.
- **Inheritance:** `SavingsAccount`/`CurrentAccount` extend `Account` (true is-a).
- **Polymorphism:** `Bank` holds `unique_ptr<Account>` and calls virtual functions; behaviour depends on the real type.
- **Composition/association:** `Account` has a `Customer` and `Transaction`s; `Bank` manages `Account`s; `Menu` uses `Bank`.

## 9. C++ Features Used
Basic: variables, I/O, operators, `if/switch`, `for/while/do-while`, functions with default arguments, `vector`.
Advanced (bonus): smart pointers (`unique_ptr`), templates, exception handling (custom hierarchy), file handling (`ifstream/ofstream`), operator overloading, lambdas with STL algorithms, Factory design pattern, `constexpr`, const correctness, namespaces, move semantics (`std::move`).

## 10. Implementation
- **Create:** `Bank::openAccount` validates deposit and duplicate IC+type, then uses the factory `createAccount`.
- **Read:** `findAccount`, `searchByName` (case-insensitive), `listSorted` (`std::sort` + lambda), `report` (`std::accumulate`).
- **Update:** `updateHolder`, `changePin`. **Delete:** `closeAccount` (pays out balance; rejects overdrawn).
- **Transfer:** withdraw from sender (rule-checked) then deposit to recipient; self-transfer and unknown account rejected.
- **Persistence:** pipe-delimited text (`ACC|...` followed by `TXN|...` lines); loading detects corrupted lines.
- **Validation:** `Util::readNumber<T>` loops until valid; IC, phone, PIN formats checked; EOF handled.

## 11. Testing
27 test cases (`docs/Test_Cases.md`) were run with `tests/run_tests.sh` against the final build. Categories: CRUD, validation, security (PIN/password), transactions, account rules, persistence, corrupted file.
One bug found during testing: test scripts exited early because "logout" and "exit" choices were confused – fixed in the test script (not the program). The compiler is run with `-Wall -Wextra` and produces no warnings.

## 12. Results
All 27 test cases passed (27/27). The program handles invalid input, missing accounts, duplicates, insufficient funds, wrong PINs and a corrupted data file without crashing. Data persists between runs.

## 13. Limitations
- PINs are stored as plain text in the file (not hashed).
- Single fixed admin password; one user at a time; text-file storage only.
- Only two account types; no GUI; interest is simple monthly calculation.

## 14. Future Improvements
PIN hashing, separate staff accounts, more account types (Fixed Deposit), database storage, GUI, unit-test framework, loan module.

## 15. Conclusion
The project meets the OOP requirements: multiple well-defined classes, abstraction, inheritance, polymorphism and composition, plus CRUD, validation, file storage and several advanced features. The design is extensible – a new account type needs one new class and one line in the factory.

## 16. References
- Stroustrup, B. *The C++ Programming Language*, 4th ed. Addison-Wesley.
- cppreference.com – C++ standard library documentation. https://en.cppreference.com
- Lecture notes, BAI22123 Object-Oriented Programming, Raffles University.
