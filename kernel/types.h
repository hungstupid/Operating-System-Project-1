typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int  uint32;
typedef unsigned long uint64;

typedef uint64 pde_t;

struct procinfo {
  int pid;         // Process ID
  int ppid;        // Parent Process ID
  int state;       // Trạng thái tiến trình
  uint64 sz;       // Kích thước bộ nhớ
  char name[16];   // Tên tiến trình
};
