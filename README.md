# Banking System Backend

This is the first backend pass for a C++ banking system. It is designed as a small reusable backend library with a console app on top, so the UI/frontend can be added later without rewriting the business logic.

## What It Supports

- Customer creation and editing
- Savings/current account creation
- Deposits, withdrawals, and transfers
- Account statements
- Persistent data files under `data/`
- Live update events through:
  - in-process listeners via `BankService::addChangeListener`
  - append-only `data/events.jsonl`, which a future UI can watch or tail

Money is stored as integer cents to avoid floating-point rounding bugs.

## Build

```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
```

## Run The Console Backend

```powershell
.\build\banking_app.exe
```

You can pass a custom data directory:

```powershell
.\build\banking_app.exe demo-data
```

## Run Tests

```powershell
ctest --test-dir build --output-on-failure
```

## Data Files

The backend writes:

- `customers.tsv`
- `accounts.tsv`
- `transactions.tsv`
- `events.jsonl`

The `.tsv` files are the durable database for now. The `.jsonl` event file is the live-update feed we can connect to the frontend next.
