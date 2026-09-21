# Moving This Project From Firebase Emulator To Real Firebase

This note explains the current Firebase backend and the safe steps for moving it to the real Firebase project.

## 1. Current Setup

Your Firebase backend is inside `firebase-backend`.

- Firebase project: `banking-system-e346a`
- Firestore database: `(default)`
- Firestore location: `asia-southeast1`
- Functions source folder: `firebase-backend/functions`
- Local emulator ports:
  - Functions: `5001`
  - Firestore: `8080`
  - Emulator UI: `4000`

The function code is in `functions/index.js`. It creates one HTTP API called `api`.

Current routes:

- `GET /health`
  - Returns a simple status response.
- `POST /customers`
  - Creates a document in the Firestore `customers` collection.
  - Required fields: `fullName`, `phone`.
  - Optional fields: `email`, `address`.
- `GET /customers`
  - Reads all documents from the `customers` collection, ordered by `createdAt` newest first.

Current Firestore collection used by Firebase:

- `customers`

Current Firebase services used:

- Cloud Firestore
- Cloud Functions for Firebase

Services not currently used in the Firebase code:

- Firebase Authentication
- Firebase Storage
- Firebase Hosting
- Realtime Database

Important note: Firestore rules currently require `request.auth != null` for direct client reads/writes. Your Cloud Function uses the Firebase Admin SDK, so it bypasses Firestore security rules. That is normal for trusted backend code.

## 2. What Was Risky In The Emulator Setup

The old `serve` script started only the Functions emulator:

```powershell
firebase emulators:start --only functions
```

That means Firestore was not always running locally. If a function tries to use Firestore while the Firestore emulator is not running, it can try to reach production Firestore.

The safer local command is now:

```powershell
npm run serve
```

This starts both Functions and Firestore emulators.

If this command says Firestore port `8080` is already in use, close the app using that port or change the Firestore emulator port in `firebase.json`. On this machine, BlueStacks was using port `8080` during inspection.

## 3. What Must Be Deployed To Real Firebase

From the `firebase-backend/functions` folder:

```powershell
npm run deploy:firestore
```

This deploys:

- `firestore.rules`
- `firestore.indexes.json`

Then deploy the API:

```powershell
npm run deploy:functions
```

Or deploy both:

```powershell
npm run deploy
```

Firebase will print the real production URL for the HTTP function after deployment. Use that URL for testing and for your app/frontend.

## 4. Migration Options

There is no saved emulator export in this project folder right now. That means there is no local Firestore emulator data file to upload automatically.

Use one of these options:

### Option A: Recreate test data through the API

After deploying the function, call the production API:

```powershell
curl.exe -X POST "PASTE_DEPLOYED_FUNCTION_URL/customers" -H "Content-Type: application/json" -d "{\"fullName\":\"Test Customer\",\"phone\":\"0123456789\",\"email\":\"test@example.com\",\"address\":\"Kuala Lumpur\"}"
```

Then check:

```powershell
curl.exe "PASTE_DEPLOYED_FUNCTION_URL/customers"
```

If the customer appears, the deployed function is writing to real Firestore.

### Option B: Export emulator data before stopping the emulator

If you start the emulator and create data there, export it before closing the emulator:

```powershell
firebase emulators:export ./emulator-export --project banking-system-e346a
```

This is useful for keeping local emulator test data, but it is not the same as deploying to production. To move data into production, use the deployed API or a separate Admin SDK migration script.

### Option C: Migrate the C++ TSV data later

The C++ backend stores data in local files such as:

- `customers.tsv`
- `accounts.tsv`
- `transactions.tsv`
- `events.jsonl`

Those files are separate from Firestore. If you want all banking data in Firebase later, map them to Firestore collections such as:

- `customers`
- `accounts`
- `transactions`
- `events`

At the moment, only `customers` exists in the Firebase function code.

## 5. How To View Data In Firebase Console

1. Open the Firebase Console.
2. Select project `banking-system-e346a`.
3. Go to Firestore.
4. Open the Data tab.
5. Look for the `customers` collection.
6. Click a customer document to see fields such as:
   - `id`
   - `fullName`
   - `phone`
   - `email`
   - `address`
   - `createdAt`
   - `updatedAt`

## 6. Beginner Explanation For Presentation

The emulator is a local copy of Firebase that runs on my computer. It is useful for testing because it does not affect real cloud data.

The real Firebase project stores data in the cloud. Firestore stores data as collections and documents. In this project, `customers` is a collection. Each customer is one document inside that collection.

Cloud Functions is the backend API. The app sends requests to the function. The function checks the input, writes data to Firestore, and returns a response.

The switch from emulator to real Firebase means:

- Local test URL changes to the deployed production function URL.
- Firestore data is stored in the Firebase Console instead of only on the local computer.
- Rules and indexes should be deployed so production matches the local Firebase configuration.

## 7. Final Test Checklist

Before saying the migration is complete:

- Run the local emulator with `npm run serve`.
- Test `GET /health` locally.
- Test `POST /customers` locally.
- Deploy Firestore rules and indexes.
- Deploy Cloud Functions.
- Test production `GET /health`.
- Test production `POST /customers`.
- Confirm the new customer appears in Firebase Console.
- Test production `GET /customers`.
