#!/bin/bash
# Automated test runner. Usage: from project root:  bash tests/run_tests.sh
# Each test feeds keyboard input to ./bank using a temporary data file.
BIN=./bank
PASS=0; FAIL=0
RESULTS=tests/results.txt; : > $RESULTS

ADM="1\nadmin123\n"
ALI="1\n1\nAli Abu\n010203-07-1234\n0123456789\n1234\n500\n"
SITI="1\n2\nSiti Aminah\n020304-07-5678\n0198765432\n4321\n100\n"
OUT_ADM="7\n3\n"
CUST1="2\n1001\n1234\n"
OUT_CUST="7\n3\n"

# t ID "description" "input" "expected text"
t() {
  local tmp; tmp=$(mktemp)
  rm -f "$tmp"
  local out; out=$(printf "$3" | $BIN "$tmp" 2>&1)
  if echo "$out" | grep -qF -- "$4"; then s=PASS; PASS=$((PASS+1)); else s=FAIL; FAIL=$((FAIL+1)); echo "$out" > "tests/fail_$1.log"; fi
  echo "$1|$2|$4|$s" >> $RESULTS
  printf "%-5s %-58s %s\n" "$1" "$2" "$s"
  rm -f "$tmp"
}

t TC01 "Create savings account"                 "${ADM}${ALI}${OUT_ADM}"  "Account number: 1001"
t TC02 "Duplicate IC + same type rejected"      "${ADM}${ALI}${ALI}${OUT_ADM}" "already has an account"
t TC03 "Invalid main menu input (letters)"      "abc\n3\n" "Invalid input"
t TC04 "Wrong admin password 3 times"           "1\nx\nx\nx\n3\n" "Too many failed attempts"
t TC05 "Opening deposit below RM 10"            "${ADM}1\n1\nAli Abu\n010203071234\n0123456789\n1234\n5\n${OUT_ADM}" "at least RM 10.00"
t TC28 "Phone with 11 digits rejected"           "${ADM}1\n1\nAli\n010203071234\n01234567890\n0123456789\n1234\n50\n${OUT_ADM}" "Phone must be 9-10 digits"
t TC06 "Invalid IC format rejected"             "${ADM}1\n1\nAli\n123\n010203071234\n0123456789\n1234\n50\n${OUT_ADM}" "IC must contain 12 digits"
t TC07 "View all accounts (sorted by balance)"  "${ADM}${ALI}${SITI}2\n2\n${OUT_ADM}" "Siti Aminah"
t TC08 "Search by name (partial, case-insens.)" "${ADM}${ALI}3\n2\nALI\n${OUT_ADM}" "Ali Abu"
t TC09 "Search name with no match"              "${ADM}${ALI}3\n2\nzzz\n${OUT_ADM}" "No matching accounts"
t TC10 "Search account number not found"        "${ADM}${ALI}3\n1\n9999\n${OUT_ADM}" "was not found"
t TC11 "Update holder name and phone"           "${ADM}${ALI}4\n1001\n1\nAli Updated\n0111111111\n${OUT_ADM}" "Account updated"
t TC12 "Delete account and pay out balance"     "${ADM}${ALI}5\n1001\ny\n${OUT_ADM}" "Cash returned to customer: RM 500.00"
t TC13 "Customer wrong PIN 3 times"             "2\n1001\n0000\n0000\n0000\n3\n" "Too many failed attempts"
t TC14 "Deposit RM 200"                         "${ADM}${ALI}7\n${CUST1}2\n200\n${OUT_CUST}" "New balance: RM 700.00"
t TC15 "Savings withdrawal below min balance"   "${ADM}${ALI}7\n${CUST1}3\n495\n${OUT_CUST}" "Insufficient funds"
t TC16 "Zero amount rejected"                   "${ADM}${ALI}7\n${CUST1}3\n0\n${OUT_CUST}" "Amount must be greater than zero"
t TC17 "Negative / text amount rejected"        "${ADM}${ALI}7\n${CUST1}2\n-5\nabc\n50\n${OUT_CUST}" "Invalid input"
t TC18 "Transfer RM 100 to another account"     "${ADM}${ALI}${SITI}7\n${CUST1}4\n1002\n100\n${OUT_CUST}" "Transfer successful. New balance: RM 400.00"
t TC19 "Transfer to same account blocked"       "${ADM}${ALI}7\n${CUST1}4\n1001\n10\n${OUT_CUST}" "same account"
t TC20 "Transfer to non-existent account"       "${ADM}${ALI}7\n${CUST1}4\n7777\n10\n${OUT_CUST}" "was not found"
t TC21 "Current account overdraft limit"        "${ADM}${SITI}7\n2\n1001\n4321\n3\n1000\n3\n200\n${OUT_CUST}" "up to RM 100.00"
t TC22 "Account report shows history"            "${ADM}${ALI}6\n1001\n${OUT_ADM}" "Initial deposit"
t TC23 "Change PIN (customer)"                  "${ADM}${ALI}7\n${CUST1}6\n1234\n9999\n${OUT_CUST}" "PIN changed successfully"
t TC24 "Account report: unknown account number"   "${ADM}${ALI}6\n9999\n${OUT_ADM}" "was not found"
t TC25 "Cannot close overdrawn account"         "${ADM}${SITI}7\n2\n1001\n4321\n3\n500\n7\n${ADM}5\n1001\ny\n${OUT_ADM}" "Cannot close an overdrawn account"

# --- file persistence tests (need two runs sharing one file) ---
tmp=$(mktemp); rm -f $tmp
printf "${ADM}${ALI}${OUT_ADM}" | $BIN $tmp >/dev/null 2>&1
out=$(printf "3\n" | $BIN $tmp 2>&1)
if echo "$out" | grep -qF "Loaded 1 account(s)"; then s=PASS; PASS=$((PASS+1)); else s=FAIL; FAIL=$((FAIL+1)); fi
echo "TC26|Data persists after restart (file load)|Loaded 1 account(s)|$s" >> $RESULTS
printf "%-5s %-58s %s\n" TC26 "Data persists after restart (file load)" $s

echo "NEXT|1001
ACC|Savings|garbage" > $tmp
out=$(printf "3\n" | $BIN $tmp 2>&1)
if echo "$out" | grep -qF "Data file is corrupted"; then s=PASS; PASS=$((PASS+1)); else s=FAIL; FAIL=$((FAIL+1)); fi
echo "TC27|Corrupted data file handled without crash|Data file is corrupted|$s" >> $RESULTS
printf "%-5s %-58s %s\n" TC27 "Corrupted data file handled without crash" $s
rm -f $tmp

echo; echo "PASSED: $PASS   FAILED: $FAIL"
