# cmatch

[![LICENSE](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE.md)
[![CONTACT](https://img.shields.io/badge/contact-hello%40michaelsyao.com-blue)](mailto:hello@michaelsyao.com)

Interested in better understanding how [The Match&trade;](https://www.nrmp.org) for fourth-year US medical students works from an algorithmic standpoint? This is a barebone C implementation of the [Roth-Peranson algorithm](https://doi.org/10.1257/aer.89.4.748) for two-sided matching markets, as used in the National Resident Matching Program (NRMP).

## Problem Formulation

Let $A = \{a_1, a_2, \ldots, a_m\}$ be a finite set of **applicants** and $P = \{p_1, p_2, \ldots, p_n\}$ a finite set of **programs**. Each program $p_j$ has an integer **capacity** $q_j \geq 1$.

Each applicant $a_i$ holds a strict, transitive preference ordering $\succ_{a_i}$ over a subset $L_i \subseteq P$ of acceptable programs. Each program $p_j$ holds a strict, transitive preference ordering $\succ_{p_j}$ over a subset $R_j \subseteq A$ of acceptable applicants.

A **matching** is defined as a a correspondence $\mu : A \to 2^P$ and $\mu^{-1} : P \to 2^A$ satisfying

$$
\mu(a_i) \in L_i \cup \{\emptyset\}, \quad |\mu(a_i)| \leq 1 \quad \forall\, a_i \in A
$$

$$
\mu^{-1}(p_j) \subseteq R_j, \quad |\mu^{-1}(p_j)| \leq q_j \quad \forall\, p_j \in P
$$

$$
\mu(a_i) = p_j \iff a_i \in \mu^{-1}(p_j)
$$

An unmatched applicant satisfies $\mu(a_i) = \emptyset$.

Not all matches are desirable - for example, having all applicants go unmatched in a particular year would be a valid matching correspondence, but is obviously sub-optimal. We define a "desirable" match based on its **stability**: a particular matching $\mu$ is **stable** if it contains no **blocking pair**. A pair $(a_i, p_j)$ with $a_i \in R_j$ and $p_j \in L_i$ is blocking if both of the following hold:

1. **Applicant prefers $p_j$:**
$$
p_j \succ_{a_i} \mu(a_i) \quad \text{(or } \mu(a_i) = \emptyset\text{)}
$$

2. **Program prefers $a_i$:** either the program has unused capacity (i.e., $|\mu^{-1}(p_j)| < q_j$)
or $a_i$ is ranked above the program's current worst match: i.e.,
$$
a_i \succ_{p_j} a^* \quad \text{where } a^* = \underset{a' \in \mu^{-1}(p_j)}{\arg\max}\, \text{rank}_{p_j}(a')
$$

Here $\text{rank}_{p_j}(a)$ denotes the 0-based position of $a$ in $p_j$'s preference list (lower is better).

[Gale and Shapley, 1962](https://doi.org/10.2307/2312726) previously showed that stable matching always exists.

## The Algorithm

The implementation uses **applicant-proposing deferred acceptance**. Applicants hold the proposal initiative; programs tentatively hold offers and may upgrade.

**Notation.** Let $\text{next}(a_i)$ denote the next unproposed program on $a_i$'s rank list, advancing $a_i$'s proposal pointer by one each call.

---

**Algorithm 1** (Applicant-Proposing Deferred Acceptance)

**Input:** Sets $A$, $P$; preference lists $\{L_i\}$, $\{R_j\}$; capacities $\{q_j\}$.  
**Output:** Stable matching $\mu$.

**Initialize:**
$$
\mu(a_i) \leftarrow \emptyset \;\forall\, a_i \in A, \qquad
\mu^{-1}(p_j) \leftarrow \emptyset \;\forall\, p_j \in P, \qquad
Q \leftarrow A
$$

**Iterate** while $Q \neq \emptyset$:

1. Remove any $a \in Q$.
2. Let $p \leftarrow \text{next}(a)$.  If no such $p$ exists, skip (a remains unmatched).
3. If $a \notin R_p$, re-enqueue $a$ and go to 1.  *(Program does not rank applicant.)*
4. **If** $|\mu^{-1}(p)| < q_p$ &nbsp;&nbsp; *(open slot)*:
$$
\mu^{-1}(p) \leftarrow \mu^{-1}(p) \cup \{a\}, \quad \mu(a) \leftarrow p
$$
5. **Else** *(program is full)*: let
$$
a^* = \underset{a' \in \mu^{-1}(p)}{\arg\max}\, \text{rank}_p(a')
$$
   - **If** $\text{rank}_p(a) < \text{rank}_p(a^*)$ &nbsp;&nbsp; *(a is preferred over worst current match)*:
$$
\mu^{-1}(p) \leftarrow \bigl(\mu^{-1}(p) \setminus \{a^*\}\bigr) \cup \{a\}
$$
$$
\mu(a) \leftarrow p, \quad \mu(a^*) \leftarrow \emptyset, \quad Q \leftarrow Q \cup \{a^*\}
$$
   - **Else**: re-enqueue $a$.

---

### Termination

Each call to $\text{next}(a_i)$ irrevocably advances $a_i$'s proposal pointer. The total number of proposals across all applicants is therefore bounded by

$$
\Pi = \sum_{i=1}^{m} |L_i|
$$

Since $\Pi$ is finite and strictly decreases the remaining proposal budget, the algorithm terminates in at most $\Pi$ iterations.

### Correctness

[Gale–Shapley, 1962](https://doi.org/10.2307/2312726) showed that **Algorithm 1** terminates, produces a stable matching, and is also *optimal*. That is, every applicant is matched to the best program reachable by that applicant across *all* stable matchings.

Formally, let $\mathcal{S}$ denote the set of all stable matchings. Then for the output $\mu$ of Algorithm 1,

$$
\mu(a_i) \succeq_{a_i} \mu'(a_i) \quad \forall\, \mu' \in \mathcal{S},\; \forall\, a_i \in A
$$

By the **rural hospitals theorem** [Roth, 1986](https://doi.org/10.2307/1913160), the set of unmatched applicants and the set of unfilled program slots are identical across every stable matching.

### Complexity

The time bound is $O\!\left(\displaystyle\sum_{i=1}^{m} \|L_i\| \cdot n\right)$ and follows from the $\Pi$ iteration limit and the $O(n)$ linear scan used to look up applicants and programs by UUID. With hash maps the per-proposal cost drops to $O(1)$ amortised, giving an overall bound of $O(\Pi)$.

The space bound $O\!\left(\displaystyle\sum_{i=1}^{m} \|L_i\| + \displaystyle\sum_{j=1}^{n} q_j\right)$ accounts for the rank lists and the matched-list arrays. The proposal queue has a non-dominant contribution to the space complexity.

## Build

Requires a C11-compatible compiler (e.g. `gcc`) and `make`.

```sh
make          # produces ./cmatch
make test     # builds and runs the test suite
```

## Input Format

We provide a helper script `generate.py` to help generate sample inputs for our program. Run `python generate.py -h` for more information (compatible with any version of Python 3).

### Applicants File

One applicant per line:

```
<uuid> <prog_idx_1> <prog_idx_2> ...
```

- `<uuid>` — unique string identifier for the applicant (no spaces)
- `<prog_idx_*>` — 0-based indices into the programs file, listed in descending preference order

Example:
```
abc-123 2 0 4 1
def-456 0 3
```

### Programs File

One program per line:

```
<capacity> "<name>" <uuid_1> <uuid_2> ...
```

- `<capacity>` — integer number of available slots
- `<name>` — quoted program name (may contain spaces)
- `<uuid_*>` — applicant UUIDs in descending preference order

Example:
```
3 "General Hospital" abc-123 def-456
1 "Regional Medical Center" def-456
```

## Usage

```sh
./cmatch -a <applicants_file> -p <programs_file>
```

The program writes one result per applicant to `stdout`:

```
<uuid-0>: <program_name>
<uuid-1>: unmatched
```

Example:

```sh
./cmatch -a applicants.txt -p programs.txt
```

A [Docker file](./Dockerfile) has been provided to specify a working image to support our implementation.

## Limitations

Couples matching and sequential matching (e.g., into preliminary and transitional programs) is not supported. The real Roth–Peranson algorithm used by the NRMP extends deferred acceptance to handle couples who submit a joint rank-order list of *program pairs* and must be placed together (or go unmatched together). This implementation treats every applicant independently and has no notion of coupled proposals. Note that the introduction of couples matching makes it possible for no stable matching to exist ([Kojima, Pathak, Roth, 2013](https://doi.org/10.1093/qje/qjt019)).

We also do not implement a "post-match scramble." The real NRMP runs a [Supplemental Offer and Acceptance Program (SOAP)](https://www.nrmp.org/residency-applicants/soap) after the main match, allowing unmatched applicants and unfilled programs to negotiate directly. In contrast, our implementation produces a single stable matching with no post-match phase.

Finally, our implementation assumes strict, transitive orderings for both applicants and programs. Indifferences (ties) in a rank list are not detected and will produce arbitrary but internally consistent behavior depending on list order.

## References

[1] Roth AE and Peranson E. The redesign of the matching market for American physicians: some engineering aspects of economic design. *Am Econ Rev*, 89(4):748-80. (1999). doi: [10.1257/aer.89.4.748](https://doi.org/10.1257/aer.89.4.748)

[2] Gale D and Shapley LS. College admissions and the stability of marriage. *Am Math Monthly*, 69(1):9–15. (1962). doi: [10.2307/2312726](https://doi.org/10.2307/2312726)

[3] Roth AE. On the allocation of residents to rural hospitals: A general property of two-sided matching markets. *Econometrica*, 54(2):425-7. (1986). doi: [10.2307/1913160](https://www.jstor.org/stable/1913160)

[4] Kojima F, Pathak PA, Roth AE. Matching with couples: Stability and incentives in large markets. *Quarterly J Econ*, 128(4):1585-632. (2013). doi: [10.1093/qje/qjt019](https://doi.org/10.1093/qje/qjt019)

## License

This repository is MIT licensed (see [LICENSE](./LICENSE)).