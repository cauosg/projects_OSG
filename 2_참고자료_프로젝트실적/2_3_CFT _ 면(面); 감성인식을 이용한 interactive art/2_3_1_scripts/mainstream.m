clear all;
close all;
clc;
%%
%��Ʈ��ũ �ʱ�ȭ
net = alexnet;
netname = 'AlexNet';
layer = 23;

levels = 2;       % dep
iterations = 20;  %
specific = 1; % �ڿ���������(����� feature maps�� ��) �������� ����
%%
%���� Ʈ���̴׼� �ε�
load preset.mat;
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
tidx(ii) = idx;
end
%%
faceDetector = vision.CascadeObjectDetector();
%webcam �ʱ�ȭ
webcamlist%��밡��ķ����Ʈ
cam = webcam;
cam.AvailableResolutions
cam.Resolution = '640x480';
%%
for maxruntime = 1:10
    
%%
img = snapshot(cam);
bbox = step(faceDetector,img);
[fnum, ~] = size(bbox);
%webcam���� �̹��� grabing �� �󱼿��� ����, **�νľ� 0���Ͻ� ���� �ʿ�

if fnum>0
    
fregion = imcrop(img,bbox);
fregion = rgb2gray(fregion);
%**�Կ�ȯ�� ���� preprocessing �ʿ�
fregion = imresize(fregion,[160 160]);
%%
%�����ν�(1~7)
[Matrix, ~]=extractHOGFeatures(fregion,'CellSize',[CS CS]);
emotionnum = knn.predict(Matrix);
end
%%
%���
outimg = img;
[ow oh oc] = size(outimg);
outimg = imresize(outimg, InputSize(1:2));

I = deepDreamImage(net, layer, tidx(emotionnum), ...
	'Verbose', true, ...
	'NumIterations', iterations, ...
	'PyramidLevels', levels, ...
	'InitialImage', outimg);

[w h c z] = size(I);
output = zeros(w, h, c);

for index = 1:z
%figure;
temp = I(:, : , : , index);
%imshow(temp);
output = output + temp;
end

output = output / z;
output = imresize(output,[ow oh]);
imshow(output);
emotions(emotionnum)
%title(emotions(emotionnum));
end