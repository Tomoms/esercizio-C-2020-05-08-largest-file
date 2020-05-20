#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

void err_exit(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

char *find_largest_file(char *directory_name, int explore_subdirectories_recursively, unsigned long *largest_file_size, char *oldret)
{
	char *ret = oldret;
	int dir_name_len = strlen(directory_name);
	struct stat st;
	struct dirent *ep;
	DIR *dir_ptr = opendir(directory_name);

	if (dir_ptr == NULL)
		err_exit("opendir error");

	while ((ep = readdir(dir_ptr)) != NULL) {
		if (ep->d_type == DT_REG) {
			char *fname = calloc(dir_name_len + 1 + strlen(ep->d_name), sizeof(*fname));
			if (fname == NULL)
				err_exit("calloc error");

			strcpy(fname, directory_name);
			strcat(fname, ep->d_name);
			if (stat(fname, &st) == -1)
				err_exit("stat error");

			if (st.st_size > *largest_file_size) {
				*largest_file_size = st.st_size;
				char *new_ptr = realloc(ret, strlen(fname) + 1);
				if (new_ptr == NULL)
					err_exit("realloc error");

				ret = strcpy(new_ptr, fname);
				free(fname);
			}
		} else if (explore_subdirectories_recursively && ep->d_type == DT_DIR && (strcmp(ep->d_name, ".") && strcmp(ep->d_name, ".."))) {
			char *newdir = calloc(dir_name_len + strlen(ep->d_name) + 2, sizeof(*newdir));
			if (newdir == NULL)
				err_exit("calloc error");

			strcpy(newdir, directory_name);
			strcat(newdir, ep->d_name);
			strcat(newdir, "/");
			ret = find_largest_file(newdir, explore_subdirectories_recursively, largest_file_size, ret);
			free(newdir);
		}
	}

	closedir(dir_ptr);
	return ret;
}

int main()
{
	unsigned long size = 0;
	char *home = getenv("HOME");
	char *start = calloc(strlen(home) + 2, sizeof(*start));
	strcpy(start, home);
	strcat(start, "/");
	char *biggest = find_largest_file(start, 0, &size, NULL);
	printf("Largest file in the given directory is\n%s\nwith a size of %lu\n", biggest, size);
	free(home);
	free(start);
	free(biggest);
	return 0;
}
