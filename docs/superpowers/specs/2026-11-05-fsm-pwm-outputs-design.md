# Design: FSM State PWM Output Updates

## Intent
Update servo PWM pulse widths directly within each state of `fsm_pitch()`, `fsm_roll()`, and `fsm_tilt()` according to the assignment specification and FSM diagrams.

## Goals
- Add a helper to convert servo angles (degrees) to pulse width in microseconds:
  $$\text{pulse\_us} = 500 + \text{angle\_deg} \times \frac{2000}{180} = 500 + \text{angle\_deg} \times \frac{100}{9}$$
- Inside `fsm_pitch()`:
  - `FREE`: set pitch servo to $90^\circ$ ($1500\,\mu\text{s}$)
  - `COMPLIANT`: set pitch servo to $90^\circ + \text{angles.pitch}$
  - `CLAMPED`: set pitch servo locked at $90^\circ + 50^\circ = 140^\circ$ ($2056\,\mu\text{s}$) if `angles.pitch > 0`, or $90^\circ - 50^\circ = 40^\circ$ ($944\,\mu\text{s}$) if `angles.pitch < 0`
- Inside `fsm_roll()`:
  - `FREE`: set roll servo to $90^\circ$ ($1500\,\mu\text{s}$)
  - `COMPLIANT`: set roll servo to $90^\circ + \text{angles.roll}$
  - `CLAMPED`: set roll servo locked at $90^\circ + 50^\circ = 140^\circ$ ($2056\,\mu\text{s}$) if `angles.roll > 0`, or $90^\circ - 50^\circ = 40^\circ$ ($944\,\mu\text{s}$) if `angles.roll < 0`
- Inside `fsm_tilt()`:
  - `FREE`: set tilt servo to $0^\circ$ ($500\,\mu\text{s}$)
  - `COMPLIANT`: set tilt servo to $2 \times \text{angles.tilt}$
  - `CLAMPED`: set tilt servo locked at $140^\circ$ ($2056\,\mu\text{s}$)
- Verify PWM outputs with automated unit tests in `tests/test_fsm.c`.

## Non-Goals & Anti-Goals
- Non-goal: Modifying Mode FSM (AUTO / MANUAL transitions).
- Non-goal: Modifying MPU6050 angle calculations or I2C communication.
- Anti-goal: Overriding or clobbering servo pulse calculations with integer truncation errors; use appropriate float conversion rounding.

## Constraints & Mappings
- TIM3 PWM period is 20 ms (50 Hz), ARR = 19999, prescaler = 47 (1 µs per tick).
- Servo range is $0^\circ \to 180^\circ$ corresponding to $500\,\mu\text{s} \to 2500\,\mu\text{s}$.
- Pulse width values must remain clamped within safe physical bounds $[500, 2500]\,\mu\text{s}$.

## Approaches
1. **Approach 1 (Direct State-Level Output - Selected)**: Compute the angle/pulse and call `set_pitch()`, `set_roll()`, or `set_tilt()` directly inside the respective state switch cases of each FSM function. Selected by user for direct correspondence with FSM state diagrams.
2. **Approach 2 (Separated PWM Subroutines)**: FSM functions remain pure state mutators; a subsequent handler updates PWMs based on the resulting states. Rejected to keep state transition and actuation co-located.

## Decisions & Builder Scope
- **Decided by user**: Approach 1, computing and applying PWM within each state case.
- **Left to builder**: Exact helper naming (`servo_deg_to_us`), rounding strategy (`(uint16_t)roundf(...)`), and safe boundary clamping.
