function [Matrix,No_Of_Images] = HOG3(CS,Path)
Images = imageSet(Path);
No_Of_Images=Images.Count;
Matrix = [];
    for i = 1:Images.Count
        
        temp = imresize(read(Images,i),[223 223]);
        [hogFeature, ~]=extractHOGFeatures(temp, 'CellSize',[CS CS]);

        Matrix = [Matrix;hogFeature];
    end
 
end