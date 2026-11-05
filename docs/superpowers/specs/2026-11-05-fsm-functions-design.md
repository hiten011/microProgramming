# Design: Axis and Tilt FSM State Update Functions

## Intent
Implement three finite state machine (FSM) update functions (`fsm_pitch`, `fsm_roll`, and `fsm_tilt`) for STM32 orientation control to track the operational zone of each axis based on MPU6050 angle calculations.

## Goals
- Add `void fsm_pitch(void)`, `void fsm_roll(void)`, and `void fsm_tilt(void)` in `assignment2/Src/main.c`.
- Update the global state variables `pitch`, `roll`, and `tilt` (`FREE`, `COMPLIANT`, `CLAMPED`) using the hysteresis thresholds from `docs/fsm_diagrams.drawio`.

## Non-Goals & Anti-Goals
- Non-goal: Modifying servo PWM outputs (`set_pitch`, `set_roll`, `set_tilt`) inside these functions (deferred per user confirmation).
- Non-goal: Modifying mode transitions (AUTO / MANUAL).
- Anti-goal: Single-threshold switching that causes state oscillation or chatter around boundary angles.

## Constraints & Thresholds
- **Pitch and Roll Axis FSM** (using `fabsf(angles.pitch)` and `fabsf(angles.roll)`):
  - In `FREE`: if `|angle| >= 30.0f` &rarr; `COMPLIANT`
  - In `COMPLIANT`:
    - if `|angle| < 25.0f` &rarr; `FREE`
    - if `|angle| > 50.0f` &rarr; `CLAMPED`
  - In `CLAMPED`: if `|angle| <= 45.0f` &rarr; `COMPLIANT`
- **Tilt FSM** (using `angles.tilt` / $\alpha \ge 0$):
  - In `FREE`: if `tilt > 20.0f` &rarr; `COMPLIANT`
  - In `COMPLIANT`:
    - if `tilt < 15.0f` &rarr; `FREE`
    - if `tilt >= 70.0f` &rarr; `CLAMPED`
  - In `CLAMPED`: if `tilt <= 65.0f` &rarr; `COMPLIANT`

## Approaches
1. **Approach 1 (Direct Global State Mutators - Selected)**: Three standalone functions reading global `angles` and mutating `pitch`, `roll`, `tilt` enums directly. Simple, explicit, directly fulfills user request.
2. **Approach 2 (Shared Axis Transition Helper)**: Generic transition helper for pitch/roll with wrappers. Avoids threshold duplication, but rejected by user in favor of Approach 1.

## Decisions & Builder Scope
- **Decided by user**: Approach 1, updating only enum states for now.
- **Left to builder**: Exact function placement in [main.c](file:///home/hiten/dev/UNI/microPrograming/assignment2/Src/main.c) (above `TIM3_IRQHandler` / `init`), and including `<math.h>` for `fabsf` if needed.
