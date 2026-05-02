#!/usr/bin/python3
"""
Generates sample applicant and program input files.

Author(s):
    Michael Yao @michael-s-yao

Licensed under the MIT License. Copyright Michael Yao 2026.
"""
import argparse
import os
import random
import string
from typing import Final, List, Optional, Set


def random_uuids(num_uuids: int, length: int = 12) -> List[str]:
    def gen_uuid() -> str:
        tmp_length = length
        chars = string.ascii_lowercase + string.digits
        parts: List[str] = []
        while tmp_length > 0:
            seg_len = min(4, length)
            parts.append("".join(random.choices(chars, k=seg_len)))
            tmp_length -= seg_len
        return "-".join(parts)

    uuids: List[str] = []
    seen: Set[str] = set([])
    while len(uuids) < num_uuids:
        if (u := gen_uuid()) not in seen:
            seen.add(u)
            uuids.append(u)

    return uuids


def random_program_names(num_programs: int) -> List[str]:
    ADJECTIVES: Final[List[str]] = [
        "University",
        "General",
        "Memorial",
        "Regional",
        "Community",
        "Children's",
        "National",
        "Metropolitan",
        "Central",
        "Baptist",
        "Presbyterian",
        "Methodist",
        "Catholic",
        "Mercy"
    ]

    LOCATIONS: Final[List[str]] = [
        "Philadelphia",
        "Boston",
        "Los Angeles",
        "New York",
        "Chicago",
        "Houston",
        "Seattle",
        "Baltimore",
        "San Francisco",
        "Cleveland",
        "Detroit",
        "Atlanta",
        "Denver",
        "Phoenix",
        "Minneapolis",
        "Miami",
        "Dallas",
        "Portland",
        "Nashville"
    ]

    SUFFIXES: Final[List[str]] = [
        "Hospital",
        "Medical Center",
        "Health System",
        "Hospital and Medical Center",
        "Healthcare"
    ]

    idxs: List[int] = random.sample(
        range(len(ADJECTIVES) * len(LOCATIONS) * len(SUFFIXES)),
        k=num_programs
    )

    def idx_to_name(idx: int) -> str:
        adj = ADJECTIVES[idx // (len(LOCATIONS) * len(SUFFIXES))]
        loc = LOCATIONS[
            (idx % (len(LOCATIONS) * len(SUFFIXES))) // len(SUFFIXES)
        ]
        suf = SUFFIXES[idx % len(SUFFIXES)]
        return f"{adj} {loc} {suf}"

    return list(map(idx_to_name, idxs))


def generate(
    num_applicants: int,
    num_programs: int,
    out_dir: str,
    seed: Optional[int],
    min_cap: int,
    max_cap: int,
    pref_frac: float
) -> None:
    if seed is not None:
        random.seed(seed)

    os.makedirs(out_dir, exist_ok=True)

    appl_uuids = random_uuids(num_applicants)
    prog_names = random_program_names(num_programs)

    prog_indices = list(range(num_programs))
    app_indices = list(range(num_applicants))

    app_prefs: List[List[int]] = []
    for _ in range(num_applicants):
        k = round(
            num_programs * random.uniform(
                pref_frac * 0.5, pref_frac * 1.5
            )
        )
        k = min(max(1, k), num_programs)
        ranked = random.sample(prog_indices, k)
        app_prefs.append(ranked)

    prog_caps: List[int] = [
        random.randint(min_cap, max_cap) for _ in range(num_programs)
    ]
    prog_ranks: List[List[int]] = []
    for _ in range(num_programs):
        k = round(
            random.uniform(pref_frac * 0.5, pref_frac * 1.5) * num_applicants
        )
        prog_ranks.append(
            random.sample(app_indices, min(max(1, k), num_applicants))
        )

    with open(os.path.join(out_dir, "applicants.txt"), "w") as f:
        for i, (uuid, prefs) in enumerate(zip(appl_uuids, app_prefs)):
            if prefs:
                f.write(f"{uuid} {' '.join(str(p) for p in prefs)}\n")
            else:
                f.write(f"{uuid}\n")

    with open(os.path.join(out_dir, "programs.txt"), "w") as f:
        for i, (name, cap, ranks) in enumerate(
            zip(prog_names, prog_caps, prog_ranks)
        ):
            ranked_uuids = " ".join(appl_uuids[a] for a in ranks)
            if ranked_uuids:
                f.write(f'{cap} "{name}" {ranked_uuids}\n')
            else:
                f.write(f'{cap} "{name}"\n')


def main():
    parser = argparse.ArgumentParser(
        description="Generate sample input files for the match."
    )
    parser.add_argument(
        "-a",
        "--applicants",
        type=int,
        required=True,
        help="Number of applicants to generate."
    )
    parser.add_argument(
        "-p",
        "--programs",
        type=int,
        required=True,
        help="Number of programs to generate."
    )
    parser.add_argument(
        "-o",
        "--output-dir",
        required=True,
        help="Directory to write applicants.txt and programs.txt."
    )
    parser.add_argument(
        "--seed",
        type=int,
        default=None,
        help="Random seed for reproducibility.",
    )
    parser.add_argument(
        "--min-capacity",
        type=int,
        default=1,
        help="Minimum program capacity (default: 1).",
    )
    parser.add_argument(
        "--max-capacity",
        type=int,
        default=10,
        help="Maximum program capacity (default: 10).",
    )
    parser.add_argument(
        "--pref-fraction",
        type=float,
        default=0.4,
        help=(
            "Expected fraction of the other side each participant ranks "
            "(default: 0.4)."
        ),
    )
    args = parser.parse_args()

    if args.applicants < 1:
        parser.error("--applicants must be at least 1")
    if args.programs < 1:
        parser.error("--programs must be at least 1")
    if args.min_capacity < 1:
        parser.error("--min-capacity must be at least 1")
    if args.max_capacity < args.min_capacity:
        parser.error("--max-capacity must be >= --min-capacity")
    if not (0.0 < args.pref_fraction <= 1.0):
        parser.error("--pref-fraction must be in (0, 1]")

    generate(
        num_applicants=args.applicants,
        num_programs=args.programs,
        out_dir=args.output_dir,
        seed=args.seed,
        min_cap=args.min_capacity,
        max_cap=args.max_capacity,
        pref_frac=args.pref_fraction,
    )


if __name__ == "__main__":
    main()
