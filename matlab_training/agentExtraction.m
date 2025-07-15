% Extract parameters (weights and biases) from the actor and critic networks
agent = ppoAdversarialAgent

actor = getActor(agent);  % Extract the actor representation from the agent
actorParams = getLearnableParameters(actor);  % Get the learnable parameters (weights and biases)
critic = getCritic(agent);  % Extract the critic representation from the agent
criticParams = getLearnableParameters(critic);  % Get the learnable parameters (weights and biases)
criticNet = getModel(critic)
actorNet = getModel(actor)

% Extract weights and biases from actorParams
fc1_weights = actorParams{1};  % 32x7 weights
fc1_biases  = actorParams{2};  % 32x1 biases
fc2_weights = actorParams{3};  % 32x32 weights
fc2_biases  = actorParams{4};  % 32x1 biases
fc3_weights = actorParams{5};  % 6x32 weights
fc3_biases  = actorParams{6};  % 6x1 biases

% save network to file
fileID = fopen('ANNdataExtracted.txt','w');       
exportPolicyToC(fc1_weights,'fc1_weights',fileID);
exportPolicyToC(fc1_biases, 'fc1_biases',fileID);
exportPolicyToC(fc2_weights,'fc2_weights',fileID);
exportPolicyToC(fc2_biases, 'fc2_biases',fileID);
exportPolicyToC(fc3_weights,'fc3_weights',fileID);
exportPolicyToC(fc3_biases, 'fc3_biases',fileID);
fclose(fileID);
