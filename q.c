#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list.h>
#include <q.h>

U8 msg_init(queue_t **obj, char *name, int count)
{
	*obj = malloc(sizeof(queue_t));
	list_init(&(*obj)->head);
	(*obj)->name = name;
	(*obj)->count = count;
	(*obj)->index = 0;
	return 0;
}

U8 msg_deinit(queue_t *obj)
{
	free(obj);
	return 0;
}

msg_t *msg_pack(char *buf, U8 size)
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
	list_insert_behind(&obj->head, &msg->list);
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
	while (!is_list_last(tmp)) {
		msg = list_entry(tmp->next, msg_t, list);
		print("msg buf: %s\n", msg->buf);
		list_delete(tmp->next);
		break;
	}
	/*else {
		retval = -1;
	}*/
end:
	return msg;
}

U8 msg_put_buf(queue_t *obj, char *buf, U8 size)
{
	int retval = 0;
	msg_t *msg = msg_pack(buf, size);
	if (msg == NULL) {
		retval = -1;
		goto end;
	}
	retval = msg_put(obj, msg);
end:
	return retval;
}

U8 msg_get_buf(queue_t *obj, char *buf, U8 size)
{
	int retval = 0;
	int len;
	msg_t *msg;
	msg = msg_get(obj);
	if (msg == NULL) {
		retval = -1;
		goto end;
	}
	print("msg buf: %s\n", msg->buf);

	if (size > msg->length) len = msg->length;
	else len = size;

	memcpy(buf, msg->buf, len);
	print("%s\n", buf);
	msg_depack(msg);
end:
	return retval;
}

int main()
{
	queue_t *obj;
	msg_init(&obj, "helloworld", 100);
	msg_put_buf(obj, "1", 1);
	msg_put_buf(obj, "2", 1);
	msg_put_buf(obj, "3", 1);
	msg_put_buf(obj, "4", 1);
	msg_put_buf(obj, "5", 1);
	msg_put_buf(obj, "6", 1);

	char buf[1] = {0};
	msg_get_buf(obj, buf, 1);
	printf("%s\n", buf);
	msg_get_buf(obj, buf, 1);
	printf("%s\n", buf);
	msg_get_buf(obj, buf, 1);
	msg_get_buf(obj, buf, 1);
	msg_get_buf(obj, buf, 1);
	msg_get_buf(obj, buf, 1);
	msg_get_buf(obj, buf, 1);
	msg_deinit(obj);
	return 0;
}
