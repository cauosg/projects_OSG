clear all
clc;
close all;
res_proc = 1024;
FDetect = vision.CascadeObjectDetector;

nums_faces = 0;

emotions = {'Angry','Disgusted','Fear','Happy','Neutral', 'Sad', 'Surprised'};


fs = matlab.io.datastore.DsFileSet('ck+\ck+_full\emotions\','IncludeSubfolders',true);
allfiles = resolve(fs);

for ind_img = 1:size(allfiles,1)
    
    path_now = allfiles.FileName(ind_img);
    paths = split(path_now,'\');
    temp_name = split(paths(13),'_emotion.txt');
    path_img = strcat(paths(8),'\',paths(9),'\','faces','\',paths(11),'\',...
        paths(12),'\',temp_name(1),'.png');
    
    
    img = imread(path_img);
    [m,n,c] = size(img);
    if c>1
        img = rgb2gray(img);
    end
    BB = step(FDetect,img);
    
    num_faces = size(BB,1);
    if num_faces>1
        [~,ind_temp] = max(BB(:,3)); 
        BB = BB(ind_temp,:);
    end
    nums_faces = nums_faces + 1;
    
    imgset{nums_faces} = imresize(imcrop(img,BB),[res_proc,res_proc]);
    C = textscan(fopen(path_now),'%f');
    emoset(nums_faces) = cell2mat(C);
    
    imwrite(imcrop(img,BB),char(strcat('collected_ck\',temp_name(1),'.png')),'png');
end

