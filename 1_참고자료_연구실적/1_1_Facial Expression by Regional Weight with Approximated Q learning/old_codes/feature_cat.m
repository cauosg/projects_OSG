function out_features = feature_cat(origin_features,input_features,ind)

[~,num_features] = size(origin_features);

side_flag = true;
if ind<1
    side_flag = false;
    out_features = horzcat(input_features,origin_features(:, 2:num_features));
end

if ind>num_features - 1
    side_flag = false;
    out_features = horzcat(origin_features(:, 1:num_features-1),input_features);
end

if side_flag
    out_features = horzcat(origin_features(:, 1:ind - 1), input_features, origin_features(:,ind + 1 : num_features));
end
end