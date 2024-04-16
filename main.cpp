/*LICENSE? MIT >3*/
/*How to compile? >> g++ -std=c++11 main.cpp -I/usr/include -L/usr/lib -lpthread -o webserver*/
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <memory>

using namespace std;

// This is very, very raw and sucks project in C++, however beloved api lol
std::string api_version = "1.0.0-yuribeloved";

std::string exec(const char* cmd) {
    /*Credit to https://gist.github.com/meritozh/f0351894a2a4aa92871746bf45879157 >_<*/
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}

int main() {
    // Create a socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        cerr << "Error creating socket\n";
        return 1;
    }

    // Bind the socket to a port
    int port = 8080; // Example port
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY; 
    server_address.sin_port = htons(port);

    if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) < 0) {
        cerr << "Error binding socket\n";
        return 1;
    }

    // Start listening for connections
    listen(server_socket, 5); 

    cout << "Server listening on port " << port << endl;

    // Accept connections and handle requests
    while (true) {
        int client_socket = accept(server_socket, nullptr, nullptr);
        if (client_socket < 0) {
            cerr << "Error accepting connection\n";
            continue;
        }

        // Receive and parse HTTP request (simplified)
        char buffer[1024];
        recv(client_socket, buffer, sizeof(buffer), 0); 

        string request(buffer);
        istringstream iss(request);
        string method, path;
        iss >> method >> path; // Extract method and path

        // Handle request 
        if (method == "GET" && (path == "/" || path == "/index.html")) {
            // Load index.html
            ifstream file("index.html");
            string html_content;
            if (file.is_open()) {
                getline(file, html_content, '\0'); // Read entire file
                file.close();
            }

            // Send HTTP response
            string response = "HTTP/1.1 200 OK\r\n"
                              "Content-Type: text/html\r\n"
                              "Content-Length: " + to_string(html_content.size()) + "\r\n"
                              "\r\n" + html_content;
            send(client_socket, response.c_str(), response.size(), 0);
        } 
        else if (method == "GET" && path == "/api/yuriwallpaper") {
            // random while 001 to 160
            int random = rand() % 160 + 1;
            // return with json like {"url": "https://noidea.deeka.me/cdn/yuriwallpaper/001.jpg"}
            string string_random;
            if (to_string(random).length() == 1) {
                string_random = "00" + to_string(random);
            } else if (to_string(random).length() == 2) {
                string_random = "0" + to_string(random);
            }
            else {
                string_random = to_string(random);
            }
            string json_response = "{\"url\": \"https://noidea.deeka.me/cdn/yuriwallpaper/" + string_random + ".jpg\"}";
            string response = "HTTP/1.1 200 OK\r\n"
                              "Content-Type: application/json\r\n"
                              "Content-Length: " + to_string(json_response.size()) + "\r\n"
                              "\r\n" + json_response;
            send(client_socket, response.c_str(), response.size(), 0);
        }
        // For debug my server, if you don't like or scared, just removed :P
        else if (method == "GET" && path == "/api/sysinfo") {
            string kernel_info = exec("uname -a");
            string json_response = "{\"kernel_info\": \"" + kernel_info + "\", \"api_version\": \"" + api_version + "\"}";
            string response = "HTTP/1.1 200 OK\r\n"
                              "Content-Type: application/json\r\n"
                              "Content-Length: " + to_string(json_response.size()) + "\r\n"
                              "\r\n" + json_response;
            send(client_socket, response.c_str(), response.size(), 0);
        }
        else {
            // Send 404 Not Found
            string response = "HTTP/1.1 404 Not Found\r\n"
                              "Content-Length: 0\r\n"
                              "\r\n";
            send(client_socket, response.c_str(), response.size(), 0);
        }

        close(client_socket); 
    }

    close(server_socket);
    return 0;
}
