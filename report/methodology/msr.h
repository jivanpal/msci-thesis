#include <linux/kernel.h>

/* Write the value `data` to the MSR `msr` via the assembly instruction WRMSR. */
static void write_msr(u32 msr, u64 data)
{
	u32 low = (u32)data;
	u32 high = (u32)(data >> 32);

	asm volatile("wrmsr" : : "c"(msr), "a"(low), "d"(high));
}

/* Set the core (and cache) voltage offset as described by `hex_offset`, which is a 16-bit value whose lowest 11 bits encode an 11-bit signed integer whose value is the desired core voltage offset in units of 1/1024 volts. */
static void write_vcore_hex(u16 hex_offset)
{
	write_msr(0x150, 0x8000001100000000 | ((u32)hex_offset << 21));
		// plane index = 0, write flag = 1
	write_msr(0x150, 0x8000021100000000 | ((u32)hex_offset << 21));
		// plane index = 2, write flag = 1
}

/* Set the core (and cache) voltage offset to `signed_offset`, which is in units of 1/1024 volts. The offset must not exceed +/-1 volt. */
static void write_vcore_dec(s16 signed_offset)
{
	u16 hex_offset = signed_offset < 0 ? signed_offset + 2048 : signed_offset;
	
	if (hex_offset & ~(~0 << 5) << 11) {
		printk(KERN_MSG "The value %d is not an 11-bit signed integer!\n", signed_offset);
		return;
	}

	write_vcore_hex(hex_offset);
}
