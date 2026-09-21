const { onRequest } = require("firebase-functions/v2/https");
const { setGlobalOptions } = require("firebase-functions/v2");
const admin = require("firebase-admin");
const express = require("express");
const cors = require("cors");

setGlobalOptions({ region: "us-central1" });

admin.initializeApp();

const db = admin.firestore();
const app = express();

app.use(cors({ origin: true }));
app.use(express.json());
// created a helper functions for repeated backend logic , 
// it helps like converting money into cents , validating positive amounts , checking account types and generateing account number.
function nowIso() {
return new Date().toISOString();
}

function toCents(value) {
  const numberValue = Number(value);
  if (!Number.isFinite(numberValue)) {
    throw new Error ("Amount must be a valid number");
  }
  return Math.round(numberValue * 100);
}
function requirePositiveAmount(amountCents){
  if (!Number.isInteger (amountCents) || amountCents <= 0) {
    throw new Error("Amount must be greater than zero");
  }
}

function normalizeAccountType (type) {
  const value = String(type || "savings").toLowerCase();
 if (value !== "savings" && value !== "current") {
    throw new Error ("Account type must be savings or current");
  }
  return value;
}

function generateAccountNumber() {
  return String(Date.now()).slice(-10);
  
}

app.get("/health", (req, res) => {
  res.json({
    status: "ok",
    service: "banking-backend",
  });
});

app.post("/customers", async (req, res) => {
  try {
    const fullName = String(req.body.fullName || "").trim();
    const phone = String(req.body.phone || "").trim();
    const email = String(req.body.email || "").trim();
    const address = String(req.body.address || "").trim();

    if (!fullName) {
      return res.status(400).json({ error: "fullName is required" });
    }

    if (!phone) {
      return res.status(400).json({ error: "phone is required" });
    }

    const customerRef = db.collection("customers").doc();

    const customer = {
      id: customerRef.id,
      fullName,
      phone,
      email,
      address,
      createdAt: new Date().toISOString(),
      updatedAt: new Date().toISOString(),
    };

    await customerRef.set(customer);

    res.status(201).json(customer);
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

app.get("/customers", async (req, res) => {
  try {
    const snapshot = await db
      .collection("customers")
      .orderBy("createdAt", "desc")
      .get();

    const customers = snapshot.docs.map((doc) => doc.data());

    res.json(customers);
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

exports.api = onRequest(app);