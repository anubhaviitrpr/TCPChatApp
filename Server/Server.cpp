#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <vector>
#include <algorithm>
#include <mutex>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

// Constants
const int BUFFER_SIZE = 4096;
const int DEFAULT_PORT = 12345;
const string SERVER_IP = "0.0.0.0";  // Listen on all available interfaces

// Global variables
vector<SOCKET> clients;  // List of connected clients
mutex clientsMutex;      // Mutex for thread-safe client list operations

// Initialize Winsock library
bool InitializeWinsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "Failed to initialize Winsock. Error code: " << result << endl;
        return false;
    }
    return true;
}

// Broadcasts a message to all clients except the sender
void BroadcastMessage(const string& message, SOCKET sender) {
    lock_guard<mutex> lock(clientsMutex);
    for (const auto& client : clients) {
        if (client != sender) {
            send(client, message.c_str(), message.length(), 0);
        }
    }
}

// Removes a client from the clients list
void RemoveClient(SOCKET clientSocket) {
    lock_guard<mutex> lock(clientsMutex);
    auto it = find(clients.begin(), clients.end(), clientSocket);
    if (it != clients.end()) {
        clients.erase(it);
        cout << "Client disconnected. Total clients: " << clients.size() << endl;
    }
}

// Handles communication with a connected client
void HandleClient(SOCKET clientSocket) {
    cout << "New client connected. Total clients: " << clients.size() << endl;

    char buffer[BUFFER_SIZE];
    
    while (true) {
        // Receive message from client
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        
        if (bytesReceived <= 0) {
            cout << "Client disconnected" << endl;
            break;
        }

        // Convert received bytes to string
        string message(buffer, bytesReceived);
        cout << "Received: " << message << endl;

        // Broadcast message to other clients
        BroadcastMessage(message, clientSocket);
    }

    // Cleanup when client disconnects
    RemoveClient(clientSocket);
    closesocket(clientSocket);
}

// Creates and sets up the server socket
SOCKET CreateServerSocket(int port) {
    // Create socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        throw runtime_error("Failed to create socket");
    }

    // Set up server address
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (InetPton(AF_INET, TEXT(SERVER_IP.c_str()), &serverAddr.sin_addr) != 1) {
        closesocket(serverSocket);
        throw runtime_error("Invalid server address");
    }

    // Bind socket
    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(serverSocket);
        throw runtime_error("Failed to bind socket");
    }

    // Start listening
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(serverSocket);
        throw runtime_error("Failed to start listening");
    }

    return serverSocket;
}

int main() {
    cout << "=== Chat Server ===" << endl;

    try {
        // Initialize Winsock
        if (!InitializeWinsock()) {
            return 1;
        }

        // Create and set up server socket
        SOCKET serverSocket = CreateServerSocket(DEFAULT_PORT);
        cout << "Server is listening on port " << DEFAULT_PORT << endl;

        // Main server loop
        while (true) {
            // Accept new client connection
            SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
            if (clientSocket == INVALID_SOCKET) {
                cout << "Warning: Failed to accept client connection" << endl;
                continue;
            }

            // Add client to list
            {
                lock_guard<mutex> lock(clientsMutex);
                clients.push_back(clientSocket);
            }

            // Create thread to handle client
            thread clientThread(HandleClient, clientSocket);
            clientThread.detach();
        }

    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
        return 1;
    }

    // Cleanup
    WSACleanup();
    return 0;
}