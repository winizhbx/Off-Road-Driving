fileId = fopen('MountainView');
C = textscan(fileId, '%d %d %f', 1000000);
fclose(fileId);

fileId2 = fopen('out');
path = textscan(fileId2, '%d %d', -1);
% [x, y, z] = textread('MountainView', '%d %d %f', 1000000);
length = cellfun('length', path);
height = zeros(length(1), 1);
for i = 1: length(1)
    height(i) = C{3}(path{1}(i)*1000 + path{2}(i));
end
height = height';
% plot3(x, y, z, '.');
% plot3(C{1}, C{2}, C{3}, '.b');
pathnext = cell2mat(path)';
hold on;
plot3(pathnext(1), pathnext(2), height, '.');

% for i = 1:1000000 
%     if (C{3}(i) == 0) 
%         disp(i);
%     end
% end