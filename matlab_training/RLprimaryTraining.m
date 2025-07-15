clear all
clc
%load PV and weather data    
load('trainingData.mat');
seedDay=0;
timeEpisode=336;
model = "Simulation_Model_Env";
actionInfo = rlFiniteSetSpec([0 20 26.25 32.5 38.75 45 51.25 57.5 63.75 70]);
actionInfo.Name = "action";
actionInfo.Description = "Command of press, 0 for 0ff, speed from 20 to 70";

observationInfo = rlNumericSpec([7 1],...
    LowerLimit=[0  0   0      0     0     0    0]',...
    UpperLimit=[24 12  436    max(trainingDataIrr)   100   max(trainingDataIrr) 70]');

%env data   hour month  PP  PvP   SOC    Irrweek prevAction
observationInfo.Name = "observation";
observationInfo.Description = "time, press power, PV power SOC, previous irradiation, previous action";

% Define actor network
actorNetwork = [
    featureInputLayer(7)
    fullyConnectedLayer(32)
    reluLayer
    fullyConnectedLayer(32)
    reluLayer
    fullyConnectedLayer(10)
    softmaxLayer];
actorNetwork = dlnetwork(actorNetwork);

% Define critic network
criticNetwork = [
    featureInputLayer(7)
    fullyConnectedLayer(32)
    reluLayer
    fullyConnectedLayer(32)
    reluLayer
    fullyConnectedLayer(1)];
criticNetwork = dlnetwork(criticNetwork);

% Create actor and critic representations
actor = rlDiscreteCategoricalActor(actorNetwork,observationInfo,actionInfo);
critic = rlValueFunction(criticNetwork, observationInfo);

% Define PPO agent options
agentOpts = rlPPOAgentOptions;
agentOpts.ClipFactor = 0.2;
agentOpts.EntropyLossWeight = 0.01;
agentOpts.MiniBatchSize = 64;
agentOpts.ExperienceHorizon = timeEpisode;
agentOpts.NumEpoch = 20;
agentOpts.AdvantageEstimateMethod="GAE";
agentOpts.DiscountFactor = 0.9;
agentOpts.ActorOptimizerOptions.GradientThreshold = Inf;
agentOpts.CriticOptimizerOptions.GradientThreshold = Inf;
agentOpts.CriticOptimizerOptions.LearnRate = 1e-4; % Reduced learning rate
agentOpts.ActorOptimizerOptions.LearnRate = 1e-4; % Reduced learning rate

% Create PPO agent
ppoAgent = rlPPOAgent(actor, critic, agentOpts);

% Train the agent
trainOpts = rlTrainingOptions;
trainOpts.MaxEpisodes = 100000;
trainOpts.MaxStepsPerEpisode = 256;
trainOpts.ScoreAveragingWindowLength = 1000;

% ADDED FOR STOPPING CRITERIA
function stop = customStopCriteria(stats)
    recentRewards = stats.AverageReward; % Vector of rewards
    recentSteps = stats.EpisodeSteps; % Vector of steps
    rewardThreshold = 0.01; % Minimal change in reward
    patience = 250; % Number of episodes to evaluate stability
    stepThreshold = 243.2; % Minimum average steps condition

    if length(recentRewards) > patience
        lastRewards = recentRewards(end-patience+1:end);
        lastSteps = recentSteps(end-patience+1:end);

        % Check if reward stabilization and average steps condition are met
        if (abs((max(lastRewards) - min(lastRewards))/mean(lastRewards)) < rewardThreshold) && (mean(lastSteps) > stepThreshold)
            disp('Reward stabilization and sufficient steps detected. Stopping training.');
            stop = true; % Signal to stop training
        else
            stop = false; % Continue training
        end
    else
        stop = false; % Not enough data to evaluate
    end
end
trainOpts.StopTrainingCriteria = 'Custom';
trainOpts.StopTrainingValue = @customStopCriteria;
trainOpts.SaveAgentCriteria = 'EpisodeFrequency';
trainOpts.SaveAgentValue = 50;
trainOpts.Verbose = false;

trainOpts.Plots = 'training-progress';
trainOpts.UseParallel = true;  % Enable parallelism in training
trainOpts.ParallelizationOptions.Mode = 'sync';  % Enable parallel mode
trainOpts.ParallelizationOptions.StepsUntilDataIsSent = 256; % Parallel training configuration
trainOpts.ParallelizationOptions.WorkerRandomSeeds = -1;
trainOpts.SimulationStorageType = 'none'; % Use disk for simulation storage

% Environment  configured
envPVPress = rlSimulinkEnv(model,model + "/RL Agent",observationInfo,actionInfo);
envPVPress.ResetFcn = @(in) setVariable(in,"seedDay",randi(5480),"Workspace",model);

if gpuDeviceCount > 0
    disp('GPU is available!');
else
    disp('No GPU found. Training will proceed on CPU.');
end

% Train the agent
trainingStats = train(ppoAgent, envPVPress, trainOpts)
