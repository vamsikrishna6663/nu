#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <alloca.h>
#include <signal.h>

#include "config.h"

#define INPUT_BUFFER_SIZE 512

char* freadall(FILE* fp) {
	struct input_part {
		char buffer[INPUT_BUFFER_SIZE];
		struct input_part* next;
	};

	struct input_part* head = malloc(sizeof(struct input_part));
	head->next = NULL;
	struct input_part* current = head;
	size_t total_size = 0;

	char c;

	while((c = fgetc(fp)) != EOF) { // TODO: This is fragile, change it to '\n' and find a better way to reopen. Also, don’t feel tempted to just change this to '\n', as it will allocate memory forever
		current->buffer[total_size++ % INPUT_BUFFER_SIZE] = c;

		if(total_size % INPUT_BUFFER_SIZE == 0) {
			current->next = malloc(sizeof(struct input_part));
			current->next->next = NULL;
			current = current->next;
		}
	}

	char* result = malloc(total_size + 1);
	result[total_size] = '\0';

	size_t i;

	for(i = 0; total_size >= INPUT_BUFFER_SIZE; i++) {
		memcpy(result + i * INPUT_BUFFER_SIZE, head->buffer, INPUT_BUFFER_SIZE);
		total_size -= INPUT_BUFFER_SIZE;
		struct input_part* next = head->next;
		free(head);
		head = next;
	}

	memcpy(result + i * INPUT_BUFFER_SIZE, head->buffer, total_size);
	free(head);

	return result;
}

int main() {
	int child_id = 0;
	chdir(MUSIC_DIR);

	int paused = 0;

	while(1) {
		FILE* control = fopen(CONTROL, "r");
		char* path = freadall(control);
		fclose(control);

		if(strcmp(path, "pause") == 0) {
			if(child_id) {
				kill(child_id, SIGSTOP);
				paused = 1;
			}

			free(path);
			continue;
		}

		if(strcmp(path, "play") == 0) {
			if(child_id) {
				kill(child_id, SIGCONT);
				paused = 0;
			}

			free(path);
			continue;
		}

		if(strcmp(path, "toggle") == 0) {
			if(child_id) {
				kill(child_id, paused ? SIGCONT : SIGSTOP);
				paused = !paused;
			}

			free(path);
			continue;
		}

		size_t len = strlen(path);
		char* full_path = alloca(sizeof MUSIC_DIR + len);
		memcpy(full_path, MUSIC_DIR, sizeof MUSIC_DIR - 1);
		memcpy(full_path + sizeof MUSIC_DIR - 1, path, len + 1);
		free(path);

		if(child_id) {
			kill(child_id, SIGHUP);
		}

		if((child_id = fork()) == 0) {
			if(execlp(PLAYER, PLAYER, full_path, NULL) == -1) {
				perror("exec failed");
				return 1;
			}
		} else if(child_id == -1) {
			perror("fork failed");
			return 1;
		}
	}

	return 0;
}
