#include <gtk/gtk.h>
#include <gmodule.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <stddef.h>
#include <json.h>


#define BUFFER_MSJ_SIZE 1024
#define MESSAGE_BUFFER 500
#define USERNAME_BUFFER 10

typedef struct {
    char* prompt;
    int socket;
} thread_data;

static char username[32];
static char user_id[100];
static char user_status[32];
static char local_IP[100]; 
static char actual_json[1000];  
static char * server_address;
static char server_reply[2000]; 
static char chating_user_id[100]; 

 

char servInfoIp[32];
char servInfoPort[32]; 

char getUsersResult[1000];

// message struct: this will hold messages provided by server
typedef struct {
  char *from;
  char *to;
  char *message;
  char *action;
} message;

typedef struct{
  char ip[32];
  char port[32];
} serverInfo;

typedef struct {
  char *id;
  char *name;
  char *status;
} user;

static user user_list[];
//Information of self
user my;
serverInfo servInfo;

typedef struct chat_client_ui {
  GtkWidget *window;
  GtkWidget *vbox;
  GtkWidget *vMainBox;
  GtkWidget *vFriendsBox;
  GtkWidget *hInputBox;
  GtkWidget *menubar;
  GtkWidget *fileMenu;
  GtkWidget *aboutMenu;
  GtkWidget *fileMi;
  GtkWidget *quitMi;
  GtkWidget *aboutMi;
  GtkWidget *helpMi;
  GtkWidget *sendBtn;
  GtkWidget *chatEntry;
  GtkWidget *statusCombo;
  GtkWidget *hBox;
  GtkWidget *friendsLabel;
  GtkWidget *messagesScrollWindow;
  GtkWidget *vChatBox;
  GtkWidget *friendsScrollWindow;
  GtkWidget *vFriendsBoxView;
  GtkWidget *hFriendInfoBox;
  GtkWidget *friendInfoBtn;
  GtkWidget *friendSendChatBtn;
  GtkWidget *msg;
  GtkWidget *chatMsg;
  GtkWidget *statusLbl;
  GtkWidget *infoHBox;
  GtkWidget *infoLabelHBox;
  GtkWidget *userEntry;
  GtkWidget *portEntry;
  GtkWidget *ipEntry;
  GtkWidget *sendInfoBtn;
  GtkWidget *connectionLbl;
  GtkWidget *renderUsersBtn;
  int totalMessages;
  int totalUsers;
  char buffer[32];
  char userId[32];
  char *currentStatus;
  char activeConver[32];
  message messages[500];
  user users[50];
  int i, j;
} ChatClient;

static pthread_mutex_t flock; 
static pthread_mutex_t unable_socket; 
static int sock; 


void showHelp(GtkWidget *widget, gpointer window) {
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new(window,
    GTK_DIALOG_DESTROY_WITH_PARENT,
    GTK_MESSAGE_INFO,
    GTK_BUTTONS_OK,
    "First add your username, the IP address for server and port in the upper inputs. \nSend messages using the bottom input and clicking on \"Send\" button.\nChange your status using the contiguous combobox.\nYou can send messages to active users by clicking on the buttons with their names on the right.");
  gtk_window_set_title(GTK_WINDOW(dialog), "Help");
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

//char * getHandshakeJson(char , char , char );

void initConnection(GtkWidget *button, gpointer data) {
  char username[32];
  char ip[32];
  char port[32];
  //
  // sprintf(username, "%s", gtk_entry_get_text(GTK_ENTRY(((ChatClient *)data)->userEntry)));
  // sprintf(ip, "%s", gtk_entry_get_text(GTK_ENTRY(((ChatClient *)data)->ipEntry)));
  // sprintf(port, "%s", gtk_entry_get_text(GTK_ENTRY(((ChatClient *)data)->portEntry)));
  //
  // sprintf(((ChatClient *)data)->userId, "2");
  //
  // g_print("%s, %s, %s\n", username, ip, port);

  char * json = getHandshakeJson(&username, &port, &ip);
  printf("%s", json);


  //user identification  = {.id = handshake,.name =  username,.status =  ""};
  //myself = identification;
}


// void changeStatus(GtkWidget *combo, gpointer data) {
//   char comboBuffer[32];
//   char labelBuffer[32];
//   sprintf(comboBuffer, "%s", gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo)));
//   sprintf(labelBuffer, "You are %s", gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo)));
//   gtk_label_set_text(GTK_LABEL(((ChatClient *)data)->statusLbl),labelBuffer);
//   // send status with comboBuffer
//   g_print("%s\n", comboBuffer);

//   //==============Creacion del JSON==================
//   //Json dentro del cambio de status
//   struct json_object *userStatus = json_object_new_object(),
//   *actionSon = json_object_new_string("CHANGED_STATUS"),
//   *userSon = json_object_new_string(my.id),
//   *statusSon = json_object_new_string(labelBuffer);

//   json_object_object_add(userStatus, "action", actionSon);
//   json_object_object_add(userStatus, "user", userSon);
//   json_object_object_add(userStatus, "status", statusSon);
  
//   const char *statusChanged = json_object_to_json_string(userStatus);

//   //Get my IP

//  //---------------Envio de paquetes
//   int newPort = strtol(servInfoPort , NULL, 10);
//   int sock;
//   struct sockaddr_in server;
//   char message[1000] , server_reply[2000];

//   //Create socket
//   sock = socket(AF_INET , SOCK_STREAM , 0);
//   if (sock < 0)
//   {
//       printf("Could not create socket");
//   }
//   puts("Socket created");

//   server.sin_addr.s_addr = inet_addr(servInfoIp);
//   server.sin_family = AF_INET;
//   server.sin_port = htons( newPort );

//   //Connect to remote server
//   int sock_stat = connect(sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
  
//   if (sock_stat < 0) {
//     printf("Could not connect socket");
//     return NULL; 
//   }
//   //Send some data
//   if( send(sock , statusChanged , strlen(statusChanged) , 0) < 0)
//   {
//       puts("Send failed");
//   }
//   //Receive a reply from the server
//   if( recv(sock , server_reply , 2000 , 0) < 0)
//   {
//       puts("recv failed");
//   }
//   puts("\n-----------------------------------------------\nServer reply :");
//   puts(server_reply);
//   puts("\n-----------------------------------------------");

//   close(sock);
//   //=============================================================


// }

// here we need to send the message to server
void sendMessage(GtkWidget *button, gpointer data, struct serverInfo *info) {
  char entryBuffer[32];
  sprintf(entryBuffer, "%s", gtk_entry_get_text(GTK_ENTRY(((ChatClient *)data)->chatEntry)));
  sprintf(((ChatClient *)data)->buffer, "You: %s\n", gtk_entry_get_text(GTK_ENTRY(((ChatClient *)data)->chatEntry)));

  ((ChatClient *)data)->chatMsg = gtk_label_new(((ChatClient *)data)->buffer);
  gtk_misc_set_alignment(GTK_MISC(((ChatClient *)data)->chatMsg), 0.0, 0.5);
  gtk_box_pack_start(GTK_BOX(((ChatClient *)data)->vChatBox), ((ChatClient *)data)->chatMsg, FALSE, FALSE, 0);

  // send message with entryBuffer
  g_print("%s\n", entryBuffer);
  g_print("%s\n", ((ChatClient *)data)->buffer);

  gtk_widget_show_all(((ChatClient *)data)->window);

  //==============Creacion del JSON==================
  char destinatario[32];
  sprintf(destinatario, ((ChatClient *)data)->activeConver);

  //Funcion en desarrollo, obtencion de datos del usuario y servidor al que se va a conectar

  struct json_object *messageSent = json_object_new_object(),
  *actionSon = json_object_new_string("SEND_MESSAGE"),
  *fromSon = json_object_new_string(my.id),
  *toSon = json_object_new_string(destinatario),
  *messageSon = json_object_new_string(entryBuffer);  

  json_object_object_add(messageSent, "action", actionSon);
  json_object_object_add(messageSent, "from", fromSon);
  json_object_object_add(messageSent, "to", toSon);
  json_object_object_add(messageSent, "message", messageSon);

  const char *messageString = json_object_to_json_string(messageSent);
  puts(messageString);
  //Get my IP

  //==============Envio de Paquetes=====================
  int newPort = strtol(servInfoPort , NULL, 10);
  int sock;
  struct sockaddr_in server;
  char message[1000] , server_reply[2000];
  puts("despues de newport"); 

  //Create socket
  sock = socket(AF_INET , SOCK_STREAM , 0);
  if (sock == -1)
  {
      printf("Could not create socket");
  }
  puts("Socket created");

  printf("%s", servInfoIp); 
  printf("%s", servInfoPort); 

  server.sin_addr.s_addr = inet_addr(servInfoIp);
  server.sin_family = AF_INET;
  server.sin_port = htons( newPort );

  //Connect to remote server
  if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
  {
      perror("connect failed. Error");
      return 1;
  }
  puts("Connected for messages \n");
  //keep communicating with server
  //Send some data
  if( send(sock , messageString , strlen(messageString) , 0) < 0)
  {
      puts("Send failed");
  }
  //Receive a reply from the server
  if( recv(sock , server_reply , 2000 , 0) < 0)
  {
      puts("recv failed");
  }
  puts("\n-----------------------------------------------\nServer reply :");
  puts(server_reply);
  puts("\n-----------------------------------------------");

  close(sock);

  //=============================================================



}

// Here we need to make the call to the server to fill up messages list
void renderMessages(GtkWidget *widget, gpointer data){
  int i = ((ChatClient *)data)->i;
  char userIdBuffer[32];

  GList *children, *iter;
  children = gtk_container_get_children(GTK_CONTAINER(((ChatClient *)data)->vChatBox));
  for (iter = children; iter != NULL; iter = g_list_next(iter))
    gtk_widget_destroy(GTK_WIDGET(iter->data));
  g_list_free(children);

  // find out what is the id of the user sending the messages
  // sizeof(((ChatClient *)data)->users)/sizeof(((ChatClient *)data)->users[0])
  for (i = 0; i < ((ChatClient *)data)->totalUsers; i++) {
    if (strcmp(gtk_button_get_label(GTK_BUTTON(widget)), ((ChatClient *)data)->users[i].name) == 0) {
      sprintf(userIdBuffer, "%s", ((ChatClient *)data)->users[i].id);
      sprintf(((ChatClient *)data)->activeConver, ((ChatClient *)data)->users[i].id);
    }
  }

  // sizeof(((ChatClient *)data)->messages)/sizeof(((ChatClient *)data)->messages[0])
  for (i = 0; i < ((ChatClient *)data)->totalMessages; i++) {
    // gtk_button_get_label(GTK_BUTTON(widget))    ((ChatClient *)data)->messages[i].from
    if ((strcmp(userIdBuffer, ((ChatClient *)data)->messages[i].from) == 0)
              && (strcmp(((ChatClient *)data)->userId, ((ChatClient *)data)->messages[i].to) == 0)) {
        sprintf(((ChatClient *)data)->buffer, "%s: %s\n",
            gtk_button_get_label(GTK_BUTTON(widget)), ((ChatClient *)data)->messages[i].message);
        ((ChatClient *)data)->chatMsg = gtk_label_new(((ChatClient *)data)->buffer);
        gtk_misc_set_alignment(GTK_MISC(((ChatClient *)data)->chatMsg), 0.0, 0.5);
        gtk_box_pack_start(GTK_BOX(((ChatClient *)data)->vChatBox), ((ChatClient *)data)->chatMsg, FALSE, FALSE, 0);
        g_print("%s: %s\n", ((ChatClient *)data)->messages[i].from, ((ChatClient *)data)->messages[i].message);
        g_print("chatMsg %s", gtk_label_get_text(GTK_LABEL(((ChatClient *)data)->chatMsg)));
    }

    if ((strcmp(userIdBuffer, ((ChatClient *)data)->messages[i].to) == 0)
              && (strcmp(((ChatClient *)data)->userId, ((ChatClient *)data)->messages[i].from) == 0)) {
        sprintf(((ChatClient *)data)->buffer, "You: %s\n", ((ChatClient *)data)->messages[i].message);
        ((ChatClient *)data)->chatMsg = gtk_label_new(((ChatClient *)data)->buffer);
        gtk_misc_set_alignment(GTK_MISC(((ChatClient *)data)->chatMsg), 0.0, 0.5);
        gtk_box_pack_start(GTK_BOX(((ChatClient *)data)->vChatBox), ((ChatClient *)data)->chatMsg, FALSE, FALSE, 0);
        g_print("%s: %s\n", ((ChatClient *)data)->messages[i].from, ((ChatClient *)data)->messages[i].message);
        g_print("chatMsg %s", gtk_label_get_text(GTK_LABEL(((ChatClient *)data)->chatMsg)));
    }
  }

  gtk_widget_show_all(((ChatClient *)data)->window);
}

// void *fillUserList(){
//   struct json_object *requestUsers = json_object_new_object(),
//   *actionSon = json_object_new_string("LIST_USER"); 

//   json_object_object_add(requestUsers, "action", actionSon);
//   //Get my IP

//   puts("asdfasdfasdfasdf");
//   printf("%s\n", json_object_to_json_string(requestUsers));

//   const char *reqStr = json_object_to_json_string(requestUsers);

//   if( write(sockfd2 , reqStr , strlen(reqStr)) < 0)
//   {
//       puts("Send failed");
//   }

//   if( read(sockfd2 , server_reply , 1000) < 0)
//   {
//     puts("recv failed");
//   }
// }


void *renderUsers(gpointer data) {
  int i;
  for (i = 0; i < ((ChatClient *)data)->totalUsers; i++) {
    sprintf(((ChatClient *)data)->buffer, "User is: %s", ((ChatClient *)data)->users[i].status);
    ((ChatClient *)data)->msg = gtk_label_new(((ChatClient *)data)->buffer);
    gtk_misc_set_alignment(GTK_MISC(((ChatClient *)data)->msg), 0.0, 0.5);
    ((ChatClient *)data)->friendInfoBtn = gtk_button_new_with_label("View info");
    ((ChatClient *)data)->friendSendChatBtn = gtk_button_new_with_label(((ChatClient *)data)->users[i].name);
    ((ChatClient *)data)->hFriendInfoBox = gtk_hbox_new(TRUE, 0);

    g_signal_connect(GTK_OBJECT(((ChatClient *)data)->friendSendChatBtn), "clicked", GTK_SIGNAL_FUNC(renderMessages), data);

    gtk_box_pack_start(GTK_BOX(((ChatClient *)data)->hFriendInfoBox), ((ChatClient *)data)->friendSendChatBtn, TRUE, TRUE, 2);
    gtk_box_pack_start(GTK_BOX(((ChatClient *)data)->hFriendInfoBox), ((ChatClient *)data)->friendInfoBtn, TRUE, TRUE, 2);
    gtk_box_pack_start(GTK_BOX(((ChatClient *)data)->hFriendInfoBox), ((ChatClient *)data)->msg, TRUE, TRUE, 2);
    gtk_box_pack_start(GTK_BOX(((ChatClient *)data)->vFriendsBoxView), ((ChatClient *)data)->hFriendInfoBox, FALSE, FALSE, 5);
  }

  gtk_widget_show_all(((ChatClient *)data)->window);  
}

void renderUsersWithBtn(GtkWidget *button, gpointer data) {
  int i;
  for (i = 0; i < ((ChatClient *)data)->totalUsers; i++) {
    sprintf(((ChatClient *)data)->buffer, "User is: %s", ((ChatClient *)data)->users[i].status);
    ((ChatClient *)data)->msg = gtk_label_new(((ChatClient *)data)->buffer);
    gtk_misc_set_alignment(GTK_MISC(((ChatClient *)data)->msg), 0.0, 0.5);
    ((ChatClient *)data)->friendInfoBtn = gtk_button_new_with_label("View info");
    ((ChatClient *)data)->friendSendChatBtn = gtk_button_new_with_label(((ChatClient *)data)->users[i].name);
    ((ChatClient *)data)->hFriendInfoBox = gtk_hbox_new(TRUE, 0);

    g_signal_connect(GTK_OBJECT(((ChatClient *)data)->friendSendChatBtn), "clicked", GTK_SIGNAL_FUNC(renderMessages), data);

    gtk_box_pack_start(GTK_BOX(((ChatClient *)data)->hFriendInfoBox), ((ChatClient *)data)->friendSendChatBtn, TRUE, TRUE, 2);
    gtk_box_pack_start(GTK_BOX(((ChatClient *)data)->hFriendInfoBox), ((ChatClient *)data)->friendInfoBtn, TRUE, TRUE, 2);
    gtk_box_pack_start(GTK_BOX(((ChatClient *)data)->hFriendInfoBox), ((ChatClient *)data)->msg, TRUE, TRUE, 2);
    gtk_box_pack_start(GTK_BOX(((ChatClient *)data)->vFriendsBoxView), ((ChatClient *)data)->hFriendInfoBox, FALSE, FALSE, 5);
  }

  gtk_widget_show_all(((ChatClient *)data)->window);
}

// make connection to server here
void fetchUsers(gpointer data) {
  ((ChatClient *)data)->totalUsers = 2;
  user usr1, usr2;
  usr1.id = "0";
  usr1.name = "user_1";
  usr1.status = "active";
  ((ChatClient *)data)->users[0] = usr1;
  usr2.id = "1";
  usr2.name = "user_2";
  usr2.status = "busy";
  ((ChatClient *)data)->users[1] = usr2;
}

// make connection to server here
void fetchMessages(gpointer data) {
  ((ChatClient *)data)->totalMessages = 2;
  message msg1, msg2;
  msg1.from = "0";
  msg1.to = "2";
  msg1.message = "msg_1";
  ((ChatClient *)data)->messages[0] = msg1;
  msg2.from = "1";
  msg2.message = "msg_2";
  msg2.to = "0";
  ((ChatClient *)data)->messages[1] = msg2;
}

//============================================================
//============================================================
//============================================================
//=======================dick=================================
//============================================================
//============================================================
//============================================================

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
      else if (strncmp(message, "1", 1) == 0) {
        //Listar usuarios
        printf("Listar Usuarios...\n");
        pthread_mutex_lock(&flock); 
        list_users(); 
        pthread_mutex_unlock(&flock); 
      }
      else if (strncmp(message, "2", 1) == 0) {
        //Enviar Mensaje
        printf("Enviar Mensaje...\n");
      }
      else if (strncmp(message, "3", 1) == 0) {
        //Cambiar Status
        char status; 
        printf("Cambiar Status...\n");
        pthread_mutex_lock(&flock); 
        printf("%s", "Status que desea: 1. Active, 2. Busy, 3. Inactive");
        fgets(status, USERNAME_BUFFER, 1);
        changeStatus(socket_fd, &address, status);
        pthread_mutex_unlock(&flock); 

      }
      else{
        puts("No se entendio el mensaje"); 
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
    while(1) {
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



void getHandshakeJson(int socket_fd, struct sockaddr_in *address){

//======================================================= 
  getIP(socket_fd, &address);
//=======================================================
  struct json_object *requestID = json_object_new_object();
  struct json_object *ipSon = json_object_new_string(local_IP);
  struct json_object *bufferSon = json_object_new_string(&server_address);
  struct json_object *userSon = json_object_new_string(&username);
//=======================================================
  json_object_object_add(requestID, "host", ipSon);
  json_object_object_add(requestID, "origin", bufferSon);
  json_object_object_add(requestID, "user", userSon);
  //Get my IP

  puts("asdfasdfasdfasdf");
  printf("%s\n", json_object_to_json_string(requestID));
  const char *reqStr = json_object_to_json_string(requestID);
  
  sprintf(actual_json, reqStr);
  printf("\nMy handshake json is: %s", actual_json);   
//=======================================================
//=====================envio de handshake================

int envio = write(socket_fd, reqStr, strlen(reqStr)); 
if (envio < 0){
  puts("Envio de Handshake Fallido");
  return NULL; 
}

int recibo = read(socket_fd, server_reply, 1000);
if (recibo < 0){
  puts("handshake del servidor inentendible");
  return NULL; 
}

//=======================================================

  struct json_object *status, *userinfo, *replyObj, *id, *user_name, *user_status;
  replyObj = json_tokener_parse(server_reply);
  json_object_object_get_ex(replyObj, "status", &status);
  json_object_object_get_ex(replyObj, "user", &userinfo);

  printf("%s\n", json_object_to_json_string(status));
  printf("%s\n", json_object_to_json_string(userinfo));

  json_object_object_get_ex(userinfo, "id", &id);
  json_object_object_get_ex(userinfo, "name", &user_name);
  json_object_object_get_ex(userinfo, "status", &user_status);

  printf("%s\n", json_object_to_json_string(id));
  printf("%s\n", json_object_to_json_string(user_name));
  printf("%s\n", json_object_to_json_string(user_status));


  puts("");
  puts("-----------------------------------------------");

  // close(sock);

  sprintf(user_id, json_object_to_json_string(id));
  sprintf(user_status, json_object_to_json_string(user_status)) ;
  // close(sockfd);
}
void getIP(int socket_fd, struct sockaddr_in *address){
  struct sockaddr_in name;
  socklen_t namelen = sizeof(name);
  socket_fd = getsockname(socket_fd, (struct sockaddr*) &name, &namelen);
  char buffer[100];
  const char* p = inet_ntop(AF_INET, &name.sin_addr, buffer, 100);
   
  sprintf(local_IP, p); 
  int length = strlen(p) + 1 ;
  local_IP[length] = '\0'; 
  printf("\nMy local ip is:");
  printf("%s", local_IP);   
  return; 
}

void changeStatus(int socket_fd, struct sockaddr_in *address) {
  //==============Creacion del JSON==================
  //Json dentro del cambio de status
  struct json_object *userStatus = json_object_new_object();
  struct json_object *actionSon = json_object_new_string("CHANGED_STATUS"); 
  struct json_object *userSon = json_object_new_string(user_id);
  struct json_object *statusSon = json_object_new_string(user_status);

  json_object_object_add(userStatus, "action", actionSon);
  json_object_object_add(userStatus, "user", userSon);
  json_object_object_add(userStatus, "status", statusSon);
  
  const char *statusChanged = json_object_to_json_string(userStatus);
  printf("%s", statusChanged); 

  int envio = write(socket_fd, statusChanged, strlen(statusChanged)); 
  if (envio < 0){
    puts("Envio de status Fallido");
    return NULL; 
  }

  int recibo = read(socket_fd, server_reply, 1000);
  if (recibo < 0){
    puts("Respuesta de status del servidor inentendible");
    return NULL; 
  }

  printf("\n%s\n", server_reply); 

}

void list_users(int socket_fd, struct sockaddr_in *address) {
  //==============Creacion del JSON==================
  //Json dentro del cambio de status
  struct json_object *userStatus = json_object_new_object();
  struct json_object *actionSon = json_object_new_string("LIST_USER");

  json_object_object_add(userStatus, "action", actionSon);
  
  const char *statusChanged = json_object_to_json_string(userStatus);
  printf("%s", statusChanged); 

  int envio = write(socket_fd, statusChanged, strlen(statusChanged)); 
  if (envio < 0){
    puts("Envio de status Fallido");
    return NULL; 
  }

  int recibo = read(socket_fd, server_reply, 1000);
  if (recibo < 0){
    puts("Respuesta de status del servidor inentendible");
    return NULL; 
  }

  pthread_mutex_lock(&unable_socket); 
  sprintf(actual_json, server_reply);
  printf("\nSending: %s", actual_json);
  pthread_mutex_unlock(&unable_socket);

}



void create_user_list(){ 

  struct json_object *list,  *action, *userlist, *contentList, *user, *id, *user_name, *user_status;
  userlist = json_tokener_parse(&list);
  json_object_object_get_ex(userlist, "action", &action);
  json_object_object_get_ex(userlist, "users", &contentList);

  char *users_string = json_object_get_string(contentList);

  int user_count = json_object_array_length(contentList);
  int i; 
  for (i = 0; i < user_count; ++i){
    user = json_object_array_get_idx(contentList, i); 
    json_object_object_get_ex(user, "id", id);
    json_object_object_get_ex(user, "name", user_name);
    json_object_object_get_ex(user, "status", user_status);

    char param1, param2, param3; 
    param1 = json_object_get_string(id); 
    param2 = json_object_get_string(user_name); 
    param3 = json_object_get_string(user_status);
  }
  
}

int main(int argc, char **argv) {
      long port = strtol(argv[2], NULL, 10);
    struct sockaddr_in address, cl_addr;
    int socket_fd, response;
    char prompt[USERNAME_BUFFER+4];
    pthread_t thread;

    // Check for required arguments
    if (argc < 3) {
        printf("Usage: client ip_address port_number\n");
        exit(1);
    }

    // Get user handle
    printf("Enter your user name: ");
    fgets(username, USERNAME_BUFFER, stdin);
    printf("%s", username);  
    username[strlen(username) - 1] = 0; // Remove newline char from end of string
    strcpy(prompt, username);
    strcat(prompt, "> ");

    server_address = argv[1];
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(server_address);
    address.sin_port = htons(port);
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    char status_proto = "inactive"; 
    connect_to_server(socket_fd, &address); 
    getHandshakeJson(socket_fd, &address); 


    // Create data struct for new thread
    thread_data data;
    data.prompt = prompt;
    data.socket = socket_fd;

    // menu << "=============== Menu Principal ==============="
    // "1. Listar Usuarios"
    // "2. Enviar Mensaje"
    // "3. Cambiar Status"
    // "/quit para terminar le chat"
    // "=============================================="

    // Create new thread to receive messages
    pthread_create(&thread, NULL, receive, (void *) &data);

    // Send message
    send_message(prompt, socket_fd, &address);

    // Close socket and kill thread
    close(socket_fd);
    pthread_exit(NULL);
    return 0;
}