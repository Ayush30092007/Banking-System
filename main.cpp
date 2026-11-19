#include <iostream>
#include "Bank.h"
#include "Menu.h"

// Usage: ./bank [datafile]    (default: data/accounts.txt)
int main(int argc, char* argv[]) {
    std::string file = (argc > 1) ? argv[1] : "data/accounts.txt";
    try {
        Bank bank(file);
        Menu menu(bank);
        menu.run();
    } catch (const Util::InputClosedException&) {
        std::cout << "\nInput closed. Exiting.\n";
    } catch (const std::exception& e) {
        std::cout << "Fatal error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
