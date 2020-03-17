clear all;
close all;
clc;

%% init
CS = 24;

emotion_nums = 7;
emotions = {'Angry','Disgusted','Fear','Happy','Neutral', 'Sad', 'Surprised'};

basepath = 'C:\Emotion Estimation From Facial Images\Databases\After Pre-Pocessing\4DBs (7 Expressions)';
datasets = {'JAFFE','KDEF', 'MUG', 'WSEFEP'};
%datanum = 4;

%% proceeding
total_train_mats = [];
total_test_mats = [];
total_train_class = [];
total_test_class = [];

total_nums_train = 1;total_nums_test = 1;
figure; 
set(gcf, 'Position',  [500, 10, 1000, 1000]);
%%  loop
for datanum=1:4
clear tempknn;
clear train_class;
clear test_class;
clear test_rst;
clear train_rst;
nums_train = 1;nums_test = 1;
train_mats = [];
test_mats = [];

for i = 1:emotion_nums

    train_pname = strcat(basepath, '\', datasets(datanum), '\Train\', emotions(i));
    tic
    [temp_train_mat,tempnum_train] = HOG2(CS, char(train_pname));
    toc
    train_class(nums_train : nums_train + tempnum_train - 1) = i;
    
    test_pname = strcat(basepath, '\', datasets(datanum), '\Test\', emotions(i));
    [temp_test_mat,tempnum_test] = HOG2(CS, char(test_pname));
    test_class(nums_test : nums_test + tempnum_test - 1) = i;
    
    train_mats = vertcat(train_mats, temp_train_mat);
    test_mats = vertcat(test_mats, temp_test_mat);
    
    nums_train = nums_train + tempnum_train;
    nums_test = nums_test + tempnum_test;
end
%% proceeding
total_train_mats = vertcat(total_train_mats,train_mats);
total_test_mats = vertcat(total_test_mats,test_mats);
total_train_class = horzcat(total_train_class, train_class);
total_test_class = horzcat(total_test_class, test_class);

total_nums_train = total_nums_train + nums_train;
total_nums_test = total_nums_test + nums_test;

%% train
tempknn = fitcknn(train_mats, train_class);

%% test
test_rst = tempknn.predict(test_mats);

[error_test accu] = getaccuracy(test_rst,test_class);

subplot(2,2,datanum);
C = confusionmat(test_rst,test_class);
confusionchart(C,emotions);
title(strcat('dataset : ',datasets(datanum),', confusion Matrix of train , accuracy : ',num2str(accu),'%'));
end

%% train
knn = fitcknn(total_train_mats, total_train_class);

%% testing
total_test_rst = knn.predict(total_test_mats);

[errorsum_test t_accu] = getaccuracy(total_test_rst,total_test_class);
%% show
figure; 
set(gcf, 'Position',  [500, 10, 700, 700]);

Ct= confusionmat(total_test_rst, total_test_class);
confusionchart(Ct,emotions);
title(strcat('dataset : All, confusion Matrix of test , accuracy : ',num2str(t_accu),'%'));

