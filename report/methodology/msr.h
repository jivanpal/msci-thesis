#include <linux/kernel.h>

/* Get the value of the MSR `msr` via the assembly instruction RDMSR.
 * Adapted from code by Michael Guyver. */
static u64 read_msr(u32 msr)
{
	u32 low = 0;
	u32 high = 0;
	u64 result = 0;

	asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));

	result = low | ((u64)high << 32);
	return result;
}

/* Write the value `data` to the MSR `msr` via the assembly instruction WRMSR.
 * Adapted from code by Michael Guyver. */
static void write_msr(u32 msr, u64 data)
{
	u32 low = (u32)data;
	u32 high = (u32)(data >> 32);

	asm volatile("wrmsr" : : "c"(msr), "a"(low), "d"(high));
}

/* Get the 11-bit signed integer (in a 16-bit buffer) describing the core
 * voltage offset in multiples of 1/1024 volts. */
static u16 read_vcore_hex(void)
{
	u64 msr_response;
	u16 result;
	s16 signed_result;

	write_msr(0x150, 0x8000001000000000);
		// plane index = 0, write flag = 0
	msr_response = read_msr(0x150);

	result = (u16)((msr_response >> 21) & 0x7ff);
	signed_result = result < 1024 ? result : result - 2048;
	return result;
}

/* Set the core (and cache) voltage offset as described by `hex_offset`,
 * which is a 16-bit value whose lowest 11 bits encode an 11-bit signed
 * integer whose value is the desired core voltage offset in units of
 * 1/1024 volts. */
static void write_vcore_hex(u16 hex_offset)
{
	write_msr(0x150, 0x8000001100000000 | ((u32)hex_offset << 21));
		// plane index = 0, write flag = 1
	write_msr(0x150, 0x8000021100000000 | ((u32)hex_offset << 21));
		// plane index = 2, write flag = 1
}

/* Set the core (and cache) voltage offset to `signed_offset`, in units of
 * 1/1024 volts. The offset must not exceed +/-1 volt. */
static void write_vcore_dec(s16 signed_offset)
{
	u16 hex_offset = signed_offset < 0 ? signed_offset + 2048 : signed_offset;
	
	if (hex_offset & ~(~0 << 5) << 11) {
		printk(KERN_MSG "The value %d is not an 11-bit signed integer!\n", signed_offset);
		return;
	}

	write_vcore_hex(hex_offset);
}
