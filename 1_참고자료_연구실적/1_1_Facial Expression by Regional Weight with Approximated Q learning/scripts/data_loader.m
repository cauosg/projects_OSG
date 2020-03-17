clear all;
close all;
clc;
%% init
proc_res = 128;

nums_emotion = 7;
emotions = {'Neutral','Angry','Disgusted','Fear','Happy','Sad', 'Surprised'};

basepath = 'C:\Emotion Estimation From Facial Images\Databases\After Pre-Pocessing\4DBs (7 Expressions)';
datasets = {'MUG', 'WSEFEP', 'JAFFE', 'KDEF'};
%% image collect
nums_interv = 1;
for ind_data=1:4
    min_interv = 99999;
    for ind_emo = 1:nums_emotion
        name_train = strcat(basepath, '\', datasets(ind_data), '\Train\', emotions(ind_emo));
        Images = imageSet(char(name_train));
        nums_temp = Images.Count;
        min_interv = min(min_interv,nums_temp);
        
        for img_ind = 1:nums_temp
            train_data{ind_emo, nums_interv + img_ind - 1} = imresize(read(Images,img_ind),[proc_res,proc_res]);
        end   
        
        train_class(ind_emo,nums_interv : nums_interv + nums_temp - 1) = ind_emo;
    end
    nums_interv = nums_interv + min_interv;
end
[~,nums_train] = size(train_class);
disp('img load complete')