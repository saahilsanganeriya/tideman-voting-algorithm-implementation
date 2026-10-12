# Tideman voting in C

A command-line program that runs a ranked-pairs (Tideman) election and sets the result beside other voting
rules. It started as the Tideman problem in Harvard's CS50 (Problem Set 3) and grew into one C file of
1,323 lines. Beyond the problem set, it can compare voting rules, simulate random elections, and save and
load elections.

Each voter ranks every candidate. The program then reports:

- the ranked-pairs winner, with the locked graph and every head-to-head margin;
- the Condorcet winner, if there is one, and a check for three candidates that beat each other in a circle;
- the Borda count winner and an estimated plurality winner (see below);
- a 0–10 "competitiveness" figure built from the margins.

## The methods, as the code implements them

**Ranked pairs (Tideman).** For every pair of candidates, count how many voters ranked one above the other
(`record_preferences`). Keep each pair one candidate wins outright (`add_pairs`), sort those wins from the
largest margin to the smallest (`sort_pairs`), then add them to a directed graph in that order, skipping any
win whose loser can already reach its winner by following arrows, since locking it would close a cycle
(`lock_pairs`). The winner is the candidate with no arrow pointing at them (`print_winner`). If one
candidate beats every other head to head, ranked pairs always picks them.

**Condorcet winner.** A candidate who beats every other candidate head to head. There may be none: with
three candidates, A can beat B, B beat C and C beat A. The paradox check looks for exactly that pattern
among every set of three candidates.

**Borda count.** On each ballot a candidate scores one point for every candidate ranked below them, so first
of three scores 2 and last scores 0. The program sums these from the pairwise table, which gives the same
totals as scoring each ballot.

**Plurality (estimated).** Plurality should count first choices. The program keeps only the pairwise table,
not the ballots, so it estimates first-place votes as a candidate's pairwise total divided by the number of
opponents. That is the Borda score scaled down, so the "plurality" and Borda winners almost always match. In
the demo below the estimate even ties Alice and Bob at 5, while the true first-choice counts are 4, 3 and 2.

**Competitiveness.** `10 - 20 × (average winning margin ÷ number of voters)`, clamped to 0–10. Smaller
margins give a higher score. It is a figure this program defines, not a standard measure.

## Build and run

```bash
gcc -o tideman tideman.c
```

It needs only the C standard library. Small `scanf` helpers stand in for CS50's `get_int` and `get_string`,
so the CS50 library is not needed. Built and run with gcc 13.3 on Ubuntu 24.04, where gcc prints one
format-overflow warning about the date string.

```bash
./tideman Alice Bob Charlie      # election name, number of voters, then each voter's ranking
./tideman -load demo_election.dat  # reprint the results of a saved election
./tideman -i                     # menu: load an election, or simulate elections
./demo.sh                        # scripted 9-voter election; build ./tideman first
```

Candidate names are single words (at most 15 candidates). An unknown name at a rank prompt asks again. At the
end the program offers to save the election to a file.

`demo.sh` pipes a fixed election into `./tideman` and writes `test_output.txt` (the full results),
`demo_election.dat` (the saved election), `load_test.txt` and `interactive_test.txt`.

### Simulation

Simulation runs from the menu and uses the candidates of a loaded election (three or more). After
`./demo.sh` has written `demo_election.dat`:

```bash
printf '2\ndemo_election.dat\n3\n1000\n4\n' | ./tideman -i
```

Each simulated election has 10 to 100 voters, and each voter's ranking is a random shuffle of the candidates.
For every election it records the winner under each method and whether a Condorcet winner exists. At the end
it prints how often each candidate won under each method, the share of elections with no Condorcet winner, and
the average competitiveness. The "agreement" lines compare how often each method picked each candidate across
all runs, not whether two methods picked the same winner in the same election, so they are an upper bound.
The random generator is seeded from the clock, so numbers change from run to run.

## Example: the demo election

`demo.sh` casts these nine ballots:

| Voter | 1st | 2nd | 3rd |
|---|---|---|---|
| 1 | Alice | Bob | Charlie |
| 2 | Alice | Bob | Charlie |
| 3 | Bob | Alice | Charlie |
| 4 | Charlie | Alice | Bob |
| 5 | Alice | Charlie | Bob |
| 6 | Bob | Charlie | Alice |
| 7 | Alice | Bob | Charlie |
| 8 | Bob | Alice | Charlie |
| 9 | Charlie | Bob | Alice |

Head to head, Alice beats Bob 5–4 (voters 1, 2, 4, 5, 7), Alice beats Charlie 6–3 and Bob beats Charlie 6–3.
Sorted by margin, the program locks Alice → Charlie, Bob → Charlie, then Alice → Bob. None closes a cycle,
nothing points at Alice, and she wins. Part of `test_output.txt`:

```
═══ TIDEMAN METHOD WINNER ═══
🏆 TIDEMAN WINNER: Alice

═══ VOTING METHOD COMPARISON ═══
Plurality Winner: Alice
Borda Count Winner: Alice
Condorcet Winner: Alice

═══ PREFERENCE MATRIX ═══
(Row candidate preferred over column candidate)

               Alice     Bob Charlie
Alice              -       5       6
Bob                4       -       6
Charlie            3       3       -

═══ LOCKED PAIRS GRAPH ═══
(Arrows show locked preferences)

Alice → Bob (margin: 1)
Alice → Charlie (margin: 3)
Bob → Charlie (margin: 3)

═══ MARGIN ANALYSIS ═══
Alice beats Charlie by 3 votes (33.3%)
Bob beats Charlie by 3 votes (33.3%)
Alice beats Bob by 1 votes (11.1%)
Smallest margin: 1 votes
Largest margin: 3 votes

Election Competitiveness Score: 4.81/10.0
```

The Borda totals are Alice 11, Bob 10 and Charlie 6. The competitiveness is 10 − 20 × ((1 + 3 + 3) ÷ 3 ÷ 9) = 4.81.

## Saved election format

Plain text: the line `ELECTION_DATA`, the election name, the date, the number of candidates, the number of
voters, one candidate name per line, then the pairwise table one row per line. Loading rebuilds the pairs and
the locked graph from the table. The ballots themselves are not saved.

## Known limits

- Running fewer than 10 simulations crashes with a floating-point exception: the progress check divides by
  `num_simulations / 10`, which is 0.
- Menu option 1 ("Run New Election") is not implemented; it prints a note and returns to the menu.
- The first character of the election name is dropped (`tideman.c` line 441 reads one character before the
  name), so "Student Council Election 2025" is saved as "tudent Council Election 2025".
- A ballot may name the same candidate twice; the program does not reject it.
- The functions ending in `_optimized` (merge sort and a depth-first cycle check) are never called, so
  "Operations performed" in the performance section is always 0. The complexity lines printed there are
  formulas, not measurements.

## Files

| File | What it is |
|---|---|
| `tideman.c` | the whole program |
| `demo.sh` | the scripted demo election and smoke checks |
| `LICENSE` | MIT |

## Credits and licence

Written by Saahil Sanganeriya. The starting point is CS50's Tideman distribution code: the outline of `main`
(usage check, ballot loop, then the add, sort and lock calls), the global tables, and the empty `vote`,
`record_preferences`, `add_pairs`, `sort_pairs`, `lock_pairs` and `print_winner` with their comments. CS50 course materials are licensed
[CC BY-NC-SA 4.0](https://cs50.harvard.edu/x/license/). Everything else is released under the MIT licence in
[`LICENSE`](LICENSE).
