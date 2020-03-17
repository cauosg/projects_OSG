clear all;
close all;
clc;
%% init
max_iter = 100;
reward_func = 1.00;

res_proc = 128;
res_show = 700;
res_ext = 12;
min_csize = 1;
now_emo = 2;

nums_emotion = 7;
emotions = {'Angry','Disgusted','Fear','Happy','Neutral', 'Sad', 'Surprised'};

basepath = 'C:\Emotion Estimation From Facial Images\Databases\After Pre-Pocessing\4DBs (7 Expressions)';
datasets = {'JAFFE','KDEF', 'MUG', 'WSEFEP'};
%% image collect
nums_train = 1;
load train_class_3105;
load train_data_3105;

[~,nums_train] = size(train_class);
disp('img load complete')
%%

origin_grids(:,:) = [1,1,res_proc,res_proc];
origin_features = [];
%% iteration
for iter = 1:max_iter
    
    [num_grids,~] = size(origin_grids);
    rewards = 0;
    %out_grids = origin_grids;
    out_grids = [];
    clear rst_svm;
    clear rst_mat;
    clear rst_train;
    %% division
    tic
    for ind_grid=1:num_grids
        [proc_len,nex_grids] = step_grid(origin_grids(ind_grid,:));
        min_check = min(min(nex_grids(:,3),nex_grids(:,4)));
        if min_check<min_csize*2
            continue;
        end
        %findreward
        %% reward
        
        check_grids = grid_extract(origin_grids,nex_grids,ind_grid);
        %% fragment
        
        [num_ci,~] = size(nex_grids);
        clear cimg;
        %close all;
        %clear tot_features;
        tot_features = [];
        for img_ind = 1:nums_train
            timg = tot_imgset(:,:,img_ind);
            temp_features = [];
            for ci = 1:num_ci
                
                c_grid = nex_grids(ci,:);
                c_length = ceil(c_grid(3)/2);
                
                cp = [c_grid(1) + c_length, c_grid(2) + c_length];
                c_rad = c_grid(3);
                
                nbp = [max(cp(1) - c_rad,1),max(cp(2) - c_rad,1)];%new begin point
                nep = [min(cp(1) + c_rad, res_proc),min(cp(2) + c_rad, res_proc)];%new end point
                
                c_grid = cat(2,nbp,nep-nbp);

                %cs = floor(min(c_grid(3),c_grid(4))/2);
                cs = 0.5*res_ext;
                [hogFeature, ~]=extractHOGFeatures(imresize(imcrop(timg,c_grid),[res_ext,res_ext]), 'CellSize',[cs cs]);
                temp_features = cat(2,temp_features,hogFeature);

            end
            tot_features = [tot_features;temp_features];
        end
        tot_features = feature_cat(origin_features,tot_features,ind_grid);
        %% train
        svm = fitcecoc(tot_features, train_class);
        
        cvval = crossval(svm);
        loss = kfoldLoss(cvval);
        %% testing
        total_train_rst = kfoldPredict(cvval);
        %% testing
        [~,t_accu] = getaccuracy(total_train_rst,train_class);

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
    [~,input_grids] = divide_grid(origin_grids(max_ind,:));
    out_grids = grid_extract(origin_grids,input_grids,max_ind);
    
    [~,~,grids_to_draw] = size(out_grids);
    %%
    
    toc
    for di = 1:grids_to_draw
        timg= insertObjectAnnotation(timg,'rectangle',out_grids(:,:,di) + [0,0,1,1],'','Color','green');
    end
   
    %figure;
    origin_features = rst_mat;
    imshow(imresize(timg,[res_show,res_show],'box'));
    %set(gcf, 'Position',  [500, 10, 700, 700]);
    title(strcat(num2str(iter),', grids'));
    origin_grids = out_grids;
    saveas(gcf,strcat('train_rst\grids_',num2str(iter),'.png'));
    save(strcat('train_rst\rewards_',num2str(iter),'.mat'),'rewards');
    %% show confumat
    [errorsum_test,t_accu] = getaccuracy(rst_train,train_class);
    %% show
    set(gcf, 'Position',  [500, 10, 700, 700]);
    
    Ct= confusionmat(rst_train, train_class);
    confusionchart(Ct,emotions);
    title(strcat('dataset : All, confusion Matrix of test , accuracy : ',num2str(t_accu),'%'));
    saveas(gcf,strcat('train_rst\confusion_',num2str(iter),'.png'));
    acctory(iter) = t_accu;
end

profile viewer