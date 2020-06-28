#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <signal.h>
#include <time.h>
#include "cJSON.h"
#include <stdarg.h>
#include <pthread.h>

int sockfd = 0;
int reinit = 0;
//char server_ip[] = "192.168.1.134";
char server_ip[] = "0.0.0.0";
//char server_ip[] = "10.0.105.10";
int server_port = 35001;
char recvBuff[1024];
char sendBuff[1024];
int counter = 0;
//pthread_mutex_t socket_lock;

void signal_handler(int signal) {
    printf("intHanlder(): signal %d\n", signal);
    close(sockfd);
    if (signal == 13) {
        reinit = 1;
    }

    if (signal == 2) {
        exit(signal);
    }

}

char * build_json(char * tag_id, char * event_time, char * image) {
    cJSON *object = cJSON_CreateObject();

    if (tag_id != NULL) {
        cJSON_AddStringToObject(object, "TAGID", tag_id);
    } else {
        cJSON_AddStringToObject(object, "TAGID", "");
    }

    if (event_time != NULL) {
        cJSON_AddStringToObject(object, "AT", event_time);
    }

    if (image != NULL) {
        cJSON_AddStringToObject(object, "IMAGE", image);
    }
    return cJSON_PrintUnformatted(object);

}

void socket_connect(void) {
    int ret = 0;
    struct sockaddr_in serv_addr;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, '0', sizeof (serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        printf("\n inet_pton error occured\n");
        exit(EXIT_FAILURE);
    }

    if (ret = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
        printf("Error : Connect Failed, Error code: %d\n", ret);
        exit(EXIT_FAILURE);
    } else {
        printf("Socket Connected\n");
    }

}

void get_formatted_time(char * time_tmp) {
    FILE *fp;
    fp = popen("date +\"%d/%m/%Y %H:%M:%S\"", "r");
    if (fp == NULL) {
        printf("cannot open process\n");
        exit(EXIT_FAILURE);
    } else {
        char time[512];
        //printf("Reading and comparing buffer\n");
        if (fgets(time, sizeof (time), fp) > 0) {
            char *pos;
            if ((pos = strchr(time, '\n')) != NULL)
                *pos = '\0';
            strcpy(time_tmp, time);
        } else {
            printf("fgets error\n");
        }
    }
}

#define data_offset 5
#define cmd_len 2

int prepare_data(char * data, char cmd, char * jsonString, int json_len) {

    memcpy(data + data_offset + cmd_len, jsonString, json_len);
    data[0] = (char) 0x01;
    data[1] = cmd_len + json_len;
    data[5] = cmd;
    data[6] = 0;
    int buff_len = data_offset + cmd_len + json_len;
    return buff_len;
}

#define ID_STARTUP 1
#define ID_DOOR_OPEN 10
#define ID_DOOR_CLOSE 11
#define ID_UNLOCK_FAIL 12
#define ID_LOCK_FAIL 13
#define ID_2FA_SUCCESS 14
#define ID_2FA_NOT_MATCH 15
#define ID_KEYPAD_3RD 16
#define ID_TAMPER 17
#define ID_TAMPER_OK 18
#define ID_UNAUTHORIZED 19
#define ID_OVERWRITE 20
#define ID_LOWBAT 21
#define ID_CAMERA_ERROR 22
#define ID_1FA_SUCCESS 23
#define ID_RESTARTED 24
#define ID_ROUTING 100
#define ID_ACCESS_ERR 253
#define ID_HEART_BEAT 254

void socket_send_data(char * data, int buff_len) {
    int ret = 0;
//    pthread_mutex_lock(&socket_lock);
    socket_connect();
    ret = write(sockfd, data, buff_len);
    if (ret < 0) {
        printf("Write Error - code: %d\n", ret);
    }
    close(sockfd);
//    pthread_mutex_unlock(&socket_lock);
}

void send_heartbeat_event(char cmd) {
    char data[7] = {0};
    data[0] = (char) 0x01;
    data[1] = cmd_len;
    data[5] = cmd;
    data[6] = 0;
    socket_send_data(data, 7);
}

void send_event(char *tag_id, char cmd, char * image) {
    char data[512] = {0};
    char event_time[512] = {0};
    char jsonString[512] = {0};

    get_formatted_time(event_time);
    int json_len = snprintf(jsonString, sizeof (jsonString), "%s", build_json(tag_id, event_time, image));
    int buff_len = prepare_data(data, cmd, jsonString, json_len);

    socket_send_data(data, buff_len);
}

void *socket_event_task(void*arg) {
    char s[] = "Hello World\n";
    socket_send_data(s, sizeof (s));
    while (1);
}

int main(int argc, char *argv[]) {
    signal(SIGINT, signal_handler);
    signal(SIGPIPE, signal_handler);

    pthread_t id[5];
    //    if (pthread_mutex_init(&socket_lock, NULL) != 0) {
    //        printf("\n mutex init has failed\n");
    //        return 1;
    //    }
    //    Receive data
    //    while ((n = read(sockfd, recvBuff, sizeof (recvBuff) - 1)) > 0) {
    //        recvBuff[n] = 0;
    //        //printf("n: %d\n", n);
    //        if (fputs(recvBuff, stdout) == EOF) {
    //            printf("\n Error : Fputs error\n");
    //        }
    //    }

//    pthread_create(&id[0], NULL, socket_event_task, NULL);
//    pthread_create(&id[1], NULL, socket_event_task, NULL);
//    pthread_create(&id[2], NULL, socket_event_task, NULL);
//    pthread_create(&id[3], NULL, socket_event_task, NULL);
//    pthread_create(&id[4], NULL, socket_event_task, NULL);


        send_event("0001", ID_DOOR_OPEN, NULL);
        send_event("0001", ID_DOOR_CLOSE, NULL);
        send_event("0001", ID_UNLOCK_FAIL, NULL);
        send_event("0001", ID_LOCK_FAIL, NULL);
        send_event("0001", ID_2FA_SUCCESS, NULL);
        send_event("0001", ID_2FA_NOT_MATCH, NULL);
        send_event("0001", ID_KEYPAD_3RD, NULL);
        send_event("0001", ID_TAMPER, NULL);
        send_event("0001", ID_TAMPER_OK, NULL);
        send_event("0001", ID_UNAUTHORIZED, NULL);
        send_event("0001", ID_OVERWRITE, NULL);
        send_event("0001", ID_LOWBAT, NULL);
        send_event("0001", ID_CAMERA_ERROR, NULL);
        send_event("0001", ID_1FA_SUCCESS, NULL);
        send_event("0001", ID_RESTARTED, NULL);
        send_event("0001", ID_ROUTING, NULL);
        send_event("0001", ID_ACCESS_ERR, NULL);
        send_heartbeat_event(ID_HEART_BEAT);
//    while (1);
    printf("End of Program\n");
//    pthread_mutex_destroy(&socket_lock);
    return 0;
}

