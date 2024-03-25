//
// Created by tjl on 3/25/24.
//
#define MAX_CALLBACKS 10

#include <app.h>
#include <stdio.h>
#include <stdlib.h>

static int exit_callbacks_size = 0;

static void (*app_exit_callbacks[MAX_CALLBACKS])() = {NULL};

void app_exit(int status) {
    for (int i = 0; i < exit_callbacks_size; ++i) {
        if (app_exit_callbacks[i] != NULL) {
            if (get_verbosity() > v_medium) {
                printf("Executing callback %d\n", i);
            }
            (*app_exit_callbacks[i])();
        }
    }
    if (get_verbosity() > v_medium) {
        printf("Exiting\n");
    }
    exit(status);
}

void register_app_exit(void(*on_exit)()) {
    if (exit_callbacks_size >= MAX_CALLBACKS) {
        printf("Can't register any more callbacks");
    }
    app_exit_callbacks[exit_callbacks_size] = on_exit;
    exit_callbacks_size++;
}

enum verbosity get_verbosity() {
    return v_medium;
}

