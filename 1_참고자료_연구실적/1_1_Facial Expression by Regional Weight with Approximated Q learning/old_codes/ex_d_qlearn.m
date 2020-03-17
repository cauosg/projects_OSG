clear all;
close all;
clc;
%% init
max_iter = 5;
reward_func = 1.00;

proc_res = 100;
show_res = 700;
min_csize = 2;

nums_emotion = 7;
nums_timg = 1;
emotions = {'Angry','Disgusted','Fear','Happy','Neutral', 'Sad', 'Surprised'};

basepath = 'D:\Emotion Estimation From Facial Images\Databases\After Pre-Pocessing\4DBs (7 Expressions)';
datasets = {'JAFFE','KDEF', 'MUG', 'WSEFEP'};
%% image collect
nums_interv = 1;
load train_data;
for ind_data=1:4
    min_interv = 99999;
    for ind_emo = 1:nums_emotion
        name_train = strcat(basepath, '\', datasets(ind_data), '\Train\', emotions(ind_emo));
        Images = imageSet(char(name_train));
        nums_temp = Images.Count;
        min_interv = min(min_interv,nums_temp);
        
        %         for img_ind = 1:nums_temp
        %             train_data{ind_emo, nums_interv + img_ind - 1} = imresize(read(Images,img_ind),[proc_res,proc_res]);
        %         end
        
        train_class(ind_emo,nums_interv : nums_interv + nums_temp - 1) = ind_emo;
    end
    nums_interv = nums_interv + min_interv;
end
[~,nums_train] = size(train_class);
disp('img load complete')
%%
%cell2mat(train_data(1,1))
profile on;
% for ind_iter = 1:nums_train
for ind_timg = 1:nums_timg
    origin_grids(:,:) = [1,1,proc_res,proc_res];
    origin_features = [];
    q_val = zeros(100,100);
    
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
        
        flag_skip = false;
        for ind_grid=1:num_grids
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
            
            for img_ind = 1:nums_emotion
                
                timg = cell2mat(train_data(img_ind, ind_timg));
                temp_features = [];
                for ci = 1:num_ci
                    
                    c_grid = nex_grids(ci,:);
                    c_length = ceil(c_grid(3)/2);
                    
                    cp = [c_grid(1) + c_length, c_grid(2) + c_length];
                    c_rad = c_grid(3);
                    
                    nbp = [max(cp(1) - c_rad,1),max(cp(2) - c_rad,1)];%new begin point
                    nep = [min(cp(1) + c_rad, proc_res),min(cp(2) + c_rad, proc_res)];%new end point
                    
                    c_grid = cat(2,nbp,nep-nbp);
                    cs = floor(min(c_grid(3),c_grid(4))/2);
                    [hogFeature, ~]=extractHOGFeatures(imcrop(timg,c_grid), 'CellSize',[cs cs]);
                    temp_features = cat(2,temp_features,hogFeature);
                end
                tot_features = [tot_features;temp_features];
            end
            tot_features = feature_cat(origin_features,tot_features,ind_grid);
            
            %% train
            otd_class = train_class(:,1:ind_timg);
            svm = fitcecoc(tot_features, otd_class);
            
            cvval = crossval(svm);
            loss = kfoldLoss(cvval);
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
        [~,input_grids] = divide_grid(origin_grids(max_ind,:));
        out_grids = grid_extract(origin_grids,input_grids,max_ind);
        
        
        origin_features = rst_mat;
        origin_grids = out_grids;
    end
    %%
    [~,~,grids_to_draw] = size(out_grids);
    
    for di = 1:grids_to_draw
        timg= insertObjectAnnotation(timg,'rectangle',out_grids(:,:,di),'','Color','green');
    end
    
    %figure;
    origin_features = rst_mat;
    imshow(imresize(timg,[show_res,show_res],'box'));
    %set(gcf, 'Position',  [500, 10, 700, 700]);
    title(strcat('epoch',num2str(ind_iter),', grids'));
    origin_grids = out_grids;
    
    saveas(gcf,strcat('train_rst\grids_',num2str(ind_iter),'.png'));
    save(strcat('train_rst\rewards_',num2str(ind_iter),'.mat'),'rewards');
    %% show confumat
    [errorsum_test,t_accu] = getaccuracy(rst_train,otd_class);
    %% show
    set(gcf, 'Position',  [500, 10, 700, 700]);
    
    Ct= confusionmat(rst_train, otd_class);
    confusionchart(Ct,emotions);
    title(strcat('dataset : All, confusion Matrix of test , accuracy : ',num2str(t_accu),'%'));
    saveas(gcf,strcat('train_rst\confusion_',num2str(ind_iter),'.png'));
    
    
end
profile viewer