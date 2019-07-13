#ifndef NOTIFY_H_
#define NOTIFY_H_

#include "estructurasMemoria.h"

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024* (EVENT_SIZE + 16))

void notify();


#endif /* NOTIFY_H_ */
