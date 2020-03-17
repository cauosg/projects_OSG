function [now_length,nex_grids] = divide_grid(origin_grids)
[num_grids,~] = size(origin_grids);

for i = 1:num_grids
    [now_length,nex_grids] = step_grid(origin_grids(i,:));   
end
end
%% ver_origin
% function nex_grids = divide_grid(origin_grids)
% [~,~,num_grids] = size(origin_grids);
% 
% for i = 1:num_grids
%     now_grid = origin_grids(:,:,i);
%     
%     %% grid devide, row-col 가로 세로
%     now_length = ceil((now_grid(3))/2);
%     
%     cp1 = now_grid(1) + now_length; %center point
%     cp2 = now_grid(2) + now_length;
%     
%     nex_grids(:,:,1) = [now_grid(1), now_grid(2), now_length, now_length];
%     nex_grids(:,:,2) = [cp1, now_grid(2), now_length, now_length];
%     nex_grids(:,:,3) = [now_grid(1), cp2, now_length, now_length];
%     nex_grids(:,:,4) = [cp1, cp2, now_length, now_length];
%     
% end
% end
% function [now_length,nex_grids] = divide_grid(origin_grids)
% [num_grids,~] = size(origin_grids);
% 
% for i = 1:num_grids
%     now_grid = origin_grids(i,:);
%     
%     %% grid devide, row-col 가로 세로
%     now_length = ceil((now_grid(3))/2);
%     
%     cp1 = now_grid(1) + now_length; %center point
%     cp2 = now_grid(2) + now_length;
%     
%     nex_grids = [now_grid(1), now_grid(2), now_length, now_length];
%     nex_grids = [nex_grids;cp1, now_grid(2), now_length, now_length];
%     nex_grids = [nex_grids;now_grid(1), cp2, now_length, now_length];
%     nex_grids = [nex_grids;cp1, cp2, now_length, now_length];
%     
% end
% end