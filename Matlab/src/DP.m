% load('MountainViewElevationMap.mat');
% load('MountainViewWeightMap.mat');
mapSize = 1000;
% [NI, NNI] = generateNI(mapSize);


StartPoint = (123 - 1) * 1000 + 230;
EndPoint = (100 - 1) * 1000 + 200;


R = zeros(mapSize^2, 1);
R(EndPoint) = -2000;
NI(EndPoint, 1) = EndPoint;
NI(EndPoint, 2:NNI(EndPoint))=zeros(1,NNI(EndPoint)-1);
NNI(EndPoint) = 1;

times = 100;
Cost = zeros(mapSize^2, times+1);
Po = zeros(mapSize^2, times);
for i = 1 : times
    for j = 1 : mapSize^2
        Min = zeros(1, 4);
        for k = 1 : NNI(j)
            [x, y] = transformToPoint(NI(j, k), mapSize);
            Min(k) = Cost(NI(j, k), i) + R(j) + WeightMap(x, y);
        end
        [Cost(j,i+1), m]=min(Min);
        Po(j,i) = NI(j,m);
    end
end

PO=Po(:,times);
PGSP=zeros(100,1);
PGSP(1)=StartPoint;
Path = zeros(100, 2);
[Path(1, 1), Path(1, 2)]= transformToPoint(StartPoint, 1000);

for i=1:100
PGSP(i+1)=PO(PGSP(i));
[Path(i + 1, 1), Path(i + 1, 2)] = transformToPoint(PO(PGSP(i)), 1000);
end


function [x, y] = transformToPoint(point, mapSize)
y = mod(point, mapSize);
if (y == 0) 
    y = 1000;
    point  = point - 1000;
end
x = floor(point/mapSize) + 1;
end

function [NI, NNI] = generateNI(mapSize)
NI = zeros(mapSize^2, 4);
NNI = zeros(mapSize^2, 1);
for i = 2 : 999
    for j = 2 : 999
        point = (i - 1) * 1000 + j;
        NI(point, 1) = (i - 1 - 1) * 1000 + j;
        NI(point, 2) = (i - 1 + 1) * 1000 + j;
        NI(point, 3) = (i - 1) * 1000 + j - 1;
        NI(point, 4) = (i - 1) * 1000 + j + 1;
        NNI(point) = 4;
    end
end
NI(1, 1) = 2;
NI(1, 2) = 1 + 1000;
NNI(1) = 2;
NI(1000, 1) = 999;
NI(1000, 2) = 1999;
NNI(1000) = 2;
NI(999 * 1000 + 1, 1) = 999 * 1000 + 1;
NI(999 * 1000 + 1, 2) = 999 * 1000 - 1000;
NNI(999 * 1000 + 1) = 2;
NI(1000 * 1000, 1) = 1000 * 1000 - 1;
NI(1000 * 1000, 2) = 1000 * 1000 - 1000;
NNI(1000 * 1000) = 2;

for i = 2 : 999
    NI(i, 1) = i - 1;
    NI(i, 2) = i + 1;
    NI(i, 3) = i + 1000;
    NNI(i) = 3;
    NI((i-1)*1000 + 1, 1) = (i-1)*1000 + 1 - 1000;
    NI((i-1)*1000 + 1, 2) = (i-1)*1000 + 1 + 1;
    NI((i-1)*1000 + 1, 3) = (i-1)*1000 + 1 + 1000;
    NNI((i-1)*1000 + 1) = 3;
    NI((i-1)*1000 + 1000, 1) = (i-1)*1000 + 1000 - 1000;
    NI((i-1)*1000 + 1000, 2) = (i-1)*1000 - 1;
    NI((i-1)*1000 + 1000, 3) = (i-1)*1000 + 1000 + 1000;
    NNI((i-1)*1000 + 1000) = 3;
    NI(999*1000 + i, 1) = 999*1000 + i - 1;
    NI(999*1000 + i, 2) = 999*1000 + i + 1;
    NI(999*1000 + i, 3) = 999*1000 + i - 1000;
    NNI(999*1000 + i) = 3;
end
end




% fileId = fopen('MountainView');
% C = textscan(fileId, '%d %d %f', 1000000);
% fclose(fileId);
% 
% B = C(3);
% data = B{1,1};
% Map = zeros(1000, 1000);
% for i = 1 : 1000
%     for j = 1 : 1000
%         Map(i, j) = data((i - 1) *1000 + j);
%     end
% end

% WeightMap = zeros(1000, 1000);
% for i = 1 : 999
%     for j = 1 : 999
%         WeightMap(i, j) = (Map(i + 1, j) + Map(i, j + 1) - 2 * Map(i, j)) / 2; 
%     end
% end
% 
% for j = 1 : 999
%     WeightMap(1000, j) = Map(i, j + 1) - Map(i, j);
% end
% 
% for i = 1 : 999
%     WeightMap(i, 1000) = Map(i + 1, j) - Map(i, j);
% end