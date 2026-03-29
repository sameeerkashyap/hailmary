# PHM Engine — Physics Reference

All formulas used in the simulation. Every C++ function in `src/physics/` maps to an equation here. Cross-validate your implementation against the overvieweffekt.com calculators using the test cases at the bottom.

---

## Constants

```
c   = 299,792,458     m/s         speed of light
g   = 9.80665         m/s²        standard gravity (1g acceleration)
ly  = 9.4607 × 10¹⁵  m           one light-year in meters
AU  = 1.4960 × 10¹¹  m           one astronomical unit in meters
pc  = 3.0857 × 10¹⁶  m           one parsec in meters
```

---

## 1. Special Relativity Basics

### Lorentz Factor (γ)
The fundamental quantity of special relativity. As velocity approaches c, γ → ∞.

```
γ = 1 / √(1 - v²/c²)
```

| v / c | γ |
|---|---|
| 0.10 | 1.005 |
| 0.50 | 1.155 |
| 0.90 | 2.294 |
| 0.99 | 7.089 |
| 0.999 | 22.37 |
| 0.9999 | 70.71 |

### Time Dilation
A clock on the ship ticks slower than a clock on Earth.

```
t_earth = γ · τ_ship

where:
  t_earth  = time elapsed on Earth
  τ_ship   = proper time elapsed on ship (what the crew experiences)
  γ        = Lorentz factor at velocity v
```

### Relativistic Kinetic Energy
Energy required to accelerate a mass m to velocity v.

```
E = (γ - 1) · m · c²

Note: at low v, this reduces to the classical ½mv² (good sanity check)
```

---

## 2. Brachistochrone Trajectory

The Hail Mary uses constant thrust — accelerate at 1.5g to the midpoint, flip, decelerate at 1.5g to the destination. This is the **brachistochrone** (minimum time) trajectory.

### Ship Proper Time (what the crew experiences)

```
τ = (2c/a) · acosh( a·d / (2c²) + 1 )

where:
  τ = ship proper time (seconds)
  a = constant acceleration (m/s²)
  d = total distance (meters)
  c = speed of light (m/s)
  acosh = inverse hyperbolic cosine
```

### Earth Coordinate Time (what Earth observes)

```
t = (2c/a) · sinh( a·τ / (2c) )

where:
  t = Earth coordinate time (seconds)
  τ = ship proper time from above
```

### Peak Velocity at Midpoint

```
v_max = c · tanh( a·τ / (2c) )
```

### Distance Covered at Any Ship Time τ' (for animation)

```
d(τ') = (c²/a) · ( cosh(a·τ'/c) - 1 )    [for first half, 0 ≤ τ' ≤ τ/2]
```

### Earth Time at Any Ship Time τ'

```
t(τ') = (c/a) · sinh( a·τ'/c )
```

---

## 3. Relativistic Rocket Equation

The relativistic extension of Tsiolkovsky's rocket equation. Determines how much fuel mass is needed given an exhaust velocity.

### Mass Ratio (m₀ / m_f)

```
m₀/m_f = exp( Δv / v_e )        [non-relativistic — NOT valid near c]

Relativistic version:
m₀/m_f = [ (1 + Δv/c) / (1 - Δv/c) ] ^ (c / 2v_e)

where:
  m₀   = initial mass (ship + fuel)
  m_f  = final mass (ship only, fuel spent)
  Δv   = total velocity change (2 × v_max for brachistochrone, accel + decel)
  v_e  = effective exhaust velocity (m/s)
```

### Fuel Mass

```
m_fuel = m_ship · ( m₀/m_f - 1 )
```

### For Astrophage (photon drive approximation)

Astrophage converts stored energy directly to photon thrust. For a photon rocket:
```
v_e = c   (exhaust velocity = c, maximum possible)

m₀/m_f = exp( arctanh(v_max/c) × 2 )   [for full accel + decel]
```

---

## 4. Energy Density Comparison

Energy available per kilogram of fuel (joules per kg):

```
Chemical propellant (H₂/O₂):   ~1.4 × 10⁷  J/kg   (14 MJ/kg)
Fission (U-235, full burnup):   ~8.2 × 10¹³  J/kg   (82 TJ/kg)
Fusion (D-T):                   ~3.4 × 10¹⁴  J/kg   (340 TJ/kg)
Matter-antimatter (theoretical): 9.0 × 10¹⁶  J/kg   (90 PJ/kg)
Astrophage (book value):        ~1.0 × 10²¹  J/kg   (1 ZJ/kg)
```

### Required Fuel Mass

```
m_fuel_required = E_required / energy_density_of_source

E_required = (γ - 1) · m_ship · c²   (relativistic kinetic energy at peak v)
```

The ratio between Astrophage and Chemical for Earth → Tau Ceti is approximately 10¹⁴ — fourteen orders of magnitude. Chemical propellant would require a fuel mass greater than the mass of the observable universe.

---

## 5. Orbital Mechanics

### Kepler Orbit — Planet Position at Time t

For a circular/elliptical orbit:

```
M = n · (t - t_periapsis)     mean anomaly
n = 2π / T                     mean motion
T = 2π · √(a³ / GM_sun)       orbital period

Eccentric anomaly E: solve Kepler's equation
  M = E - e · sin(E)          (iterate with Newton's method)

True anomaly ν:
  tan(ν/2) = √((1+e)/(1-e)) · tan(E/2)

Position in orbital plane:
  r = a · (1 - e · cos(E))
  x = r · cos(ν)
  y = r · sin(ν)
```

### Runge-Kutta 4th Order (RK4) Integrator

For N-body gravitational simulation:

```
State vector: [x, y, z, vx, vy, vz] for each body

Derivatives:
  dx/dt = vx,  dy/dt = vy,  dz/dt = vz
  dvx/dt = Fx/m,  dvy/dt = Fy/m,  dvz/dt = Fz/m

Gravitational force on body i from body j:
  F = G · mᵢ · mⱼ / r²  (direction: unit vector from i to j)

RK4 step (h = timestep):
  k1 = h · f(t, y)
  k2 = h · f(t + h/2, y + k1/2)
  k3 = h · f(t + h/2, y + k2/2)
  k4 = h · f(t + h, y + k3)
  y_next = y + (k1 + 2k2 + 2k3 + k4) / 6
```

Energy conservation (E = KE + PE = constant) is your correctness check — if total energy drifts, your timestep is too large or your integrator has a bug.

---

## 6. Astrophage Physics

Based on values stated or implied in the novel:

```
Energy density:       ~10²¹ J/kg          (1 zettajoule per gram)
Mass:                 ~1 × 10⁻¹⁵ kg       (roughly a bacterium)
Reproduction:         doubles every ~8 hours on photon source
Absorption spectrum:  specific wavelength range (visible + IR)
Operating temp:       ~96.415°C           (dies above/below narrow band)
```

### Astrophage Particle Simulation (GPU)

Each particle (organism) tracks:
```
struct Astrophage {
  float3 position;       // AU units for solar system scale
  float3 velocity;       // m/s
  float  energy;         // joules stored
  float  mass;           // kg
  uint   state;          // 0=alive, 1=reproducing, 2=dead
};
```

Update rules per frame:
```
energy += absorbed_photons × absorption_efficiency
if (energy > reproduction_threshold) → spawn child, halve own energy
if (energy < death_threshold)        → state = dead (flagged for removal)
velocity += gravitational_force × dt / mass
position += velocity × dt
```

---

## 7. Solar System Data

| Body | Semi-major axis (AU) | Period (years) | Eccentricity |
|---|---|---|---|
| Mercury | 0.387 | 0.241 | 0.205 |
| Venus | 0.723 | 0.615 | 0.007 |
| Earth | 1.000 | 1.000 | 0.017 |
| Mars | 1.524 | 1.881 | 0.093 |
| Jupiter | 5.203 | 11.86 | 0.048 |
| Saturn | 9.537 | 29.46 | 0.054 |
| Uranus | 19.19 | 84.01 | 0.047 |
| Neptune | 30.07 | 164.8 | 0.009 |

## 8. Tau Ceti System Data

| Parameter | Value |
|---|---|
| Distance from Sol | 11.9 light-years |
| Spectral type | G8.5V (slightly cooler, dimmer than Sun) |
| Mass | 0.783 × M_sun |
| Luminosity | 0.516 × L_sun |
| Confirmed planets | 5 (Tau Ceti e and f in habitable zone) |

---

## 9. Validation Test Cases

Use these to verify your `physics/` implementations match the overvieweffekt.com calculators.

### Test Case A: Earth → Tau Ceti @ 1.0g
```
Input:  d = 11.9 ly,  a = 1.0g = 9.80665 m/s²
Output:
  Ship time:     3.54 years
  Earth time:    13.78 years
  Peak velocity: 0.9944c
  γ at peak:     9.54
```

### Test Case B: Earth → Tau Ceti @ 1.5g
```
Input:  d = 11.9 ly,  a = 1.5g = 14.71 m/s²
Output:
  Ship time:     2.74 years
  Earth time:    13.00 years
  Peak velocity: 0.9968c
  γ at peak:     12.56
```

### Test Case C: Earth → Alpha Centauri @ 1.0g
```
Input:  d = 4.37 ly,  a = 1.0g
Output:
  Ship time:     ~3.6 years
  Earth time:    ~5.9 years
  Peak velocity: ~0.945c
```

### Test Case D: Energy sanity check
```
Ship mass: 1000 tonnes = 1 × 10⁶ kg
Peak velocity: 0.9944c  →  γ = 9.54
E = (9.54 - 1) × 10⁶ × (3×10⁸)² = 7.69 × 10²³ joules

Astrophage fuel needed: 7.69×10²³ / 10²¹ = 769 kg   ← feasible
Fusion fuel needed:     7.69×10²³ / 3.4×10¹⁴ = 2.26×10⁹ kg ≈ 2.26 million tonnes ← impossible
```

If your code produces these numbers (within floating-point tolerance), your physics engine is correct.
