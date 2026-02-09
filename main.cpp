#include "test_task.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "1 argument required." << std::endl;
        return 0;
    }
    Computer_club club;
    club.Work(argv[1]);
    return 0;
}