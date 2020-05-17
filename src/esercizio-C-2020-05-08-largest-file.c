#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

void err_exit(char *msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

char *find_largest_file(char *directory_name, int explore_subdirectories_recursively, unsigned long *largest_file_size)
{
	char *ret = NULL;
	*largest_file_size = 0;
	struct stat st;
	struct dirent *ep;
	DIR *dir_ptr = opendir(directory_name);

	if (dir_ptr == NULL)
		err_exit("opendir error");

	if (chdir(directory_name) == -1)
		err_exit("chdir error");

	while ((ep = readdir(dir_ptr)) != NULL) {
		if (ep->d_type == DT_REG) {
			if (stat(ep->d_name, &st) == -1)
				err_exit("stat error");

			if (st.st_size > *largest_file_size) {
				*largest_file_size = st.st_size;
				char *new_ptr = realloc(ret, strlen(ep->d_name) + 1);
				if (new_ptr == NULL)
					err_exit("realloc error");

				ret = strcpy(new_ptr, ep->d_name);
			}
		}
	}

	return ret;
}

int main()
{
	unsigned long size = 0;
	char *biggest = find_largest_file(getenv("HOME"), 0, &size);
	printf("Largest file in the home directory is\n%s\nwith a size of %lu\n", biggest, size);
	return 0;
}
