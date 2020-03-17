clc;clear;close all;

res_proc = 128;
res_show = 700;
res_ext = 12;
min_csize = 1;
now_emo = 2;

nums_emotion = 7;
emotions = {'Angry','Disgusted','Fear','Happy','Neutral', 'Sad', 'Surprised'};

basepath = 'C:\Emotion Estimation From Facial Images\Databases\After Pre-Pocessing\4DBs (7 Expressions)';
datasets = {'JAFFE','KDEF', 'MUG', 'WSEFEP'};

load('svm_t191002.mat');
load('grids_t191002.mat');

nums_train = 0;
[num_ci,~] = size(origin_grids);
for datanum=1:4
    for i = 1:nums_emotion
        train_pname = strcat(basepath, '\', datasets(datanum), '\Train\', emotions(i));
        Images = imageSet(char(train_pname));
        
        tempnum_train = Images.Count;
        %tot_imgset(:,:,1) = zeros(res_proc,res_proc);
        for img_count = 1:tempnum_train
            tot_imgset{datanum, nums_train + img_count - 1} = imresize(read(Images,img_count),[res_proc,res_proc]);
        end
        train_class(nums_train : nums_train + tempnum_train - 1) = i;
        
        nums_train = nums_train + tempnum_train;
    end


tot_features = [];

now_datanum = datanum;

for img_i = 1:nums_train
    timg = cell2mat(tot_imgset(now_datanum, img_i));
    temp_features = [];
    for ci = 1:num_ci
        
        c_grid = origin_grids(ci,:);
        c_length = ceil(c_grid(3)/2);
        
        cp = [c_grid(1) + c_length, c_grid(2) + c_length];
        c_rad = c_grid(3);
        
        nbp = [max(cp(1) - c_rad,1),max(cp(2) - c_rad,1)];%new begin point
        nep = [min(cp(1) + c_rad, res_proc),min(cp(2) + c_rad, res_proc)];%new end point
        
        c_grid = cat(2,nbp,nep-nbp);

        cs = 0.5*res_ext;
        
        [hogFeature, ~]=extractHOGFeatures(imresize(imcrop(timg,c_grid),[res_ext,res_ext]), 'CellSize',[cs cs]);
        temp_features = cat(2,temp_features,hogFeature);
        
        %figure;imshow(cimg);
    end
    tot_features = [tot_features;temp_features];
end
feature_bank{now_datanum} = tot_features;
end

total_train_rst = svm.predict(tot_features);
        
[~,t_accu] = getaccuracy(total_train_rst,train_class);