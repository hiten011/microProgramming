# Design: Mode FSM (Manual / Autopilot)

## Intent
Add a fourth FSM, `fsm_mode()`, that switches between MANUAL and AUTOPILOT according to `docs/mode_fsm.png` and Assignment 2 rules 5–7, and run it after the tilt FSM in the 20 ms TIM3 tick. Detect the autopilot button by polling (no EXTI).

## Goals
- Add `void fsm_mode(void)` in `assignment2/Src/main.c`, same style as `fsm_pitch` / `fsm_roll` / `fsm_tilt`.
- Call order in `TIM3_IRQHandler`: `fsm_pitch()` → `fsm_roll()` → `fsm_tilt()` → `fsm_mode()`.
- Reuse existing `enum MODE { AUTO, MANUAL }` and `curMode` (start in `MANUAL`).
- Drive LEDs from each mode state:
  - MANUAL: manual LED on (PB11), autopilot LED off (PA2)
  - AUTO: autopilot LED on (PA2), manual LED off (PB11)
- Transitions:
  - MANUAL → AUTO if `(roll == CLAMPED && pitch == CLAMPED)` **OR** rising button edge on PA12
  - AUTO → MANUAL if rising button edge **AND** `(roll == FREE && pitch == FREE)`
- Button counts once per press: poll `GPIO_InputReadPin(GPIOA, 12)` each tick; rising-edge detect with a static previous level (`!prev && now`).
- Remove EXTI entirely: delete `EXTI4_15_IRQHandler` and `EXTI_Init(...)` in `init()`; keep PA12 as input with pull-down for polling.

## Non-Goals & Anti-Goals
- Non-goal: Changing pitch / roll / tilt thresholds or servo PWM math.
- Non-goal: Renaming `AUTO` to `AUTOPILOT` in the enum (diagram wording only).
- Anti-goal: Level-triggered button handling that re-enters every 20 ms while held.
- Anti-goal: Leaving a partial EXTI path that still toggles mode without roll/pitch checks.

## Constraints
- Source of truth for transitions: `docs/mode_fsm.png` and Assignment 2 (auto-enter when both axes clamped; leave autopilot only with button + both axes FREE; LEDs indicate mode).
- Mode FSM must observe axis FSM state after pitch and roll have updated that tick (hence call after those, and after tilt as requested).
- PA12 remains pull-down input; pressed = high.

## Approaches
1. **Poll in `fsm_mode()` on TIM3 tick (Selected)**: Edge-detect PA12 inside the Mode FSM; apply roll/pitch rules; update LEDs. Remove EXTI. Matches the other FSMs and the full diagram.
2. **EXTI flag + TIM3 consumer**: Interrupt sets a pressed flag; FSM still runs in TIM3. Extra machinery; rejected by user.
3. **Split EXTI (button) / TIM3 (clamp)**: Splits one Mode FSM across two sites; rejected.

## Edge Cases
- Held button: only the rising edge triggers; sustained high does nothing further.
- AUTO + button while not both FREE: remain AUTO.
- MANUAL + both become CLAMPED: enter AUTO with no button.
- Startup: `init()` already sets MANUAL LEDs; `fsm_mode` preserves until a transition.

## Testing
Host-style checks (if/when tests exist):
- MANUAL + both CLAMPED → AUTO
- MANUAL + rising edge → AUTO
- AUTO + rising edge + both FREE → MANUAL
- AUTO + rising edge + not both FREE → stay AUTO
- Held high without new edge → no extra transitions

## Decisions & Builder Scope
- **Decided by user**: Approach 1; remove EXTI completely; call `fsm_mode()` after `fsm_tilt()`.
- **Left to builder**: Exact static edge-detect variable naming; whether LEDs are refreshed every tick or only on transition (either is fine if outputs match the state).
