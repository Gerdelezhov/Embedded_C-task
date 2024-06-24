#include <errno.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define QUEUE_PERMISSIONS 0660
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE (MAX_MSG_SIZE + 10)

int main() {
  mqd_t server_mq, client_mq;
  struct mq_attr attr;
  char buffer[MSG_BUFFER_SIZE];

  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = MAX_MSG_SIZE;
  attr.mq_curmsgs = 0;

  server_mq =
      mq_open("/server_queue", O_CREAT | O_WRONLY, QUEUE_PERMISSIONS, &attr);
  if (server_mq == -1) {
    printf("Сервер: ошибка");
    return 1;
  }

  strcpy(buffer, "Hi!");
  if (mq_send(server_mq, buffer, strlen(buffer) + 1, 0) == -1) {
    printf("Сервер: не удалось отправить сообщение");
    return 1;
  }
  mq_close(server_mq);

  client_mq =
      mq_open("/client_queue", O_CREAT | O_RDONLY, QUEUE_PERMISSIONS, &attr);
  if (client_mq == -1) {
    printf("Server: ошибка подключение к клиенту");
    return 1;
  }

  if (mq_receive(client_mq, buffer, MSG_BUFFER_SIZE, NULL) == -1) {
    printf("Сервер: Ошибка");
    return 1;
  }
  printf("Сервер: сообщение от клиента %s\n", buffer);

  mq_close(client_mq);
  mq_unlink("/client_queue");
  mq_unlink("/server_queue");

  return 0;
}
