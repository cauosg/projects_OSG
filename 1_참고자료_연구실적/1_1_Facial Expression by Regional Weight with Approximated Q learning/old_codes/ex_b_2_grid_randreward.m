clc;clear;close all;

%figure; 

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
    %%
    [num_ci,~] = size(check_grids);
    for ci = 1:num_ci
        
    end
    %%
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