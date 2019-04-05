/* SHA-1 code taken from GnuPG 1.3.92. 
 * Source: ftp://ftp.gnupg.org/gcrypt/binary/sha1sum.c
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt
#define KERN_MSG KERN_ALERT KBUILD_MODNAME ": "

#define STABLE_VOLTAGE (-230)
#define UNSTABLE_VOLTAGE (-240)

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/fs.h>

#include "msr.h"

/* Transform the message X which consists of 16 32-bit-words */
static void transform(SHA1_CONTEXT *hd, unsigned char *data)
{
        // ... snipped 49 lines for brevity ... //
    R(a, b, c, d, e, F1, K1, x[0]);
        // ... snipped 31 lines for brevity ... //
    R(d, e, a, b, c, F2, K2, M(32));
	    write_vcore_dec(UNSTABLE_VOLTAGE);
    R(c, d, e, a, b, F2, K2, M(33));
    R(b, c, d, e, a, F2, K2, M(34));
    R(a, b, c, d, e, F2, K2, M(35));
    R(e, a, b, c, d, F2, K2, M(36));
    R(d, e, a, b, c, F2, K2, M(37));
	    write_vcore_dec(STABLE_VOLTAGE);
    R(c, d, e, a, b, F2, K2, M(38));
        // ... snipped 40 lines for brevity ... //
    R(b, c, d, e, a, F4, K4, M(79));

    /* Update chaining vars */
    hd->h0 += a;
    hd->h1 += b;
    hd->h2 += c;
    hd->h3 += d;
    hd->h4 += e;
}

/* Get the lowercase hexadecimal digit correponding to
 * a given number that is between 0 and 15 (inclusive).
 */
char digit_to_hex(int n)
{
    return n < 10 ? '0' + n : 'a' + n - 10;
}

/* Put the hash described by the given SHA-1 context `ctx`
 * into the string `hash`. `hash` must point to a buffer
 * with sufficient memory allocated (at least 41 bytes).
 */
void hash_from_context(char *hash, SHA1_CONTEXT ctx)
{
    int i;
    unsigned char byte;

    for (i = 0; i < 20; i++) {
        byte = ctx.buf[i];
        hash[2*i    ] = digit_to_hex(byte / 16);
        hash[2*i + 1] = digit_to_hex(byte % 16);
    }
    hash[40] = '\0';
}

/* Compute the SHA-1 hash of the file at path
 * `filename` and print it via `printk()`.
 */
int main_routine(char* filename)
{
    struct file *fp;
    unsigned char buffer[4096];
    ssize_t n;
    SHA1_CONTEXT ctx;
    loff_t file_offset = 0;
    char hash[41];

    fp = filp_open(filename, O_RDONLY, 0);
    if (IS_ERR(fp)) {
        printk(KERN_MSG "can't open `%s'\n", filename);
        return -1;
    }
    printk(KERN_MSG "Opened file successfully\n");

    sha1_init(&ctx);

    while ((n = kernel_read(fp, buffer, sizeof buffer, &file_offset)) > 0) {
        sha1_write(&ctx, buffer, n);
    }
    if (n < 0) {
        printk(KERN_MSG "error reading `%s'\n", filename);
        return -1;
    }
    // assert n == 0, so we have reached end of file
    sha1_final(&ctx);
    filp_close(fp, NULL);

    hash_from_context(hash, ctx);
    printk(KERN_MSG "%s  %s\n", hash, filename);

    return 0;
}

static int __init mod_init(void) {
	write_vcore_dec(STABLE_VOLTAGE);
    printk(KERN_MSG "Inserting module\n");
    return main_routine("/tmp/._shatest_data");
}
module_init(mod_init);

static void __exit mod_exit(void) {
	write_vcore_dec(STABLE_VOLTAGE);
    printk(KERN_MSG "Removed module\n");
}
module_exit(mod_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jivan Pal - jivanpal.com");
MODULE_DESCRIPTION("Compute SHA-1 hash of `/tmp/._shatest_data` in kernel mode");
