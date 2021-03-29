#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "q.h"

static FILE *wav;
static FILE *tmp;
void reverse(char s[])
{
	int i, j;
	char c;

	for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

queue_t *obj;
void itoa(int n, char s[])
{
	int i, sign;

	if ((sign = n) < 0)  /* record sign */
		n = -n;/* make n positive */
	i = 0;
	do {
		/* generate digits in reverse order */
		s[i++] = n % 10 + '0';   /* get next digit */
	} while ((n /= 10) > 0); /* delete it */
	if (sign < 0)
		s[i++] = '-';
	s[i] = '\0';
	reverse(s);
}

void *thrd_func_write(void *arg)
{
	char buf[3200] = {0};
	char *wavpath = "1.wav";
	char *tmppath = "2.wav";
	int bytes;
	wav = fopen(wavpath, "rb");
	if (!wav) {
		printf("open wav :%s failed\n", wavpath);
		exit(0);
	} else {
		printf("open wav :%s success\n", wavpath);
	}

	tmp = fopen(tmppath, "wb");
	if (!tmp) {
		printf("open wav :%s failed\n", tmppath);
		exit(0);
	} else {
		printf("open wav :%s success\n", tmppath);
	}

	while ((bytes = fread(buf, 1, sizeof(buf), wav))) {
		msg_put_buf(obj, buf, bytes);
	}
	fclose(wav);

	return NULL;
}

void *thrd_func_read(void *arg)
{
	char buf[3200] = {0};
	int retval = 0;
	int bytes;
	sleep(2);
	while (1) {
		memset(buf, 0, 3200);
		retval = msg_get_buf(obj, buf, 3200);
		if (retval == -1) break;
		fwrite(buf, 1, retval, tmp);
	}
	printf("\n\n\n----------------\n\n\n");
	fclose(tmp);
	return NULL;
}

pthread_t tid1;
pthread_t tid2;

int main()
{
	msg_init(&obj, "helloworld", 100*1024*10);
#if 0
	if (pthread_create(&tid1, NULL, thrd_func_write, NULL)!=0) {
        	printf("Create thread error!\n");
        	exit(1);
	}

	if (pthread_create(&tid2, NULL, thrd_func_read, NULL)!=0) {
        	printf("Create thread error!\n");
        	exit(1);
	}
#endif

	thrd_func_write(NULL);
	thrd_func_read(NULL);
#if 0
	msg_put_buf(obj, "1221346568523233", 10);
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
#endif
	return 0;
}
