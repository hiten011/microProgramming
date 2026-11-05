# FSM Functions Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement `fsm_pitch`, `fsm_roll`, and `fsm_tilt` state transition functions in `assignment2/Src/main.c` to update axis state enums based on MPU6050 angle calculations.

**Architecture:** Functions read global `angles` (`pitch`, `roll`, `tilt`) and mutate global state enums (`pitch`, `roll`, `tilt`) using hysteresis thresholds to prevent chatter. Testing is executed using a host C test harness (`tests/test_fsm.c`) compiled with GCC.

**Tech Stack:** C99, GCC, math.h (`fabsf`)

**Spec:** [`docs/superpowers/specs/2026-11-05-fsm-functions-design.md`](file:///home/hiten/dev/UNI/microPrograming/docs/superpowers/specs/2026-11-05-fsm-functions-design.md)

## Global Constraints

- Must implement three functions: `void fsm_pitch(void)`, `void fsm_roll(void)`, and `void fsm_tilt(void)` in `assignment2/Src/main.c`.
- Must operate on global `angles` and update `pitch`, `roll`, and `tilt` state enums (`FREE`, `COMPLIANT`, `CLAMPED`).
- Only update state enums (no servo control or PWM modifications in these functions).
- Negative angle magnitudes must be handled via `fabsf()` for pitch and roll.

## Review Focus

1. Boundary conditions at exact thresholds (e.g. `fabsf(angle) == 30.0f` enters COMPLIANT; `fabsf(angle) == 25.0f` stays COMPLIANT).
2. Negative angles for pitch and roll (e.g. `-35.0f` triggers COMPLIANT, `-55.0f` triggers CLAMPED).
3. Hysteresis deadband: in COMPLIANT, angles between 25.0° and 30.0° remain COMPLIANT.
4. Clamped release: in CLAMPED, angles between 45.0° and 50.0° remain CLAMPED until $\le 45.0^\circ$.
5. Tilt non-negative boundaries: tilt at 20.0° stays FREE ($> 20^\circ$ required); tilt at 70.0° enters CLAMPED ($\ge 70^\circ$).

---

### Task 1: Setup Host Test Harness & Implement `fsm_pitch`

**Files:**
- Create: `tests/test_fsm.c`
- Modify: `assignment2/Src/main.c`

**Interfaces:**
- Consumes: `Angles angles`, `enum AXIS_STATE pitch`
- Produces: `void fsm_pitch(void)`

- [ ] **Step 1: Write host test harness and failing test for `fsm_pitch`**

Create `tests/test_fsm.c` containing test cases verifying:
- Transition from `FREE` &rarr; `COMPLIANT` when `pitch >= 30.0f` and `pitch <= -30.0f`
- Transition from `COMPLIANT` &rarr; `FREE` when `|pitch| < 25.0f`
- Transition from `COMPLIANT` &rarr; `CLAMPED` when `|pitch| > 50.0f`
- Transition from `CLAMPED` &rarr; `COMPLIANT` when `|pitch| <= 45.0f`
- Deadband retention (no state change within hysteresis band)

- [ ] **Step 2: Run test to verify it fails**

Run: `gcc -Wall -Wextra tests/test_fsm.c -lm -o tests/test_fsm && ./tests/test_fsm`
Expected: FAIL (missing `fsm_pitch` or assertions failing).

- [ ] **Step 3: Implement `void fsm_pitch(void)` in `assignment2/Src/main.c`**

Add `#include <math.h>` to `assignment2/Src/main.c` if needed.
Implement `void fsm_pitch(void)` evaluating `fabsf(angles.pitch)`:
- In `FREE`: if `|pitch| >= 30.0f` &rarr; `pitch = COMPLIANT`
- In `COMPLIANT`:
  - if `|pitch| < 25.0f` &rarr; `pitch = FREE`
  - else if `|pitch| > 50.0f` &rarr; `pitch = CLAMPED`
- In `CLAMPED`: if `|pitch| <= 45.0f` &rarr; `pitch = COMPLIANT`

- [ ] **Step 4: Run test to verify it passes**

Run: `gcc -Wall -Wextra tests/test_fsm.c -lm -o tests/test_fsm && ./tests/test_fsm`
Expected: PASS for all pitch test assertions.

- [ ] **Step 5: Commit**

```bash
git add tests/test_fsm.c assignment2/Src/main.c
git commit -m "feat: implement fsm_pitch state transition function"
```

---

### Task 2: Implement `fsm_roll`

**Files:**
- Modify: `tests/test_fsm.c`
- Modify: `assignment2/Src/main.c`

**Interfaces:**
- Consumes: `Angles angles`, `enum AXIS_STATE roll`
- Produces: `void fsm_roll(void)`

- [ ] **Step 1: Add failing tests for `fsm_roll`**

Add tests to `tests/test_fsm.c` verifying:
- `FREE` &rarr; `COMPLIANT` when `roll >= 30.0f` and `roll <= -30.0f`
- `COMPLIANT` &rarr; `FREE` when `|roll| < 25.0f`
- `COMPLIANT` &rarr; `CLAMPED` when `|roll| > 50.0f`
- `CLAMPED` &rarr; `COMPLIANT` when `|roll| <= 45.0f`
- Hysteresis preservation for roll

- [ ] **Step 2: Run test to verify it fails**

Run: `gcc -Wall -Wextra tests/test_fsm.c -lm -o tests/test_fsm && ./tests/test_fsm`
Expected: FAIL (missing `fsm_roll`).

- [ ] **Step 3: Implement `void fsm_roll(void)` in `assignment2/Src/main.c`**

Implement `void fsm_roll(void)` evaluating `fabsf(angles.roll)`:
- In `FREE`: if `|roll| >= 30.0f` &rarr; `roll = COMPLIANT`
- In `COMPLIANT`:
  - if `|roll| < 25.0f` &rarr; `roll = FREE`
  - else if `|roll| > 50.0f` &rarr; `roll = CLAMPED`
- In `CLAMPED`: if `|roll| <= 45.0f` &rarr; `roll = COMPLIANT`

- [ ] **Step 4: Run test to verify it passes**

Run: `gcc -Wall -Wextra tests/test_fsm.c -lm -o tests/test_fsm && ./tests/test_fsm`
Expected: PASS for pitch and roll tests.

- [ ] **Step 5: Commit**

```bash
git add tests/test_fsm.c assignment2/Src/main.c
git commit -m "feat: implement fsm_roll state transition function"
```

---

### Task 3: Implement `fsm_tilt`

**Files:**
- Modify: `tests/test_fsm.c`
- Modify: `assignment2/Src/main.c`

**Interfaces:**
- Consumes: `Angles angles`, `enum AXIS_STATE tilt`
- Produces: `void fsm_tilt(void)`

- [ ] **Step 1: Add failing tests for `fsm_tilt`**

Add tests to `tests/test_fsm.c` verifying:
- `FREE` &rarr; `COMPLIANT` when `tilt > 20.0f` (and remains `FREE` at `20.0f`)
- `COMPLIANT` &rarr; `FREE` when `tilt < 15.0f`
- `COMPLIANT` &rarr; `CLAMPED` when `tilt >= 70.0f`
- `CLAMPED` &rarr; `COMPLIANT` when `tilt <= 65.0f`
- Hysteresis preservation for tilt

- [ ] **Step 2: Run test to verify it fails**

Run: `gcc -Wall -Wextra tests/test_fsm.c -lm -o tests/test_fsm && ./tests/test_fsm`
Expected: FAIL (missing `fsm_tilt`).

- [ ] **Step 3: Implement `void fsm_tilt(void)` in `assignment2/Src/main.c`**

Implement `void fsm_tilt(void)` evaluating `angles.tilt`:
- In `FREE`: if `angles.tilt > 20.0f` &rarr; `tilt = COMPLIANT`
- In `COMPLIANT`:
  - if `angles.tilt < 15.0f` &rarr; `tilt = FREE`
  - else if `angles.tilt >= 70.0f` &rarr; `tilt = CLAMPED`
- In `CLAMPED`: if `angles.tilt <= 65.0f` &rarr; `tilt = COMPLIANT`

- [ ] **Step 4: Run test to verify it passes**

Run: `gcc -Wall -Wextra tests/test_fsm.c -lm -o tests/test_fsm && ./tests/test_fsm`
Expected: PASS for pitch, roll, and tilt tests.

- [ ] **Step 5: Commit**

```bash
git add tests/test_fsm.c assignment2/Src/main.c
git commit -m "feat: implement fsm_tilt state transition function"
```
