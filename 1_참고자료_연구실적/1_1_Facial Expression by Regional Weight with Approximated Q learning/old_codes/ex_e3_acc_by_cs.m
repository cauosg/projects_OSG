clc;clear;close all;

load tc3105;
load train_data_3105.mat;
%% train
for ind = 2:64
    tic
    total_train_mats = [];
    CS = ind;
    for i = 1:3105
        [feature, ~]=extractHOGFeatures(tot_imgset(:,:,i), 'CellSize',[CS CS]);
        total_train_mats = [total_train_mats;feature];
    end
    toc
    
    svm = fitcecoc(total_train_mats, total_train_class,'Coding','onevsall');
    %% show
    
    cvval = crossval(svm);
    loss = kfoldLoss(cvval);
    
    accs(ind) = 1 - loss;
end
accs = accs';