# MATLAB PPO Training – SolPrInt

This folder contains all MATLAB scripts used to train the RL agent deployed in SolPrInt.

## Folder structure

- `train/ppo_train.m` – Main PPO training script
- `validate/` – Scripts for robustness and yield testing under weather variability
- `env/OilPressEnv.m` – Custom RL environment with reward and system dynamics
- `convertToC.m` – Export the trained policy to C-style arrays for deployment
- `netDataBacked.txt` – Output weight file (to be copied to `arduino_agent/`)

## Requirements

- MATLAB R2024a
- Reinforcement Learning Toolbox
- Simulink

## Agent overview

- **State vector (7D)**: hour, month, PV power, press power, SoC, weekly irradiance, previous action
- **Actions**: Discrete speed values [0, 10, ..., 70 rpm]
- **Reward**: Penalizes interruptions and favors continuous desirable output


## Simulation model

- Simulink model includes PV system, oil press model, environment and reward
- Simulation outputs used to train and validate RL agent
- Pre-logged dataset `trainingData.mat` included

