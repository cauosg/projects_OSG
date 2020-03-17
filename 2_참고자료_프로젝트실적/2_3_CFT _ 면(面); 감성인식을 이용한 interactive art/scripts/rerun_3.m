runtimes = 100000;
levels = 3;       % dep
iterations = 20;  %
specific = 1; % 자연스런정도(사용할 feature maps의 수) 적을수록 빠름
procsize = [227 227];
emotionnum = 7; 
buffer = zeros(1200,1600,3);
%%
close all;
tidx = [];
for ii=1:7
timg_name = strcat(num2str(ii),'.jpg');
initimg = imread(timg_name);
initimg = imresize(initimg, InputSize(1:2));
[label, scores] = classify(net, initimg);
[~, idx] = sort(scores, 'descend');
idx = idx(specific:-1 : 1);
tidx(:,:,ii) = idx;
end
%close all;figure('Position',[-1600 -150 1600 1150]);
%close all;%figure('Position',[0 -150 1600 1150]);
%close all;figure('Position',[0 -120 1550 1200]);
%close all;figure('Position',[-40 -140 1850 1500]);
close all;figure('Position',[-48 76 1600 1200]);
imh = imshow(buffer,'Border','tight','InitialMagnification','fit','InitialMagnification','fit');
%implay(buffer);
for maxruntime = 1:runtimes
    
%%
img = snapshot(cam);
bbox = step(faceDetector,img);
[fnum, ~] = size(bbox);
%webcam에서 이미지 grabing 및 얼굴영역 추출, **인식얼굴 0개일시 예외 필요

if fnum>0
    
fregion = imcrop(img,bbox(1,:));
fregion = rgb2gray(fregion);
%**촬영환경 따라 preprocessing 필요
fregion = imresize(fregion,[160 160]);
%%
%감정인식(1~7)
[Matrix, ~]=extractHOGFeatures(fregion,'CellSize',[CS CS]);
emotionnum = knn.predict(Matrix);
end
%%
%출력
img = imadjust(img,adjpreset(:,:,emotionnum));

outimg = img;
[ow oh oc] = size(outimg);
outimg = imresize(outimg, procsize);
%outimg = imresize(outimg, InputSize(1:2));
%outimg = gpuArray(imresize(outimg, InputSize(1:2)));
I = deepDreamImage(net, layer, tidx(:,:,emotionnum), ...
	'Verbose', true, ...
	'NumIterations', iterations, ...
	'PyramidLevels', levels, ...
	'InitialImage', outimg);

[w h c z] = size(I);
outputimg = zeros(w, h, c);

for index = 1:z
%figure;
temp = I(:, : , : , index);
%imshow(temp);
outputimg = outputimg + temp;
end

outputimg = outputimg / z;
outputimg = imresize(outputimg,[1275 1600]);

%output = gpuArray(imresize(output,[ow oh]));
%figure;
%hold on;
%imshow(output);
%implay(buffer);
set(imh, 'CData', outputimg);
drawnow();

emotions(emotionnum)
%title(emotions(emotionnum));

% drawnow();
% hold on;
end