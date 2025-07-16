# SolPrInt

SolPrInt is a RL agent to control oil extraction in rural, off-grid contexts. A policy trained with 15 years of solar irradiance data is embedded into an ESP32-based controller driving a mechanical press powered by standalone photovoltaics.

---

## Project structure

```
SolPrInt/
├── arduino_agent/          ← ESP32 : RL agent + motor/display control
├── matlab_training/        ← MATLAB PPO training + export ANN to C
├── simulink_model/         ← Model for simulation
└── README.md               ← Project overview (this file)
```

---

##  requirements
### Hardware
- Mechanical oil press equipped with VFD
- PV simulator - Victron MPPT, Victron inverter, GX monitoring
- ESP32, step Motor 28BYJ-48, Joy-it 1.3" ST7789 display

### Software
- Matlab
- Simulink
- Arduino
- Thingspeak


##  Getting started

### 1. Train RL agent in MATLAB

### 2. Deploy agent to ESP32




## License

- Simulink model and SolPrInt code: CC-BY 4.0
