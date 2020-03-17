clear all;
close all;
clc;
%%
%감정 트레이닝셋 로드
load preset.mat;
%%  *********************** Call HOG Functions ******************************
[Train_Angry_Matrix,Train_No_Of_AN_Images] = HOG(CS,Train_Angry_Folder);
[Train_Disgusted_Matrix,Train_No_Of_DI_Images] = HOG(CS,Train_Disgusted_Folder);
[Train_Fear_Matrix,Train_No_Of_FE_Images] = HOG(CS,Train_Fear_Folder);
[Train_Happy_Matrix,Train_No_Of_HA_Images] = HOG(CS,Train_Happy_Folder);
[Train_neutral_Matrix,Train_No_Of_NE_Images] = HOG(CS,Train_Neutral_Folder);
[Train_Sad_Matrix,Train_No_Of_SA_Images] = HOG(CS,Train_Sad_Folder);
[Train_Surprised_Matrix,Train_No_Of_SU_Images] = HOG(CS,Train_Surprised_Folder);
Train_Matrix = vertcat(Train_Angry_Matrix, Train_Disgusted_Matrix, Train_Fear_Matrix, Train_Happy_Matrix, Train_neutral_Matrix, Train_Sad_Matrix, Train_Surprised_Matrix);

Train_Class = zeros(size(length(Train_Matrix),1),1);
    Train_Class(1:  Train_No_Of_AN_Images,1) = 1;  % Angry;
    Train_Class(    Train_No_Of_AN_Images+1:...
                    Train_No_Of_AN_Images+Train_No_Of_DI_Images,1) = 2; %Disgusted;
    Train_Class(    Train_No_Of_AN_Images+Train_No_Of_DI_Images+1:...
                    Train_No_Of_AN_Images+Train_No_Of_DI_Images+Train_No_Of_FE_Images,1) = 3; %Fear;
    Train_Class(    Train_No_Of_AN_Images+Train_No_Of_DI_Images+Train_No_Of_FE_Images+1:...
                    Train_No_Of_AN_Images+Train_No_Of_DI_Images+Train_No_Of_FE_Images+Train_No_Of_HA_Images,1) = 4; %Happy;
    Train_Class(    Train_No_Of_AN_Images+Train_No_Of_DI_Images+Train_No_Of_FE_Images+Train_No_Of_HA_Images+1:...
                    Train_No_Of_AN_Images+Train_No_Of_DI_Images+Train_No_Of_FE_Images+Train_No_Of_HA_Images+Train_No_Of_NE_Images,1) = 5; %neutral;
    Train_Class(    Train_No_Of_AN_Images+Train_No_Of_DI_Images+Train_No_Of_FE_Images+Train_No_Of_HA_Images+Train_No_Of_NE_Images+1:...
                    Train_No_Of_AN_Images+Train_No_Of_DI_Images+Train_No_Of_FE_Images+Train_No_Of_HA_Images+Train_No_Of_NE_Images+Train_No_Of_SA_Images,1) = 6; %Sad;
    Train_Class(    Train_No_Of_AN_Images+Train_No_Of_DI_Images+Train_No_Of_FE_Images+Train_No_Of_HA_Images+Train_No_Of_NE_Images+Train_No_Of_SA_Images+1:...
                    Train_No_Of_AN_Images+Train_No_Of_DI_Images+Train_No_Of_FE_Images+Train_No_Of_HA_Images+Train_No_Of_NE_Images+Train_No_Of_SA_Images+Train_No_Of_SU_Images,1) = 7; %Surprised;
                
[Test_Angry_Matrix,Test_No_Of_AN_Images] = HOG(CS,Test_Angry_Folder);
[Test_Disgusted_Matrix,Test_No_Of_DI_Images] = HOG(CS,Test_Disgusted_Folder);
[Test_Fear_Matrix,Test_No_Of_FE_Images] = HOG(CS,Test_Fear_Folder);
[Test_Happy_Matrix,Test_No_Of_HA_Images] = HOG(CS,Test_Happy_Folder);
[Test_neutral_Matrix,Test_No_Of_NE_Images] = HOG(CS,Test_Neutral_Folder);
[Test_Sad_Matrix,Test_No_Of_SA_Images] = HOG(CS,Test_Sad_Folder);
[Test_Surprised_Matrix,Test_No_Of_SU_Images] = HOG(CS,Test_Surprised_Folder);

Test_Matrix = vertcat(Test_Angry_Matrix, Test_Disgusted_Matrix, Test_Fear_Matrix, Test_Happy_Matrix, Test_neutral_Matrix, Test_Sad_Matrix, Test_Surprised_Matrix);                
Test_Class = zeros(size(length(Test_Matrix),1),1);

    Test_Class(1:  Test_No_Of_AN_Images,1) = 1;  % Angry;
    Test_Class(    Test_No_Of_AN_Images+1:...
                   Test_No_Of_AN_Images+Test_No_Of_DI_Images,1) = 2; %Disgusted;
    Test_Class(    Test_No_Of_AN_Images+Test_No_Of_DI_Images+1:...
                   Test_No_Of_AN_Images+Test_No_Of_DI_Images+Test_No_Of_FE_Images,1) = 3; %Afraid;
    Test_Class(    Test_No_Of_AN_Images+Test_No_Of_DI_Images+Test_No_Of_FE_Images+1:...
                   Test_No_Of_AN_Images+Test_No_Of_DI_Images+Test_No_Of_FE_Images+Test_No_Of_HA_Images,1) = 4; %Happy;
    Test_Class(    Test_No_Of_AN_Images+Test_No_Of_DI_Images+Test_No_Of_FE_Images+Test_No_Of_HA_Images+1:...
                   Test_No_Of_AN_Images+Test_No_Of_DI_Images+Test_No_Of_FE_Images+Test_No_Of_HA_Images+Test_No_Of_NE_Images,1) = 5; %neutral;
    Test_Class(    Test_No_Of_AN_Images+Test_No_Of_DI_Images+Test_No_Of_FE_Images+Test_No_Of_HA_Images+Test_No_Of_NE_Images+1:...
                   Test_No_Of_AN_Images+Test_No_Of_DI_Images+Test_No_Of_FE_Images+Test_No_Of_HA_Images+Test_No_Of_NE_Images+Test_No_Of_SA_Images,1) = 6; %Sad;
    Test_Class(    Test_No_Of_AN_Images+Test_No_Of_DI_Images+Test_No_Of_FE_Images+Test_No_Of_HA_Images+Test_No_Of_NE_Images+Test_No_Of_SA_Images+1:...
                   Test_No_Of_AN_Images+Test_No_Of_DI_Images+Test_No_Of_FE_Images+Test_No_Of_HA_Images+Test_No_Of_NE_Images+Test_No_Of_SA_Images+Test_No_Of_SU_Images,1) = 7; %Surprised;
knn = ClassificationKNN.fit(Train_Matrix, Train_Class);
% %%
% emotionnum = 5;%
% emotions = {'angry','disgust','fear','happy','neutral', 'sad', 'surprised'};
% 
% %%
% %감정인식(1~7)
% [Matrix, ~]=extractHOGFeatures(fregion,'CellSize',[CS CS]);
emotionnum = knn.predict(Test_Matrix);