#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void)
{
  int mask;
  
  // Lấy đối số đầu tiên (chính là mask) do người dùng truyền vào
  argint(0, &mask); 
  
  // Gán mask này cho tiến trình hiện tại đang chạy
  myproc()->trace_mask = mask;
  
  return 0; // Trả về 0 báo hiệu thành công
}

// Lấy mảng proc từ file proc.c để có thể duyệt qua danh sách tiến trình
extern struct proc proc[];

uint64
sys_procinfo(void)
{
  int pid;
  uint64 addr; // Địa chỉ của struct procinfo ở không gian người dùng (User space)
  struct proc *p;
  struct procinfo info;

  // Lấy 2 đối số từ người dùng: pid và địa chỉ con trỏ
  argint(0, &pid);
  argaddr(1, &addr);

  // Duyệt qua toàn bộ danh sách tiến trình trong hệ thống
  for(p = proc; p < &proc[NPROC]; p++){
    acquire(&p->lock); // Khóa tiến trình lại để đọc thông tin an toàn
    
    // Nếu tìm thấy tiến trình có PID trùng khớp và đang hoạt động
    if(p->state != UNUSED && p->pid == pid){
      info.pid = p->pid;
      info.ppid = p->parent ? p->parent->pid : 0; // Lấy PID của tiến trình cha (nếu có)
      info.state = p->state;
      info.sz = p->sz;
      safestrcpy(info.name, p->name, sizeof(info.name)); // Copy tên tiến trình
      
      release(&p->lock); // Đọc xong thì mở khóa

      // Chép dữ liệu từ biến 'info' của Kernel sang địa chỉ 'addr' của User
      if(copyout(myproc()->pagetable, addr, (char *)&info, sizeof(info)) < 0)
        return -1;
      
      return 0; // Trả về 0 báo hiệu thành công
    }
    release(&p->lock); // Mở khóa nếu không khớp PID và đi tiếp
  }
  
  return -1; // Trả về -1 nếu không tìm thấy tiến trình nào
}