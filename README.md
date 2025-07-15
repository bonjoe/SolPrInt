# SolPrInt – Reinforcement Learning Control of a Standalone PV Oil Press

SolPrInt is a RL agent to optimize oil extraction in rural, off-grid contexts. A policy trained with 15 years of solar irradiance data is embedded into an ESP32-based controller driving a mechanical press powered by standalone photovoltaics.

---

## Project structure

```
SolPrInt/
├── arduino_agent/          ← ESP32 : RL agent + motor/display control
├── matlab_training/        ← MATLAB PPO training + export ANN to C
├── simulink_model/         ← Model for simulation
├── requirements.txt        ← requirements
└── README.md               ← Project overview (this file)
```

---

## 🚀 Getting started

### 1. Clone the repository

```bash
git clone https://github.com/<user>/SolPrInt.git
cd SolPrInt
```

### 2. Train RL agent in MATLAB

### 3. Deploy agent to ESP32



## Agent overview

- **State vector (7D)**: hour, month, PV power, press power, SoC, weekly irradiance, previous action
- **Actions**: Discrete speed values [0, 10, ..., 70 rpm]
- **Reward**: Penalizes interruptions and favors continuous desirable output

---

## Simulation model

- Simulink model includes PV system, oil press model, environment and reward
- Simulation outputs used to train and validate RL agent
- Pre-logged dataset `trainingData.mat` included


---

## License

- Simulink model and SolPrInt code: CC-BY 4.0
