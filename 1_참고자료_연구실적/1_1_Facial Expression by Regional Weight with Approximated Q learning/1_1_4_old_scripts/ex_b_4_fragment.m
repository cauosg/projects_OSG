clear all;
close all;
clc;
% %% init
% proc_size = 100;
% 
% emotion_nums = 7;
% emotions = {'Angry','Disgusted','Fear','Happy','Neutral', 'Sad', 'Surprised'};
% 
% basepath = 'C:\Emotion Estimation From Facial Images\Databases\After Pre-Pocessing\4DBs (7 Expressions)';
% datasets = {'JAFFE','KDEF', 'MUG', 'WSEFEP'};
% %% image collect
% nums_train = 1;
% 
% for datanum=1:4
%     for i = 1:emotion_nums
%         train_pname = strcat(basepath, '\', datasets(datanum), '\Train\', emotions(i));
%         Images = imageSet(char(train_pname));
%         
%         tempnum_train = Images.Count;
%             %tot_imgset(:,:,1) = zeros(proc_size,proc_size);
%             for img_count = 1:tempnum_train
%                 tot_imgset(:,:,nums_train + img_count - 1) = imresize(read(Images,img_count),[proc_size,proc_size]);
%             end
%         train_class(nums_train : nums_train + tempnum_train - 1) = i;
%         
%         nums_train = nums_train + tempnum_train;
%     end
% end
% %%

max_iter = 100;
reward_func = 1.00;

proc_res = 100;
show_res = 700;
min_csize = 2;
timg = im2double(imread('testimg.jpg'));
timg = imresize(timg,[proc_res, proc_res]);

origin_grids(:,:) = [1,1,proc_res,proc_res];
%% iteration
for iter = 1:max_iter
  
    [num_grids,~] = size(origin_grids);
    %rewards = [];
    %out_grids = origin_grids;
    out_grids = [];
    
    %% division
    for i=1:num_grids
    [proc_len,nex_grids] = divide_grid(origin_grids(i,:));
    
    %findreward   
    %% reward
    
    check_grids = grid_extract(origin_grids,nex_grids,i);
    %% fragment
    tic
    [num_ci,~] = size(check_grids);
    clear cimg;
    %close all;
    for ci = 1:num_ci
        c_grid = check_grids(ci,:);
        c_length = ceil(c_grid(3)/2);
        
        cp = [c_grid(1) + c_length, c_grid(2) + c_length];
        c_rad = c_grid(3);
        
        nbp = [max(cp(1) - c_rad,1),max(cp(2) - c_rad,1)];%new begin point 
        nep = [min(cp(1) + c_rad, proc_res),min(cp(2) + c_rad, proc_res)];%new end point
        
        c_grid = cat(2,nbp,nep-nbp);
        cimg = imcrop(timg,c_grid);
        %figure;imshow(cimg);
    end
    %%
    toc
    rwd = rand;
    
    if proc_len < min_csize
         rwd = -1;
    end
    
    rewards(i) = rwd;
    
    end
    
    %% output grid
    [rwd_val, max_ind] = max(rewards);
    
    if rwd_val<0
        disp('training terminated by no more division');
        break;%terminate training
    end
    [~,input_grids] = divide_grid(origin_grids(max_ind,:));
    out_grids = grid_extract(origin_grids,input_grids,max_ind);
    
[~,~,grids_to_draw] = size(out_grids);
%%
for i = 1:grids_to_draw
    timg= insertObjectAnnotation(timg,'rectangle',out_grids(:,:,i),'','Color','green');
end
%figure; 
imshow(imresize(timg,[show_res,show_res],'box'));
%set(gcf, 'Position',  [500, 10, 700, 700]);
title(strcat('epoch',num2str(iter),', grids'));
 drawnow;
origin_grids = out_grids;
end