#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

// Functie pentru preluarea cookies din raspunsul serverului
char * get_cookies(char *msg) {
    char* cookies = (char*)calloc(LINELEN, sizeof(char));

    char *p = strstr(msg, "Set-Cookie");
    char *q = strstr(p, "; path");

    strcpy(cookies, "Cookie: ");
    memcpy(cookies + strlen("Cookie: "), p + strlen("Set-Cookie: "), q-p-strlen("Set-Cookie: "));
    strcat(cookies, "; ");

    p = strstr(q + 1, "Set-Cookie: ");
    q = strstr(p + 1, "; path");

    memcpy(cookies + strlen(cookies), p + strlen("Set-Cookie: "), q - p -  strlen("Set-Cookie: "));
    strcat(cookies, "\0");

    return cookies;
}

// Functie pentru preluarea ip-ului (DNS)
void get_ip(char* name, char* ip)
{
    int ret;
    struct addrinfo hints, *res;
    char buff[INET_ADDRSTRLEN];
    const char *result;

    memset(&hints, 0, sizeof(struct addrinfo));
    
    hints.ai_family = AF_INET;

    ret = getaddrinfo(name, NULL, &hints, &res);
    if (ret < 0) {
        exit(-1);
    }

    struct sockaddr_in *help = (struct sockaddr_in *) res->ai_addr;
    result = inet_ntop(res->ai_family, &(help->sin_addr), buff, res->ai_addrlen);
    
    strcpy(ip, result);

    freeaddrinfo(res);
}

// Rezolvarea task-urilor
int main(int argc, char *argv[])
{
    char *message;
    char *response;
    int sockfd;
    char url[100];
    char method[5];
    char type[100];
    char user_key[20];
    char pass_key[20];
    char username[50];
    char password[50];
    char *cookies;
    char token[800];
  

    // Task 1
    // Creare conexiune
    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    
    // Compunerea, trimiterea mesajului si primirea raspunsului
    message = compute_get_request(IP_SERVER, "/task1/start", NULL, NULL, NULL);
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    // Inchidere conexiune
    close_connection(sockfd);

    // Eliberare memorie
    free(message);

    // Task 2

    // Parsarea mesajului de la server
    JSON_Value* json = json_parse_string(strchr(response, '{'));
    JSON_Object* ob = json_value_get_object(json);

    strcpy(url, json_value_get_string(json_object_get_value_at(ob, 1)));
    strcpy(method, json_value_get_string(json_object_get_value_at(ob, 2)));
    strcpy(type, json_value_get_string(json_object_get_value_at(ob, 3)));

    JSON_Value* data = json_object_get_value_at(ob, 4);
    JSON_Object* ob_data = json_value_get_object(data);

    strcpy(user_key, json_object_get_name(ob_data, 0));
    strcpy(username, json_value_get_string(json_object_get_value_at(ob_data, 0)));

    strcpy(pass_key, json_object_get_name(ob_data, 1));
    strcpy(password, json_value_get_string(json_object_get_value_at(ob_data, 1)));

    // Preluare cookie
    cookies = get_cookies(response);
    free(response);
    json_value_free(json);

    // Compunere mesaj
    char form_data[200];
    strcpy(form_data, user_key);
    strcat(form_data, "=");
    strcat(form_data, username);
    strcat(form_data, "&");
    strcat(form_data, pass_key);
    strcat(form_data, "=");
    strcat(form_data, password);

    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    message = compute_post_request(IP_SERVER, url, form_data, type, cookies, NULL);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    close_connection(sockfd);
    free(cookies);
    free(message);


    // Task 3    
    char id_key[10];
    char id_value[100];
    char url_param[500];
    json = json_parse_string(strchr(response, '{'));
    ob = json_value_get_object(json);

    strcpy(url, json_value_get_string(json_object_get_value_at(ob, 1)));
    strcpy(method, json_value_get_string(json_object_get_value_at(ob, 2)));
    data = json_object_get_value_at(ob, 3);
    ob_data = json_value_get_object(data);

    strcpy(token, json_value_get_string(json_object_get_value_at(ob_data, 0)));
    JSON_Value* query_param = json_object_get_value_at(ob_data, 1);
    JSON_Object* ob_query_param = json_value_get_object(query_param);    

    strcpy(id_key, json_object_get_name(ob_query_param, 0));
    strcpy(id_value, json_value_get_string(json_object_get_value_at(ob_query_param, 0)));
    cookies = get_cookies(response);
    free(response);
    json_value_free(json);

    strcpy(url_param, "raspuns1=");
    strcat(url_param, "omul");
    strcat(url_param, "&");

    strcat(url_param, "raspuns2=");
    strcat(url_param, "numele");
    strcat(url_param, "&");

    strcat(url_param, id_key);
    strcat(url_param, "=");
    strcat(url_param, id_value);

    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(IP_SERVER, url, url_param, cookies, token);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    close_connection(sockfd);
    free(cookies);
    free(message);

    // Task 4
    json = json_parse_string(strchr(response, '{'));
    ob = json_value_get_object(json);
    strcpy(url, json_value_get_string(json_object_get_value_at(ob, 1)));
    strcpy(method, json_value_get_string(json_object_get_value_at(ob, 2)));

    cookies = get_cookies(response);
    free(response);
    json_value_free(json);

    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(IP_SERVER, url, NULL, cookies, token);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    close_connection(sockfd);
    free(cookies);
    free(message);


    // Task 5
    json = json_parse_string(strchr(response, '{'));
    ob = json_value_get_object(json);

    strcpy(url, json_value_get_string(json_object_get_value_at(ob, 1)));
    strcpy(method, json_value_get_string(json_object_get_value_at(ob, 2)));
    strcpy(type, json_value_get_string(json_object_get_value_at(ob, 3)));

    data = json_object_get_value_at(ob, 4);
    ob_data = json_value_get_object(data);
    char url_data[150];
    char q_key[3];
    char q_value[50];
    char APPID_key[10];
    char APPID_value[100];
    char method_data[5];
    strcpy(url_data, json_value_get_string(json_object_get_value_at(ob_data, 0)));
    query_param = json_object_get_value_at(ob_data, 1);
    ob_query_param = json_value_get_object(query_param);

    strcpy(q_key, json_object_get_name(ob_query_param, 0));
    strcpy(q_value, json_value_get_string(json_object_get_value_at(ob_query_param, 0)));
    strcpy(APPID_key, json_object_get_name(ob_query_param, 1));
    strcpy(APPID_value, json_value_get_string(json_object_get_value_at(ob_query_param, 1)));
    strcpy(method_data, json_value_get_string(json_object_get_value_at(ob_data, 2)));
    cookies = get_cookies(response);
    free(response);
    json_value_free(json);

    // Aflarea adresei ip
    char ip[20];
    char name[100];
    char *p = strchr(url_data, '/');
    memcpy(name, url_data, p - url_data);
    name[p - url_data] = '\0';
    get_ip(name, ip);

    strcpy(url_param, q_key);
    strcat(url_param, "=");
    strcat(url_param, q_value);
    strcat(url_param, "&");
    strcat(url_param, APPID_key);
    strcat(url_param, "=");
    strcat(url_param, APPID_value);

    // Cerere pentru aflarea vremii
    sockfd = open_connection(ip, 80, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(ip, p, url_param, NULL, NULL);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);

    close_connection(sockfd);
    free(message);
    
    // Extragere continut mesaj
    p = strchr(response, '{');

    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    message = compute_post_request(IP_SERVER, url, p, type, cookies, token);
    free(response);

    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    puts(response);

    close_connection(sockfd);
    free(cookies);
    free(response);
    free(message);

    
    return 0;
}