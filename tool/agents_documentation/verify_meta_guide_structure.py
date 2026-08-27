#!/usr/bin/env python3

import pathlib
import sys


AGENTS_DOCUMENTATION_DIRECTORY_PATH = pathlib.Path("documentation/agents")
MAINTENANCE_SECTION_HEADING = "## Maintaining This Guide"
UPDATE_BOUNDARY_PARAGRAPH_PREFIX = "Update this guide only "


def main() -> int:
    if len(sys.argv) != 2:
        print("usage: verify_meta_guide_structure.py <repository-root>", file=sys.stderr)
        return 2

    repository_root_path = pathlib.Path(sys.argv[1]).resolve()
    violation_found = False
    for guide_path in sorted(
        (repository_root_path / AGENTS_DOCUMENTATION_DIRECTORY_PATH).rglob("*_GUIDE.md")
    ):
        source_line_list = guide_path.read_text(encoding="utf-8").splitlines()
        if (
            len(source_line_list) < 3
            or not source_line_list[0].startswith("# ")
            or source_line_list[1]
            or source_line_list[2] != MAINTENANCE_SECTION_HEADING
        ):
            violation_found = True
            print(
                f"{guide_path.relative_to(repository_root_path)}:1:1: error: "
                f"a guide must begin with a title followed by '{MAINTENANCE_SECTION_HEADING}' "
                "[memly-guide-maintenance-section]",
                file=sys.stderr,
            )
            continue

        maintenance_section_heading_index = 2
        next_level_two_heading_index = next(
            (
                line_index
                for line_index in range(
                    maintenance_section_heading_index + 1,
                    len(source_line_list),
                )
                if source_line_list[line_index].startswith("## ")
            ),
            len(source_line_list),
        )
        maintenance_section_start_index = maintenance_section_heading_index + 1
        maintenance_section_line_index_range = range(
            maintenance_section_start_index,
            next_level_two_heading_index,
        )
        if not any(
            source_line_list[line_index].strip()
            and not source_line_list[line_index].lstrip().startswith("#")
            for line_index in maintenance_section_line_index_range
        ):
            violation_found = True
            print(
                f"{guide_path.relative_to(repository_root_path)}:"
                f"{maintenance_section_heading_index + 1}:1: error: "
                "the guide maintenance section must contain scope and routing prose "
                "[memly-guide-maintenance-section]",
                file=sys.stderr,
            )
            continue

        if not any(
            source_line_list[line_index].startswith(UPDATE_BOUNDARY_PARAGRAPH_PREFIX)
            and (
                line_index == maintenance_section_start_index
                or not source_line_list[line_index - 1].strip()
            )
            for line_index in maintenance_section_line_index_range
        ):
            violation_found = True
            print(
                f"{guide_path.relative_to(repository_root_path)}:"
                f"{maintenance_section_heading_index + 1}:1: error: "
                f"the guide maintenance section must contain a paragraph beginning "
                f"'{UPDATE_BOUNDARY_PARAGRAPH_PREFIX}...' "
                "[memly-guide-update-boundary]",
                file=sys.stderr,
            )

    return int(violation_found)


if __name__ == "__main__":
    raise SystemExit(main())
