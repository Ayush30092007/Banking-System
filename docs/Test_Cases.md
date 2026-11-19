# Test Cases

Run automatically with `bash tests/run_tests.sh` (all 27 executed on the final build).
Setup used in most tests: Account 1001 = Savings "Ali Abu" (PIN 1234, RM 500); Account 1002 = Current "Siti Aminah" (PIN 4321, RM 100).

| Test ID | Description | Input | Expected Result | Actual Result | Status |
|---|---|---|---|---|---|
| TC01 | Create savings account | Admin > Create > Savings, valid details, RM 500 | "Account number: 1001" shown | As expected | PASS |
| TC02 | Duplicate IC + same type | Create the same Savings account twice | "already has an account" error | As expected | PASS |
| TC03 | Invalid menu input | Main menu: `abc` | "Invalid input" and re-prompt | As expected | PASS |
| TC04 | Wrong admin password | Wrong password x3 | "Too many failed attempts" | As expected | PASS |
| TC05 | Opening deposit too low | Opening deposit `5` | "at least RM 10.00" error | As expected | PASS |
| TC06 | Invalid IC | IC `123` | "IC must contain 12 digits", re-prompt | As expected | PASS |
| TC07 | View all (sorted) | Admin > View all > sort by balance | Table lists both accounts | As expected | PASS |
| TC08 | Search by name | Keyword `ALI` | "Ali Abu" listed (case-insensitive) | As expected | PASS |
| TC09 | Search – no match | Keyword `zzz` | "No matching accounts" | As expected | PASS |
| TC10 | Search – not found | Account no. `9999` | "was not found" | As expected | PASS |
| TC11 | Update holder | New name + phone for 1001 | "Account updated" | As expected | PASS |
| TC12 | Delete account | Close 1001, confirm `y` | "Cash returned to customer: RM 500.00" | As expected | PASS |
| TC13 | Customer wrong PIN | Wrong PIN x3 | "Too many failed attempts" | As expected | PASS |
| TC14 | Deposit | Deposit `200` | "New balance: RM 700.00" | As expected | PASS |
| TC15 | Savings min balance | Withdraw `495` from RM 500 | "Insufficient funds" | As expected | PASS |
| TC16 | Zero amount | Withdraw `0` | "Amount must be greater than zero" | As expected | PASS |
| TC17 | Negative / text amount | Deposit `-5`, then `abc` | "Invalid input", re-prompt | As expected | PASS |
| TC18 | Transfer | 1001 → 1002, `100` | "Transfer successful. New balance: RM 400.00" | As expected | PASS |
| TC19 | Transfer to self | 1001 → 1001 | "same account" error | As expected | PASS |
| TC20 | Transfer – no recipient | 1001 → 7777 | "was not found" | As expected | PASS |
| TC21 | Overdraft limit | Current acc: withdraw 1000, then 200 | "up to RM 100.00" error | As expected | PASS |
| TC22 | Account report | Admin > Account report > 1001 | Shows "Initial deposit" | As expected | PASS |
| TC23 | Change PIN | Customer > Change PIN 1234 → 9999 | "PIN changed successfully" | As expected | PASS |
| TC24 | Report, unknown account | Admin > Account report > 9999 | "was not found" | As expected | PASS |
| TC25 | Close overdrawn account | Overdraw 1002, then delete | "Cannot close an overdrawn account" | As expected | PASS |
| TC26 | File persistence | Create account, exit, restart | "Loaded 1 account(s)" | As expected | PASS |
| TC27 | Corrupted data file | Invalid lines in data file | "Data file is corrupted" warning, no crash | As expected | PASS |
