# Simulink Model – SolPrInt

This directory hosts the model used for  RL training.

## Files
- `Simulation_Model_Env.slx` Main Simulink system: PV system  → oil press load → RL framework
- `trainingData.mat` 15‑year simulation log (hourly) used to train the PPO agent

## How to run
```matlab
load trainingData.mat
open('Simulation_Model_SolPrInt.slx');
% Set simulation length 
% Run
