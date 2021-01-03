#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fcntl.h>           /* Definition of AT_* constants */
#include <sys/stat.h>



/*int main(int argc, char *argv[]){
	char lastTty[16384];

	printf("User id: %d\n", getuid());
	if(listTty(lastTty) == 1){
		printf("Last tty: %s\n", lastTty);
	}
	return 0;
}*/

int compare(struct timespec a, struct timespec b){
	if(b.tv_sec == a.tv_sec) {
		if (b.tv_nsec == a.tv_nsec) {
			return 0;
		}
		if (b.tv_nsec > a.tv_nsec) {
			return 1;
		}
		return -1;
	}
	if(b.tv_sec > a.tv_sec) {
		return 1;
	}
	return -1;
}

int listTty(char * ttyBuffer){
	DIR *d;
	struct dirent *dir;
	uid_t userId = getuid();
	d = opendir("/dev/pts/");

	char filePath[16384];

	struct timespec lastTime;
	lastTime.tv_sec = 0;
	lastTime.tv_nsec = 0;
	char lastFilePath[16384];

	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if( dir -> d_type == DT_CHR) {
				struct stat fileInfo;
				filePath[0]=NULL;
				sprintf(filePath, "/dev/pts/%s", dir->d_name);

				if (stat(filePath, &fileInfo) == 0){
					if (fileInfo.st_uid == userId) {
						if (compare(lastTime, fileInfo.st_atim) == 1) {
							lastTime.tv_sec = fileInfo.st_atim.tv_sec;
							lastTime.tv_nsec = fileInfo.st_atim.tv_nsec;
							strncpy(&lastFilePath[0], filePath, strlen(filePath));
						}
					}
				}
			}
	    }
	    closedir(d);
	}

	if(lastTime.tv_sec == 0){
		return -1;
	}
	strncpy(ttyBuffer, lastFilePath, strlen(lastFilePath));
	return 1;
}
