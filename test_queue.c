#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "q.h"

queue_t *obj;

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

static int _index;
char EXPECT_OUTPUT[][100];
char REAL_OUTPUT[][100];

int INPUT(char *buf, int size)
{
	int retval = 0;
	if (-1 != msg_put_buf(obj, buf, size)) {
		retval = 1;
		memcpy(&EXPECT_OUTPUT[_index][0], buf, size);
		printf("input[%d]: %s\t", _index, &EXPECT_OUTPUT[_index][0]);
		_index++;
	}
	printf("\t");
	return retval;
}

int EXPECT_OUTPUT_QUEUE()
{
	int i;
	char buf[1024] = {0};
	for (i = 0; i < _index; i++) {
		msg_get_buf(obj, buf, 1024);
		memcpy(&REAL_OUTPUT[i][0], buf, 1024);
		printf("real_output[%d]: %s\t", i, &REAL_OUTPUT[i][0]);
		memset(&buf[0], 0, 1024);
	}
	return 0;
}

int COMPARE(char expect_output[][100], char real_output[][100])
{
	int i = 0;
	int pass = 0;
	for (i = 0; i < _index; i++) {
		if (strncmp(expect_output[i],
			real_output[i],
			strlen(&expect_output[i][0])) == 0) {
			pass++;
		}
	}
	printf("\n\nAll Test Done!\n\n");
	printf("\nResult: %2d/%2d [PASS/TOTAL]\n\n", pass, _index);
	return 0;
}

int main()
{
	msg_init(&obj, "helloworld", 100*1024*10);
#if 0
	thrd_func_write(NULL);
	thrd_func_read(NULL);
#endif
	printf("\n===============================================================================\n\n");

	INPUT("12345", 5);
	INPUT("abc", 3);
	INPUT("aaaaaaaaaaaaaaa", 10);
	INPUT("bbbbbbbbbbbbbbb", 10);
	printf("\n");
	EXPECT_OUTPUT_QUEUE();
	COMPARE(EXPECT_OUTPUT, REAL_OUTPUT);
	printf("\n===============================================================================\n\n");
	msg_deinit(obj);
	return 0;
}
