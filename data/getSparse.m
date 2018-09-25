function [ B ] = getSparse( A )
%GETSPARSE Summary of this function goes here
%   Detailed explanation goes here

[a b] = find(A);

B = [a b];

B = sortrows(B, 1, 'ascend');

% Save results to bin file
fileID = fopen('sparseMatrix.bin','w');
fwrite(fileID,B','int');
fclose(fileID);

end

