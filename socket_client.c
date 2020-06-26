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

int sockfd = 0;
int reinit = 0;
//char server_ip[] = "192.168.1.134";
//char server_ip[] = "0.0.0.0";
char server_ip[] = "10.0.105.10";
int server_port = 35001;
char recvBuff[1024];
char sendBuff[1024];

void intHandler(int signal) {
    printf("intHanlder(): signal %d\n", signal);
    close(sockfd);
    if (signal == 13) {
        reinit = 1;
    }

    if (signal == 2) {
        exit(signal);
    }

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

int main(int argc, char *argv[]) {
    signal(SIGINT, intHandler);
    signal(SIGPIPE, intHandler);
    int ret = 0, n = 0;
    time_t ticks;

    socket_connect();
    // ID 1
    //    char test_data[] = {0x00, 0x00, 0x00, 0x04, \
//                                0x00, 0x01, \
//                                0x00, 0x01};

    //ID 10 - door open event - working data
    char door_open_cmd[] = {0x0A, 0x00};
    char test_data[512] = {0x01,
        0x14, 0x00, 0x00, 0x00, \
                                0x0A, 0x00};

    // ID254 - heartbeat
    //    char test_data[] = {0x01,
    //        0x02, 0x00, 0x00, 0x00, \
//                                0xFE, 0x00};

    char event_time[512];
    get_formatted_time(event_time);
    //printf("time : %s\n", event_time);
    char tmp[512];
    int len = snprintf(tmp, sizeof (tmp), "%s", event_time);
    printf("time : %s, %d bytes\n", tmp, len);
    memcpy(test_data+5+sizeof(door_open_cmd), tmp, len);
    test_data[1] = sizeof(door_open_cmd) + len;
    //strcat(test_data, event_time);
    //    //ID 100 - routing
    //    char test_data[] = {0x01, 
    //                                0x0E, 0x00, 0x00, 0x00, \
//                                0x64, 0x00, \
//                                0x31, 0x39, 0x32, 0x2e, 0x31, 0x36, 0x38, 0x2e, 0x31, 0x2e, 0x31, 0x30,0x30};


    int buff_len = 5 + sizeof(door_open_cmd) + len;
    //    printf("Sending data. Buffer length: %d\n", buff_len);
    //    for(int i =0; i < buff_len; i++) {
    //        printf("%02x", test_data[i]);
    //    }


    //while(1);
    ret = write(sockfd, test_data, buff_len);
    if (ret < 0) {
        printf("Write Error - code: %d\n", ret);
    }

    //    Receive data
    //    while ((n = read(sockfd, recvBuff, sizeof (recvBuff) - 1)) > 0) {
    //        recvBuff[n] = 0;
    //        //printf("n: %d\n", n);
    //        if (fputs(recvBuff, stdout) == EOF) {
    //            printf("\n Error : Fputs error\n");
    //        }
    //    }

    close(sockfd);
    sleep(1);
    printf("End of Program\n");
    return 0;
}

