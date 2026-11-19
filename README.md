# Banking System (C++17) – BAI22123 OOP Project

Console banking application with Admin and Customer roles, full CRUD, file persistence and OOP design.

## Folder structure
```
BankingSystem/
├── main.cpp            entry point
├── Makefile
├── include/            header files (.h)
├── src/                implementation files (.cpp)
├── data/               accounts.txt is created here on first run
├── docs/               report, user manual, test cases, UML (.puml), oral prep
└── tests/run_tests.sh  automated test runner (27 tests)
```

## Compile and run
```bash
make            # or: g++ -std=c++17 -Iinclude main.cpp src/*.cpp -o bank
./bank          # Windows: bank.exe
```
Admin password: `admin123`. Run `bash tests/run_tests.sh` to execute all test cases.
