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
- ~4 hours on an NVIDIA RTX 4090 for full training
- Weather data in `trainingData.mat`

