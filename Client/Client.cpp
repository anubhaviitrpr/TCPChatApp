#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <string>
#include <stdexcept>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

// Constants
const int BUFFER_SIZE = 4096;
const string DEFAULT_SERVER = "127.0.0.1";
const int DEFAULT_PORT = 12345;

// Initializes the Winsock library
bool InitializeWinsock() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "Failed to initialize Winsock. Error code: " << result << endl;
        return false;
    }
    return true;
}

// Creates and connects a socket to the server
SOCKET ConnectToServer(const string& serverAddress, int port) {
    // Create socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        throw runtime_error("Socket creation failed");
    }

    // Set up server address
    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, serverAddress.c_str(), &serverAddr.sin_addr) != 1) {
        closesocket(clientSocket);
        throw runtime_error("Invalid server address");
    }

    // Connect to server
    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(clientSocket);
        throw runtime_error("Failed to connect to server");
    }

    return clientSocket;
}

// Handles sending messages to the server
void SendMessages(SOCKET clientSocket) {
    // Get user's chat name
    cout << "Enter your chat name: ";
    string name;
    getline(cin, name);
    
    // Main message loop
    string message;
    while (true) {
        getline(cin, message);
        
        // Format message with username
        string formattedMsg = name + ": " + message;
        
        // Send message to server
        if (send(clientSocket, formattedMsg.c_str(), formattedMsg.length(), 0) == SOCKET_ERROR) {
            cout << "Error: Failed to send message" << endl;
            break;
        }

        // Check for quit command
        if (message == "quit") {
            cout << "Goodbye! Closing connection..." << endl;
            break;
        }
    }

    closesocket(clientSocket);
}

// Handles receiving messages from the server
void ReceiveMessages(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE];
    
    while (true) {
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
        
        if (bytesReceived <= 0) {
            cout << "Disconnected from server" << endl;
            break;
        }

        // Convert received bytes to string and display
        string message(buffer, bytesReceived);
        cout << message << endl;
    }

    closesocket(clientSocket);
}

int main() {
    cout << "=== Chat Client ===" << endl;

    try {
        // Initialize Winsock
        if (!InitializeWinsock()) {
            return 1;
        }

        // Connect to server
        SOCKET clientSocket = ConnectToServer(DEFAULT_SERVER, DEFAULT_PORT);
        cout << "Successfully connected to server!" << endl;

        // Create sender and receiver threads
        thread sender(SendMessages, clientSocket);
        thread receiver(ReceiveMessages, clientSocket);

        // Wait for threads to finish
        sender.join();
        receiver.join();

    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
        return 1;
    }

    // Cleanup
    WSACleanup();
    return 0;
}