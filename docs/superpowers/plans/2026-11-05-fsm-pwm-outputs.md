# FSM PWM Outputs Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement servo PWM output actuation directly inside each state of `fsm_pitch()`, `fsm_roll()`, and `fsm_tilt()` in `assignment2/Src/main.c`, converting angle targets to microsecond pulse widths according to the assignment brief and FSM diagrams.

**Architecture:** A conversion helper function `servo_deg_to_us(float deg)` maps angles ($0^\circ \to 180^\circ$) to PWM pulse widths ($500\,\mu\text{s} \to 2500\,\mu\text{s}$) with safe bounds clamping. In each FSM state, `set_pitch()`, `set_roll()`, and `set_tilt()` are called with the computed pulse width. Host unit tests in `tests/test_fsm.c` mock the servo setters to verify exact pulse widths, and the project is built with the ARM toolchain.

**Tech Stack:** C99, STM32C031xx HAL/registers, GCC, CMake, math.h (`roundf`, `fabsf`)

**Spec:** [`docs/superpowers/specs/2026-11-05-fsm-pwm-outputs-design.md`](file:///home/hiten/dev/UNI/microPrograming/docs/superpowers/specs/2026-11-05-fsm-pwm-outputs-design.md)

## Global Constraints

- Pulse width conversion: $\text{pulse\_us} = 500 + \text{deg} \times \frac{2000}{180} = 500 + \text{deg} \times \frac{100}{9}$, clamped to $[500, 2500]\,\mu\text{s}$.
- Neutral pulse widths: Pitch = $1500\,\mu\text{s}$ ($90^\circ$), Roll = $1500\,\mu\text{s}$ ($90^\circ$), Tilt = $500\,\mu\text{s}$ ($0^\circ$).
- Clamped limits: Pitch/Roll clamped at $90^\circ \pm 50^\circ$ ($140^\circ \approx 2056\,\mu\text{s}$ or $40^\circ \approx 944\,\mu\text{s}$ based on angle sign); Tilt clamped at $140^\circ \approx 2056\,\mu\text{s}$.
- Compliant tracking: Pitch = $90^\circ + \text{angles.pitch}$; Roll = $90^\circ + \text{angles.roll}$; Tilt = $2 \times \text{angles.tilt}$.
- Code must compile cleanly for host testing with GCC and for target STM32C031 via CMake.

## Review Focus

1. Rounding and pulse clamping: ensure $0^\circ \to 500\,\mu\text{s}$, $90^\circ \to 1500\,\mu\text{s}$, $140^\circ \to 2056\,\mu\text{s}$, $180^\circ \to 2500\,\mu\text{s}$, and negative or $>180^\circ$ inputs stay clamped.
2. Sign preservation on pitch clamping: pitch $> 50^\circ$ clamps to $140^\circ$ ($2056\,\mu\text{s}$); pitch $< -50^\circ$ clamps to $40^\circ$ ($944\,\mu\text{s}$).
3. Sign preservation on roll clamping: roll $> 50^\circ$ clamps to $140^\circ$ ($2056\,\mu\text{s}$); roll $< -50^\circ$ clamps to $40^\circ$ ($944\,\mu\text{s}$).
4. Tilt doubling in compliant mode: tilt at $25^\circ$ outputs $50^\circ$ ($1056\,\mu\text{s}$); tilt clamped outputs $140^\circ$ ($2056\,\mu\text{s}$).
5. Host test compatibility: `#ifndef HOST_TEST` guards enable isolated compilation and pulse assertion without hardware dependencies.

---

### Task 1: Setup Host Test Harness & Implement `servo_deg_to_us`

**Files:**
- Modify: `assignment2/Src/main.c`
- Modify: `tests/test_fsm.c`

**Interfaces:**
- Consumes: `<math.h>`, `roundf`
- Produces: `uint16_t servo_deg_to_us(float deg)`

- [ ] **Step 1: Add failing test for `servo_deg_to_us` and mocked setters in `tests/test_fsm.c`**

Add mock variables `last_pitch_us`, `last_roll_us`, `last_tilt_us` and mock functions `set_pitch`, `set_roll`, `set_tilt` in `tests/test_fsm.c`. Add test verifying `servo_deg_to_us`:
- `servo_deg_to_us(0.0f) == 500`
- `servo_deg_to_us(90.0f) == 1500`
- `servo_deg_to_us(140.0f) == 2056`
- `servo_deg_to_us(40.0f) == 944`
- `servo_deg_to_us(180.0f) == 2500`
- `servo_deg_to_us(-10.0f) == 500` (clamped)
- `servo_deg_to_us(200.0f) == 2500` (clamped)

- [ ] **Step 2: Run test to verify it fails**

Run: `gcc -Wall -Wextra tests/test_fsm.c -lm -o /tmp/test_fsm && /tmp/test_fsm`
Expected: FAIL (missing `servo_deg_to_us` or compilation error due to hardware includes).

- [ ] **Step 3: Implement `servo_deg_to_us` and guard hardware headers in `assignment2/Src/main.c`**

In `assignment2/Src/main.c`:
1. Guard hardware-specific includes and functions (`TIM3_IRQHandler`, `EXTI4_15_IRQHandler`, `init`, `main`) under `#ifndef HOST_TEST`.
2. Add `uint16_t servo_deg_to_us(float deg)`:
   - Clamp `deg` to `[0.0f, 180.0f]`.
   - Compute `(uint16_t)roundf(500.0f + deg * (2000.0f / 180.0f))`.

- [ ] **Step 4: Run test to verify it passes**

Run: `gcc -Wall -Wextra tests/test_fsm.c -lm -o /tmp/test_fsm && /tmp/test_fsm`
Expected: PASS (`servo_deg_to_us` unit tests pass).

---

### Task 2: Implement PWM Outputs in `fsm_pitch`

**Files:**
- Modify: `assignment2/Src/main.c`
- Modify: `tests/test_fsm.c`

**Interfaces:**
- Consumes: `servo_deg_to_us`, `set_pitch(uint16_t pulse_us)`, `angles.pitch`
- Produces: `void fsm_pitch(void)` updating PWM output

- [ ] **Step 1: Add failing assertions in `tests/test_fsm.c` for pitch PWM output**

Assert `last_pitch_us` across states:
- `FREE`: `last_pitch_us == 1500`
- `COMPLIANT` at $+35^\circ$: `last_pitch_us == servo_deg_to_us(125.0f)` ($1889\,\mu\text{s}$)
- `COMPLIANT` at $-35^\circ$: `last_pitch_us == servo_deg_to_us(55.0f)` ($1111\,\mu\text{s}$)
- `CLAMPED` at $+55^\circ$: `last_pitch_us == servo_deg_to_us(140.0f)` ($2056\,\mu\text{s}$)
- `CLAMPED` at $-55^\circ$: `last_pitch_us == servo_deg_to_us(40.0f)` ($944\,\mu\text{s}$)

- [ ] **Step 2: Run test to verify it fails**

Run: `gcc -Wall -Wextra tests/test_fsm.c -lm -o /tmp/test_fsm && /tmp/test_fsm`
Expected: FAIL (`last_pitch_us` assertions fail).

- [ ] **Step 3: Update `fsm_pitch()` in `assignment2/Src/main.c` with PWM outputs**

Inside `fsm_pitch()`:
- `case FREE:` `set_pitch(1500);`
- `case COMPLIANT:` `set_pitch(servo_deg_to_us(90.0f + angles.pitch));`
- `case CLAMPED:` `set_pitch(angles.pitch >= 0.0f ? servo_deg_to_us(140.0f) : servo_deg_to_us(40.0f));`

- [ ] **Step 4: Run test to verify it passes**

Run: `gcc -Wall -Wextra tests/test_fsm.c -lm -o /tmp/test_fsm && /tmp/test_fsm`
Expected: PASS (`fsm_pitch` PWM tests pass).

---

### Task 3: Implement PWM Outputs in `fsm_roll`

**Files:**
- Modify: `assignment2/Src/main.c`
- Modify: `tests/test_fsm.c`

**Interfaces:**
- Consumes: `servo_deg_to_us`, `set_roll(uint16_t pulse_us)`, `angles.roll`
- Produces: `void fsm_roll(void)` updating PWM output

- [ ] **Step 1: Add failing assertions in `tests/test_fsm.c` for roll PWM output**

Assert `last_roll_us` across states:
- `FREE`: `last_roll_us == 1500`
- `COMPLIANT` at $+35^\circ$: `last_roll_us == servo_deg_to_us(125.0f)` ($1889\,\mu\text{s}$)
- `COMPLIANT` at $-35^\circ$: `last_roll_us == servo_deg_to_us(55.0f)` ($1111\,\mu\text{s}$)
- `CLAMPED` at $+55^\circ$: `last_roll_us == servo_deg_to_us(140.0f)` ($2056\,\mu\text{s}$)
- `CLAMPED` at $-55^\circ$: `last_roll_us == servo_deg_to_us(40.0f)` ($944\,\mu\text{s}$)

- [ ] **Step 2: Run test to verify it fails**

Run: `gcc -Wall -Wextra tests/test_fsm.c -lm -o /tmp/test_fsm && /tmp/test_fsm`
Expected: FAIL (`last_roll_us` assertions fail).

- [ ] **Step 3: Update `fsm_roll()` in `assignment2/Src/main.c` with PWM outputs**

Inside `fsm_roll()`:
- `case FREE:` `set_roll(1500);`
- `case COMPLIANT:` `set_roll(servo_deg_to_us(90.0f + angles.roll));`
- `case CLAMPED:` `set_roll(angles.roll >= 0.0f ? servo_deg_to_us(140.0f) : servo_deg_to_us(40.0f));`

- [ ] **Step 4: Run test to verify it passes**

Run: `gcc -Wall -Wextra tests/test_fsm.c -lm -o /tmp/test_fsm && /tmp/test_fsm`
Expected: PASS (`fsm_roll` PWM tests pass).

---

### Task 4: Implement PWM Outputs in `fsm_tilt`

**Files:**
- Modify: `assignment2/Src/main.c`
- Modify: `tests/test_fsm.c`

**Interfaces:**
- Consumes: `servo_deg_to_us`, `set_tilt(uint16_t pulse_us)`, `angles.tilt`
- Produces: `void fsm_tilt(void)` updating PWM output

- [ ] **Step 1: Add failing assertions in `tests/test_fsm.c` for tilt PWM output**

Assert `last_tilt_us` across states:
- `FREE`: `last_tilt_us == 500`
- `COMPLIANT` at $25.0^\circ$: `last_tilt_us == servo_deg_to_us(50.0f)` ($1056\,\mu\text{s}$)
- `COMPLIANT` at $50.0^\circ$: `last_tilt_us == servo_deg_to_us(100.0f)` ($1611\,\mu\text{s}$)
- `CLAMPED` at $75.0^\circ$: `last_tilt_us == servo_deg_to_us(140.0f)` ($2056\,\mu\text{s}$)

- [ ] **Step 2: Run test to verify it fails**

Run: `gcc -Wall -Wextra tests/test_fsm.c -lm -o /tmp/test_fsm && /tmp/test_fsm`
Expected: FAIL (`last_tilt_us` assertions fail).

- [ ] **Step 3: Update `fsm_tilt()` in `assignment2/Src/main.c` with PWM outputs**

Inside `fsm_tilt()`:
- `case FREE:` `set_tilt(500);`
- `case COMPLIANT:` `set_tilt(servo_deg_to_us(2.0f * angles.tilt));`
- `case CLAMPED:` `set_tilt(servo_deg_to_us(140.0f));`

- [ ] **Step 4: Run test to verify it passes**

Run: `gcc -Wall -Wextra tests/test_fsm.c -lm -o /tmp/test_fsm && /tmp/test_fsm`
Expected: PASS (`fsm_tilt` PWM tests pass).

---

### Task 5: Build Verification with Target Toolchain

**Files:**
- None (Build verification)

- [ ] **Step 1: Build the STM32 firmware using CMake**

Run: `cmake --build assignment2/build/Release`
Expected: Build succeeds with 0 errors, generating `assignment2.elf`.
