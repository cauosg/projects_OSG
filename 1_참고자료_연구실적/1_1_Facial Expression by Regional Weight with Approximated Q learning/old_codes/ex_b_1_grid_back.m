clc;clear;close all;

figure; 

max_iter = 2;
reward_func = 1.00;

proc_res = 100;
show_res = 700;

timg = im2double(imread('testimg.jpg'));
timg = imresize(timg,[proc_res, proc_res]);

origin_grids(:,:,1) = [1,1,proc_res,proc_res];
%% iteration
for iter = 1:max_iter
    
[~,~,num_grids] = size(origin_grids);

for i = 1:num_grids
    now_grid = origin_grids(:,:,i);
    
    %% grid devide, row-col 가로 세로
    now_length = ceil((now_grid(3) - now_grid(1))/2);
    
    cp1 = now_grid(1) + now_length; %center point
    cp2 = now_grid(2) + now_length;
    
    nex_grids(:,:,1) = [now_grid(1), now_grid(2), now_length, now_length];
    nex_grids(:,:,2) = [cp1, now_grid(2), now_length, now_length];
    nex_grids(:,:,3) = [now_grid(1), cp2, now_length, now_length];
    nex_grids(:,:,4) = [cp1, cp2, now_length, now_length];
    
end

[~,~,grids_to_draw] = size(nex_grids);
for i = 1:grids_to_draw
    timg= insertObjectAnnotation(timg,'rectangle',nex_grids(:,:,i),'','Color','green');
end
imshow(imresize(timg,[show_res,show_res],'box'));
set(gcf, 'Position',  [500, 10, 700, 700]);
title(strcat('epoch',num2str(i),', grids'));

end