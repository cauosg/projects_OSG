function [errorsum, accu] = getaccuracy(rst,classes)
[len ~] = size(rst);
errorsum = 0;

for i=1:len
    if classes(i)==rst(i)
        errorsum = errorsum + 1;
    end
end

accu = errorsum/len*100;
end