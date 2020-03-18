clc;clear;close all;

%figure; 

max_iter = 3;
reward_func = 1.00;

proc_res = 100;
show_res = 700;

timg = im2double(imread('testimg.jpg'));
timg = imresize(timg,[proc_res, proc_res]);

origin_grids(:,:,1) = [1,1,proc_res,proc_res];
%% iteration
for iter = 1:max_iter
  
    [~,~,num_grids] = size(origin_grids);
    for i=1:num_grids
        
    nex_grids = divide_grid(origin_grids(:,:,i));
    
    %[~,~,grids_to_draw] = size(nex_grids);
    %if
    out_grids(:,:,1+(i-1)*4 : i*4) = nex_grids;
    end

[~,~,grids_to_draw] = size(out_grids);
for i = 1:grids_to_draw
    timg= insertObjectAnnotation(timg,'rectangle',out_grids(:,:,i),'','Color','green');
end
figure; 
imshow(imresize(timg,[show_res,show_res],'box'));
set(gcf, 'Position',  [500, 10, 700, 700]);
title(strcat('epoch',num2str(iter),', grids'));
 
origin_grids = out_grids;
end