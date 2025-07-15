% Example function to convert weight matrix to C array for Arduino
function exportPolicyToC(weights, layerName, fileID)
    [rows, cols] = size(weights);
    
    % Print weights as C arrays
    fprintf(fileID,'float %s[%d][%d] = {\n', layerName, rows, cols);
    for r = 1:rows
        fprintf(fileID,'    ');
        fprintf(fileID,'%.6f, ', weights(r, :)); % Print weights with 6 decimal precision
        fprintf(fileID,'\n');
    end
    fprintf(fileID,'};\n');

end
