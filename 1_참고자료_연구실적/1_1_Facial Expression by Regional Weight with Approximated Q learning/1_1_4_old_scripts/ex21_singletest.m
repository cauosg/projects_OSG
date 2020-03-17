clear all;
close all;
clc;
%% init
res_proc = 128;

emotion_nums = 7;
emotions = {'Angry','Disgusted','Fear','Happy','Neutral', 'Sad', 'Surprised'};

basepath = 'C:\Emotion Estimation From Facial Images\Databases\After Pre-Pocessing\4DBs (7 Expressions)';
datasets = {'JAFFE','KDEF', 'MUG', 'WSEFEP'};
%% image collect
load train_class_3105;
load train_data_3105;

[~,nums_train] = size(train_class);
disp('img load complete')

origin_grids(:,:) = [1,1,res_proc,res_proc];

for ind_CS = 12:16
    CS = 2*(17-ind_CS);
    tot_features = [];
    clear rst_train;
    clear svm;
    clear cvval;
    
    for ind_img = 1:nums_train
        timg = tot_imgset(:,:,ind_img);
        [hogFeature, ~] = extractHOGFeatures(timg, 'CellSize',[CS,CS]);
        tot_features = [tot_features;hogFeature];
    end
    svm = fitcecoc(tot_features, train_class);
    
    cvval = crossval(svm);
    loss = kfoldLoss(cvval);
    
    rst_train = kfoldPredict(cvval);
    
    set(gcf, 'Position',  [500, 10, 700, 700]);
        
    Ct= confusionmat(rst_train, train_class);
    confusionchart(Ct,emotions);
    title(strcat('dataset : All, CS:',num2str(CS),', accuracy : ',num2str((1-loss)*100),'%'));
    saveas(gcf,strcat('train_rst\confusion_',num2str(ind_CS),'.png'));
    
    [errorsum_test,t_accu] = getaccuracy(rst_train,train_class);
    
    t_accus(ind_CS) = t_accu;
    nums_feature(ind_CS) = size(tot_features,2);
    acctory(ind_CS) = (1-loss)*100;
%     xlswrite('train_rst\accs.xls',acctory);
%     xlswrite('train_rst\taccs.xls',t_accus);
    disp(strcat('CS ',num2str(CS),' is complete'));
end