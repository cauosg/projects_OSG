function [Matrix,No_Of_Images] = HOG2(CS,Path)
Images = imageSet(Path);
No_Of_Images=Images.Count;
Matrix = [];
    for i = 1:Images.Count
        
        temp = imresize(read(Images,i),[100 100]);
        [hogFeature, ~]=extractHOGFeatures(temp, 'CellSize',[CS CS]);

        Matrix = [Matrix;hogFeature];
        disp(strcat('training step: ',num2str(i),'%'));
    end
 
end