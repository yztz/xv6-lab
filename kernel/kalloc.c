// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"


struct pg_ref_count_t {
  struct spinlock lock;
  int count[PG_NUM];
};

struct pg_ref_count_t pg_ref_count;

void freerange(void *pa_start, void *pa_end);
void initpgcnt();
extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit()
{
  initpgcnt();
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);
}

void initpgcnt() {
  for(int i = 0; i < PG_NUM; i++) pg_ref_count.count[i] = 1;
  initlock(&pg_ref_count.lock, "pg_ref_count");
}

void add_ref_count(uint64 pa) {
  acquire(&pg_ref_count.lock);
  // if(pg_ref_count.count[REF_X(pa)] >= 3) printf(">= 3\n");
  pg_ref_count.count[REF_X(pa)]++;
  release(&pg_ref_count.lock);
}

void sub_ref_count(uint64 pa) {
  acquire(&pg_ref_count.lock);
  pg_ref_count.count[REF_X(pa)]--;
  release(&pg_ref_count.lock);
}
void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;
  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  acquire(&pg_ref_count.lock);
  pg_ref_count.count[REF_X(pa)]--;
  if (pg_ref_count.count[REF_X(pa)] > 0) {  // 若-1后大于0
    release(&pg_ref_count.lock);
    return;
  }
  release(&pg_ref_count.lock);
  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r) {
    memset((char*)r, 5, PGSIZE); // fill with junk
    add_ref_count((uint64)r);
  }
  // printf("%p\n", kmem.freelist);
  return (void*)r;
}
