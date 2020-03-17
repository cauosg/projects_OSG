clear all;
close all;
clc;

load('train_rst - บนป็บป (4)/features_23.mat');
load('ck327_emos.mat');

svm = fitcecoc(rst_mat, emoset)
cvval = crossval(svm)
loss = kfoldLoss(cvval)
for i=1:1000
    tic;
    total_train_rst = kfoldPredict(cvval);
    times(i) = toc;
end
[~,t_accu] = getaccuracy(total_train_rst,emoset);
times = times';

mean(times)

set(gcf, 'Position',  [500, 10, 700, 700]);
plot(times);
title(strcat('mean : ',num2str(mean(times))));