for i=1:1000
    tic;
    total_train_rst = kfoldPredict(cvval);
    times(i) = toc;
end
times = times';
mean(times)

set(gcf, 'Position',  [500, 10, 700, 700]);
plot(times);
title(strcat('mean : ',num2str(mean(times)*1000),'ms'));