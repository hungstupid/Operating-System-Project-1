#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/stat.h"

void
tree(char *path, int depth)
{
  char buf[512], *p;
  int fd;
  struct dirent d;
  struct stat s;
  int n;
  char name[DIRSIZ + 1];

  fd = open(path, O_RDONLY);
  if (fd < 0) {
    fprintf(2, "tree: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &s) < 0) {
    fprintf(2, "tree: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (s.type != T_DIR) {
    close(fd);
    return;
  }

  if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
    fprintf(2, "tree: path too long\n");
    close(fd);
    return;
  }

  strcpy(buf, path);
  p = buf + strlen(buf);
  if (p == buf || *(p - 1) != '/')
    *p++ = '/';

  while ((n = read(fd, &d, sizeof(d))) == sizeof(d)) {
    if (d.inum == 0)
      continue;

    memmove(name, d.name, DIRSIZ);
    name[DIRSIZ] = 0;

    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
      continue;

    memmove(p, d.name, DIRSIZ);
    p[DIRSIZ] = 0;

    if (stat(buf, &s) < 0) {
      fprintf(2, "tree: cannot stat %s\n", buf);
      continue;
    }

    for (int i = 0; i < depth; i++)
      printf("  ");

    if (s.type == T_DIR)
      printf("%s/\n", name);
    else
      printf("%s\n", name);

    if (s.type == T_DIR)
      tree(buf, depth + 1);
  }

  if (n < 0 || n > 0)
    fprintf(2, "tree: read error on %s\n", path);

  close(fd);
}

int
main(int argc, char *argv[])
{
  struct stat s;
  int fd;
  char *path;

  if (argc > 2) {
    fprintf(2, "usage: tree [directory]\n");
    exit(1);
  }

  if (argc < 2)
    path = ".";
  else
    path = argv[1];

  fd = open(path, O_RDONLY);
  if (fd < 0) {
    fprintf(2, "tree: cannot open %s\n", path);
    exit(1);
  }

  if (fstat(fd, &s) < 0) {
    fprintf(2, "tree: cannot stat %s\n", path);
    close(fd);
    exit(1);
  }
  close(fd);

  if (s.type == T_DIR) {
    int n = strlen(path);
    if (n > 0 && path[n - 1] == '/')
      printf("%s\n", path);
    else
      printf("%s/\n", path);
  } else {
    printf("%s\n", path);
    exit(0);
  }

  tree(path, 1);
  exit(0);
}