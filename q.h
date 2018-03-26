typedef struct queue_str {
	LIST list;
	LIST head;
	U8 count;
	U8 *name;
} queue_t;

typedef struct msg_str {
	LIST list;
	U8 type;
	U32 length;
	U8 *buff;
} msg_t;

u8 msg_init(queue_t *obj, char *name, int count);
u8 msg_put(queue_t *obj, msg_t *msg, u8 size);
u8 msg_get(queue_t *obj, msg_t *msg, u8 size);
u8 msg_deinit(queue_t *obj);
