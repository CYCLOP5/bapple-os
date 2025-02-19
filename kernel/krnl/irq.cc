#include <stdint.h>
#include <system.h>
#include <port.h>
#include <stdio.h>
#include <string.h>
#include <linkage.h>
asmlinkage {
void irq_handler0();
void irq_handler1();
void irq_handler2();
void irq_handler3();
void irq_handler4();
void irq_handler5();
void irq_handler6();
void irq_handler7();
void irq_handler8();
void irq_handler9();
void irq_handler10();
void irq_handler11();
void irq_handler12();
void irq_handler13();
void irq_handler14();
void irq_handler15();
}
namespace irq {
namespace {
#define IRQ_VECTOR_OFFSET 32
#define MAX_IRQ_ENTRY     16
#define PORT_MASTER_PIC_CMD 0x20
#define PORT_MASTER_PIC_DAT 0x21
#define PORT_SLAVE_PIC_CMD  0xa0
#define PORT_SLAVE_PIC_DAT  0xa1
fn_irq_handler_t lpfn_irq_handler[MAX_IRQ_ENTRY];
void dispatcher(irq_context_t *ptr) {
  int irq_index = ptr->irq_index;
  fn_irq_handler_t handler = lpfn_irq_handler[irq_index];
  if (handler == NULL) {
    printf("[IRQ dispatcher] Unhandled IRQ %d.\n", irq_index);
  } else {
    handler(ptr);
  }
  if (irq_index >= 8) {
    port::outb(PORT_SLAVE_PIC_CMD, 0x20);
  }
  port::outb(PORT_MASTER_PIC_CMD, 0x20);
}
}
void initialize(void) {
  port::outb(PORT_MASTER_PIC_CMD, 0x11);
  port::outb(PORT_SLAVE_PIC_CMD, 0x11);
  port::outb(PORT_MASTER_PIC_DAT, IRQ_VECTOR_OFFSET);     
  port::outb(PORT_SLAVE_PIC_DAT, IRQ_VECTOR_OFFSET + 8);  
  port::outb(PORT_MASTER_PIC_DAT, 0x4);                   
  port::outb(PORT_SLAVE_PIC_DAT, 0x2);                    
  port::outb(PORT_MASTER_PIC_DAT, 0x1);
  port::outb(PORT_SLAVE_PIC_DAT, 0x1);
  port::outb(PORT_MASTER_PIC_DAT, 0xff);
  port::outb(PORT_SLAVE_PIC_DAT, 0xff);
#define set_irq(n) idt::set_gate(n + IRQ_VECTOR_OFFSET, (uint32_t) irq_handler##n, KERNEL_CODE_SEL, 0x8e);
  set_irq(0);
  set_irq(1);
  set_irq(2);
  set_irq(3);
  set_irq(4);
  set_irq(5);
  set_irq(6);
  set_irq(7);
  set_irq(8);
  set_irq(9);
  set_irq(10);
  set_irq(11);
  set_irq(12);
  set_irq(13);
  set_irq(14);
  set_irq(15);
#undef set_irq
  memset(lpfn_irq_handler, 0, sizeof(lpfn_irq_handler));
}
void install(int index, fn_irq_handler_t handler) {
  if (lpfn_irq_handler[index] != NULL) {
    printf("[irq::install] IRQ%d handler already exists, overwritten.\n", index);
  }
  lpfn_irq_handler[index] = handler;
}
void uninstall(int index) {
  if (lpfn_irq_handler[index] == NULL) {
    printf("[irq::uninstall] IRQ%d handler not exists.\n", index);
  }
  lpfn_irq_handler[index] = NULL;
}
void disable(int index) {
  uint16_t port;
  uint8_t value;
  if (index < 8) {
    port = PORT_MASTER_PIC_DAT;
  } else {
    port = PORT_SLAVE_PIC_DAT;
    index -= 8;
  }
  value = port::inb(port) | (1 << index);
  port::outb(port, value);
}
void enable(int index) {
  uint16_t port;
  uint8_t value;
  if (index < 8) {
    port = PORT_MASTER_PIC_DAT;
  } else {
    port = PORT_SLAVE_PIC_DAT;
    index -= 8;
  }
  value = port::inb(port) & ~(1 << index);
  port::outb(port, value);
}
} 
asmlinkage void irq_dispatcher(irq::irq_context_t *ptr) {
  irq::dispatcher(ptr);
}