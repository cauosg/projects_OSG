function out_grids = grid_extract(origin_grids,input_grid,ind)

[num_grids,~] = size(origin_grids);

temp1 = origin_grids(1 : ind - 1, :);
temp2 = origin_grids(ind + 1 : num_grids, :);
[~,~,empty_check] = size(temp1);
if empty_check < 1
    out_grids = input_grid;
else
    out_grids = [temp1; input_grid];
end

[~,~,empty_check] = size(temp2);
if empty_check > 0
    out_grids = [out_grids ; temp2];
end
end