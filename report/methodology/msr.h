#include <linux/kernel.h>

// #define __MSR_MODULE_DEBUG__

#ifdef __MSR_MODULE_DEBUG__
#define dprintk(fmt, args...) printk(KERN_DEBUG KBUILD_MODNAME ": " fmt, ##args)
#else
#define dprintk(fmt, args...)
#endif


/* Get the value of the MSR `msr` via the assembly instruction RDMSR.
 *
 * Adapted from code by Michael Guyver at:
 * http://www.mindfruit.co.uk/2012/11/a-linux-kernel-module-for.html
 */
static u64 read_msr(u32 msr)
{
	u32 low = 0;
	u32 high = 0;
	u64 result = 0;

	asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));

	result = low | ((u64)high << 32);
	dprintk(KERN_MSG "MSR 0x%08x => 0x%016llx\n", msr, result);
	return result;
}

/* Write the value `data` to the MSR `msr` via the assembly instruction WRMSR.
 *
 * Adapted from code by Michael Guyver at:
 * http://www.mindfruit.co.uk/2012/11/a-linux-kernel-module-for.html
 */
static void write_msr(u32 msr, u64 data)
{
	u32 low = (u32)data;
	u32 high = (u32)(data >> 32);

	dprintk(KERN_MSG "MSR 0x%08x <= 0x%016llx\n", msr, data);
	asm volatile("wrmsr" : : "c"(msr), "a"(low), "d"(high));
}

/* Get the 11-bit signed integer (in a 16-bit buffer) describing the core
 * voltage offset in multiples of 1/1024 volts.
 */
static u16 read_vcore_hex(void)
{
	u64 msr_response;
	u16 result;
	s16 signed_result;

	dprintk("Entering `read_vcore_hex()`\n");

	dprintk("... Calling `write_msr()`\n");
	write_msr(0x150, 0x8000001000000000);
		// plane index = 0, write flag = 0

	dprintk("... Calling `read_msr()` to get response\n");
	msr_response = read_msr(0x150);
	dprintk("... Got response of 0x%016llx\n", msr_response);

	dprintk("... Computing result from response\n");
	result = (u16)((msr_response >> 21) & 0x7ff);
	signed_result = result < 1024 ? result : result - 2048;
	dprintk("... Got result of 0x%08x\n", result);
	dprintk(KERN_MSG "VCore hex offset => 0x%03x = %d\n", result, signed_result);
	dprintk("Exiting `read_vcore_hex()`\n");
	return result;
}

/* Set the core (and cache) voltage offset as described by `hex_offset`,
 * which is a 16-bit value whose lowest 11 bits encode an 11-bit signed
 * integer whose value is the desired core voltage offset in units of
 * 1/1024 volts.
 */
static void write_vcore_hex(u16 hex_offset)
{
#ifdef __MSR_MODULE_DEBUG__
	s16 signed_offset = hex_offset < 1024 ? hex_offset : hex_offset - 2048;
#endif

	dprintk(KERN_MSG "VCore hex offset <= 0x%03x = %d\n", hex_offset, signed_offset);
	write_msr(0x150, 0x8000001100000000 | ((u32)hex_offset << 21));
		// plane index = 0, write flag = 1

	dprintk(KERN_MSG "VCache hex offset <= 0x%03x = %d\n", hex_offset, signed_offset);
	write_msr(0x150, 0x8000021100000000 | ((u32)hex_offset << 21));
		// plane index = 2, write flag = 1
}

/* Set the core (and cache) voltage offset to `signed_offset`, in units of
 * 1/1024 volts. The offset must not exceed +/-1 volt.
 */
static void write_vcore_dec(s16 signed_offset)
{
	u16 hex_offset = signed_offset < 0 ? signed_offset + 2048 : signed_offset;

	if (hex_offset & ~(~0 << 5) << 11) {
		dprintk(KERN_MSG "The value %d is not an 11-bit signed integer!\n", signed_offset);
		return;
	}

	write_vcore_hex(hex_offset);
}
