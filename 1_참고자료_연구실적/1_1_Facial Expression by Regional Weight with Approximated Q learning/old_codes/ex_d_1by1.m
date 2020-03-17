clear all;
close all;
clc;
%% init
max_iter = 100;
reward_func = 1.00;

proc_res = 100;
show_res = 700;
min_csize = 2;

emotion_nums = 7;
emotions = {'Angry','Disgusted','Fear','Happy','Neutral', 'Sad', 'Surprised'};

basepath = 'C:\Emotion Estimation From Facial Images\Databases\After Pre-Pocessing\4DBs (7 Expressions)';
datasets = {'JAFFE','KDEF', 'MUG', 'WSEFEP'};
%% image collect
nums_train = 1;
nums_interv = 1;
for ind_data=1:4
    nums_temp2 = 0;
    min_interv = 99999;
    for ind_emo = 1:emotion_nums
        name_train = strcat(basepath, '\', datasets(ind_data), '\Train\', emotions(ind_emo));
        Images = imageSet(char(name_train));
        nums_temp = Images.Count;
        min_interv = min(min_interv,nums_temp);
        for img_ind = 1:nums_temp
            dataset{ind_emo, nums_interv + img_ind - 1} = imresize(read(Images,img_ind),[proc_res,proc_res]);
        end
        
        train_class(ind_emo,nums_interv : nums_interv + nums_temp - 1) = ind_emo;
        nums_temp2 = nums_temp2 + nums_temp;   
    end
    nums_interv = nums_interv + min_interv;
    nums_train = nums_train + nums_temp2;
end
nums_train = nums_train - 1;
disp('img load complete')
%%
%cell2mat(dataset(1,1))
for ind_img = 1:nums_train
    
end