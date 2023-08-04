#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* fmtname(char *path) {
  static char buf[DIRSIZ + 1];
  char *p;
  for (p = path + strlen(path); p >= path && *p != '/';p--);
  p++;
  if (strlen(p) >= DIRSIZ) return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  buf[strlen(p)] = 0;
  return buf;
}

void find(char *path, char *fileName) {
  char buf[512], *p;
  int fd;
  struct stat st;
  struct dirent de;
  if ((fd = open(path, 0)) < 0 ) {
    fprintf(2, "find: cannot open %s\n", path);
	return;
  }
  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
	close(fd);
	return;
  }

  switch(st.type) {
  //DFS
  case T_FILE:
	if (strcmp(fmtname(path), fileName) == 0) {
	  printf("%s\n", path);
	}
	break;
  case T_DIR:
	if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
	  printf("find: path too long\n");
	  break;
	}
	strcpy(buf, path);
	p = buf + strlen(buf);
	*p++ = '/';
	while (read(fd, &de, sizeof(de)) == sizeof(de)) {
	  if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
		continue;
	  }
	  memmove(p, de.name, DIRSIZ);
	  p[DIRSIZ] = 0;
	  //DFS recursive
	  find(buf, fileName);
	}
	break;
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(2, "Please use it by: find sp sp\n");
    exit(1);
  }
  find(argv[1], argv[2]);
  exit(0);
}
