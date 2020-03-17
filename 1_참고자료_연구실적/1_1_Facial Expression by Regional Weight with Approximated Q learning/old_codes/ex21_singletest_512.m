clear all;
close all;
clc;
%% init
res_proc = 512;

emotion_nums = 7;
emotions = {'Angry','Disgusted','Fear','Happy','Neutral', 'Sad', 'Surprised'};

basepath = 'C:\Emotion Estimation From Facial Images\Databases\After Pre-Pocessing\4DBs (7 Expressions)';
datasets = {'JAFFE','KDEF', 'MUG', 'WSEFEP'};
%% image collect
load ck327_emos;
load ck327_faces_512;

train_class = emoset;
clear emoset;
tot_imgset = imgset;
clear imgset;

[~,nums_train] = size(train_class);
disp('img load complete')

origin_grids(:,:) = [1,1,res_proc,res_proc];

for ind_CS = 16:16
    %CS = 8*(17-ind_CS);
    CS = 32;
    tot_features = [];
    clear rst_train;
    clear svm;
    clear cvval;
    
    for ind_img = 1:nums_train
        timg = cell2mat(tot_imgset(ind_img));
        tic
        [hogFeature, ~] = extractHOGFeatures(timg, 'CellSize',[CS,CS]);
        times(ind_img) = toc;
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