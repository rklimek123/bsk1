#include "officerfiles.h"

int print_file(int index, filecontent_t file) {

}

int print_files(filecontent_t* files, int files_count) {
    for (int i = 0; i < files_count; ++i) {
        print_file(i + 1, *(files + i));
        print("\n");
        print("--------------------------------------------------------------------------------\n");
        print("\n");
    }
}