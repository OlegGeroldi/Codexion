*This project has been created as part of the 42 curriculum by \<login\>.*

# Codexion

## Description

Codexion is a concurrency simulation. Coders sit around a shared table
and take turns compiling, debugging and refactoring. Compiling requires
two USB dongles held at once — the one on the coder's left and the one on
their right — and there are exactly as many dongles as coders, so
neighbours can never compile at the same time.

Every coder must start compiling again within `time_to_burnout`
milliseconds of the last time they started, or they burn out and the run
ends. Coders do not communicate and cannot tell how close anyone else is
to burning out.

The program is the dining philosophers problem with three additions that
make it a scheduling problem rather than a synchronisation exercise:

- **A cooldown.** A released dongle is unavailable for
  `dongle_cooldown` milliseconds. It is a resource that is neither held
  nor available, which no amount of mutex discipline can express on its
  own.
- **An explicit arbitration policy.** When several coders want the same
  dongle, `fifo` grants it to whoever asked first and `edf` to whoever
  will burn out soonest. The decision is made by the program, not by
  whichever thread the kernel happens to wake.
- **A priority queue written by hand.** Each dongle owns a binary
  min-heap of pending requests. Changing the whole scheduling policy of
  the program means changing one comparison function.

Each coder is a thread; each dongle is a mutex, a condition variable and
a queue; and a separate monitor thread ends the run — either because
somebody burned out, or because every coder reached
`number_of_compiles_required` compiles.

## Instructions

Requirements: a C compiler and `make`. The mandatory program uses only
the functions the subject allows and links nothing but `pthread`.

### Build

```sh
make
```

Builds `./codexion`. Objects go to `obj/`, and running `make` twice
performs no work the second time.

| Target | Effect |
|--------|--------|
| `make` / `make all` | Build `./codexion`. |
| `make clean` | Remove the object files. |
| `make fclean` | Remove the objects, the binary and the test binary. |
| `make re` | `fclean` then `all`. |
| `make heaptest` | Build and run the priority queue's unit test. |
| `make test` | Build, run the unit test, then run the Python suite. |
| `make play` | Open the teaching shell (see below). |

### Run

```sh
./codexion number_of_coders time_to_burnout time_to_compile \
           time_to_debug time_to_refactor number_of_compiles_required \
           dongle_cooldown scheduler
```

All eight arguments are mandatory. Durations are in milliseconds, and
`scheduler` is exactly `fifo` or `edf`.

| Argument | Meaning |
|----------|---------|
| `number_of_coders` | Number of coders, and of dongles. 1 to 1000. |
| `time_to_burnout` | Milliseconds from the start of a compile to burning out. At least 1. |
| `time_to_compile` | How long a compile holds both dongles. |
| `time_to_debug` | How long debugging takes. |
| `time_to_refactor` | How long refactoring takes; a coder reaches for the dongles again immediately afterwards. |
| `number_of_compiles_required` | When every coder reaches this, the run stops successfully. |
| `dongle_cooldown` | How long a released dongle stays unavailable. |
| `scheduler` | `fifo` or `edf`. |

Examples:

```sh
./codexion 5 800 200 200 0 7 0 edf
./codexion 1 800 200 200 200 5 0 fifo
./codexion 4 2000 200 200 0 5 300 fifo
```

Anything invalid — the wrong number of arguments, a negative number, a
non-integer, an unknown scheduler — is refused before the simulation
starts. The refusal goes to stderr, nothing is written to stdout, and the
exit code is 1:

```
$ ./codexion 5 800 200 200 0 5 0 rr
codexion: scheduler must be 'fifo' or 'edf'
usage: codexion number_of_coders time_to_burnout ...
```

A run that finishes exits 0 whether it ended on the quota or on a
burnout: both are the program working.

### Output

Five messages, and nothing else on stdout:

```
0 1 has taken a dongle
1 1 has taken a dongle
1 1 is compiling
201 1 is debugging
401 1 is refactoring
1204 3 burned out
```

Every `is compiling` is preceded by exactly two `has taken a dongle`
lines. A burnout is the last line of the run and is printed within 10 ms
of the moment it happened.

## Blocking cases handled

### Deadlock — Coffman's four conditions

A deadlock requires all four of these at once, so preventing any one of
them prevents it:

| Condition | Status in this program |
|-----------|------------------------|
| Mutual exclusion | Present, and required: a dongle is held by one coder at a time. |
| Hold and wait | Present: a coder holds its lower dongle while waiting for the higher one. |
| No preemption | Present: nothing takes a dongle back by force. |
| **Circular wait** | **Broken.** |

Circular wait is broken by a global order on the resources. The dongles
are numbered, and every coder takes the lower-numbered of its two first
(`src/coder_pair.c`, `src/coder_dongles.c`). A cycle would need some
coder to hold a high number while waiting for a low one, and under this
rule nobody ever does — follow any chain of blocked coders and the
numbers strictly increase, so the chain cannot close on itself.

The naive alternative — everyone takes their left dongle, then their
right — produces exactly that cycle in a ring, and it produces it rarely
enough to pass casual testing.

The same technique is used for the two mutexes that are ever nested:
`log_lock` is always taken before `stop_lock`, and no path takes them the
other way round.

### Starvation and liveness

Deadlock and starvation are different failures: a deadlocked program
makes no progress at all, a starving one makes plenty — just not for one
particular coder. The subject requires liveness under `edf`, and two
mechanisms provide it.

**Explicit arbitration.** Without an arbiter, a released dongle goes to
whichever waiting thread the kernel wakes, and nothing stops the same
unlucky coder losing every time. Each dongle owns a priority queue, and a
request is granted only when it is at the head of that queue
(`can_grant` in `src/dongle_wait.c`). The policy chooses, not the
scheduler.

**Announce before claiming.** This is the subtler half, and the first
version of this project got it wrong. A coder needs two dongles and can
claim them only one at a time. If it joined the second dongle's queue
only after winning the first, its deadline would be invisible to the
second dongle's arbiter for the whole time it spent waiting on the first
— so `edf` would hand that dongle to a coder with far more slack, while
implementing the policy perfectly. The fix is that
`coder_take_both` enqueues the request on **both** dongles before
claiming either, so a coder close to burnout is already the most urgent
entry in the second queue while it is still blocked on the first.

That raises a fair question: a coder can now be at the head of a queue it
cannot yet collect from, blocking a neighbour. It cannot stall the
program. A coder heading dongle *j* is always waiting for some dongle
*i < j*, so following the chain of blocked heads always walks downwards;
the chain is finite and ends at dongle 0, whose head is by definition a
coder for which dongle 0 is the *lower* one — and that coder can always
proceed.

### Cooldown handling

A dongle has three states, not two: held, cooling, and free. `dongle_drop`
sets `free_at = now + dongle_cooldown` and does **not** empty the queue,
so a coder at the head keeps its place while the dongle is unavailable
rather than losing it to whoever asks next.

A cooldown expiring is caused by the clock, not by any thread, so nothing
can signal a condition variable to announce it. Waiting coders therefore
use `pthread_cond_timedwait` with a 0.3 ms slice and re-check
(`wait_for_grant`).

### Precise burnout detection

A coder cannot check its own deadline: the moment it is most likely to
have missed one is the moment it is blocked on a dongle or asleep in a
long debug, and therefore not running at all. A separate monitor thread
sweeps every coder every 0.2 ms — fifty times finer than the 10 ms the
subject allows.

The comparison is `now - last_start > time_to_burnout`, strictly greater:
the subject says a coder burns out if it did not start compiling *within*
`time_to_burnout`, and starting exactly then is starting within it.

The timestamp is measured by the monitor and passed to `log_burnout` by
value, so waiting for the log mutex can delay the announcement — the
subject allows 10 ms of that — without moving the moment it claims the
burnout occurred.

### Log serialisation

One mutex, one module, one `printf`. `src/log.c` is the only file in the
project that writes to stdout, which turns "two messages never interleave"
from a discipline into a property of the code.

The stop flag is read *inside* the log mutex, not before taking it. The
alternative leaves a real race: a coder passes the check, waits for the
mutex behind the monitor, and prints a state change after the burnout
line — which the subject forbids.

### Clean shutdown

Every loop in the program checks a single stop flag; no thread is ever
cancelled or killed. A cancelled thread can stop at a cancellation point
while holding a mutex, and that mutex is then never unlocked. Because
shutdown is cooperative, every thread returns, every `pthread_join`
returns, and `sim_destroy` runs on a program in which nothing is
executing — which is the precondition for destroying a mutex at all.

## Thread synchronization mechanisms

### What is shared, and what protects it

```
shared by every thread    sim->stop        stop_lock
                          sim->seq         seq_lock
                          stdout           log_lock

shared by two coders      dongle.held      dongle.lock
                          dongle.free_at   dongle.lock
                          dongle.waiters   dongle.lock

shared with the monitor   coder.last_start coder.guard
                          coder.compiles   coder.guard

shared by nobody          sim->cfg, sim->start_ms
                          (written before any thread exists)
```

There are three run-wide mutexes rather than one, on purpose. A coder
asking for a sequence number must not have to queue behind a thread
printing a line; the three pieces of state never need to be consistent
with each other, so they do not need a common lock. Where two values
*do* have to agree — `last_start` and `compiles`, read together by the
monitor — they share one guard and are read in one critical section
(`coder_snapshot`).

`sim->cfg` and `sim->start_ms` are read by every thread with no lock at
all, and that is correct rather than sloppy: they are written before
`pthread_create` is called for the first time and never again. A value
with no concurrent writer is not shared state.

### `pthread_mutex_t` — three guarantees, not one

Mutual exclusion is the obvious one. The other two are why a mutex is
used even to read a single `bool`:

- **Visibility.** Everything written before an unlock is guaranteed
  visible to the next thread that locks. Without it, a value written on
  one core can sit in that core's store buffer indefinitely.
- **An optimisation barrier.** The compiler may not move memory accesses
  across the lock. Without synchronisation it may assume no other thread
  writes `sim->stop`, hoist the read out of the loop, and spin on a
  register forever — the program does not read a stale value, it stops
  reading.

`volatile` fixes only the compiler half of that and provides neither
atomicity nor ordering. C11's `_Atomic` would be correct and is not among
the functions the subject allows; `pthread_mutex_lock` is.

**Race prevented — the sequence counter.** `sim_next_seq` reads,
increments and returns inside one critical section. `seq++` is three
operations; two threads doing it unprotected can both read 7 and both
return 7, and duplicate sequence numbers would break the FIFO ordering,
the EDF tie-breaker *and* the identity check a waiting coder uses to
recognise its own request.

**Race prevented — the compile count.** `coder->compiles++` has the same
shape and is protected by `coder->guard`, which the monitor also takes to
read it.

### `pthread_cond_t` — waiting without spinning

A mutex answers "may I touch this?"; a condition variable answers "has it
become what I need?". `pthread_cond_timedwait` releases the mutex and
sleeps *atomically*, which is the property that makes it impossible to
build out of a mutex and a sleep: releasing and then sleeping would let a
signal arrive in between and be lost.

Two rules are followed everywhere:

- **The wait is always inside a `while`, never an `if`.** POSIX permits
  spurious wakeups; a broadcast wakes every waiter although at most one
  may proceed; and the state can change again between the wake-up and
  re-acquiring the mutex. Every wake-up re-asks `can_grant` from scratch.
- **Release always broadcasts, never signals.** A signal wakes an
  arbitrary waiter, and the *policy* must decide who proceeds. With a
  signal the woken thread might not be the one the heap named — it would
  check, fail, sleep again, and the rightful winner would never be woken.

The wait is timed rather than indefinite for two events that no thread
causes: a cooldown expiring, and the stop flag going up (which touches
the run, not any dongle, so it broadcasts nothing). Without the timeout
the program would print a correct log and then hang forever in a join.

### The custom event: request, queue, grant

The coordination between coders and dongles is a hand-written ticket
system rather than a bare lock:

1. `request_make` stamps a request with a globally unique arrival number
   and the coder's current deadline.
2. `dongle_enqueue` pushes it onto **both** dongles' queues.
3. `dongle_claim` blocks until `can_grant` reports that the dongle is
   free, its cooldown has expired, and this request is at the head of the
   queue.
4. `dongle_drop` releases, starts the cooldown and broadcasts.
5. `dongle_withdraw` takes a request back out when a coder gives up;
   removing one that is not there is harmless, which is what lets the
   abort path sweep both dongles with two identical calls.

The monitor coordinates differently, and deliberately: it polls. A
burnout is the *absence* of an event, so no thread is in a position to
signal it. Condition variables are for state a thread changes; time is
not changed by any thread, so time is read.

## Architecture

| File | Responsibility |
|------|----------------|
| `include/heap.h`, `src/heap.c` | The priority queue: init, push, pop, peek, free. |
| `src/heap_ops.c` | Its ordering — `fifo` and `edf` — and the two sift loops. |
| `include/codexion.h` | Every other type in the program. |
| `src/main.c` | Validate, build, run, tear down. |
| `src/args.c`, `src/args_check.c` | Eight strings into a `t_config`, and every way they can be wrong. |
| `src/time_utils.c` | The only file that reads a clock. |
| `src/log.c` | The only file that writes stdout. |
| `src/dongle.c` | One dongle: mutex, condition variable, queue, cooldown. |
| `src/dongle_wait.c` | Making a request, and the loop that waits for it. |
| `src/coder.c` | The life of one thread: compile, debug, refactor. |
| `src/coder_state.c` | The two fields shared with the monitor. |
| `src/coder_pair.c` | Which two dongles a coder reaches, and in which order. |
| `src/coder_dongles.c` | Taking both without building a cycle. |
| `src/monitor.c` | The watchdog that ends the run. |
| `src/sim.c` | The stop flag, the arrival counter, the run's clock. |
| `src/sim_run.c` | Creating and joining threads. |
| `src/sim_setup.c`, `src/sim_free.c` | Building the run, and taking it apart. |

### Design decisions

**The policy is one comparison.** `heap_less` is the only function in the
project that mentions `POLICY_FIFO` or `POLICY_EDF`. Under `fifo` the
arrival number is the only key; under `edf` the earliest deadline wins
and equal deadlines fall back to the arrival number — the tie-breaker the
subject requires, and the reason EDF here is fully deterministic.

**A dongle's state is fields, not its lock.** Conflating "the mutex is
locked" with "the dongle is taken" is the usual shortcut, and it cannot
represent a dongle that is cooling: neither held nor available. The mutex
is held for a few instructions at a time; a dongle can be held for 200 ms
with its mutex unlocked throughout.

**The queue knows nothing about threads.** No function in `heap.c` or
`heap_ops.c` takes a lock, reads a clock or includes `codexion.h`. That
is why `tests/heap_test.c` can link the two files on their own and check
the ordering without creating a thread.

**Everything internal is an absolute timestamp.** `free_at`,
`last_start` and `deadline` are all milliseconds on the same clock, so
comparisons never contain a conversion. Only the log is relative, and it
becomes relative at the moment of printing.

**Errors travel back as return values.** Nothing calls `exit()` from deep
inside, so `sim_destroy` always runs and the program is testable from a
script.

## Testing

```sh
make test
```

Runs the priority queue's C unit test and then the Python suite — 354
tests, about a minute, all of which drive the real binary.

The suite is built on one idea: a concurrency bug is a wrong
*interleaving*, so "I ran it and it looked fine" is a statement about one
sample. Instead, the rules of the subject are written down as invariants
and checked mechanically over every run
(`codexion_shell/checks.py`):

| Check | What it would catch |
|-------|---------------------|
| monotonic timestamps | Two threads printing out of order. |
| burnout ends the run | A line printed after the burnout. |
| phase cycle | A coder debugging twice, or refactoring before compiling. |
| two dongles per compile | A compile announced with one hand, or three. |
| **mutual exclusion** | **Two coders sharing a dongle — a missing mutex, detected from the log alone.** |
| dongle cooldown | A dongle retaken before its cooldown expired. |
| burnout precision | An announcement early, or more than 10 ms late. |
| compiles required | A run that ended without a burnout and without the quota. |
| process behaviour | A non-zero exit, or anything on stderr. |

`tests/heap_test.c` links only `heap.c` and `heap_ops.c` and checks the
FIFO order, the EDF order, the tie-breaker on equal deadlines, and that
withdrawing an absent request is harmless.

Timing checks carry an explicit 25 ms slack, named and explained in
`checks.py`: everything is measured on a wall clock on a machine that is
also doing other things, and without the slack the checks would fail on a
correct program.

Under `valgrind --tool=helgrind` the program runs ten to fifty times
slower, which changes the timings enough that coders burn out — use a
large `time_to_burnout` when checking for races that way.

## The teaching shell

Beyond the mandatory program, this repository carries a `pygame`
front-end. It is an addition and never a replacement: it lives in
`codexion_shell/`, it runs `./codexion` as a subprocess and reads the
same log the subject specifies, and nothing in it can change what the
binary prints.

```sh
make play        # or: python3 play.py
```

It needs `pygame`. Run it from the root of the repository, and build the
binary first.

### Lab

The test interface: every scenario in the catalogue, run on a worker
thread so the window keeps responding, with the verdict of all nine
checks. Seventeen parameter sets, each chosen to exercise one rule —
`lonely` for the one-coder case, `cool_ok` and `cool_fatal` for what a
cooldown costs, `many` for a hundred threads.

| Key | Effect |
|-----|--------|
| `up` / `down` | Pick a scenario. |
| `enter` | Run the selected one. |
| `a` | Run every one. |
| `c` | Cancel the queue. |
| `tab` | Detail: the arguments, what the run shows, every violation in full. |
| `v` | Watch the last run of this scenario. |
| `esc` | Back. |

### Replay

The log drawn as the table it describes: coders in a ring coloured by
phase, dongles between them coloured by held, cooling or free, and a bar
per coder showing how much of its deadline is left. Nothing is
re-simulated — every frame is reconstructed from the printed lines.

| Key | Effect |
|-----|--------|
| `space` | Pause and resume. |
| `left` / `right` | Step one moment back or forward. |
| `+` / `-` | Change the playback speed. |
| `r` | Restart. |
| `esc` | Back. |

### Arbiter

The one place the shell simulates rather than observes, and it says so.
You grant the dongles by hand under the same rules — both neighbouring
dongles at once, cooldowns respected, the same strict burnout comparison
— and `g` shows which coder the configured policy would have chosen.

It exists to make one thing physical: in the C program that decision is
`heap_less`, five lines run thousands of times a second and impossible to
watch.

| Key | Effect |
|-----|--------|
| `1`–`9`, or click a coder | Grant them both dongles. |
| `g` | Let the configured policy choose once. |
| `a` | Autopilot on or off. |
| `space` | Pause and resume. |
| `.` | Step one tick. |
| `+` / `-` | Change the speed. |
| `r` | Restart the match. |
| `esc` | Back. |

### Academy

A training mode built from the project itself: nineteen lessons, one per
file or pair of files, 143 cards and 121 questions.

A card is a walkthrough rather than a summary: the real code in small
pieces, each followed by what it does and why it is written that way.
Across the curriculum that is 476 annotated steps. Two lessons are about
the ideas rather than the files — one on the C the project is written in,
one on races, Coffman's conditions, condition variables and the
difference between deadlock and starvation — and both are taught entirely
on code from these files. A final lesson turns the other eighteen inside
out: the same architecture, presented as the exact order to *write* it
in, with a small throwaway test to pass at every phase before the next
one exists.

The academy can be read in English or in Russian. Code excerpts,
identifiers and error messages stay in English throughout, since that is
what the reader will meet in the project itself; only the explanations
are translated. Progress is keyed by lesson rather than by language, so
switching keeps every score, and a lesson with no translation stays in
English and is marked `[en]` in the list.

Lessons unlock one after another, so the project is met in the order it
has to be understood: the language, then the data structure, then the
theory of deadlock, then the threads, then the monitor that ends them.
Sixty per cent of a lesson, rounded up, unlocks the next. Progress is
stored in `.codexion_progress.json` and the chosen language in
`.codexion_settings.json`; delete those files to start over.

| Key | Effect |
|-----|--------|
| `up` / `down`, `enter` | Pick a lesson and open it. |
| `left` / `right` | Turn the cards back and forth. |
| `up` / `down` | Scroll a long card. |
| `enter` | Continue: next card, answer, next question. |
| `1` to `6`, or click | Pick and unpick answers. |
| `l` | Switch between English and Russian. |
| `r` | Take a finished lesson again. |
| `esc` | Back. |

## Resources

Classic references on the problem and the primitives:

- E. W. Dijkstra, *Hierarchical Ordering of Sequential Processes* (1971)
  — the dining philosophers problem, and the resource-ordering argument
  this project uses to break circular wait.
- E. G. Coffman, M. Elphick, A. Shoshani, *System Deadlocks*, ACM
  Computing Surveys (1971) — the four conditions, and the observation
  that preventing any one of them is enough.
- C. L. Liu, J. W. Layland, *Scheduling Algorithms for Multiprogramming
  in a Hard-Real-Time Environment*, JACM (1973) — earliest deadline
  first, and the optimality result behind the subject's liveness
  requirement.
- *The Little Book of Semaphores*, Allen B. Downey — the clearest
  treatment of why a condition variable wait belongs inside a `while`.
- POSIX / `man` pages: `pthread_create(3)`, `pthread_mutex_lock(3)`,
  `pthread_cond_timedwait(3)`, `gettimeofday(2)`, `clock_gettime(2)`,
  `usleep(3)`.
- Valgrind's helgrind manual — how a data race and a lock-order inversion
  are actually detected.

### How AI was used

AI was used as a reviewer and a rubber duck, not as an author of code
that was then submitted unread. Specifically:

- **Discussing the design before writing it.** Whether to break hold-and-wait
  or circular wait, and why resource ordering is easier to defend than the
  odd/even trick.
- **Finding a real bug by reasoning about the log.** The first version
  claimed both dongles in order but joined each dongle's queue only when
  claiming it, so a coder waiting on its lower dongle was absent from the
  higher dongle's queue and `edf` could not see its urgency. The symptom
  was that `edf` and `fifo` produced identical outcomes on tight
  parameters. The fix — enqueue on both queues before claiming either —
  and the argument that it does not reintroduce a cycle were worked out in
  that discussion and are documented above.
- **Drafting the invariant checks.** Turning each sentence of the subject
  into something checkable from the log, especially the cooldown check,
  which had to reconstruct dongle ownership from a log that never names a
  dongle.
- **Drafting prose.** This README and the academy lessons.

Everything generated was read, compiled, run and tested before being
kept, and the parts of it that are non-obvious — the announce-then-claim
protocol, the lock ordering between `log_lock` and `stop_lock`, the
strict inequality in the burnout comparison — are written up above
precisely because they have to be defended in person.
