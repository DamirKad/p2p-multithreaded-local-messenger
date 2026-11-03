#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

void receiving(SOCKET client) {
    while (true) {
        char buffer[2048];
        int res = recv(client, buffer, sizeof(buffer) - 1, 0);
        if (res <= 0) {
            cout << "\n[-] Connection lost." << endl;
            break;
        }
        if (res > 0) {
            buffer[res] = '\0';
            cout << endl;
            cout << ">> ";
            for (int i = 0; i < res; i++) {
                cout << buffer[i];
            }
            cout << endl;
            cout << "<< ";
        }
    }
  
}
void sending(SOCKET client) {
    while (true) {
        string message;
        cout << "<< ";
        getline(cin, message);

        if (message == "!exit") {
            break;
        }
        send(client, message.c_str(), message.size(), 0);
    }
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    cout << "Choose your role: " << endl;
    cout << "1.HOST" << endl;
    cout << "2.CLIENT" << endl;

    int choice;
    cin >> choice;
    cin.ignore(32767, '\n');
    sockaddr_in sock;
    sock.sin_family = AF_INET;
    sock.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &sock.sin_addr);
    SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (choice == 1) {
        cout << "PEER::HOST" << endl;

        bind(server, (sockaddr*)&sock, sizeof(sock));
        listen(server, SOMAXCONN);
        SOCKET client = accept(server, 0, 0);

        if (client == INVALID_SOCKET) {
            cout << WSAGetLastError() << endl;
            WSACleanup();
            return 0;
        }
        cout << "PEER connected...";

        thread t1(receiving, client);
        thread t2(sending, client);

        t1.join();
        t2.join();

        closesocket(server);
        closesocket(client);
        WSACleanup();

    }
    else if(choice == 2) {
        cout << "PEER::CLIENT" << endl;

        if (connect(server, (sockaddr*)&sock, sizeof(sock)) != SOCKET_ERROR) {
            cout << "PEER connected...";

            thread t1(receiving, server);
            thread t2(sending, server);

            t1.join();
            t2.join();

            closesocket(server);
            WSACleanup();
            return 0;
        }
    }
    else {
        WSACleanup();
        return 0;
    }
}