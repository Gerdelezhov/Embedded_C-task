#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define QUEUE_KEY 1234

struct message {
  long message_type;
  char message_text[100];
};

int main() {
  int msgid;
  struct message msg;

  msgid = msgget(QUEUE_KEY, 0666 | IPC_CREAT);
  if (msgid == -1) {
    printf("msgget");
    return 1;
  }

  msg.message_type = 1;
  strcpy(msg.message_text, "Hi!");
  if (msgsnd(msgid, &msg, sizeof(msg.message_text), 0) == -1) {
    printf("msgsnd");
    return 1;
  }

  if (msgrcv(msgid, &msg, sizeof(msg.message_text), 2, 0) == -1) {
    printf("msgrcv");
    return 1;
  }
  printf("Сервер: сообщение от клиента %s\n", msg.message_text);

  if (msgctl(msgid, IPC_RMID, NULL) == -1) {
    printf("msgctl");
    return 1;
  }

  return 0;
}
