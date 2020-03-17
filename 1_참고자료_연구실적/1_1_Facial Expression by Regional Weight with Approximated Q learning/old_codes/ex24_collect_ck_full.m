clear all
clc;
close all;
res_proc = 128;
FDetect = vision.CascadeObjectDetector;
numchecker = 0;
nums_faces = 0;

emotions = {'Angry','Disgusted','Fear','Happy','Neutral', 'Sad', 'Surprised'};
ck_dir = {'S052'};

ind_dir2 = 6;
ind_img = 1;
path = char(strcat('ck+\ck+_full\faces\',ck_dir,'\',num2str(ind_dir2,'%03.f'),...
    '\',ck_dir,'_',num2str(ind_dir2,'%03.f'),'_',...
    num2str(ind_img,'%08.f'),'.png'));

img = imread(path);
BB = step(FDetect,img);

num_faces = size(BB,1);
if num_faces>1
    numchecker = numchecker + 1
end
nums_faces = nums_faces + 1;

imgset{nums_faces} = imresize(imcrop(img,BB),[res_proc,res_proc]);

path2 = char(strcat('ck+\ck+_full\emotions\',ck_dir,'\',num2str(ind_dir2,'%03.f'),...
    '\',ck_dir,'_',num2str(ind_dir2,'%03.f'),'_',...
    num2str(ind_img,'%08.f'),'_emotion.txt'));
fileID = fopen(path2);
%fprintf(fileID,'%4.4f\n',x);

%emoset = 

figure;
imshow(img);
hold on;
for i = 1:size(BB,1)
    rectangle('Position',BB(i,:),'LineWidth',1,'LineStyle','-','EdgeColor','g');
end
title('Face Detection');

%%
I = imread('face1.jpg');

BB = step(FDetect,I);

figure,
imshow(I); hold on
for i = 1:size(BB,1)
    rectangle('Position',BB(i,:),'LineWidth',5,'LineStyle','-','EdgeColor','r');
end
title('Face Detection');

EyeDetect = vision.CascadeObjectDetector('EyePairBig');

I = imread('harry_potter.jpg');

BB=step(EyeDetect,I);



figure,imshow(I);
rectangle('Position',BB,'LineWidth',4,'LineStyle','-','EdgeColor','b');
title('Eyes Detection');
Eyes=imcrop(I,BB);
figure,imshow(Eyes);

hold off;
