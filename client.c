#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include <json.h>

#define MESSAGE_BUFFER 500
#define USERNAME_BUFFER 10

typedef struct {
    char* prompt;
    int socket;
} thread_data;

char username;
char local_IP; 
char actual_json;  

// Connect to server
void * connect_to_server(int socket_fd, struct sockaddr_in *address) {
    int response = connect(socket_fd, (struct sockaddr *) address, sizeof *address);
    if (response < 0) {
        fprintf(stderr, "connect() failed: %s\n", strerror(errno));
        exit(1);
    } else {
      printf("Connected\n");
    }
}

// Get message from stdin and send to server
void * send_message(char prompt[USERNAME_BUFFER+4], int socket_fd, struct sockaddr_in *address) {
  printf("%s", prompt);
  char message[MESSAGE_BUFFER];
  char final_message[MESSAGE_BUFFER+USERNAME_BUFFER+1];
  while (fgets(message, MESSAGE_BUFFER, stdin) != NULL) {
      memset(final_message,0,strlen(final_message)); // Clear final message buffer
      strcat(final_message, prompt);
      strcat(final_message, message);
      printf("\n%s", prompt);
      if (strncmp(message, "/quit", 5) == 0) {
        printf("Closing connection...\n");
        exit(0);
      }
      send(socket_fd, final_message, strlen(final_message)+1, 0);
  }
}

void * receive(void * threadData) {
    int socket_fd, response;
    char message[MESSAGE_BUFFER];
    thread_data* pData = (thread_data*)threadData;
    socket_fd = pData->socket;
    char* prompt = pData->prompt;
    memset(message, 0, MESSAGE_BUFFER); // Clear message buffer

    // Print received message
    while(true) {
        response = recvfrom(socket_fd, message, MESSAGE_BUFFER, 0, NULL, NULL);
        if (response == -1) {
          fprintf(stderr, "recv() failed: %s\n", strerror(errno));
          break;
        } else if (response == 0) {
              printf("\nPeer disconnected\n");
              break;
        } else {
              printf("\nServer> %s", message);
              printf("%s", prompt);
              fflush(stdout); // Make sure "User>" gets printed
          }
    }
}

void getHandshakeJson(int socket_fd, struct sockaddr_in *address, char *host){
  
  getIP(socket_fd, &address);

  struct json_object *requestID = json_object_new_object(),
  *ipSon = json_object_new_string(myip),
  *bufferSon = json_object_new_string(host),
  *userSon = json_object_new_string(username);

  json_object_object_add(requestID, "host", ipSon);
  json_object_object_add(requestID, "origin", bufferSon);
  json_object_object_add(requestID, "user", userSon);
  //Get my IP

  puts("asdfasdfasdfasdf");
  printf("%s\n", json_object_to_json_string(requestID));
  const char *reqStr = json_object_to_json_string(requestID);
  sprintf(actual_json, reqStr);
  puts("\nMy handshake json is: %s", actual_json);   

 
  // // printf("%s\n", server_reply);

  // struct json_object *status, *userinfo, *replyObj, *id, *user_name, *user_status;
  // replyObj = json_tokener_parse(server_reply);
  // json_object_object_get_ex(replyObj, "status", &status);
  // json_object_object_get_ex(replyObj, "user", &userinfo);

  // printf("%s\n", json_object_to_json_string(status));
  // printf("%s\n", json_object_to_json_string(userinfo));

  // json_object_object_get_ex(userinfo, "id", &id);
  // json_object_object_get_ex(userinfo, "name", &user_name);
  // json_object_object_get_ex(userinfo, "status", &user_status);

  // printf("%s\n", json_object_to_json_string(id));
  // printf("%s\n", json_object_to_json_string(user_name));
  // printf("%s\n", json_object_to_json_string(user_status));


  // puts("");
  // puts("-----------------------------------------------");

  // // close(sock);

  // //Separando las partes del Json
  // const char delimiter[] = ",";
  // char * running = strdup(server_reply);
  // char * token;

  // char * statusString = strsep(&running, delimiter);
  // char * userString = strsep(&running, delimiter);

  // my.id = json_object_to_json_string(id);
  // my.name = json_object_to_json_string(user_name);
  // my.status = json_object_to_json_string(user_status);
  // sprintf(servInfoIp, ip); 
  // sprintf(servInfoPort, port);
  // // close(sockfd);
}

void getIP(int socket_fd, struct sockaddr_in *address){
  struct sockaddr_in name;
  socklen_t namelen = sizeof(name);
  socket_fd = getsockname(socket_fd, (struct sockaddr*) &name, &namelen);
  char buffer[100];
  const char* p = inet_ntop(AF_INET, &name.sin_addr, buffer, 100);
  sprintf(local_IP, p); 
  puts("\nMy local ip is: %s", local_IP);  
}

int main(int argc, char**argv) {
    long port = strtol(argv[2], NULL, 10);
    struct sockaddr_in address, cl_addr;
    char * server_address;
    int socket_fd, response;
    char prompt[USERNAME_BUFFER+4];
    char username[USERNAME_BUFFER];
    pthread_t thread;

    // Check for required arguments
    if (argc < 3) {
        printf("Usage: client ip_address port_number\n");
        exit(1);
    }

    // Get user handle
    printf("Enter your user name: ");
    fgets(username, USERNAME_BUFFER, stdin);
    username[strlen(username) - 1] = 0; // Remove newline char from end of string
    strcpy(prompt, username);
    strcat(prompt, "> ");

    server_address = argv[1];
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(server_address);
    address.sin_port = htons(port);
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    connect_to_server(socket_fd, &address); 

    // Create data struct for new thread
    thread_data data;
    data.prompt = prompt;
    data.socket = socket_fd;

    // Create new thread to receive messages
    pthread_create(&thread, NULL, receive, (void *) &data);

    // Send message
    send_message(prompt, socket_fd, &address);

    // Close socket and kill thread
    close(socket_fd);
    pthread_exit(NULL);
    return 0;
}