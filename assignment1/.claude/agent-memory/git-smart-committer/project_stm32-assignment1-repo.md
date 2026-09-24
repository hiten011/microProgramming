---
name: stm32-assignment1-repo
description: Repo layout and workflow notes for the STM32C031 bare-metal assignment1 firmware repo
metadata:
  type: project
---

Repo root for git purposes is one level above the `assignment1/` project folder — git status/diff paths are prefixed `assignment1/Src/...`, `assignment1/cmake/...` etc. There is a sibling `../.vscode/` directory that sits outside the git repo entirely (untracked, shows in `git status` as `?? ../.vscode/`); never stage or commit it.

The `Src/assignment1/` module (item-colour + LFSR fault-simulation logic: `ItemColour_t`, `Apply_LFSR_Fault`, `Generate_Next_Item`, `Set_Truth_RGB`, `Set_Sensor_RGB`, `SysTick_Init`) has gone back and forth between living in its own `assignment_1.c`/`.h` pair and being inlined directly into `Src/main.c`, across several commits (see `9f44a0a refactor(assignment1): extract LFSR sim module` vs. the later revert-like move back into main.c). `assignment_1.h` is kept either way as the shared header for the `ItemColour_t` enum and prototypes, included by main.c. Don't assume the header/module split is stable — re-check current state before reasoning about where this logic lives. See [[check-diff-vs-description]].

Commit convention used in this repo: Conventional Commits with scope, e.g. `refactor(assignment1): extract LFSR sim module`, `fix(stm_helper): fix GPIO overload, EXTI switch`, `feat(main): finish init() and add state machine`. Subject lines are terse (well under 50 chars), generated via the `caveman:caveman-commit` skill.

`Src/stm_helper/my_stm_helper.c`'s timer-channel config function was renamed `Timer_ConfigChannel` -> `PWM_Init` (commit `5bb099b`), dropping the `polarity`/`enable_preload` params and the `TimerPolarity_t` enum, and narrowing support to only `TIM_CHANNEL_2`/`TIM_CHANNEL_3` (hardcoded preload+output-enable per channel) since that's all the servo-gate PWM work in `Src/assignment1/` actually uses. Re-check the current signature before assuming either name/shape is current — this function has already been reworked once.

Servo-gate PWM helpers `Gate1_SetWidth`/`Gate2_SetWidth` (TIM3 CH3/PB0 and CH2/PC7) live in `Src/assignment1/my_helpers.c`, writing straight to `TIM3->CCR3`/`CCR2`. The gate-width constants in `my_helpers.h` were renamed from `GATE_ANGLE_0/90/180` to `GATE_ANGLE_AC/NEUTRAL/C` (accept/neutral/reject-conveyor naming, not degrees) — re-check the current names before referencing them. `main.c`'s `init()` now calls `PWM_Init` for both channels then sets both gates to `GATE_ANGLE_C`, replacing raw `TIM3->CCR2/CCR3 = 2500` writes that used to run every iteration of the main loop.

`PWM_Init` in `my_stm_helper.c` was reworked to set `CCxS` (capture/compare selection) and `OCxM` (output compare mode) as two separate bitfield writes (`CC2S`, `OC2M` locals) instead of one combined lookup-table value (`ccmr_val`) written in one shot — same behavior, clearer mapping to the two separate CCMR bitfields. The inline `CCxS=.., OCxM=..` bit-value comments were dropped from the `TimerChannelMode_t` enum in `my_stm_helper.h` since `PWM_Init` no longer needs a full CCxS/OCxM table per mode (only `TIM_MODE_OUTPUT_PWM1` is handled; everything else falls to a default `000,00`).
