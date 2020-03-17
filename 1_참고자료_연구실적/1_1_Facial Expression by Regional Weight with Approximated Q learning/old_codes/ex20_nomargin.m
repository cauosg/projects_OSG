clear all;
close all;
clc;
%% init
max_iter = 100;
reward_func = 1.00;

res_proc = 100;
res_show = 700;
min_csize = 2;
now_emo = 2;

nums_emotion = 7;
emotions = {'Angry','Disgusted','Fear','Happy','Neutral', 'Sad', 'Surprised'};

basepath = 'C:\Emotion Estimation From Facial Images\Databases\After Pre-Pocessing\4DBs (7 Expressions)';
datasets = {'JAFFE','KDEF', 'MUG', 'WSEFEP'};
%% image collect
nums_train = 1;

for datanum=1:4
    for i = 1:nums_emotion
        train_pname = strcat(basepath, '\', datasets(datanum), '\Train\', emotions(i));
        Images = imageSet(char(train_pname));
        
        tempnum_train = Images.Count;
        %tot_imgset(:,:,1) = zeros(proc_size,proc_size);
        for img_count = 1:tempnum_train
            tot_imgset(:,:,nums_train + img_count - 1) = imresize(read(Images,img_count),[res_proc,res_proc]);
        end
        train_class(nums_train : nums_train + tempnum_train - 1) = i;
        
        nums_train = nums_train + tempnum_train;
    end
end
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
                % extract hog
                %cimg = imcrop(timg,c_grid);
                cs = floor(min(c_grid(3),c_grid(4))/2);
                [hogFeature, ~]=extractHOGFeatures(imcrop(timg,c_grid), 'CellSize',[cs cs]);
                temp_features = cat(2,temp_features,hogFeature);
                
                %figure;imshow(imcrop(timg,c_grid));
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
        %total_train_rst = svm.predict(tot_features);
        
        [~,t_accu] = getaccuracy(total_train_rst,train_class);
        %     %% show
        %     figure;
        %     set(gcf, 'Position',  [500, 10, 700, 700]);
        %
        %     Ct= confusionmat(total_train_rst, train_class);
        %     confusionchart(Ct,emotions);
        %     title(strcat('dataset : All, confusion Matrix of test , accuracy : ',num2str(t_accu),'%'));
        %
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
        timg= insertObjectAnnotation(timg,'rectangle',out_grids(:,:,di),'','Color','green');
    end
   
    %figure;
    origin_features = rst_mat;
    imshow(imresize(timg,[res_show,res_show],'box'));
    %set(gcf, 'Position',  [500, 10, 700, 700]);
    title(strcat('epoch',num2str(iter),', grids'));
    origin_grids = out_grids;
    saveas(gcf,strcat('train_rst\grids_epoch',num2str(iter),'.png'));
    save(strcat('train_rst\rewards_epoch',num2str(iter),'.mat'),'rewards');
    %% show confumat
    [errorsum_test,t_accu] = getaccuracy(rst_train,train_class);
    %% show
    set(gcf, 'Position',  [500, 10, 700, 700]);
    
    Ct= confusionmat(rst_train, train_class);
    confusionchart(Ct,emotions);
    title(strcat('dataset : All, confusion Matrix of test , accuracy : ',num2str(t_accu),'%'));
    saveas(gcf,strcat('train_rst\confusion_epoch',num2str(iter),'.png'));    
end

profile viewer