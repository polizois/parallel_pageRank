function [ pr ] = getPageRanks( matrixFile, saveFile )
%GETPAGERANKS Summary of this function goes here
%   Detailed explanation goes here

% Load matri from file to memory
data = load(matrixFile);
A = data.A;

% Calculate PageRanks
G = digraph(A);
pr = centrality(G, 'pagerank', 'MaxIterations', 200, 'FollowProbability', 0.85);

% Save results to bin file
fileID = fopen(saveFile,'w');
fwrite(fileID,pr,'double');
fclose(fileID);

end

