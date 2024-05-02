/* Copyright? MIT >3 */
/* How to compile? >> g++ -std=c++11 main.cpp -I/usr/include -L/usr/lib -lpthread -o webserver */
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <memory>

using namespace std;

// Some montherfucker in discord want to attack my stuff, let him 1v1 with C++ then.
std::string api_version = "1.1.0-attackerkys";

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
    // Loading index.txt because attack should cry
    ifstream file("index.txt");
    string txt_content;
    printf("Loading index.txt\n");
    // show index.txt content
    if (file.is_open()) {
        getline(file, txt_content, '\0'); // Read entire file
        file.close();
        cout << txt_content << endl;
    }
    else {
        cout << "Failed to load index.txt\n";
    }
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
        
        if (method == "GET" && path == "/api/yuriwallpaper") {
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
        // cdn/yuriwallpaper/*.jpg
        else if (method == "GET" && path.find("/cdn/yuriwallpaper/") == 0) {
            /* If attacker know, u need 1TiB of ram then */
            string file_path = path.substr(1); // Remove leading slash
            ifstream file(file_path, ios::binary);
            string content;
            // Send index.txt file
                if (file.is_open()) {
                    ostringstream datajpeg;
                    datajpeg << file.rdbuf();
                    // debug how much bytes loaded
                    cout << "Loaded " << datajpeg.str().size() << " bytes\n";
                    // send to client with jpeg content type
                    string response = "HTTP/1.1 200 OK\r\n"
                                      "Content-Type: image/jpeg\r\n"
                                      "Content-Length: " + to_string(datajpeg.str().size()) + "\r\n"
                                      "\r\n" + datajpeg.str();
                    send(client_socket, response.c_str(), response.size(), 0);
                    // Delete image from memory
                    datajpeg.str("");
                }
                else {
                    string response = "HTTP/1.1 404 Not Found\r\n"
                                      "Content-Type: text/plain\r\n"
                                      "Content-Length: 9\r\n"
                                      "\r\nNot found";
                    send(client_socket, response.c_str(), response.size(), 0);
                }
        }
        else {
                string response = "HTTP/1.1 200 OK\r\n"
                                  "Content-Type: text/plain\r\n"
                                  "Content-Length: " + to_string(txt_content.size()) + "\r\n"
                                  "\r\n" + txt_content;
                send(client_socket, response.c_str(), response.size(), 0);
        } 

        close(client_socket); 
    }

    close(server_socket);
    return 0;
}
