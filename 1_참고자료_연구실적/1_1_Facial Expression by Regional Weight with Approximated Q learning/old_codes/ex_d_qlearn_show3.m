clear all;
close all;
clc;
%% init
max_iter = 50;

res_proc = 128;
res_show = 512;
min_csize = 2;

nums_emotion = 7;
nums_timg = 10;
emotions = {'Angry','Disgusted','Fear','Happy','Neutral', 'Sad', 'Surprised'};

basepath = 'D:\Emotion Estimation From Facial Images\Databases\After Pre-Pocessing\4DBs (7 Expressions)';
datasets = {'JAFFE','KDEF', 'MUG', 'WSEFEP'};
%% image collect
nums_interv = 1;
load train_data_128;


for ind_data=1:4
    min_interv = 99999;
    for ind_emo = 1:nums_emotion
        name_train = strcat(basepath, '\', datasets(ind_data), '\Train\', emotions(ind_emo));
        Images = imageSet(char(name_train));
        nums_temp = Images.Count;
        min_interv = min(min_interv,nums_temp);
%         
%                 for img_ind = 1:nums_temp
%                     train_data{ind_emo, nums_interv + img_ind - 1} = imresize(read(Images,img_ind),[res_proc,res_proc]);
%                 end
        
        train_class(ind_emo,nums_interv : nums_interv + nums_temp - 1) = ind_emo;
    end
    nums_interv = nums_interv + min_interv;
end
[~,nums_train] = size(train_class);
disp('img load complete')
%%
%cell2mat(train_data(1,1))
%profile on;
% for ind_iter = 1:nums_train
q_mat = zeros(res_proc,res_proc);


origin_grids(:,:) = [1,1,res_proc,res_proc];
origin_features = [];
q_val = zeros(res_proc,res_proc);
rwd_val = 0;
 
for ind_iter = 1:max_iter
    disp(strcat('iteration : ',num2str(ind_iter)))
    %% divisi
    [num_grids,~] = size(origin_grids);
    rewards = 0;
    %out_grids = origin_grids;
    out_grids = [];
    clear rst_svm;
    clear rst_mat;
    clear rst_train;
    
    
    for ind_grid=1:num_grids
        flag_skip = false;
        [proc_len,nex_grids] = step_grid(origin_grids(ind_grid,:));
        min_check = min(min(nex_grids(:,3),nex_grids(:,4)));
        if min_check<min_csize*2
            flag_skip = true;
        end
        check_grids = grid_extract(origin_grids,nex_grids,ind_grid);
        if flag_skip
            continue;
        end
        %% fragment
        
        [num_ci,~] = size(nex_grids);
        clear cimg;
        tot_features = [];
        for ind_timg = 1:nums_timg
            for ind_emo = 1:nums_emotion
                timg = cell2mat(train_data(ind_emo, ind_timg));
                temp_features = [];
                for ci = 1:num_ci
                    
                    c_grid = nex_grids(ci,:);
                    c_length = ceil(c_grid(3)/2);
                    
                    cp = [c_grid(1) + c_length, c_grid(2) + c_length];
                    c_rad = c_grid(3);
                    
                    nbp = [max(cp(1) - c_rad,1),max(cp(2) - c_rad,1)];%new begin point
                    nep = [min(cp(1) + c_rad, res_proc),min(cp(2) + c_rad, res_proc)];%new end point
                    
                    c_grid = cat(2,nbp,nep-nbp);
                    cs = floor(min(c_grid(3),c_grid(4))/2);
                    [hogFeature, ~]=extractHOGFeatures(imcrop(timg,c_grid), 'CellSize',[cs cs]);
                    temp_features = cat(2,temp_features,hogFeature);
                end
                tot_features = [tot_features;temp_features];
            end
        end
        tot_features = feature_cat(origin_features,tot_features,ind_grid);
        
        %% train
        temp_class = train_class(:,1:ind_timg);
        [~,nums_cl] = size(temp_class);
        otd_class = [];
        for ind_cl = 1:nums_cl
            otd_class = [otd_class;temp_class(:,ind_cl)];
        end
        svm = fitcecoc(tot_features, otd_class);
        
        cvval = crossval(svm);
        loss = kfoldLoss(cvval);
        max_prev = rwd_val;
        for ci=1:num_ci
            temp_mat = zeros(res_proc,res_proc);
            temp_mat(nex_grids(ci,1):min(nex_grids(ci,1) + nex_grids(ci,3),res_proc),...
                nex_grids(ci,2):min(nex_grids(ci,2) + nex_grids(ci,4),res_proc)) = 1-loss-max_prev;
            q_mat = q_mat + temp_mat;
        end
        imshow(imresize(q_mat,[res_show,res_show],'box'));
        title(strcat('visualized Q_values of iteration',num2str(ind_iter)));
        drawnow
        saveas(gcf,strcat('train_rst\qvals_',num2str(ind_iter),'_',num2str(ind_grid),'.png'));
        %% testing
        total_train_rst = kfoldPredict(cvval);
        [~,t_accu] = getaccuracy(total_train_rst,otd_class);
        rwd = t_accu/100;
        if rwd>max(rewards)
            rst_svm = svm;
            rst_mat = tot_features;
            rst_train = total_train_rst;
        end
        rewards(ind_grid) = rwd;
    end
    %% output grid
    
    [rwd_val, max_ind] = max(rewards);
    
    if rwd_val<0.001
        disp('training terminated by no more division');
        break;%terminate training
    end
    rewards(ind_grid) = rwd;
    accus_iter(ind_iter) = rwd;
    [~,input_grids] = divide_grid(origin_grids(max_ind,:));
    out_grids = grid_extract(origin_grids,input_grids,max_ind);
    
    
    
    origin_features = rst_mat;
    origin_grids = out_grids;
    %%
    [~,~,grids_to_draw] = size(out_grids);
    
    for di = 1:grids_to_draw
        timg= insertObjectAnnotation(timg,'rectangle',out_grids(:,:,di),'','Color','green');
    end
    %
    %     origin_features = rst_mat;
    %     origin_grids = out_grids;
    
    %     imshow(imresize(timg,[res_show,res_show],'box'));
    %     title(strcat('epoch',num2str(ind_iter),', grids'));
    %
    %     saveas(gcf,strcat('train_rst\grids_',num2str(ind_iter),'.png'));
    %     save(strcat('train_rst\rewards_',num2str(ind_iter),'.mat'),'rewards');
    %     %% show confumat
    %     [errorsum_test,t_accu] = getaccuracy(rst_train,otd_class);
    %     %% show
    %     set(gcf, 'Position',  [500, 10, 700, 700]);
    %
    %     Ct= confusionmat(rst_train, otd_class);
    %     confusionchart(Ct,emotions);
    %     title(strcat('dataset : All, confusion Matrix of test , accuracy : ',num2str(t_accu),'%'));
    %     saveas(gcf,strcat('train_rst\confusion_',num2str(ind_iter),'.png'));
    %
end
%profile viewer