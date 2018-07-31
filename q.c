#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <_list.h>
#include <q.h>

U8 msg_init(queue_t **obj, char *name, U32 size)
{
	*obj = malloc(sizeof(queue_t));
	list_init(&(*obj)->head);
	(*obj)->name = name;
	(*obj)->size = size;
	(*obj)->index = 0;
	return 0;
}

U8 msg_deinit(queue_t *obj)
{
	int ret = 0;
	int size = 1124;
	char buf[1124] = {0};
	while (1) {
		memset(buf, 0, 1124);
		ret = msg_get_buf(obj, buf, size);
		printf("%d\n", ret);
		if (ret == -1) break;
	}
	free(obj);
	return 0;
}

msg_t *msg_pack(char *buf, U32 size)
{
	if (buf == NULL) {
		printf("buf == NULL\n");
	}
	msg_t *msg = malloc(sizeof(msg_t));;
	if (msg == NULL) {
		goto end;
	}
	msg->buf = malloc(size);
	memcpy(msg->buf, buf, size);
	msg->length = size;
	msg->type = 4;
end:
	return msg;
}

U8 msg_depack(msg_t *msg)
{
	U8 retval = 0;
	if (msg == NULL) {
		printf("msg == NULL\n");
		retval = -1;
	}
	free(msg->buf);
	free(msg);
	return retval;
}

U8 msg_put(queue_t *obj, msg_t *msg)
{
	U8 retval = 0;
	if (obj == NULL) {
		printf("obj == NULL\n");
		retval = -1;
	}
	list_insert_behind(&obj->head, &(msg->list));
	obj->index++;
	return retval;
}

msg_t *msg_get(queue_t *obj)
{
	msg_t *msg = NULL;
	if (obj == NULL) {
		printf("obj == NULL\n");
		goto end;
	}
	LIST *tmp = NULL;
	tmp = &(obj->head);
	if (!is_list_last(tmp)) {
		msg = list_entry(tmp->next, msg_t, list);
		if (msg->buf != NULL) {
			print("msg buf: %s\n", msg->buf);
		} else {
			//printf("msg buf null\n");
		}
		list_delete(tmp->next);
		obj->index--;
	} else {
		//printf("[%s]message get nothing\n", __func__);
	}
end:
	return msg;
}

U8 msg_put_buf(queue_t *obj, char *buf, U32 size)
{
	int retval = 0;
	if (obj->size <= obj->count) {
		print("queue is full, max buffer size is %d\n", obj->size);
		retval = -1;
		goto end;
	}
	msg_t *msg = msg_pack(buf, size);
	if (msg == NULL) {
		retval = -1;
		print("no data\n");
		goto end;
	}
	if (msg->buf != NULL) {
		print("msg->buf : %s, msg->size: %d\n", msg->buf, msg->length);
	} else {
		print("msg->buf null");
	}
	retval = msg_put(obj, msg);
	obj->count += size;
end:
	return retval;
}

U32 msg_get_buf(queue_t *obj, char *buf, U32 size)
{
	int len;
	msg_t *msg;
	msg = msg_get(obj);
	if ((msg == NULL) || (buf == NULL)) {
		len = -1;
		goto end;
	}
	//printf("[%s %d]queue index: %d\n", __func__, __LINE__, obj->index);
	print("msg buf: %s\n", msg->buf);

	if (size > msg->length) len = msg->length;
	else len = size;

	memcpy(buf, msg->buf, len);
	msg_depack(msg);
	obj->size -= len;
	if (obj->size < 0)
		obj->size = 0;
end:
	return len;
}

U8 print_len(char *buf, int size)
{
	int i;
	for (i = 0; i < size; i++) {
		printf("%c", buf[i]);
	}

	return 0;
}
