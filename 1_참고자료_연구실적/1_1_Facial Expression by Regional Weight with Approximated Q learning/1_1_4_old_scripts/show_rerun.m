close all; clc;
figure('Position',[-48 76 1280,960]);
imh = imshow(buffer,'Border','tight','InitialMagnification','fit','InitialMagnification','fit');
%implay(buffer);
outimg = zeros(640,480);
for maxruntime = 1:runtimes    
%%

img = snapshot(cam);

%tic
bbox = step(faceDetector,img);
%toc
[fnum, ~] = size(bbox);

if fnum>0
    
fregion = imcrop(img,bbox(1,:));
fregion = rgb2gray(fregion);
%**촬영환경 따라 preprocessing 필요
fregion = imresize(fregion,[100 100]);

[Matrix, ~]=extractHOGFeatures(fregion,'CellSize',[CS CS],'BlockOverlap',[0,0]);

emotionnum = svm.predict(Matrix);

outimg = img;
outimg = insertObjectAnnotation(outimg,'rectangle',bbox(1,:),emotions(emotionnum));

end

% %%
% %출력
% 

set(imh, 'CData', outimg);
drawnow();

%emotions(emotionnum)
end