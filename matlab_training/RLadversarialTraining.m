ppoAdversarialAgent = ppoAgent;

% Train the agent
trainOpts = rlTrainingOptions;
trainOpts.MaxEpisodes = 50000;
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

trainOpts.UseParallel = false;  % disable parallelism in training
% Train the agent, envPVPress need to be ajusted for adversarial training
trainingStats = train(ppoAdversarialAgent, envPVPress, trainOpts)
