#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "calculator.h"

#define SERVER_PORT 8765
#define REQUEST_CAPACITY 8192

static char index_path[MAX_PATH];

static int send_all(SOCKET client, const char *data, size_t length) {
    size_t sent = 0;
    while (sent < length) {
        int result = send(client, data + sent, (int)(length - sent), 0);
        if (result == SOCKET_ERROR || result == 0) return 0;
        sent += (size_t)result;
    }
    return 1;
}

static void send_response(SOCKET client, const char *status, const char *type,
                          const char *body, size_t body_length) {
    char header[512];
    int length = snprintf(header, sizeof(header),
        "HTTP/1.1 %s\r\nContent-Type: %s\r\nContent-Length: %zu\r\n"
        "Connection: close\r\nCache-Control: no-store\r\n\r\n",
        status, type, body_length);
    if (length > 0 && (size_t)length < sizeof(header)) {
        send_all(client, header, (size_t)length);
        send_all(client, body, body_length);
    }
}

static const char *find_header_end(const char *request) {
    return strstr(request, "\r\n\r\n");
}

static int request_content_length(const char *request) {
    const char *line = strstr(request, "\r\n");
    if (!line) return 0;
    line += 2;
    while (*line && !(line[0] == '\r' && line[1] == '\n')) {
        if (_strnicmp(line, "Content-Length:", 15) == 0) {
            return atoi(line + 15);
        }
        line = strstr(line, "\r\n");
        if (!line) break;
        line += 2;
    }
    return 0;
}

static int receive_request(SOCKET client, char *buffer, size_t capacity,
                           size_t *request_length) {
    size_t used = 0;
    buffer[0] = '\0';
    for (;;) {
        int received;
        const char *header_end;
        size_t header_length;
        int body_length;
        if (used >= capacity - 1) return 0;
        received = recv(client, buffer + used, (int)(capacity - used - 1), 0);
        if (received <= 0) return 0;
        used += (size_t)received;
        buffer[used] = '\0';
        header_end = find_header_end(buffer);
        if (!header_end) continue;
        header_length = (size_t)(header_end - buffer) + 4;
        body_length = request_content_length(buffer);
        if (body_length < 0 || (size_t)body_length > capacity - header_length - 1)
            return 0;
        if (used >= header_length + (size_t)body_length) {
            *request_length = used;
            return 1;
        }
    }
}

static int json_number(const char *body, const char *key, double *value) {
    char needle[32];
    const char *position;
    char *end;
    int count = snprintf(needle, sizeof(needle), "\"%s\"", key);
    if (count < 0 || (size_t)count >= sizeof(needle)) return 0;
    position = strstr(body, needle);
    if (!position) return 0;
    position = strchr(position + count, ':');
    if (!position) return 0;
    *value = strtod(position + 1, &end);
    return end != position + 1;
}

static int json_operator(const char *body, char *op) {
    const char *position = strstr(body, "\"op\"");
    if (!position || !(position = strchr(position, ':'))) return 0;
    position++;
    while (isspace((unsigned char)*position)) position++;
    if (*position++ != '"' || !*position || position[1] != '"') return 0;
    *op = *position;
    return 1;
}

static void handle_calculation(SOCKET client, const char *body) {
    static const char bad_request[] = "{\"ok\":false,\"error\":\"Invalid calculation request\"}";
    static const char division_error[] = "{\"ok\":false,\"error\":\"Cannot divide by zero\"}";
    static const char invalid_error[] = "{\"ok\":false,\"error\":\"Invalid operation or number\"}";
    char response[160];
    char op;
    double a, b, result;
    int code;

    if (!json_number(body, "a", &a) || !json_number(body, "b", &b) ||
        !json_operator(body, &op)) {
        send_response(client, "400 Bad Request", "application/json; charset=utf-8",
                      bad_request, sizeof(bad_request) - 1);
        return;
    }
    code = calculator_calculate(a, b, op, &result);
    if (code == CALCULATOR_DIVISION_BY_ZERO) {
        send_response(client, "400 Bad Request", "application/json; charset=utf-8",
                      division_error, sizeof(division_error) - 1);
        return;
    }
    if (code != CALCULATOR_OK) {
        send_response(client, "400 Bad Request", "application/json; charset=utf-8",
                      invalid_error, sizeof(invalid_error) - 1);
        return;
    }
    snprintf(response, sizeof(response), "{\"ok\":true,\"result\":%.15g}", result);
    send_response(client, "200 OK", "application/json; charset=utf-8",
                  response, strlen(response));
}

static void handle_index(SOCKET client) {
    FILE *file = fopen(index_path, "rb");
    long file_size;
    char *contents;
    if (!file) {
        static const char message[] = "Calculator UI file index.html was not found.";
        send_response(client, "500 Internal Server Error", "text/plain; charset=utf-8",
                      message, sizeof(message) - 1);
        return;
    }
    if (fseek(file, 0, SEEK_END) != 0 || (file_size = ftell(file)) < 0 ||
        fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        send_response(client, "500 Internal Server Error", "text/plain", "Unable to read UI.", 17);
        return;
    }
    contents = (char *)malloc((size_t)file_size);
    if (!contents || fread(contents, 1, (size_t)file_size, file) != (size_t)file_size) {
        free(contents);
        fclose(file);
        send_response(client, "500 Internal Server Error", "text/plain", "Unable to read UI.", 17);
        return;
    }
    fclose(file);
    send_response(client, "200 OK", "text/html; charset=utf-8", contents, (size_t)file_size);
    free(contents);
}

static void serve_client(SOCKET client) {
    char request[REQUEST_CAPACITY];
    char method[16], path[256];
    size_t request_length;
    const char *header_end;
    if (!receive_request(client, request, sizeof(request), &request_length) ||
        sscanf(request, "%15s %255s", method, path) != 2) {
        closesocket(client);
        return;
    }
    header_end = find_header_end(request);
    if (strcmp(method, "GET") == 0 && strcmp(path, "/") == 0) {
        handle_index(client);
    } else if (strcmp(method, "POST") == 0 && strcmp(path, "/calculate") == 0 && header_end) {
        handle_calculation(client, header_end + 4);
    } else {
        static const char not_found[] = "{\"ok\":false,\"error\":\"Not found\"}";
        send_response(client, "404 Not Found", "application/json; charset=utf-8",
                      not_found, sizeof(not_found) - 1);
    }
    (void)request_length;
    shutdown(client, SD_SEND);
    closesocket(client);
}

static int make_index_path(void) {
    DWORD length = GetModuleFileNameA(NULL, index_path, MAX_PATH);
    char *slash;
    if (length == 0 || length >= MAX_PATH) return 0;
    slash = strrchr(index_path, '\\');
    if (!slash) slash = strrchr(index_path, '/');
    if (!slash || (size_t)(slash - index_path) + sizeof("\\index.html") > MAX_PATH) return 0;
    strcpy(slash + 1, "index.html");
    return 1;
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE previous, LPSTR command_line, int show) {
    WSADATA wsa;
    SOCKET server;
    struct sockaddr_in address;
    char url[64] = "http://127.0.0.1:8765/";
    char edge_arguments[96];
    (void)instance; (void)previous; (void)command_line; (void)show;

    if (!make_index_path() || WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        MessageBoxA(NULL, "Unable to initialize the calculator.", "Calculator", MB_ICONERROR);
        return 1;
    }
    server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == INVALID_SOCKET) {
        WSACleanup();
        MessageBoxA(NULL, "Unable to create the local calculator server.", "Calculator", MB_ICONERROR);
        return 1;
    }
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = htons(SERVER_PORT);
    if (bind(server, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR ||
        listen(server, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(server);
        WSACleanup();
        MessageBoxA(NULL, "Port 8765 is already in use. Close the other app and try again.",
                    "Calculator", MB_ICONERROR);
        return 1;
    }

    snprintf(edge_arguments, sizeof(edge_arguments), "--app=%s", url);
    if ((INT_PTR)ShellExecuteA(NULL, "open", "msedge.exe", edge_arguments, NULL, SW_SHOWNORMAL) <= 32) {
        ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
    }

    for (;;) {
        SOCKET client = accept(server, NULL, NULL);
        if (client == INVALID_SOCKET) break;
        serve_client(client);
    }
    closesocket(server);
    WSACleanup();
    return 0;
}
