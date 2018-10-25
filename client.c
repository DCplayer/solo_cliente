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
  int sock; 
  struct sockaddr_in serv;
  char server_reply[2000];
  ssize_t n;
} ChatClient;

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


void changeStatus(GtkWidget *combo, gpointer data) {
  char comboBuffer[32];
  char labelBuffer[32];
  sprintf(comboBuffer, "%s", gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo)));
  sprintf(labelBuffer, "You are %s", gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo)));
  gtk_label_set_text(GTK_LABEL(((ChatClient *)data)->statusLbl),labelBuffer);
  // send status with comboBuffer
  g_print("%s\n", comboBuffer);

  //==============Creacion del JSON==================
  //Json dentro del cambio de status
  struct json_object *userStatus = json_object_new_object(),
  *actionSon = json_object_new_string("CHANGED_STATUS"),
  *userSon = json_object_new_string(my.id),
  *statusSon = json_object_new_string(labelBuffer);

  json_object_object_add(userStatus, "action", actionSon);
  json_object_object_add(userStatus, "user", userSon);
  json_object_object_add(userStatus, "status", statusSon);
  
  const char *statusChanged = json_object_to_json_string(userStatus);

  //Get my IP

 //---------------Envio de paquetes
  int newPort = strtol(servInfoPort , NULL, 10);
  int sock;
  struct sockaddr_in server;
  char message[1000] , server_reply[2000];

  //Create socket
  sock = socket(AF_INET , SOCK_STREAM , 0);
  if (sock == -1)
  {
      printf("Could not create socket");
  }
  puts("Socket created");

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
  if( send(sock , statusChanged , strlen(statusChanged) , 0) < 0)
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

void getHandshakeJson(GtkWidget *button, gpointer data){
  char username[32];
  char ip[32];
  char port[32];
  int portCast;
  //10.156.52.139

  sprintf(username, "%s", gtk_entry_get_text(GTK_ENTRY(((ChatClient *)data)->userEntry)));
  sprintf(ip, "%s", gtk_entry_get_text(GTK_ENTRY(((ChatClient *)data)->ipEntry)));
  sprintf(port, "%s", gtk_entry_get_text(GTK_ENTRY(((ChatClient *)data)->portEntry)));

  portCast = strtol(port, NULL, 10);

  sprintf(((ChatClient *)data)->userId, "2");

  g_print("%s, %s, %s\n", username, ip, port);

//=========((ChatClient *)data)->variable

  ((ChatClient *)data)->sock = socket ( AF_INET, SOCK_DGRAM, 0);

  //Socket could not be created
  if(((ChatClient *)data)->sock < 0)
  {
      perror("Socket error");
  }

  memset( &serv, 0, sizeof(serv) );
  ((ChatClient *)data)->serv.sin_family = AF_INET;
  ((ChatClient *)data)->serv.s_addr = inet_addr( ip );
  ((ChatClient *)data)->serv.sin_port = htons( port );

  int err = connect( sock , (const struct sockaddr*) &serv , sizeof(serv) );

  struct sockaddr_in name;
  socklen_t namelen = sizeof(name);
  err = getsockname(sock, (struct sockaddr*) &name, &namelen);

  char buffer[100];
  const char* p = inet_ntop(AF_INET, &name.sin_addr, buffer, 100);

  if(p != NULL)
  {
      printf("Local ip is : %s \n" , buffer);
  }
  else
  {
      //Some error
      printf ("Error number : %d . Error message : %s \n" , errno , strerror(errno));
  }

  //=============================================================
  int sockfd2;

  struct sockaddr_in servaddr;

  sockfd2 = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd2 == -1) {
      perror("could not create socket");
  }

  printf("Created socket\n");


  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(ip);
  servaddr.sin_port = htons(portCast);

  connect(sockfd2, (struct sockaddr *)&servaddr, sizeof(servaddr));


  //=============================================================

  //Funcion en desarrollo, obtencion de datos del usuario y servidor al que se va a conectar

  struct json_object *requestID = json_object_new_object(),
  *ipSon = json_object_new_string(ip),
  *bufferSon = json_object_new_string(buffer),
  *userSon = json_object_new_string(username);

  json_object_object_add(requestID, "host", ipSon);
  json_object_object_add(requestID, "origin", bufferSon);
  json_object_object_add(requestID, "user", userSon);
  //Get my IP

  puts("asdfasdfasdfasdf");
  printf("%s\n", json_object_to_json_string(requestID));

  const char *reqStr = json_object_to_json_string(requestID);

  if( write(sockfd2 , reqStr , strlen(reqStr)) < 0)
  {
      puts("Send failed");
  }

  if( read(sockfd2 , ((ChatClient *)data)->server_reply , 1000) < 0)
  {
    puts("recv failed");
  }

  //Receive a reply from the server

  puts("\n-----------------------------------------------\nServer reply :");
  puts("");

  // printf("%s\n", server_reply);

  struct json_object *status, *userinfo, *replyObj, *id, *user_name, *user_status;
  replyObj = json_tokener_parse(((ChatClient *)data)->server_reply);
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

  //Separando las partes del Json
  const char delimiter[] = ",";
  char * running = strdup(((ChatClient *)data)->server_reply);
  char * token;

  char * statusString = strsep(&running, delimiter);
  char * userString = strsep(&running, delimiter);

  my.id = json_object_to_json_string(id);
  my.name = json_object_to_json_string(user_name);
  my.status = json_object_to_json_string(user_status);
  sprintf(servInfoIp, ip); 
  sprintf(servInfoPort, port);
  // close(sockfd);
}

int main(int argc, char *argv[]) {


  //Enviar string requestID para inicial :D
  //Se espera recibir mensaje que contenga string de json
  //convertirlo a un json object
  //iterarlo y buscar el id que envio el server :D

  //---------------------------
  ChatClient chat;
  int i = chat.i;
  int j = chat.j;

  // temporary users creation for testing
  fetchUsers(&chat);

  // temporary message creation for testing
  fetchMessages(&chat);

  gtk_init(&argc, &argv);

  // initial user status
  chat.currentStatus = "active";

  // main window
  chat.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(chat.window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size(GTK_WINDOW(chat.window), 600, 400);
  gtk_window_set_title(GTK_WINDOW(chat.window), "Chactter");

  // scrollable window for messages
  chat.messagesScrollWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (chat.messagesScrollWindow),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  gtk_widget_set_size_request(chat.messagesScrollWindow, 300, 250);

  // scrollable window for users in chatroom
  chat.friendsScrollWindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (chat.friendsScrollWindow),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
  gtk_widget_set_size_request(chat.friendsScrollWindow, 300, 300);

  // boxes
  chat.vbox = gtk_vbox_new(FALSE, 0);
  chat.vMainBox = gtk_vbox_new(FALSE, 0);
  chat.vChatBox = gtk_vbox_new(FALSE, 0);
  chat.vFriendsBox = gtk_vbox_new(FALSE, 0);
  chat.vFriendsBoxView = gtk_vbox_new(FALSE, 0);
  chat.hInputBox = gtk_hbox_new(FALSE, 0);
  chat.infoHBox = gtk_hbox_new(FALSE, 0);
  chat.infoLabelHBox = gtk_hbox_new(FALSE, 0);
  chat.hBox = gtk_hbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(chat.window), chat.vbox);

  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(chat.messagesScrollWindow), chat.vChatBox);
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(chat.friendsScrollWindow), chat.vFriendsBoxView);

  // top menu elements
  chat.menubar = gtk_menu_bar_new();
  chat.fileMenu = gtk_menu_new();
  chat.aboutMenu = gtk_menu_new();

  chat.fileMi = gtk_menu_item_new_with_label("File");
  chat.quitMi = gtk_menu_item_new_with_label("Quit");
  chat.aboutMi = gtk_menu_item_new_with_label("About");
  chat.helpMi = gtk_menu_item_new_with_label("Help");

  // GTK widgets
  chat.sendBtn = gtk_button_new_with_label("Send");
  g_signal_connect(GTK_OBJECT(chat.sendBtn), "clicked", GTK_SIGNAL_FUNC(sendMessage), &chat);

  chat.sendInfoBtn = gtk_button_new_with_label("Connect");
  g_signal_connect(GTK_OBJECT(chat.sendInfoBtn), "clicked", GTK_SIGNAL_FUNC(getHandshakeJson), &chat);

  chat.renderUsersBtn = gtk_button_new_with_label("Get Users");
  g_signal_connect(GTK_OBJECT(chat.renderUsersBtn), "clicked", GTK_SIGNAL_FUNC(renderUsersWithBtn), &chat);

  chat.chatEntry = gtk_entry_new();
  chat.userEntry = gtk_entry_new();
  chat.portEntry = gtk_entry_new();
  chat.ipEntry = gtk_entry_new();

  chat.friendsLabel = gtk_label_new("Connected people");
  chat.connectionLbl = gtk_label_new("Username, IP address, port");

  chat.statusLbl = gtk_label_new("You are active");

  chat.statusCombo = gtk_combo_box_new_text();
  gtk_combo_box_append_text(GTK_COMBO_BOX(chat.statusCombo), "Active");
  gtk_combo_box_append_text(GTK_COMBO_BOX(chat.statusCombo), "Busy");
  gtk_combo_box_append_text(GTK_COMBO_BOX(chat.statusCombo), "Inactive");

  g_signal_connect(G_OBJECT(chat.statusCombo), "changed", G_CALLBACK(changeStatus), &chat);
  // widget placement
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(chat.fileMi), chat.fileMenu);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(chat.aboutMi), chat.aboutMenu);
  gtk_menu_shell_append(GTK_MENU_SHELL(chat.fileMenu), chat.quitMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(chat.aboutMenu), chat.helpMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(chat.menubar), chat.fileMi);
  gtk_menu_shell_append(GTK_MENU_SHELL(chat.menubar), chat.aboutMi);
  gtk_box_pack_start(GTK_BOX(chat.vbox), chat.menubar, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(chat.vbox), chat.infoLabelHBox, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(chat.infoLabelHBox), chat.connectionLbl, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(chat.vbox), chat.infoHBox, FALSE, FALSE, 10);
  gtk_box_pack_start(GTK_BOX(chat.infoHBox), chat.userEntry, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(chat.infoHBox), chat.ipEntry, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(chat.infoHBox), chat.portEntry, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(chat.infoHBox), chat.sendInfoBtn, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(chat.infoHBox), chat.renderUsersBtn, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(chat.vbox), chat.hBox, FALSE, FALSE, 10);
  gtk_box_pack_start(GTK_BOX(chat.hBox), chat.vMainBox, FALSE, FALSE, 10);
  gtk_box_pack_start(GTK_BOX(chat.hBox), chat.vFriendsBox, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(chat.vMainBox), chat.messagesScrollWindow, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(chat.vMainBox), chat.statusLbl, TRUE, TRUE, 10);
  gtk_box_pack_start(GTK_BOX(chat.vMainBox), chat.hInputBox, FALSE, FALSE, 10);

  gtk_box_pack_start(GTK_BOX(chat.vFriendsBox), chat.friendsScrollWindow, TRUE, TRUE, 0);

  gtk_box_pack_start(GTK_BOX(chat.hInputBox), chat.chatEntry, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(chat.hInputBox), chat.sendBtn, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(chat.hInputBox), chat.statusCombo, TRUE, TRUE, 0);

  // render user list
  // sizeof(chat.users)/sizeof(chat.users[0])


  // render example
//  for (i = 0; i < 10; i++) {
//    for (j = 0; j < 10; j++){
//      sprintf(chat.buffer, "Message (%d,%d)\n", i, j);
//      chat.msg = gtk_label_new(chat.buffer);
//      gtk_box_pack_start(GTK_BOX(chat.vChatBox), chat.msg, FALSE, FALSE, 0);
//    }
//  }


  g_signal_connect(G_OBJECT(chat.window), "destroy",
        G_CALLBACK(gtk_main_quit), NULL);

  g_signal_connect(G_OBJECT(chat.quitMi), "activate",
        G_CALLBACK(gtk_main_quit), NULL);

  g_signal_connect(G_OBJECT(chat.helpMi), "activate", G_CALLBACK(showHelp), (gpointer) chat.window);

  gtk_widget_show_all(chat.window);

  void * testFunction (){
      while(1){
        renderUsers(&chat);
    }
  }

  pthread_t server_thread;
  pthread_create(&server_thread, NULL, renderUsers, &chat);
  gtk_main();

  return 0;
}