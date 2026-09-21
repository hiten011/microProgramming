---
name: stm32-assignment1-repo
description: Repo layout and workflow notes for the STM32C031 bare-metal assignment1 firmware repo
metadata:
  type: project
---

Repo root for git purposes is one level above the `assignment1/` project folder — git status/diff paths are prefixed `assignment1/Src/...`, `assignment1/cmake/...` etc. There is a sibling `../.vscode/` directory that sits outside the git repo entirely (untracked, shows in `git status` as `?? ../.vscode/`); never stage or commit it.

The `Src/assignment1/` module (item-colour + LFSR fault-simulation logic: `ItemColour_t`, `Apply_LFSR_Fault`, `Generate_Next_Item`, `Set_Truth_RGB`, `Set_Sensor_RGB`, `SysTick_Init`) has gone back and forth between living in its own `assignment_1.c`/`.h` pair and being inlined directly into `Src/main.c`, across several commits (see `9f44a0a refactor(assignment1): extract LFSR sim module` vs. the later revert-like move back into main.c). `assignment_1.h` is kept either way as the shared header for the `ItemColour_t` enum and prototypes, included by main.c. Don't assume the header/module split is stable — re-check current state before reasoning about where this logic lives. See [[check-diff-vs-description]].

Commit convention used in this repo: Conventional Commits with scope, e.g. `refactor(assignment1): extract LFSR sim module`, `fix(stm_helper): fix GPIO overload, EXTI switch`, `feat(main): finish init() and add state machine`. Subject lines are terse (well under 50 chars), generated via the `caveman:caveman-commit` skill.
