clear all;
close all;
clc;
%% init
rate_expl = 0.00001;
max_iter = 100;
res_proc = 128;
res_show = 700;
min_csize = 1;
nums_emo = 7;
emotions = {'Neutral','Angry','Disgusted','Fear','Happy','Sad', 'Surprised'};
basepath = 'C:\Emotion Estimation From Facial Images\Databases\After Pre-Pocessing\4DBs (7 Expressions)';
datasets = {'MUG', 'WSEFEP', 'JAFFE', 'KDEF'};
%% image collect
load train_data_128;
load train_class;
[~,nums_train] = size(train_class);
disp('img load complete')
%% iteration
origin_grids(:,:) = [1,1,res_proc,res_proc];
origin_features = [];

now_emo = 7;

neu_class = train_class(1,:);
now_class = train_class(now_emo,:);
fit_class = horzcat(train_class(1,:),train_class(now_emo,:));

new_data  = cell2mat(train_data(1,:));
now_data  = train_data(now_emo,:);

for ind_iter = 1:max_iter
    [num_grids,~] = size(origin_grids);
    rewards = 0;
    %out_grids = origin_grids;
    out_grids = [];
    clear rst_svm;
    clear rst_mat;
    clear rst_train;
    
    ind_rwd = 1;
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
        for ind_img = 1:nums_train
            oimg = cell2mat(train_data(1,ind_img));
            timg = cell2mat(train_data(now_emo,ind_img));
            
            hogs_another = [];
            hogs_origin = [];
            for ci = 1:num_ci
                
                c_grid = nex_grids(ci,:);
                c_length = ceil(c_grid(3)/2);
                
                cp = [c_grid(1) + c_length, c_grid(2) + c_length];
                c_rad = c_grid(3);
                
                nbp = [max(cp(1) - c_rad,1),max(cp(2) - c_rad,1)];%new begin point
                nep = [min(cp(1) + c_rad, res_proc),min(cp(2) + c_rad, res_proc)];%new end point
                
                c_grid = cat(2,nbp,nep-nbp);

                cs = floor(min(c_grid(3),c_grid(4))/2);
                
                [hogFeature, ~]=extractHOGFeatures(imcrop(oimg,c_grid), 'CellSize',[cs cs]);
                hogs_origin = cat(2,hogs_origin,hogFeature);
                
                [hogFeature, ~]=extractHOGFeatures(imcrop(timg,c_grid), 'CellSize',[cs cs]);
                hogs_another = cat(2,hogs_another,hogFeature);
            end
            tot_features = [tot_features;hogs_origin];
            tot_features = [tot_features;hogs_another];
        end
        tot_features = feature_cat(origin_features,tot_features,ind_grid);
        %% train
        svm = fitcsvm(tot_features, fit_class);

        cvval = crossval(svm);
        rwd = 1 - kfoldLoss(cvval);%,'mode','individual');%sets of accu

        if rwd>max(rewards)
            rst_mat = tot_features;
        end
        rewards(ind_grid) = rwd;
        %ind_rwd = ind_rwd + 1;
    end
    %% output grid
    [~,num_rwd] = size(rewards);
    expl = rand(1,num_rwd)*rate_expl;%random exploration ÁöÁ¤ 0.000%~0.001
    
    [rwd_val, max_ind] = max(rewards - expl);
    
    if rwd_val<0.001
        disp('training terminated by no more division');
        break;%terminate training
    end
    tot_rwds(ind_iter) = rwd_val;
    
    [~,input_grids] = divide_grid(origin_grids(max_ind,:));
    out_grids = grid_extract(origin_grids,input_grids,max_ind);
    
    [num_dbox,~] = size(out_grids);
    %filler = [zeros(1,num_dbox);zeros(1,num_dbox);zeros(1,num_dbox)+1;zeros(1,num_dbox)+1]';
    %%
    grid_draw = out_grids + [zeros(1,num_dbox);zeros(1,num_dbox);zeros(1,num_dbox)+1;zeros(1,num_dbox)+1]';
    toc
    for di = 1:num_dbox
        timg= insertObjectAnnotation(timg,'rectangle',grid_draw(di,:),'','Color','green');
    end
   
    %figure;
    origin_features = rst_mat;
    imshow(imresize(timg,[res_show,res_show],'box'));
    title(strcat('grids of iteration',num2str(ind_iter),', accuracy:',num2str(rwd_val*100),'%'));
    origin_grids = out_grids;
    saveas(gcf,strcat('train_rst\grid_iter',num2str(ind_iter),'.png'));
    save(strcat('train_rst\rewards_iter',num2str(ind_iter),'.mat'),'rewards');
    set(gcf, 'Position',  [3100, 10, 700, 700]);    
    drawnow;
end

profile viewer