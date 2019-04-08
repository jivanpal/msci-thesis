This repository contains all code developed in relation to this project. It
contains the following directories:

- gnupg-sha - this contains the source code and license for the GnuPG `sha1sum`
                program, as seen in GnuPG 1.3.92. This code was obtained from:
                ftp://ftp.gnupg.org/gcrypt/binary/sha1sum.c
- modules   - this contains the source code for Linux kernel modules we have
                developed. Of the directories within `modules`, only `bad-sha`
                is of immediate interest; the other directories contain parts
                of the code in `bad-sha`, and were used for testing purposes
                when `bad-sha` itself was being developed.
- report    - this contains the LaTeX files and other assets that make up the
                dissertation document.
- scripts   - this contains various Bash shell scripts written to assist
                testing.

The code in `modules` can be compiled by running `make` in the relevant
directory. You should use GCC as your compiler.
                
The `scripts` directory itself contains two directories:

- cpu-and-sensors   - this contains scripts that simply print out metrics
                        relating to the CPU and hardware sensors to the console.
- tests             - this contains scripts used in determining the stability of
                        operating performance points.
                        
The `tests` directory contains the following:

- undervolt-test.sh     - a script detailed whose use is detailed in the
                            dissertation.
- undervolt-list*.txt   - lists of voltage offsets in millivolts. 
- generate-data.sh      - a script that generates and saves 4 KiB of random
                            data to `._shatest_data` and saves the SHA-1 hash
                            of this data to `._shatest_hash`.
- clean.sh              - removes the `._shatest_data` and `._shatest_hash`
                            files.
- common.sh             - implements Bash traps for SIGINT and SIGTERM. Sourced
                            in `undervolt-test.sh`. Previously sourced in other
                            scripts as well, which were later removed from the
                            repository.
