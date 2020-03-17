function [now_length,nex_grids] = step_grid(origin_grids)
    now_grid = origin_grids;
    
    %% grid devide, row-col 가로 세로
    now_length = floor((now_grid(3))/2)-1;
    steper = ceil((now_grid(3))/2)-1 - now_length;
    %steper = 1 - steper;
    cp1 = now_grid(1) + now_length + 1 + steper; %center point
    cp2 = now_grid(2) + now_length + 1 + steper;
    
    now_length = now_length + steper;
    
    nex_grids = [now_grid(1), now_grid(2), now_length, now_length];
    nex_grids = [nex_grids;cp1, now_grid(2), now_length, now_length];
    nex_grids = [nex_grids;now_grid(1), cp2, now_length, now_length];
    nex_grids = [nex_grids;cp1, cp2, now_length, now_length];
end