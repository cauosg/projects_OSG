clear all;
close all;
clc;

%% init
maxCS = 32;
CS = 8;

emotion_nums = 7;
emotions = {'Angry','Disgusted','Fear','Happy','Neutral', 'Sad', 'Surprised'};

basepath = 'C:\Emotion Estimation From Facial Images\Databases\After Pre-Pocessing\4DBs (7 Expressions)';
datasets = {'JAFFE','KDEF', 'MUG', 'WSEFEP'};

%% image reader
    nums_train = 1;nums_test = 1;
    train_images = [];
    test_images = [];
    nums_each_train = zeros(5,1);
    nums_each_test = zeros(5,1);
%     nums_each_train(1) = 1;
%     nums_each_test(1) = 1;
for datanum=1:4
for i = 1:emotion_nums

    train_pname = strcat(basepath, '\', datasets(datanum), '\Train\', emotions(i));
    [outp, tempnum_train] = imagereader(char(train_pname));
    train_images = cat(3,train_images,outp);
    train_class(nums_train : nums_train + tempnum_train - 1) = i;
    clear outp;
    
    test_pname = strcat(basepath, '\', datasets(datanum), '\Test\', emotions(i));
    [outp,tempnum_test] = imagereader(char(test_pname));
    test_images = cat(3,test_images,outp);
    test_class(nums_test : nums_test + tempnum_test - 1) = i;
    clear outp;
    
    nums_train = nums_train + tempnum_train;
    nums_test = nums_test + tempnum_test;
end
nums_each_train(datanum) = nums_train - sum(nums_each_train);
nums_each_test(datanum) = nums_test - sum(nums_each_test);
end
     nums_each_train(1) = nums_each_train(1) - 1;
     nums_each_test(1) = nums_each_test(1) - 1;

%% train
[~,~,strain] = size(train_images);
[~,~,stest] = size(test_images);
train_features = [];
test_features = [];

tic
for i=1:strain
[feature, ~]=extractHOGFeatures(train_images(:,:,i), 'CellSize',[CS CS]);
train_features = [train_features;feature];
end
toc;
for i=1:stest
[feature, ~]=extractHOGFeatures(test_images(:,:,i), 'CellSize',[CS CS]);
test_features = [test_features;feature];
end

tempknn = fitcknn(train_features,train_class);
test_rst = tempknn.predict(test_features);

[~, accu] = getaccuracy(test_rst,test_class);


% %% show
% figure; 
% set(gcf, 'Position',  [500, 10, 700, 700]);
% 
% Ct= confusionmat(test_rst, test_class);
% confusionchart(Ct,emotions);
% title(strcat('dataset : All, confusion Matrix of test , accuracy : ',num2str(accu),'%'));
