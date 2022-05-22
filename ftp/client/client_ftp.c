/*client_tcp*/
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFSIZE 512

void execute_put(int, char *[]);
void execute_get(int, char *[]);

struct ftp_header {
  uint8_t type;
  uint8_t code;
  uint16_t length;
};

void execute_put(int sd, char *com[]) {
  FILE *fp;
  char buf[BUFSIZE];
  int n;
  uint8_t *data, *head_data, *head_ftp_pay, *ftp_pay;
  struct ftp_header *send_header;
  struct ftp_header *datasend_header;
  struct ftp_header *recv_header;
  if ((fp = fopen(com[1], "r")) == NULL) {
    fprintf(stderr, "file %s cannot open\n", com[1]);
    exit(1);
  }
  n = fread(buf, sizeof(char), BUFSIZE, fp);
  if (n < BUFSIZE) {
    head_data = (uint8_t *)malloc(sizeof(struct ftp_header) + sizeof(com[2]));
    send_header = (struct ftp_header *)head_data;
    head_ftp_pay = head_data + sizeof(struct ftp_header);
    memcpy(head_ftp_pay, (uint8_t *)com[2], sizeof(com[2]));
    send_header->type = 0x06;
    send_header->code = 0x00;
    send_header->length = htons(strlen(com[2]) + 1);
    if ((send(sd, head_data, sizeof(struct ftp_header) + sizeof(com[2]), 0)) == -1) {
      perror("send");
      exit(1);
    } else {
      printf("send header\n");
    }

    // receive OK message
    recv_header = malloc(sizeof(struct ftp_header));
    if ((recv(sd, recv_header, sizeof(struct ftp_header), 0)) == -1) {
      perror("recv");
      exit(1);
    } else {
      if (recv_header->type == 0x10 && recv_header->code == 0x02) {
        printf("receive OK message\n");
      } else {
        fprintf(stderr, "OK Error\n");
        exit(1);
      }
    }
    data = (uint8_t *)malloc(sizeof(struct ftp_header) + sizeof(char) * n);
    datasend_header = (struct ftp_header *)data;
    ftp_pay = data + sizeof(struct ftp_header);
    memcpy(ftp_pay, (uint8_t *)buf, n);
    datasend_header->type = 0x20;
    datasend_header->code = 0x00;
    datasend_header->length = htons(n);
    if ((send(sd, data, sizeof(struct ftp_header) + sizeof(char) * n, 0)) == -1) {
      perror("send");
      exit(1);
    } else {
      printf("length : %d\n", n);
      printf("send data : %s\n", buf);
    }
    free(head_data);
    free(recv_header);
    free(data);
  }
  fclose(fp);
}

void execute_get(int s, char *com[]) {
}

int main() {
  // Variables for the file being received
  int sd, count, i = 0;
  in_port_t port;
  struct sockaddr_in skt;
  char *ip = "0.0.0.0";
  char *p;
  char sbuf[BUFSIZE];
  char *com[20];

  for (i = 0; i < 20; i++) {
    com[i] = (char *)malloc(sizeof(char) * 30);
  }
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("Could not create socket");
    return 0;
  }
  port = atoi("10001");
  memset(&skt, 0, sizeof skt);
  skt.sin_addr.s_addr = inet_addr(ip);
  skt.sin_family = AF_INET;
  skt.sin_port = htons(port);

  // Connect to server
  if (connect(sd, (struct sockaddr *)&skt, sizeof(skt)) < 0) {
    perror("Connection failed");
    return 0;
  }
  for (;;) {
    // Get a file from server
    printf("FTP $ ");
    fgets(sbuf, BUFSIZE, stdin);
    p = strtok(sbuf, " \n");
    strcpy(com[0], p);
    if (strcmp(com[0], "quit") == 0) {
      close(sd);
      return 0;
    }
    printf("com[0] = %s\n", p);
    while ((p = strtok(NULL, " \n")) != NULL) {
      count++;
      strcpy(com[count], p);
      printf("com[%d] = %s\n", count, p);
    }

    // Start receiving file
    if (strcmp(com[2], "") == 0) {
      strcpy(com[2], com[1]);
      printf("com[2] = %s\n", com[2]);
    }
    if (strcmp(com[0], "put") == 0) {
      execute_put(sd, com);
      close(sd);
    } else if (strcmp(com[0], "get") == 0) {
      printf("execute_get\n");
      execute_get(sd, com);
      close(sd);
    }
  }

  for (i = 0; i < 20; i++) {
    free(com[i]);
  }
  return 0;
}
