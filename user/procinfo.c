#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  // Kiểm tra xem người dùng có nhập PID chưa
  if(argc < 2){
    printf("Usage: procinfo <pid>\n");
    exit(1);
  }

  // Chuyển đổi tham số truyền vào từ chuỗi (string) sang số nguyên (int)
  int pid = atoi(argv[1]);
  struct procinfo info;

  // Gọi System Call chúng ta vừa viết trong kernel
  if(procinfo(pid, &info) < 0){
    printf("procinfo: khong tim thay tien trinh voi PID %d\n", pid);
    exit(1);
  }

  // In kết quả ra màn hình
  printf("--- Thong tin tien trinh ---\n");
  printf("PID   : %d\n", info.pid);
  printf("PPID  : %d\n", info.ppid);
  printf("State : %d\n", info.state);
  printf("Size  : %d bytes\n", (int)info.sz); // Ép kiểu int để tránh lỗi format như lúc nãy
  printf("Name  : %s\n", info.name);
  printf("----------------------------\n");

  exit(0);
}