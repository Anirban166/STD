Contains a simple DeepState test harness that fuzzes inputs for a rule-given action (division). STest is used for the rule construct. Both libraries (DeepState and STest) are integrated. 

## Get started
Once you have a setup with DeepState configured and this repository cloned, `cd` into this directory and build the executable using `make`: (linked against DeepState's static library)
```sh
<username>@<systemname>:~/STest-Demo/Fuzz/DeepState$ make
g++ -Wall -g -std=c++11 -Iinclude -no-pie -ldeepstate -o fuzztest_deepstateharness.bin bsd_random.o Random.o Pick.o Pick_default.o fuzztest_deepstateharness.o
```
Run it through a fuzzer with desired arguments:
```sh
<username>@<systemname>:~/STest-Demo/Fuzz/DeepState$ ./fuzztest_deepstateharness.bin --fuzz --timeout=1  --fuzz_save_failing --output_test_dir out
INFO: Starting fuzzing
WARNING: No seed provided; using 1661044705
WARNING: No test specified, defaulting to first test defined (ArithmeticOps_DivisionByZero)
CRITICAL: fuzztest_deepstateharness.cpp(24): Divisor cannot be 0! (dividend and divisor values for this run: 94, 0)
ERROR: Failed: ArithmeticOps_DivisionByZero
INFO: Saved test case in file `out/60f5f7e58fb4283ccc6ed4baa07b80ae09300258.fail`
CRITICAL: fuzztest_deepstateharness.cpp(24): Divisor cannot be 0! (dividend and divisor values for this run: 3, 0)
ERROR: Failed: ArithmeticOps_DivisionByZero
INFO: Saved test case in file `out/848f90b1d99a1491324071faf2aa60ce44de58f2.fail`

...

CRITICAL: fuzztest_deepstateharness.cpp(24): Divisor cannot be 0! (dividend and divisor values for this run: 29, 0)
ERROR: Failed: ArithmeticOps_DivisionByZero
INFO: Saved test case in file `out/84dade03325d816bd685e529b6fccb34f0ecfef2.fail`
CRITICAL: fuzztest_deepstateharness.cpp(24): Divisor cannot be 0! (dividend and divisor values for this run: 52, 0)
ERROR: Failed: ArithmeticOps_DivisionByZero
INFO: Saved test case in file `out/cc0d63cab435e99435359b316833d3990966c207.fail`
CRITICAL: fuzztest_deepstateharness.cpp(24): Divisor cannot be 0! (dividend and divisor values for this run: 40, 0)
ERROR: Failed: ArithmeticOps_DivisionByZero
INFO: Saved test case in file `out/936256140c305b1eb25769e14c0e286bcce12f52.fail`
CRITICAL: fuzztest_deepstateharness.cpp(24): Divisor cannot be 0! (dividend and divisor values for this run: 17, 0)
ERROR: Failed: ArithmeticOps_DivisionByZero
INFO: Saved test case in file `out/c0843a650e1b72f46c51cc1f137051e116540b90.fail`
CRITICAL: fuzztest_deepstateharness.cpp(24): Divisor cannot be 0! (dividend and divisor values for this run: 52, 0)
ERROR: Failed: ArithmeticOps_DivisionByZero
INFO: Saved test case in file `out/f263dfa03b181888a475e934b96feaddc3192ba5.fail`
CRITICAL: fuzztest_deepstateharness.cpp(24): Divisor cannot be 0! (dividend and divisor values for this run: 34, 0)
ERROR: Failed: ArithmeticOps_DivisionByZero
INFO: Saved test case in file `out/b54132703e73fe81e42205208d739a45f962bd3f.fail`
INFO: Done fuzzing! Ran 498928 tests (498928 tests/second) with 4988 failed/493940 passed/0 abandoned tests
```
I'm using the default one here (other fuzzing engines would require the executable to be compiled in their own ways<sup>1</sup>) with a maximum runtime of one second and I'm saving the failing tests to a pre-created output directory called 'out'. Each of these will be recorded in a separate file (so the number of such files in that directory should be equal to the number of failing tests, i.e. `ls out/*.fail | wc -l` will return 4988 for my run above).

The files from each run will stack as more fuzzing sessions with the same arguments go by. Thus, you may want to remove those files and start collecting afresh (do something like `cd out && rm -rf *.fail`, which you can also add to my `makefile` under `clean:`, and then run `make` after `make clean` per session). Note that there won't be anything interesting in these files in this case and in general too for other test cases fuzzed through this default fuzzer, which just brute forces through possible input combinations.

It doesn't work differently from say, seeded rng calls (here, those calls would be to generate the dividend and the divisor in the harness that I created) over an infinite loop for going over possible values of a data type. For instance, if I would have used two `Symbolic<uint32_t>`/`symbolic_uint32_t`s for the numbers, going full range (0 to 4,294,967,295) would mean that the desired crash result (denominator/divisor equals 0) will not be found for a very long time, and this is primarily because each value in the supplied range occurs in an approximately equal manner via a cyclic routine (appears to be a linear congruential uniform approach, as used in most random number generators; and then STest uses a linear feedback shift register approach in [bsd_random.c](https://github.com/Anirban166/STest-Demo/blob/89431c3cf43e4b4fd379ad474289beb875b8ebc9/STest/Random/bsd_random.c#L56) - not much of a difference!)

One doesn't need to explicitly look at the code to learn this. For instance, you can observe this just by looking at the metrics of a few repeated runs here - the times the tests failed or the times zero occurred for the divisor from the total is roughly equal to one:(interval range within which the numbers are being generated). For instance, with a range of 0 to 100 that I [set](https://github.com/Anirban166/STest-Demo/blob/89431c3cf43e4b4fd379ad474289beb875b8ebc9/Fuzz/DeepState/fuzztest_deepstateharness.cpp#L23) for my divisor to have possible values from, that ratio would be near about 1:100, or in other words, a zero occurs per 100 generations.

Anyways, using this very basic fuzzer won't get us anywhere new where ordinary type-constrained randomized inputs would take one with a brute force progression. Instead, fuzzers which are able to generate/throw inputs based on some heuristics and progressively iterate the process to find unique/interesting ones are what's the lookout for in general.

One of them is AFL. It takes as input a set of valid and *presumably* interesting inputs (forms a dictionary), altogether known as the 'corpus'. The set of interesting inputs and test cases eventually increases, but you need something to start with, ideally :)

These 'interesting' inputs depend on a metric, which is code coverage in AFL and most other prominent fuzzers (libFuzzer for instance). Any input resulting in new code coverage is taken to be a path to consider for further mutation, and is of exploratory interest. This tends to be a form of self-learning for the fuzzer, but more so importantly, it enables the fuzzer to navigate its way around (as opposed to dumb fuzzers, although `-d` is a thing here :) input generation through active feedback based on this metric. Such guided heuristics enable the subsequent picking of inputs to be directed at finding new bugs/leaks/vulnerabilities from crashes (for instance, unique ones could be identified from a newfound crash signature from a stacktrace) and basically anything that triggers new behaviour and falls under the instrumentation that is defined. 

As for how these inputs get transformed themselves, it depends on both deterministic and probabilistic procedures such as bitflips (basically flipping of n-consecutive bits by XOR'ing them with others), algebraic manipulations, splicing, etc. These are the 'stages' of input mutation, which are repeated over and over again, with each stage possibly having thousands of iterations themselves (just a guess based on the iterations per second shown and the noticeable change in stages visually reflected in the 'stage progress' section of the interface when running the fuzzer).

AFL generates a queue and loads into it each test case/file (which gets distributed to processes branched from Master when forked, with synchronized updates to the queue) that is presented as initial input or corpus. These get mutated by the aforementioned (and others that I don't know of) strategies during the fuzzing process, and the ones which result in a new state transition (as recorded by the instrumentation) get added to the queue.

A caveat here would be that this sort of fuzzing scales real quick (it is not a thorough search or something such as a symbolic execution engine would do as that misses the point here, but it is still quick to keep exploring the search space in a directed manner till eternity) and comes at the cost of millions of writes (apart from `forks()`, file I/O, and all that jazz) in a relatively short amount of time, which can drain your SSD (like `tmpfs`, keeping the output files in virtual memory could potentially help with this). I wish such computationally intensive workloads could be pushed onto GPUs, but AFL only uses CPU cores!

Now back to the test harness here - its a DeepState-specific one that makes use of the static library, so while keeping `-ldeepstate` to link it together, one can use AFL on the executable built from the harness by compiling with the afl version of compilers. For instance, one could either set up the environment variables for the C and C++ compilers to point to the afl versions (`afl-gcc` and `afl-g++` for Linux, `afl-clang` and `afl-clang++` or their `fast` versions for OS X) if running the compilation and linking process via shell, or change those settings in the makefile that I'm using.

For easy commanding, note that DeepState *does* come with python-scripted 'executors' that facilitate the running of fuzzers (and symbolic execution engines as well). Like for AFL, it would be `deepstate-afl ./<afl-compiled_executable>` at the simplest. But my current setup of DeepState on Docker doesn't facilitate the running of these executors (and here's a mention of the same in this [issue comment](https://github.com/trailofbits/deepstate/issues/399#issuecomment-1205607189) from Alex as I see), so I can't vouch for that at the moment. Running AFL on your own does require learning the AFL fuzzer's syntax though (which one might look into anyway when intending to use its arguments while running with DeepState's executors, i.e. by adding `--fuzzer_args ...` for any AFL-specific arguments). To avoid the learning curve that comes while setting up and using such fuzzers has been a core point of using DeepState in the first place, but if you have read and come so far, I'm sure you can make it run and fix any probable errors yourself :)

Steps are straightforward and follow the same agenda as I mentioned above. Start by making the switch to afl-based compilers and build the executable:
```sh
<username>@<systemname>:~/STest-Demo/Fuzz/DeepState$ make      
afl-gcc -Wall -g -c -Iinclude ../../STest/Random/bsd_random.c
afl-cc 2.52b by <lcamtuf@google.com>
afl-as 2.52b by <lcamtuf@google.com>
[+] Instrumented 117 locations (64-bit, non-hardened mode, ratio 100%).
afl-g++ -Wall -g -std=c++11 -Iinclude -no-pie -c ../../STest/Random/Random.cpp
afl-cc 2.52b by <lcamtuf@google.com>
afl-as 2.52b by <lcamtuf@google.com>
[+] Instrumented 25 locations (64-bit, non-hardened mode, ratio 100%).
afl-g++ -Wall -g -std=c++11 -Iinclude -no-pie -c ../../STest/Pick/Pick.cpp
afl-cc 2.52b by <lcamtuf@google.com>
afl-as 2.52b by <lcamtuf@google.com>
[+] Instrumented 1 locations (64-bit, non-hardened mode, ratio 100%).
afl-g++ -Wall -g -std=c++11 -Iinclude -no-pie -c ../../STest/Pick/Pick_default.cpp  
afl-cc 2.52b by <lcamtuf@google.com>
afl-as 2.52b by <lcamtuf@google.com>
[+] Instrumented 3 locations (64-bit, non-hardened mode, ratio 100%).
afl-g++ -Wall -g -std=c++11 -Iinclude -no-pie -c ./fuzztest_deepstateharness.cpp
afl-cc 2.52b by <lcamtuf@google.com>
afl-as 2.52b by <lcamtuf@google.com>
[+] Instrumented 197 locations (64-bit, non-hardened mode, ratio 100%).
afl-g++ -Wall -g -std=c++11 -Iinclude -no-pie -ldeepstate -o fuzztest_deepstateharness.bin bsd_random.o Random.o Pick.o Pick_default.o fuzztest_deepstateharness.o
afl-cc 2.52b by <lcamtuf@google.com>
```
Run it using `afl-fuzz` with desired parameters:
```sh
<username>@<systemname>:~/STest-Demo/Fuzz/DeepState$ afl-fuzz -i ./ -o out -- ./fuzztest_deepstateharness.bin -m none # @@
afl-fuzz 2.52b by <lcamtuf@google.com>
[+] You have 8 CPU cores and 2 runnable tasks (utilization: 25%).
[+] Try parallel jobs - see /usr/local/share/doc/afl/parallel_fuzzing.txt.
[*] Checking CPU core loadout...
[+] Found a free CPU core, binding to #0.
[*] Checking core_pattern...
[*] Setting up output directories...
[+] Output directory exists but deemed OK to reuse.
[*] Deleting old session data...
[+] Output dir cleanup successful.
[*] Scanning './'...
[+] No auto-generated dictionary tokens to reuse.
[*] Creating hard links for all input files...
[*] Validating target binary...
[*] Attempting dry run with 'id:000000,orig:Pick.o'...
[*] Spinning up the fork server...
[+] All right - fork server is up.
    len = 7376, map size = 16, exec speed = 996 us
[*] Attempting dry run with 'id:000001,orig:Pick_default.o'...
    len = 9648, map size = 16, exec speed = 902 us
[!] WARNING: No new instrumentation output, test case may be useless.
[*] Attempting dry run with 'id:000002,orig:Random.o'...
    len = 31296, map size = 16, exec speed = 880 us
[!] WARNING: No new instrumentation output, test case may be useless.
[*] Attempting dry run with 'id:000003,orig:bsd_random.o'...
    len = 31736, map size = 16, exec speed = 737 us
[!] WARNING: No new instrumentation output, test case may be useless.
[*] Attempting dry run with 'id:000004,orig:fuzztest_deepstateharness.bin'...
    len = 228632, map size = 16, exec speed = 760 us
[!] WARNING: No new instrumentation output, test case may be useless.
[*] Attempting dry run with 'id:000005,orig:fuzztest_deepstateharness.cpp'...
    len = 2882, map size = 16, exec speed = 698 us
[!] WARNING: No new instrumentation output, test case may be useless.
[*] Attempting dry run with 'id:000006,orig:fuzztest_deepstateharness.o'...
    len = 186216, map size = 16, exec speed = 750 us
[!] WARNING: No new instrumentation output, test case may be useless.
[*] Attempting dry run with 'id:000007,orig:makefile'...
    len = 779, map size = 16, exec speed = 686 us
[!] WARNING: No new instrumentation output, test case may be useless.
[+] All test cases processed.

[!] WARNING: Some test cases are huge (223 kB) - see /usr/local/share/doc/afl/perf_tips.txt!
[!] WARNING: Some test cases look useless. Consider using a smaller set.
[+] Here are some useful stats:

    Test case count : 1 favored, 0 variable, 8 total
       Bitmap range : 16 to 16 bits (average: 16.00 bits)
        Exec timing : 686 to 996 us (average: 801 us)

[*] No -t option specified, so I'll use exec timeout of 20 ms.
[+] All set and ready to roll!


            american fuzzy lop 2.52b (fuzztest_deepstateharness.bin)

┌─ process timing ─────────────────────────────────────┬─ overall results ─────┐
│        run time : 0 days, 0 hrs, 1 min, 42 sec       │  cycles done : 204    │
│   last new path : none yet (odd, check syntax!)      │  total paths : 8      │
│ last uniq crash : none seen yet                      │ uniq crashes : 0      │
│  last uniq hang : none seen yet                      │   uniq hangs : 0      │
├─ cycle progress ────────────────────┬─ map coverage ─┴───────────────────────┤
│  now processing : 0* (0.00%)        │    map density : 0.02% / 0.02%         │
│ paths timed out : 0 (0.00%)         │ count coverage : 1.00 bits/tuple       │
├─ stage progress ────────────────────┼─ findings in depth ────────────────────┤
│  now trying : havoc                 │ favored paths : 1 (12.50%)             │
│ stage execs : 34/256 (13.28%)       │  new edges on : 1 (12.50%)             │
│ total execs : 787k                  │ total crashes : 0 (0 unique)           │
│  exec speed : 1708/sec              │  total tmouts : 0 (0 unique)           │
├─ fuzzing strategy yields ───────────┴───────────────┬─ path geometry ────────┤
│   bit flips : 0/256, 0/248, 0/232                   │    levels : 1          │
│  byte flips : 0/32, 0/24, 0/8                       │   pending : 0          │
│ arithmetics : 0/1788, 0/9, 0/0                      │  pend fav : 0          │
│  known ints : 0/194, 0/657, 0/352                   │ own finds : 0          │
│  dictionary : 0/0, 0/0, 0/0                         │  imported : n/a        │
│       havoc : 0/422k, 0/361k                        │ stability : 100.00%    │
│        trim : 99.99%/167, 0.00%                     ├────────────────────────┘
^C────────────────────────────────────────────────────┘          [cpu000: 29%]

+++ Testing aborted by user +++
[+] We're done here. Have a nice day!
```
Specifying input and output directories (corpus, results) is required, apart from specifying the executable to be attacked. I'm setting the memory limit to `none`, which allows no constraints on the resource usage (`ulimit` returns `unlimited`), and I do this specifically to avoid an OOM fault in the dynamic linker that AFL at times could report due to the default cap (50 megabytes) being restrictive. Again, I'm running this on a container so the concerns are less.

> To be updated (I'll probably write a blog post on/including this in the long run)