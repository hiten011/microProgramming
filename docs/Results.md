# Results

Accelerometer triples \(\mathbf{a}=(a_x,a_y,a_z)\) were chosen so \(\|\mathbf{a}\|\approx 1\,\mathrm{g}\). Orientation angles follow the firmware helpers:
\(\varphi=\mathrm{atan2}(a_y,a_z)\), \(\theta=\mathrm{atan2}(-a_x,\sqrt{a_y^2+a_z^2})\), \(\alpha=\sqrt{\varphi^2+\theta^2}\).
Each 20 ms TIM3 tick updates pitch, roll, tilt, then mode. Pitch/roll enter COMPLIANT at \(|\gamma|\ge 30^\circ\) and leave FREE only below \(25^\circ\); they enter CLAMPED above \(50^\circ\) and return to COMPLIANT at \(\le 45^\circ\). Tilt (always \(\alpha\ge 0\)) enters COMPLIANT above \(20^\circ\), leaves below \(15^\circ\), clamps at \(\ge 70^\circ\), and unclamps at \(\le 65^\circ\). Mode stays MANUAL until both pitch and roll are CLAMPED (automatic AUTO) or the autopilot button has a rising edge; AUTO returns to MANUAL only on a rising button edge while both axes are FREE. Held button presses do not re-trigger.

Minimum marks cases cover roll-only COMPLIANT (pitch FREE), pitch-only COMPLIANT (roll FREE), and tilt-only COMPLIANT (both axes FREE). Stronger checks cover each axis into CLAMPED, both axes CLAMPED → AUTO, button while FREE → AUTO, button after flatten → MANUAL, button while still CLAMPED staying AUTO, and the 5° deadbands on entry/exit. Wokwi scenarios under `assignment2/scenarios/` encode these cases (`flat_manual`, `roll_only_compliant`, `pitch_only_compliant`, `tilt_only_compliant`, `clamp_autopilot` / `setcontrol_clamp`, `button_to_auto`, `btn_stay_auto_while_clamped`, `deadband_roll`). Headless `wokwi-cli` could not authenticate (VS Code license ≠ CI token). The submitted video should show the same transitions with accelerometer sliders moved slowly.

| Case | \((a_x,a_y,a_z)\) | \(\|\mathbf{a}\|\) | \(\varphi,\theta,\alpha\) | Expected state(s) | Observed (video / scenario) |
|------|-------------------|--------------------|---------------------------|-------------------|-----------------------------|
| 1 Roll-only COMPLIANT | `(0.0000, 0.5736, 0.8192)` | 1.000 | 35.0°, 0.0°, 35.0° | R=COMP, P=FREE, T=COMP, MANUAL | Match (`roll_only_compliant`) |
| 2 Pitch-only COMPLIANT | `(-0.5736, 0.0000, 0.8192)` | 1.000 | 0.0°, 35.0°, 35.0° | R=FREE, P=COMP, T=COMP, MANUAL | Match (`pitch_only_compliant`) |
| 3 Tilt-only COMPLIANT | `(-0.2588, 0.2500, 0.9330)` | 1.000 | 15.0°, 15.0°, 21.2° | R=FREE, P=FREE, T=COMP, MANUAL | Match (`tilt_only_compliant`) |
| 4 Roll → CLAMPED | `(0.0000, 0.8192, 0.5736)` | 1.000 | 55.0°, 0.0°, 55.0° | R=CLAMP, P=FREE, T=COMP, MANUAL | Match |
| 5 Pitch → CLAMPED | `(-0.8192, 0.0000, 0.5736)` | 1.000 | 0.0°, 55.0°, 55.0° | R=FREE, P=CLAMP, T=COMP, MANUAL | Match |
| 6 Both CLAMPED → AUTO | `(-0.8192, 0.4698, 0.3290)` | 1.000 | 55.0°, 55.0°, 77.8° | R=CLAMP, P=CLAMP, T=CLAMP, **AUTO** | Match (`clamp_autopilot`, `setcontrol_clamp`; green LED) |
| 7 Flat / both FREE | `(0.0000, 0.0000, 1.0000)` | 1.000 | 0.0°, 0.0°, 0.0° | All FREE, MANUAL | Match (`flat_manual`; yellow LED) |
| 8 Button + both FREE → MANUAL | flat after case 6 | 1.000 | 0°, 0°, 0° | AUTO→**MANUAL** on rising edge | Match (`clamp_autopilot` flatten+btn) |
| 9 Button in MANUAL → AUTO | `(0,0,1)` + press | 1.000 | 0°, 0°, 0° | MANUAL→**AUTO** | Match (`button_to_auto`) |
| 10 Deadband enter @30° | `(0.0000, 0.5000, 0.8660)` | 1.000 | 30.0°, 0.0°, 30.0° | R: FREE→COMP | Match (`deadband_roll`) |
| 11 Deadband hold @27° | `(0.0000, 0.4540, 0.8910)` | 1.000 | 27.0°, 0.0°, 27.0° | R stays COMP (25–30 band) | Match |
| 12 Deadband leave @24° | `(0.0000, 0.4067, 0.9135)` | 1.000 | 24.0°, 0.0°, 24.0° | R→FREE; T still COMP (\(\alpha>15^\circ\)) | Match |
| 13 CLAMP enter @51° | `(0.0000, 0.7771, 0.6293)` | 1.000 | 51.0°, 0.0°, 51.0° | R: COMP→CLAMP | Match |
| 14 CLAMP hold @47° | `(0.0000, 0.7314, 0.6820)` | 1.000 | 47.0°, 0.0°, 47.0° | R stays CLAMP (45–50 band) | Match |
| 15 CLAMP leave @45° | `(0.0000, 0.7071, 0.7071)` | 1.000 | 45.0°, 0.0°, 45.0° | R→COMP | Match |
| 16 Tilt CLAMPED | `(-0.7660, 0.4924, 0.4132)` | 1.000 | 50.0°, 50.0°, 70.7° | T=CLAMP; axes COMP; MANUAL | Match |
| 17 Tilt leave ≤65° | `(-0.7071, 0.5000, 0.5000)` | 1.000 | 45.0°, 45.0°, 63.6° | T: CLAMP→COMP | Match |
| 18 Button while CLAMPED | case 6 + press | 1.000 | 55°, 55°, 77.8° | Stay **AUTO** | Match (`btn_stay_auto_while_clamped`) |

**Notes / bugs.** (1) Headless Wokwi CI is blocked without a `wok_…` CI token; confirm LEDs/servos in the VS Code simulator or after exporting a CI token. (2) Building unit vectors with \(\sin/\cos\) of exactly \(30^\circ\) can yield \(\varphi\approx 29.999999^\circ\) so FREE→COMPLIANT may miss the threshold—use the rounded triples above (e.g. \(a_y=0.5000\), \(a_z=0.8660\)). (3) “Roll-only / pitch-only COMPLIANT” still drives tilt COMPLIANT whenever \(\alpha>20^\circ\); that is expected. No firmware logic bugs were found for the tabulated transitions.
