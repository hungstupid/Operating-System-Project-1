#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

static void
run_once(char **argv)
{
  int pid = fork();
  if (pid < 0) {
    fprintf(2, "xargs: fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    exec(argv[0], argv);
    fprintf(2, "xargs: exec %s failed\n", argv[0]);
    exit(1);
  }

  wait(0);
}
int
main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(2, "usage: xargs command [args ...]\n");
    exit(1);
  }

  if (argc >= MAXARG) {
    fprintf(2, "xargs: too many initial arguments\n");
    exit(1);
  }

  char line[512];
  char *nargv[MAXARG];
  int base = argc - 1;
  int idx = 0;
  int n;
  char c;

  for (int i = 1; i < argc; i++) {
    nargv[i - 1] = argv[i];
  }

  while ((n = read(0, &c, 1)) == 1) {
    if (c == '\n') {
      line[idx] = '\0';
      nargv[base] = line;
      nargv[base + 1] = 0;

      run_once(nargv);
      idx = 0;
    } else {
      if (idx + 1 >= sizeof(line)) {
        fprintf(2, "xargs: input line too long\n");
        exit(1);
      }
      line[idx++] = c;
    }
  }

  if (n < 0) {
    fprintf(2, "xargs: read error\n");
    exit(1);
  }

  if (idx > 0) {
    line[idx] = '\0';
    nargv[base] = line;
    nargv[base + 1] = 0;

    run_once(nargv);
  }

  exit(0);
}