clear all;
close all;
clc;
%% init
proc_size = 100;

emotion_nums = 7;
emotions = {'Angry','Disgusted','Fear','Happy','Neutral', 'Sad', 'Surprised'};

basepath = 'C:\Emotion Estimation From Facial Images\Databases\After Pre-Pocessing\4DBs (7 Expressions)';
datasets = {'JAFFE','KDEF', 'MUG', 'WSEFEP'};
%% image collect
nums_train = 1;

for datanum=1:4
    for i = 1:emotion_nums
        train_pname = strcat(basepath, '\', datasets(datanum), '\Train\', emotions(i));
        Images = imageSet(char(train_pname));
        
        tempnum_train = Images.Count;
            %tot_imgset(:,:,1) = zeros(proc_size,proc_size);
            for img_count = 1:tempnum_train
                tot_imgset(:,:,nums_train + img_count - 1) = imresize(read(Images,img_count),[proc_size,proc_size]);
            end
        train_class(nums_train : nums_train + tempnum_train - 1) = i;
        
        nums_train = nums_train + tempnum_train;
    end
end