function [Matrix,No_Of_Images] = HOG_o0(CS,Path)
Images = imageSet(Path);
No_Of_Images=Images.Count;
Matrix = [];
    for i = 1:Images.Count
        
        temp = imresize(read(Images,i),[100 100]);
        [hogFeature, ~]=extractHOGFeatures(temp, 'CellSize',[CS CS],'BlockOverlap',[0,0]);

        Matrix = [Matrix;hogFeature];
    end
 
end