#include "folders.h"

void MakeDir() {
    time_t sec_time = time(NULL);
    struct tm* real_time = localtime(&sec_time);

    char real_data[25] = {};
    strftime(real_data, sizeof(real_data), "%d.%m.%Y__%H_%M_%S", real_time);

    char create_dir[100] = {};
    sprintf(create_dir, "mkdir %s", real_data);
    system(create_dir);

    chdir(real_data);
}