function [Matrix,No_Of_Images] = HOG_gpu(CS,Path)
Images = imageSet(Path);
No_Of_Images=Images.Count;
Matrix = [];
    for i = 1:Images.Count
        ff = gpuArray(read(Images,i));
        temp = imresize(im2single(ff),[100 100]);
        [hogFeature, ~]=extractHOGFeatures(temp, 'CellSize',[CS CS]);

        Matrix = [Matrix;hogFeature];
    end
 
end