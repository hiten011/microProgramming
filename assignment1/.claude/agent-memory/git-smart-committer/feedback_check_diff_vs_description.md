---
name: check-diff-vs-description
description: Always verify the caller's description of a diff against the actual git diff before grouping commits — narrated summaries can be stale or wrong about scope
metadata:
  type: feedback
---

When asked to commit "pending work," the natural-language description of what changed (given by the user or a launching agent) may not match the actual diff. In one case the description said a main.c change was "just a comment renumbering fix" plus "deleting an already-empty file", implying the LFSR/colour-sim code already lived in main.c before this session. The actual `git diff HEAD` showed the opposite: the entire code block (globals + 5 functions) was being freshly added to main.c in this diff, moved from assignment_1.c which was being deleted in the same change — i.e. a real code-relocation refactor, not a no-op.

**Why:** Trusting the narration would have produced a misleading commit message (understating the change as a no-op comment fix) and wrongly split unrelated hunks together, or missed that assignment_1.h's new prototypes (Set_Truth_RGB/Set_Sensor_RGB) were directly tied to the code move, not an unrelated concern.

**How to apply:** Before grouping/committing, always run `git diff HEAD` (or equivalent) on every changed file yourself and let the actual hunks decide the grouping and commit message content — use the caller's description only as context/hints, not as ground truth. When a file has multiple unrelated hunks (e.g. a real code move + an unrelated comment-numbering fix in the same file), split via extracting individual hunks into patch files and `git apply --cached <hunk>.diff` rather than committing the whole file as one unit. See [[stm32-assignment1-repo]].
