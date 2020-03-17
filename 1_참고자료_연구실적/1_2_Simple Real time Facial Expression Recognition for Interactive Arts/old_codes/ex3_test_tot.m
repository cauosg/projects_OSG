clear all;
close all;
clc;

%% init
CS = 8;

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
%%  loop
for datanum=1:4
    
train_mats = [];
test_mats = [];
train_class = [];
test_class = [];
nums_train = 1;nums_test = 1;

for i = 1:emotion_nums

    train_pname = strcat(basepath, '\', datasets(datanum), '\Train\', emotions(i));
    [temp_train_mat,tempnum_train] = HOG2(CS, char(train_pname));
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
end

%% train
knn = fitcknn(total_train_mats, total_train_class);

%% testing
test_rst = knn.predict(total_test_mats);
train_rst = knn.predict(total_train_mats);

%% 
[len_test ~] = size(test_rst);
errorsum_test = 0;

for i=1:len_test
    p = total_test_class(i);
    q = test_rst(i);
    if p~=q
        errorsum_test = errorsum_test + 1;
    end
end
%%

[len_train ~] = size(train_rst);
errorsum_train = 0;

for i=1:len_train
    p = total_train_class(i);
    q = train_rst(i);
    if p~=q
        errorsum_train = errorsum_train + 1;
    end
end
%% show
figure; 
set(gcf, 'Position',  [500, 10, 1400, 600]);

subplot(1,2,1);
C1 = confusionmat(total_train_class', train_rst);
confusionchart(C1,emotions);
title(strcat('dataset : All, confusion Matrix of train , accuracy : ',num2str((len_train - errorsum_train)/len_train*100),'%'));

subplot(1,2,2);
C2 = confusionmat(total_test_class', test_rst);
confusionchart(C2,emotions);
title(strcat('dataset : All, confusion Matrix of test , accuracy : ',num2str((len_test - errorsum_test)/len_test*100),'%'));

