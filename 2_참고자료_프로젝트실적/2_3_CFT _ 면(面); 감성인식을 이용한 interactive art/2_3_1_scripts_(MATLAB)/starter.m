clear all;
close all;
clc;
%%
%��Ʈ��ũ �ʱ�ȭ
net = alexnet;
netname = 'AlexNet';
layer = 23;

levels = 1;       % dep
iterations = 1;  %
specific = 1; % �ڿ���������(����� feature maps�� ��) �������� ����

camresol = '800x600';
procsize = [600 800];
%%
%���� Ʈ���̴׼� �ε�
load preset.mat;
load adjpreset.mat;
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

knn = ClassificationKNN.fit(Train_Matrix, Train_Class);
%%
emotionnum = 5;%
%%
%�����鿡 ���� ��ǥ �̹��� �Է�, �� feature �з�
%7���� ���ܿ�, ������ �߰�
emotions = {'angry','disgust','fear','happy','neutral', 'sad', 'surprised'};
InputSize = net.Layers(1).InputSize;
for ii=1:7
timg_name = strcat(num2str(ii),'.jpg');
initimg = imread(timg_name);
initimg = imresize(initimg, InputSize(1:2));
[label, scores] = classify(net, initimg);
[~, idx] = sort(scores, 'descend');
idx = idx(specific:-1 : 1);
tidx(:,:,ii) = idx;
end
%%
faceDetector = vision.CascadeObjectDetector();
%webcam �ʱ�ȭ
webcamlist%��밡��ķ����Ʈ
cam = webcam;
cam.AvailableResolutions
cam.Resolution = camresol;
%%
load adjpreset.mat
rerun_3