
#include <system.h>
#include <string.h>

namespace idt {

namespace {

#pragma pack(push, 1)
struct idt_entry_t {
  uint16_t base_low;
  uint16_t sel;
  uint8_t always0;
  uint8_t flags;
  uint16_t base_high;
} __attribute__((packed));

/* flags:
 *  7                           0
  +---+---+---+---+---+---+---+---+
  | P |  DPL  | S |    GateType   |
  +---+---+---+---+---+---+---+---+
 */

struct idt_descriptior_t {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed));
#pragma pack(pop)

#define MAX_IDT_ENTRY 256

idt_entry_t idt[MAX_IDT_ENTRY];
idt_descriptior_t idt_ptr;

void flush(void) {
  __asm__ __volatile__ ("lidt %0" :"=m"(idt_ptr));
}

}

void set_gate(uint8_t index, uint32_t base, uint16_t sel, uint8_t flags) {
  idt[index].base_low = (base & 0xffff);
  idt[index].sel = sel;
  idt[index].always0 = 0;
  idt[index].flags = flags;
  idt[index].base_high = ((base >> 16) & 0xffff);
}

void initialize(void) {
  memset(idt, 0, sizeof(idt));

  idt_ptr.limit = sizeof(idt) - 1;
  idt_ptr.base = (uint32_t) idt;

  flush();
}

} 

