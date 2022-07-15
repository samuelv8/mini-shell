#include <stdio.h>

int main(int argc, char **argv) {
    if (argc == 1) {
        printf("Default print. No parameter provided.\n");
    } else {
        printf("Params are:\n");
        for (int i = 1; i < argc; i++) {
            printf("%s\n", argv[i]);
        }
    }
    return 0;
}