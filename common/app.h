//
// Created by tjl on 3/25/24.
//

#ifndef TLS_APP_H
#define TLS_APP_H

#define SERVER_PORT 8000
#define CA_PORT     8080

enum verbosity {
    v_none = 0,
    v_medium = 1,
    v_high = 2
};

void app_exit(int status);

void register_app_exit(void(*on_exit)());

enum verbosity get_verbosity();

#endif //TLS_APP_H
