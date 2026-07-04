# Introduction to Algorithms and Data Structures — Flight Management System

A **flight management system** written in C, built for the **Introdução aos Algoritmos e Estruturas de Dados** (Introduction to Algorithms and Data Structures / IAED) course at **Instituto Superior Técnico (IST)**, University of Lisbon, 2021/22.

The program is an interactive, command-driven system for managing airports, daily commercial flights, and seat reservations, with a simulated system clock. It was developed in two stages — the second building on the first — and the code here is the **final version**, implementing the full command set of both project phases.

Being an algorithms-and-data-structures course, the emphasis is on choosing appropriate data structures, implementing algorithms (notably sorting) **from scratch**, and — in the second phase — correct **dynamic memory management** with no leaks. It compiles cleanly under the course's strict flags (`-Wall -Wextra -Werror -ansi -pedantic`), passes the provided public tests, and is valgrind-clean (0 bytes in use at exit).

## What it does

The program reads single-character commands from standard input and writes results to standard output. Time is simulated: the clock starts at 1 January 2022 and only advances when instructed.

| Command | Action |
|---------|--------|
| `q` | Quit (freeing all allocated memory first) |
| `a` | Add an airport (3-letter ID, country, city) |
| `l` | List airports — alphabetically, or a given subset in argument order |
| `v` | Add a flight, or list all flights in creation order |
| `p` | List a given airport's departures, ordered by date/time |
| `c` | List a given airport's arrivals, ordered by arrival date/time |
| `t` | Advance the system date |
| `r` | Add a reservation, or list a flight's reservations (lexicographic order) |
| `e` | Delete a flight (and its reservations) or a single reservation |

Each command validates its input and reports the appropriate error (invalid ID, duplicate airport, invalid flight code, `flight already exists`, `invalid date`, `too many reservations`, etc.), matching the assignment specification exactly.

## Data structures & algorithms

This is where the course's learning goals show up:

- **Airports** — a fixed-size array (`MAX_AERO = 40`) of structs, looked up by linear search on the 3-letter ID.
- **Flights** — a fixed-size array (up to `MAX_VOOS = 30000`) of structs. Each flight keeps a pointer back to its departure airport's flight counter, so counts stay consistent when flights are deleted.
- **Reservations** — a dynamically allocated **singly linked list**, since the number and length of reservation codes are unbounded. New reservations are inserted **in sorted (lexicographic) order** (`insere_ordenado`) so listing is a simple traversal.
- **Sorting from scratch** — `qsort` was explicitly forbidden, so ordering is done with a hand-written **bubble sort** (with an early-exit optimisation): alphabetical airport listing and chronological ordering of departures/arrivals.
- **Date & time arithmetic** — dates and times are compared by encoding them as integers via macros (`data_menor`, `hora_menor`, `eh_anterior`, …); arrival date/time is computed from departure time plus duration, handling minute→hour→day and month/year rollover.
- **Dynamic memory management** — reservation nodes and their variable-length code strings are `malloc`'d and `free`'d; deleting a flight cascades to free all its reservations, and quitting frees everything. Verified leak-free with valgrind.
- **Manual input parsing** — commands are dispatched with `getchar`, and `scanf` format strings parse the structured arguments; the `l`/`v` commands distinguish "list all" from "operate on arguments" by peeking at the next character.

Only the standard-library headers permitted by the assignment are used (`stdio.h`, `stdlib.h`, `string.h`).

## Building & running

```bash
gcc -Wall -Wextra -Werror -ansi -pedantic -o proj proj.c
./proj < input.txt > output.txt
```

## Testing

The `tests/` directory contains 15 matched `.in` / `.out` cases and a `Makefile`. To run them:

```bash
cd tests
make            # runs the program against every test and diffs the output
make valgrind   # runs the tests under valgrind to check for memory leaks
```

Or manually:

```bash
./proj < tests/test01.in | diff - tests/test01.out
```

### Example

```
a LIS portugal Lisboa
a OPO portugal Porto
v TP123 LIS OPO 15-03-2022 09:00 01:30 100
p LIS
```

adds two airports and a flight, then lists Lisbon's departures.

## Repository layout

```
.
├── proj.c            # the complete program (all commands)
├── p1.pdf
├── p2.pdf
├── tests/            # 15 public test cases (.in / .out) + Makefile
└── README.md
```
