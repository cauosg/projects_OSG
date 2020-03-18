function [Matrix,No_Of_Images] = HOG2(CS,Path)
Images = imageSet(Path);
No_Of_Images=Images.Count;
Matrix = [];
    for i = 1:Images.Count

        [hogFeature, ~]=extractHOGFeatures(read(Images,i),'CellSize',[CS CS]);

        Matrix = [Matrix;hogFeature];
    end
close(waitbar1)%% WaitBar 
end